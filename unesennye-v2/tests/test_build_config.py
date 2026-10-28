"""Тесты генератора config.cpp. Запуск: python3 -m unittest discover -s tests -v"""
import json
import os
import sys
import tempfile
import unittest

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
sys.path.insert(0, os.path.join(ROOT, "tools"))

import build_config as bc  # noqa: E402


def manifest(**overrides):
    base = {
        "tracks": [
            {"id": 2, "title": "Вторая \"кавычка\"", "file": "track_002.ogg", "car": True},
            {"id": 1, "title": "Первая", "file": "track_001.ogg", "car": True, "volume": 0.5, "range": 70},
            {"id": 7, "title": "Альбомный", "file": "track_007.ogg"},
        ],
        "albums": [
            {"id": "Album02", "title": "Второй", "tracks": [7]},
            {"id": "Album01", "title": "Первый", "tracks": [7, 1]},
        ],
    }
    base.update(overrides)
    return base


class ValidateTests(unittest.TestCase):
    def test_normalizes_order(self):
        model = bc.validate(manifest())
        self.assertEqual([t["id"] for t in model["tracks"]], [1, 2, 7])
        self.assertEqual([a["id"] for a in model["albums"]], ["Album01", "Album02"])

    def test_defaults_applied(self):
        model = bc.validate(manifest())
        t7 = [t for t in model["tracks"] if t["id"] == 7][0]
        self.assertFalse(t7["car"])
        self.assertEqual(t7["range"], 40)
        self.assertAlmostEqual(t7["volume"], 0.85)

    def test_rejects_duplicate_id(self):
        bad = manifest(tracks=[{"id": 1, "title": "a", "file": "a.ogg"}, {"id": 1, "title": "b", "file": "b.ogg"}])
        with self.assertRaisesRegex(bc.ManifestError, "id повторяется"):
            bc.validate(bad)

    def test_rejects_out_of_range_id(self):
        for bad_id in (0, 1000, -3, "5", True):
            with self.subTest(bad_id=bad_id):
                bad = manifest(tracks=[{"id": bad_id, "title": "a", "file": "a.ogg"}])
                with self.assertRaises(bc.ManifestError):
                    bc.validate(bad)

    def test_rejects_bad_file_name(self):
        bad = manifest(tracks=[{"id": 1, "title": "a", "file": "Track 1.mp3"}])
        with self.assertRaisesRegex(bc.ManifestError, "file должен быть"):
            bc.validate(bad)

    def test_rejects_unknown_album_track(self):
        bad = manifest(albums=[{"id": "Album01", "title": "x", "tracks": [99]}])
        with self.assertRaisesRegex(bc.ManifestError, "не найден в tracks"):
            bc.validate(bad)

    def test_rejects_bad_album_id(self):
        bad = manifest(albums=[{"id": "Album1", "title": "x", "tracks": [1]}])
        with self.assertRaisesRegex(bc.ManifestError, "Album01"):
            bc.validate(bad)

    def test_rejects_album_over_limit(self):
        ids = list(range(1, 22))
        tracks = [{"id": i, "title": "t%d" % i, "file": "track_%03d.ogg" % i} for i in ids]
        bad = {"tracks": tracks, "albums": [{"id": "Album01", "title": "x", "tracks": ids}]}
        with self.assertRaisesRegex(bc.ManifestError, "больше 20"):
            bc.validate(bad)

    def test_collects_all_errors(self):
        bad = manifest(tracks=[{"id": 0, "title": "", "file": "x.mp3"}])
        with self.assertRaises(bc.ManifestError) as ctx:
            bc.validate(bad)
        self.assertGreaterEqual(str(ctx.exception).count("\n") + 1, 2)


class RenderTests(unittest.TestCase):
    def setUp(self):
        self.text = bc.render(bc.validate(manifest()))

    def test_track_classes_and_sets(self):
        self.assertIn("class Track_001", self.text)
        self.assertIn("class Unesennye_T002_SoundSet", self.text)
        self.assertIn("soundShaders[] = {\"Unesennye_T001_Shader\"};", self.text)

    def test_album_slots_reference_ids(self):
        self.assertIn("class Album01", self.text)
        self.assertIn("class Slot01 { track = 7; };", self.text)
        self.assertIn("class Slot02 { track = 1; };", self.text)
        self.assertIn("trackCount = 2;", self.text)

    def test_card_classes_have_explicit_scope(self):
        block = self.text.split("class UnesennyeMusicCard_Album01")[1].split("};")[0]
        self.assertIn("scope = 2;", block)

    def test_string_escaping(self):
        self.assertIn('title = "Вторая \\"кавычка\\"";', self.text)

    def test_deterministic(self):
        self.assertEqual(self.text, bc.render(bc.validate(manifest())))

    def test_braces_balanced(self):
        self.assertEqual(self.text.count("{"), self.text.count("}"))

    def test_every_slot_track_has_a_track_class(self):
        import re
        slots = [int(x) for x in re.findall(r"track = (\d+);", self.text)]
        for tid in slots:
            self.assertIn("trackId = %d;" % tid, self.text)


class CliTests(unittest.TestCase):
    def test_check_mode_does_not_write(self):
        with tempfile.TemporaryDirectory() as tmp:
            man = os.path.join(tmp, "m.json")
            out = os.path.join(tmp, "config.cpp")
            with open(man, "w", encoding="utf-8") as fh:
                json.dump(manifest(), fh, ensure_ascii=False)
            self.assertEqual(bc.main(["--manifest", man, "--output", out, "--check"]), 0)
            self.assertFalse(os.path.exists(out))

    def test_invalid_manifest_returns_error(self):
        with tempfile.TemporaryDirectory() as tmp:
            man = os.path.join(tmp, "m.json")
            with open(man, "w", encoding="utf-8") as fh:
                json.dump({"tracks": []}, fh)
            self.assertEqual(bc.main(["--manifest", man, "--output", os.path.join(tmp, "c.cpp"), "--check"]), 1)

    def test_repo_manifest_is_valid(self):
        with open(bc.DEFAULT_MANIFEST, "r", encoding="utf-8") as fh:
            model = bc.validate(json.load(fh))
        self.assertTrue(model["tracks"])


if __name__ == "__main__":
    unittest.main()
