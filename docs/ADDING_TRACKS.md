# Добавление новых треков в @unesennye_music_db

Перекомпиляция скриптов **не требуется** — только конфиг + ogg-файлы.

## Шаг 1. Файл
Положите трек в `@unesennye_music_db/data/sounds/tracks/`, например `my_song.ogg`.
Требования: формат **OGG (Vorbis)**, моно/стерео, ≤10 МБ, без DRM.
В конфиге путь указывается **без расширения**: `my_song`.

## Шаг 2. config.cpp — три блока

```cpp
// 1. SoundShader (образец + радиус слышимости)
class CfgSoundShaders
{
    class Unesennye_Track_my_song_SoundShader
    {
        samples[] = { "\\unesennye_music_db\\data\\sounds\\tracks\\my_song", 1 };
        rangeMin  = 0;
        rangeMax  = 90;     // метры от машины
        volume    = 0.8;
    };
};

// 2. SoundSet (обёртка шейдера)
class CfgSoundSets
{
    class Unesennye_Track_my_song_SoundSet
    {
        soundShaders[] = { "Unesennye_Track_my_song_SoundShader" };
        volumeFactor    = 1.0;
        spatial         = 1;
    };
};

// 3. Запись в трек-лист: имя класса = trackID для RPC
class CfgUnesennyeTracks
{
    class unes_my_song
    {
        title       = "Название";
        artist      = "Артист";
        sample      = "Unesennye_Track_my_song_SoundSet";
        durationSec = 200;
    };
};
```

## Шаг 4. Перепаковка
Перепакуйте `@unesennye_music_db` (Addons folder → .pak). Обновлённый пак
ставится **и на сервер, и клиентам** — сервер валидирует `trackID` по этому же конфигу.

## Важно
- `trackID` (`unes_my_song`) должен быть уникальным и без пробелов/спецсимволов.
- Если у клиента старый пак без нового трека — он просто не найдёт SoundSet и
  проигнорирует команду (без краша).
- Один пакет ассетов = один релиз музыки: версионировать можно меняя только
  `version` в `mod.cpp`/`CfgMods`.
