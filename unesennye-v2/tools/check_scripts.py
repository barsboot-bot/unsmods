#!/usr/bin/env python3
"""Статическая проверка Enforce-скриптов Unesennye v2 (без Workbench).

Проверки:
  1. Повторные объявления верхнего уровня (class / enum / глобальная функция) в разных файлах.
     Исключение: `modded class` — их переопределение допустимо.
  2. Баланс фигурных скобок в каждом файле.
  3. Запрещённое слово `delete` (владение через `ref`, ручное удаление не используем).
  4. Запрещённые API, которых нет в документации DayZ (см. README, раздел «Что не подтверждено»).
  5. `CallLater(this, ...)` — неправильная форма; нужен CallLaterByName.
  6. `modded class UnesennyeX` без базового определения `class UnesennyeX` где-либо в проекте.

Проверка — эвристическая: она ловит типовые ошибки, но не заменяет компиляцию в Workbench.

Использование:
    python3 tools/check_scripts.py [корень_проекта]
Код возврата: 0 — ошибок нет, 1 — есть ошибки.
"""
import glob
import os
import re
import sys

BANNED_NAMES = {
    "ConfigReadString": "нет такой функции; используйте GetGame().ConfigGetText(path, out s)",
    "ConfigReadInt": "нет такой функции; используйте GetGame().ConfigGetInt(path)",
    "ConfigGetClassName": "нет такой функции; используйте GetGame().ConfigGetChildName(path, i, out name)",
    "ConfigGetClassCount": "нет такой функции; используйте GetGame().ConfigGetChildrenCount(path)",
    "ConfigAddClass": "runtime-конфиг не поддерживается; конфиг генерируется в config.cpp",
    "ConfigAddString": "runtime-конфиг не поддерживается; конфиг генерируется в config.cpp",
    "ConfigAddInt": "runtime-конфиг не поддерживается; конфиг генерируется в config.cpp",
    "RegisterServerRpc": "не найдено в документации; используйте GetGame().RPCRegisterServer / ScriptRPC",
    "RegisterClientRpc": "не найдено в документации; используйте GetGame().RPCRegisterClient / ScriptRPC",
    "ParamWriteBuffer": "не найдено в документации; используйте ScriptRPC.Write",
    "ParamReadBuffer": "не найдено в документации; используйте ParamsReadContext",
    "RPCTargetGroup": "не найдено в документации; используйте Send(target, id, guaranteed, recipient)",
    "SetSoundAutodestroy": "не подтверждено; используйте EffectSound.SetAutodestroy(true)",
    "GetEntityFromID": "не подтверждено; используйте GetGame().GetObjectByNetworkId(low, high)",
    "GetObjectsAtPosition": "не подтверждено в документации; искать объекты через Object API / триггеры",
}

CALL_LATER_THIS_RE = re.compile(r"\bCallLater\s*\(\s*this\b")
DELETE_RE = re.compile(r"\bdelete\s+[A-Za-z_]")
MODDED_RE = re.compile(r"^\s*modded\s+class\s+([A-Za-z_]\w*)")
CLASS_RE = re.compile(r"^\s*class\s+([A-Za-z_]\w*)")
ENUM_RE = re.compile(r"^\s*enum\s+([A-Za-z_]\w*)")
FUNC_RE = re.compile(r"^\s*(?:static\s+)?(?:const\s+)?(?:ref\s+)?[A-Za-z_][\w<>,\s]*?\s+([A-Za-z_]\w*)\s*\(")

STRIP_STRING_RE = re.compile(r'"(?:\\.|[^"\\\n])*"')


def strip_code(text):
    """Удаляет комментарии и содержимое строк, сохраняя переводы строк (номера строк совпадают)."""
    out = []
    i = 0
    n = len(text)
    while i < n:
        two = text[i:i + 2]
        if two == "//":
            while i < n and text[i] != "\n":
                i += 1
        elif two == "/*":
            end = text.find("*/", i + 2)
            end = n if end < 0 else end + 2
            out.append("\n" * text.count("\n", i, end))
            i = end
        elif text[i] == '"':
            m = STRIP_STRING_RE.match(text, i)
            if m:
                out.append('""')
                i = m.end()
            else:
                out.append('"')
                i += 1
        else:
            out.append(text[i])
            i += 1
    return "".join(out)


