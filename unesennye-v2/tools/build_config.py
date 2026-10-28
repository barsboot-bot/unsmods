#!/usr/bin/env python3
"""Генератор @unesennye_music_db/config.cpp из tools/tracks.json.

Использование:
    python3 tools/build_config.py                 # записать config.cpp
    python3 tools/build_config.py --check         # только валидация манифеста
    python3 tools/build_config.py --strict-audio  # ошибка, если .ogg нет в data/sounds/tracks

Манифест — единственный источник правды. Config.cpp не редактируется вручную.
"""
import argparse
import json
import os
import re
import sys

TRACK_ID_MIN = 1
TRACK_ID_MAX = 999
ALBUM_MAX_TRACKS = 20
TRACK_FILE_RE = re.compile(r"^[a-z0-9_]+\.ogg$")
ALBUM_ID_RE = re.compile(r"^Album[0-9]{2}$")

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
DEFAULT_MANIFEST = os.path.join(ROOT, "tools", "tracks.json")
DEFAULT_OUTPUT = os.path.join(ROOT, "@unesennye_music_db", "config.cpp")
AUDIO_DIR_REL = "data/sounds/tracks"
AUDIO_DIR_ABS = os.path.join(ROOT, "@unesennye_music_db", AUDIO_DIR_REL)


class ManifestError(ValueError):
    """Манифест содержит ошибки. Текст — список всех найденных проблем."""


def cfg_str(value):
    """Экранирование строки для config.cpp."""
    return '"' + value.replace("\\", "\\\\").replace('"', '\\"') + '"'


def _is_int(value):
    return isinstance(value, int) and not isinstance(value, bool)


def validate(manifest):
    """Проверяет манифест и возвращает нормализованную структуру (треки и альбомы по возрастанию id)."""
    errors = []
    raw_tracks = manifest.get("tracks")
    if not isinstance(raw_tracks, list) or not raw_tracks:
        raise ManifestError("tracks: нужен непустой список")

    tracks = {}
    for index, raw in enumerate(raw_tracks):
        where = "tracks[%d]" % index
        if not isinstance(raw, dict):
            errors.append("%s: должен быть объектом" % where)
            continue
        tid = raw.get("id")
        if not _is_int(tid) or not (TRACK_ID_MIN <= tid <= TRACK_ID_MAX):
            errors.append("%s: id должен быть целым от %d до %d" % (where, TRACK_ID_MIN, TRACK_ID_MAX))
            continue
        where = "track %d" % tid
        if tid in tracks:
            errors.append("%s: id повторяется" % where)
            continue
        title = raw.get("title")
        if not isinstance(title, str) or not title.strip():
            errors.append("%s: нужен непустой title" % where)
        fname = raw.get("file")
        if not isinstance(fname, str) or not TRACK_FILE_RE.match(fname):
            errors.append("%s: file должен быть вида track_001.ogg (a-z, 0-9, _)" % where)
        car = raw.get("car", False)
        if not isinstance(car, bool):
            errors.append("%s: car должен быть true/false" % where)
        volume = raw.get("volume", 0.85)
        if not isinstance(volume, (int, float)) or not (0.0 < volume <= 2.0):
            errors.append("%s: volume должен быть в (0, 2]" % where)
        rng = raw.get("range", 40)
        if not _is_int(rng) or not (1 <= rng <= 500):
            errors.append("%s: range должен быть целым в [1, 500]" % where)
        tracks[tid] = {
            "id": tid,
            "title": title if isinstance(title, str) else "",
            "file": fname if isinstance(fname, str) else "",
            "car": bool(car) if isinstance(car, bool) else False,
            "volume": float(volume) if isinstance(volume, (int, float)) else 0.85,
            "range": rng if _is_int(rng) else 40,
        }

    albums = {}
    for index, raw in enumerate(manifest.get("albums", [])):
        where = "albums[%d]" % index
        if not isinstance(raw, dict):
            errors.append("%s: должен быть объектом" % where)
            continue
        aid = raw.get("id")
        if not isinstance(aid, str) or not ALBUM_ID_RE.match(aid):
            errors.append("%s: id должен быть вида Album01" % where)
            continue
        if aid in albums:
            errors.append("%s: id %s повторяется" % (where, aid))
            continue
        title = raw.get("title")
        if not isinstance(title, str) or not title.strip():
            errors.append("%s (%s): нужен непустой title" % (where, aid))
        refs = raw.get("tracks")
        if not isinstance(refs, list) or not refs:
            errors.append("%s (%s): tracks — непустой список id" % (where, aid))
            continue
        if len(refs) > ALBUM_MAX_TRACKS:
            errors.append("%s (%s): больше %d треков" % (where, aid, ALBUM_MAX_TRACKS))
        for ref in refs:
            if not _is_int(ref) or ref not in tracks:
                errors.append("%s (%s): трек %r не найден в tracks" % (where, aid, ref))
        albums[aid] = {"id": aid, "title": title if isinstance(title, str) else "", "tracks": list(refs)}

    if errors:
        raise ManifestError("\n".join(errors))

    return {
        "tracks": [tracks[k] for k in sorted(tracks)],
        "albums": [albums[k] for k in sorted(albums)],
    }


def missing_audio(model):
    """Список треков, у которых файла нет в data/sounds/tracks (не ошибка по умолчанию)."""
    return [t["file"] for t in model["tracks"]
            if not os.path.isfile(os.path.join(AUDIO_DIR_ABS, t["file"]))]


