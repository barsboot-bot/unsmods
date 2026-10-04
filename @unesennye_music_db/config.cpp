// Author: KRa Tos (Константин) | Project: Unesennye
// ============================================================================
//  @unesennye_music_db — Assets-мод: только .ogg-файлы и конфиги звуков.
//  Подключается на КЛИЕНТЕ и на СЕРВЕРЕ (сервер валидирует trackID по этому конфигу).
//  Добавление нового трека НЕ требует перекомпиляции скриптов.
// ============================================================================

class CfgPatches
{
    class Unesennye_MusicDB
    {
        author      = "KRa Tos (Константин)";
        name        = "Unesennye Music Database";
        version     = "1.0.0";
        requiredAddons[] = {"DayZ_Core"};
        requiredVersion = 1.24;
        units[]     = {};
        weapons[]   = {};
    };
};

class CfgMods
{
    class Unesennye_MusicDB
    {
        id          = "unesennye_music_db";
        dir         = "@unesennye_music_db";
        name        = "Unesennye Music DB [ASSETS] v1.0.0";
        author      = "KRa Tos (Константин)";
        version     = "1.0.0";
        type        = "soundtrack";
        hideName    = 0;
        hideIcon    = 0;
        action      = "";
    };
};

// ---------------------------------------------------------------------------
// SoundSets: движковые сеты образцов. Один сет = один трек.
// Путь к .ogg указывается относительно корня мода БЕЗ расширения.
// ---------------------------------------------------------------------------
class CfgSoundSets
{
    class Unesennye_Track_Demo_01_SoundSet
    {
        soundShaders[] = {"Unesennye_Track_Demo_01_SoundShader"};
        volumeFactor   = 1.0;
        frequencyFactor = 1.0;
        spatial        = 1;        // 3D-позиционирование от машины
        distanceDelay  = 0;
        occlusionMode  = 0;
        panWithVelocity = 0;
    };
    class Unesennye_Track_Demo_02_SoundSet
    {
        soundShaders[] = {"Unesennye_Track_Demo_02_SoundShader"};
        volumeFactor   = 1.0;
        frequencyFactor = 1.0;
        spatial        = 1;
        distanceDelay  = 0;
        occlusionMode  = 0;
        panWithVelocity = 0;
    };
};

class CfgSoundShaders
{
    class Unesennye_Track_Demo_01_SoundShader
    {
        samples[] =
        {
            "\\unesennye_music_db\\data\\sounds\\tracks\\demo_01", 1
        };
        rangeMin   = 0;
        rangeMax   = 90;          // радиус слышимости музыки из машины, метров
        volume     = 0.8;
    };
    class Unesennye_Track_Demo_02_SoundShader
    {
        samples[] =
        {
            "\\unesennye_music_db\\data\\sounds\\tracks\\demo_02", 1
        };
        rangeMin   = 0;
        rangeMax   = 90;
        volume     = 0.8;
    };
};

// ---------------------------------------------------------------------------
// Трек-лист (читается и клиентом, и сервером через $[CfgUnesennyeTracks\...]).
// Класс-трека = trackID, который гоняется по RPC. sample = SoundSet для PlaySoundCD.
// ---------------------------------------------------------------------------
class CfgUnesennyeTracks
{
    class unes_demo_01
    {
        title       = "Demo Track One";
        artist      = "KRa Tos";
        sample      = "Unesennye_Track_Demo_01_SoundSet";
        durationSec = 180;
    };
    class unes_demo_02
    {
        title       = "Demo Track Two";
        artist      = "KRa Tos";
        sample      = "Unesennye_Track_Demo_02_SoundSet";
        durationSec = 210;
    };
};