def find_script_files(root):
    pattern = os.path.join(root, "@*", "scripts", "**", "*.c")
    return sorted(glob.glob(pattern, recursive=True))


def check_braces(stripped_lines):
    """Возвращает список (номер_строки, сообщение) при нарушении баланса."""
    problems = []
    depth = 0
    for lineno, line in enumerate(stripped_lines, start=1):
        for ch in line:
            if ch == "{":
                depth += 1
            elif ch == "}":
                depth -= 1
                if depth < 0:
                    problems.append((lineno, "лишняя закрывающая '}'"))
                    depth = 0
    if depth != 0:
        problems.append((len(stripped_lines), "незакрытые скобки: глубина %d в конце файла" % depth))
    return problems


def top_level_declarations(stripped_lines):
    """Объявления на глубине 0: (номер_строки, вид, имя, modded)."""
    decls = []
    depth = 0
    for lineno, line in enumerate(stripped_lines, start=1):
        if depth == 0:
            m = MODDED_RE.match(line)
            if m:
                decls.append((lineno, "class", m.group(1), True))
            else:
                m = CLASS_RE.match(line)
                if m:
                    decls.append((lineno, "class", m.group(1), False))
                else:
                    m = ENUM_RE.match(line)
                    if m:
                        decls.append((lineno, "enum", m.group(1), False))
                    elif "(" in line and not line.strip().startswith(("return", "if", "for", "while", "switch")):
                        m = FUNC_RE.match(line)
                        if m and m.group(1) not in ("if", "for", "while", "switch"):
                            decls.append((lineno, "func", m.group(1), False))
        depth += line.count("{") - line.count("}")
        if depth < 0:
            depth = 0
    return decls


def check_project(root):
    """Возвращает список ошибок (строки 'путь:строка: сообщение')."""
    errors = []
    files = find_script_files(root)
    if not files:
        return ["%s: не найдено ни одного .c файла в @*/scripts" % root]

    declared = {}  # имя -> список (путь, строка, вид)
    modded_names = {}  # имя -> (путь, строка)
    defined_classes = set()

    for path in files:
        rel = os.path.relpath(path, root)
        with open(path, "r", encoding="utf-8") as fh:
            raw = fh.read()
        code = strip_code(raw)
        lines = code.split("\n")

        for lineno, msg in check_braces(lines):
            errors.append("%s:%d: %s" % (rel, lineno, msg))

        for lineno, line in enumerate(lines, start=1):
            if DELETE_RE.search(line):
                errors.append("%s:%d: `delete` запрещён; управляйте владением через ref" % (rel, lineno))
            if CALL_LATER_THIS_RE.search(line):
                errors.append("%s:%d: CallLater(this, ...) неверно; используйте CallLaterByName(this, \"Fn\", ms, repeat)" % (rel, lineno))
            for name, hint in BANNED_NAMES.items():
                if re.search(r"\b%s\b" % re.escape(name), line):
                    errors.append("%s:%d: %s — %s" % (rel, lineno, name, hint))

        for lineno, kind, name, modded in top_level_declarations(lines):
            if modded:
                modded_names.setdefault(name, (rel, lineno))
                continue
            if kind == "class":
                defined_classes.add(name)
            declared.setdefault(name, []).append((rel, lineno, kind))

    for name, places in sorted(declared.items()):
        if len(places) > 1:
            where = ", ".join("%s:%d (%s)" % p for p in places)
            errors.append("повторное объявление верхнего уровня '%s': %s" % (name, where))

    for name, (rel, lineno) in sorted(modded_names.items()):
        if name.startswith("Unesennye") and name not in defined_classes:
            errors.append("%s:%d: modded class %s без базового объявления в проекте" % (rel, lineno, name))

    return errors


def main(argv=None):
    argv = sys.argv[1:] if argv is None else argv
    root = os.path.abspath(argv[0]) if argv else os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    errors = check_project(root)
    for err in errors:
        print(err)
    if errors:
        print("Найдено ошибок: %d" % len(errors), file=sys.stderr)
        return 1
    print("check_scripts: ошибок нет")
    return 0


if __name__ == "__main__":
    sys.exit(main())