def render(model):
    """Возвращает текст config.cpp. Вывод детерминирован: одинаковый манифест → одинаковый файл."""
    out = []
    add = out.append

    add("// АВТОГЕНЕРАЦИЯ: не редактировать вручную. Источник: tools/tracks.json")
    add("// Команда: python3 tools/build_config.py")
    add("")
    add("class CfgPatches")
    add("{")
    add("    class unesennye_music_db")
    add("    {")
    add("        units[] = {};")
    add("        weapons[] = {};")
    add("        requiredVersion = 1.24;")
    add("        requiredAddons[] = {\"DZ_Data\", \"DZ_Scripts\"};")
    add("    };")
    add("};")
    add("")
    add("class CfgMods")
    add("{")
    add("    class unesennye_music_db")
    add("    {")
    add("        dir = \"unesennye_music_db\";")
    add("        name = \"Unesennye Music DB v2\";")
    add("        type = \"mod\";")
    add("        dependencies[] = {\"Core\", \"Game\", \"World\", \"Mission\"};")
    add("        class defs")
    add("        {")
    add("            class engineScriptModule { value = \"\"; files[] = {\"unesennye_music_db/scripts/1_Core\"}; };")
    add("            class gameScriptModule { value = \"\"; files[] = {\"unesennye_music_db/scripts/3_Game\"}; };")
    add("            class worldScriptModule { value = \"\"; files[] = {\"unesennye_music_db/scripts/4_World\"}; };")
    add("        };")
    add("    };")
    add("};")
    add("")

    add("class CfgSoundShaders")
    add("{")
    for t in model["tracks"]:
        add("    class Unesennye_T%03d_Shader" % t["id"])
        add("    {")
        add("        samples[] = {{\"%s/%s\", 1}};" % (AUDIO_DIR_REL, t["file"]))
        add("        volume = %.3f;" % t["volume"])
        add("        range = %d;" % t["range"])
        add("        frequency = 1;")
        add("    };")
    add("};")
    add("")

    add("class CfgSoundSets")
    add("{")
    for t in model["tracks"]:
        add("    class Unesennye_T%03d_SoundSet" % t["id"])
        add("    {")
        add("        soundShaders[] = {\"Unesennye_T%03d_Shader\"};" % t["id"])
        add("        spatial = 1;")
        add("        doppler = 0;")
        add("        volumeFactor = 1.0;")
        add("        distanceFactor = 1.0;")
        add("        looped = 0;")
        add("    };")
    add("};")
    add("")

    add("class CfgVehicles")
    add("{")
    add("    class ItemBase;")
    add("    class UnesennyeMusicCard_Base : ItemBase")
    add("    {")
    add("        // scope = 0: базовый класс не появляется в игре; наследники задают scope = 2 явно")
    add("        scope = 0;")
    add("        displayName = \"Музыкальная карта\";")
    add("        descriptionShort = \"Карта с альбомом Unesennye.\";")
    add("        model = \"\\dz\\gear\\notebook.p3d\";")
    add("        weight = 50;")
    add("        itemSize[] = {1, 1};")
    add("    };")
    for a in model["albums"]:
        add("    class UnesennyeMusicCard_%s : UnesennyeMusicCard_Base" % a["id"])
        add("    {")
        add("        scope = 2;")
        add("        displayName = %s;" % cfg_str("Карта: " + a["title"]))
        add("        descriptionShort = %s;" % cfg_str("Альбом: " + a["title"]))
        add("        unesennyeAlbumId = \"%s\";" % a["id"])
        add("    };")
    add("};")
    add("")

    add("class CfgUnesennyeTracks")
    add("{")
    for t in model["tracks"]:
        add("    class Track_%03d" % t["id"])
        add("    {")
        add("        trackId = %d;" % t["id"])
        add("        title = %s;" % cfg_str(t["title"]))
        add("        file = %s;" % cfg_str("%s/%s" % (AUDIO_DIR_REL, t["file"])))
        add("        soundSet = \"Unesennye_T%03d_SoundSet\";" % t["id"])
        add("        carPlaylist = %d;" % (1 if t["car"] else 0))
        add("    };")
    add("};")
    add("")

    add("class CfgUnesennyeAlbums")
    add("{")
    for a in model["albums"]:
        add("    class %s" % a["id"])
        add("    {")
        add("        title = %s;" % cfg_str(a["title"]))
        add("        trackCount = %d;" % len(a["tracks"]))
        for slot, tid in enumerate(a["tracks"], start=1):
            add("        class Slot%02d { track = %d; };" % (slot, tid))
        add("    };")
    add("};")
    add("")
    return "\n".join(out)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--manifest", default=DEFAULT_MANIFEST)
    parser.add_argument("--output", default=DEFAULT_OUTPUT)
    parser.add_argument("--check", action="store_true", help="только валидация, без записи")
    parser.add_argument("--strict-audio", action="store_true", help="ошибка при отсутствии .ogg файлов")
    args = parser.parse_args(argv)

    try:
        with open(args.manifest, "r", encoding="utf-8") as fh:
            manifest = json.load(fh)
        model = validate(manifest)
    except (OSError, json.JSONDecodeError, ManifestError) as exc:
        print("ОШИБКА манифеста:\n%s" % exc, file=sys.stderr)
        return 1

    absent = missing_audio(model)
    if absent:
        level = "ОШИБКА" if args.strict_audio else "предупреждение"
        print("%s: нет аудиофайлов в %s: %s" % (level, AUDIO_DIR_REL, ", ".join(absent)), file=sys.stderr)
        if args.strict_audio:
            return 1

    if args.check:
        print("Манифест корректен: треков %d, альбомов %d" % (len(model["tracks"]), len(model["albums"])))
        return 0

    text = render(model)
    with open(args.output, "w", encoding="utf-8", newline="\n") as fh:
        fh.write(text)
    print("Записан %s (треков %d, альбомов %d)" % (args.output, len(model["tracks"]), len(model["albums"])))
    return 0


if __name__ == "__main__":
    sys.exit(main())
