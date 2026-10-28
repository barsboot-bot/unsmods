"""Тесты статического проверяльщика. Запуск: python3 -m unittest discover -s tests -v"""
import os
import sys
import tempfile
import textwrap
import unittest

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
sys.path.insert(0, os.path.join(ROOT, "tools"))

import check_scripts as cs  # noqa: E402


def make_project(files):
    """files: {относительный путь: текст}. Возвращает корень временного проекта."""
    tmp = tempfile.mkdtemp()
    for rel, text in files.items():
        path = os.path.join(tmp, rel)
        os.makedirs(os.path.dirname(path), exist_ok=True)
        with open(path, "w", encoding="utf-8") as fh:
            fh.write(textwrap.dedent(text))
    return tmp


class StripTests(unittest.TestCase):
    def test_comments_and_strings_removed(self):
        src = 'int a; // delete x\n/* delete y */ string s = "delete z";\n'
        out = cs.strip_code(src)
        self.assertNotIn("delete", out)
        self.assertEqual(out.count("\n"), src.count("\n"))


class RuleTests(unittest.TestCase):
    def test_clean_project_passes(self):
        root = make_project({
            "@m/scripts/4_World/a.c": """
                class UnsA { void Go() { GetGame().ConfigGetInt("x"); } }
            """,
            "@m/scripts/4_World/b.c": """
                modded class CarScript { override void SetActions() { super.SetActions(); } }
            """,
        })
        self.assertEqual(cs.check_project(root), [])

    def test_duplicate_class_detected(self):
        root = make_project({
            "@a/scripts/4_World/x.c": "class Dup { }",
            "@b/scripts/4_World/y.c": "class Dup { }",
        })
        errors = cs.check_project(root)
        self.assertTrue(any("повторное объявление верхнего уровня 'Dup'" in e for e in errors), errors)

    def test_modded_class_is_not_duplicate(self):
        root = make_project({
            "@a/scripts/4_World/x.c": "modded class CarScript { }",
            "@b/scripts/4_World/y.c": "modded class CarScript { }",
        })
        self.assertEqual([e for e in cs.check_project(root) if "повторное" in e], [])

    def test_duplicate_global_function_detected(self):
        root = make_project({
            "@a/scripts/4_World/x.c": "void OnRPC(int a)\n{\n}\n",
            "@b/scripts/4_World/y.c": "void OnRPC(int b)\n{\n}\n",
        })
        errors = cs.check_project(root)
        self.assertTrue(any("'OnRPC'" in e for e in errors), errors)

    def test_delete_detected(self):
        root = make_project({"@a/scripts/4_World/x.c": "void F() { Foo f; delete f; }\n"})
        errors = cs.check_project(root)
        self.assertTrue(any("`delete` запрещён" in e for e in errors), errors)

    def test_unbalanced_braces_detected(self):
        root = make_project({"@a/scripts/4_World/x.c": "class A { void F() { }\n"})
        errors = cs.check_project(root)
        self.assertTrue(any("незакрытые скобки" in e for e in errors), errors)

    def test_extra_closing_brace_detected(self):
        root = make_project({"@a/scripts/4_World/x.c": "class A { } }\n"})
        errors = cs.check_project(root)
        self.assertTrue(any("лишняя закрывающая" in e for e in errors), errors)

    def test_banned_api_detected(self):
        root = make_project({"@a/scripts/4_World/x.c": "void F() { ConfigReadString(\"p\"); }\n"})
        errors = cs.check_project(root)
        self.assertTrue(any("ConfigReadString" in e for e in errors), errors)

    def test_banned_api_in_comment_ignored(self):
        root = make_project({"@a/scripts/4_World/x.c": "// ConfigReadString was removed\nclass A { }\n"})
        self.assertEqual(cs.check_project(root), [])

    def test_callLater_this_detected(self):
        root = make_project({"@a/scripts/4_World/x.c": "void F() { CallLater(this, \"X\", 1); }\n"})
        errors = cs.check_project(root)
        self.assertTrue(any("CallLater(this" in e for e in errors), errors)

    def test_callLaterByName_allowed(self):
        root = make_project({"@a/scripts/4_World/x.c": "void F() { CallLaterByName(this, \"X\", 1, true); }\n"})
        self.assertEqual(cs.check_project(root), [])

    def test_modded_unesennye_class_needs_base(self):
        root = make_project({"@a/scripts/4_World/x.c": "modded class UnesennyeMissing { }\n"})
        errors = cs.check_project(root)
        self.assertTrue(any("UnesennyeMissing" in e for e in errors), errors)


class RepoTests(unittest.TestCase):
    def test_unesennye_v2_is_clean(self):
        self.assertEqual(cs.check_project(ROOT), [])


if __name__ == "__main__":
    unittest.main()
