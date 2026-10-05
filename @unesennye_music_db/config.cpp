// Author: KRa Tos (Константин) | Project: Unesennye
// =====================================================================
// UNSENNYE MUSIC DB — звуковые ассеты + CfgUnesennyeTracks (v1.0.0)
// Добавление трека НЕ требует перекомпиляции скриптов:
//   1) положить .ogg в data/sounds/tracks/
//   2) добавить SoundShader + SoundSet + track_N в этот config.cpp
// Автор: KRa Tos (Константин)
// =====================================================================

class CfgPatches
{
    class unesennye_music_db
    {
        name = "Unesennye Music DB";
        author = "KRa Tos (Константин)";
        version = "1.0.0";
        url = "";
        requiredVersion = 1.24;
        requiredAddons[] = {"DZ_Data"};
    };
};

class CfgMods
{
    class unesennye_music_db
    {
        name = "Unesennye Music DB v1.0.0";
        dir = "unesennye_music_db";
        action = "";
        hideName = 0;
        hideIcon = 1;
        version = "1.0.0";
        author = "KRa Tos (Константин)";
        authorID = "KRaTos";
        description = "База музыки для автомобильного радио Unesennye.";
    };
};

// ---------- Звуковые шейдеры (по одному на трек) ----------
class CfgSoundShaders
{
    class Unesennye_Track01_Shader
    {
        samples[] = {{"data/sounds/tracks/track_01.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track02_Shader
    {
        samples[] = {{"data/sounds/tracks/track_02.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track03_Shader
    {
        samples[] = {{"data/sounds/tracks/track_03.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track04_Shader
    {
        samples[] = {{"data/sounds/tracks/track_04.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track05_Shader
    {
        samples[] = {{"data/sounds/tracks/track_05.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
};

// ---------- Звуковые сеты (то, что вызывает SEffectManager.PlaySound) ----------
class CfgSoundSets
{
    class Unesennye_Track01_SoundSet
    {
        soundShaders[] = {"Unesennye_Track01_Shader"};
        spatial = 1;              // 3D-позиционирование от машины
        looped = 1;               // трек играет до команды Stop
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track02_SoundSet
    {
        soundShaders[] = {"Unesennye_Track02_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track03_SoundSet
    {
        soundShaders[] = {"Unesennye_Track03_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track04_SoundSet
    {
        soundShaders[] = {"Unesennye_Track04_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track05_SoundSet
    {
        soundShaders[] = {"Unesennye_Track05_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
};

// =====================================================================
// ДОБАВЛЕНО v1.1.0: ПРЕДМЕТ «Музыкальная флешка» UnesennyeMusicCard
// CfgUnesennyeTracks ниже НЕ удалён и НЕ изменён — трек-листы ссылаются
// на него классами UnesennyeMusicCardN (индекс N == индекс track_N).
// =====================================================================
class CfgVehicles
{
    class ItemBase;

    // Базовый класс флешки (используется как тип предмета)
    class UnesennyeMusicCard_Base : ItemBase
    {
        author = "KRa Tos (Константин)"; // водяной знак
        displayName = "Unesennye Music Card";
        descriptionShort = "USB-флешка с записанной музыкой из базы Unesennye. Вставьте в рацию для воспроизведения. Author: KRa Tos (Константин)";
        model = "\dz\gear\notebook.p3d";      // заглушка; замените на .p3d флешки/кассеты при наличии
        icon = "DZ_Gear_Books_Epics";
        itemBank[] = {"InventoryMuseumSlot"}; // маленький предмет, помещается в карманы
    };

    // 5 предметов под 5 треков CfgUnesennyeTracks (треки НЕ дублируются — только ссылки индексом)
    class UnesennyeMusicCard0 : UnesennyeMusicCard_Base {}; // -> track_0
    class UnesennyeMusicCard1 : UnesennyeMusicCard_Base {}; // -> track_1
    class UnesennyeMusicCard2 : UnesennyeMusicCard_Base {}; // -> track_2
    class UnesennyeMusicCard3 : UnesennyeMusicCard_Base {}; // -> track_3
    class UnesennyeMusicCard4 : UnesennyeMusicCard_Base {}; // -> track_4
};

// ---------- Реестр треков (читается скриптами через Config* API) ----------
// trackID = имя класса без префикса "track_" — сервер валидирует по нему
class CfgUnesennyeTracks
{
    class track_0
    {
        title = "Track One";
        file = "data/sounds/tracks/track_01.ogg";
        soundSet = "Unesennye_Track01_SoundSet";
    };
    class track_1
    {
        title = "Track Two";
        file = "data/sounds/tracks/track_02.ogg";
        soundSet = "Unesennye_Track02_SoundSet";
    };
    class track_2
    {
        title = "Track Three";
        file = "data/sounds/tracks/track_03.ogg";
        soundSet = "Unesennye_Track03_SoundSet";
    };
    class track_3
    {
        title = "Track Four";
        file = "data/sounds/tracks/track_04.ogg";
        soundSet = "Unesennye_Track04_SoundSet";
    };
    class track_4
    {
        title = "Track Five";
        file = "data/sounds/tracks/track_05.ogg";
        soundSet = "Unesennye_Track05_SoundSet";
    };
};
