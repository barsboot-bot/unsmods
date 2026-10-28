// АВТОГЕНЕРАЦИЯ: не редактировать вручную. Источник: tools/tracks.json
// Команда: python3 tools/build_config.py

class CfgPatches
{
    class unesennye_music_db
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 1.24;
        requiredAddons[] = {"DZ_Data", "DZ_Scripts"};
    };
};

class CfgMods
{
    class unesennye_music_db
    {
        dir = "unesennye_music_db";
        name = "Unesennye Music DB v2";
        type = "mod";
        dependencies[] = {"Core", "Game", "World", "Mission"};
        class defs
        {
            class engineScriptModule { value = ""; files[] = {"unesennye_music_db/scripts/1_Core"}; };
            class gameScriptModule { value = ""; files[] = {"unesennye_music_db/scripts/3_Game"}; };
            class worldScriptModule { value = ""; files[] = {"unesennye_music_db/scripts/4_World"}; };
        };
    };
};

class CfgSoundShaders
{
    class Unesennye_T001_Shader
    {
        samples[] = {{"data/sounds/tracks/track_001.ogg", 1}};
        volume = 0.850;
        range = 60;
        frequency = 1;
    };
    class Unesennye_T002_Shader
    {
        samples[] = {{"data/sounds/tracks/track_002.ogg", 1}};
        volume = 0.850;
        range = 60;
        frequency = 1;
    };
    class Unesennye_T003_Shader
    {
        samples[] = {{"data/sounds/tracks/track_003.ogg", 1}};
        volume = 0.850;
        range = 60;
        frequency = 1;
    };
    class Unesennye_T004_Shader
    {
        samples[] = {{"data/sounds/tracks/track_004.ogg", 1}};
        volume = 0.850;
        range = 40;
        frequency = 1;
    };
    class Unesennye_T005_Shader
    {
        samples[] = {{"data/sounds/tracks/track_005.ogg", 1}};
        volume = 0.850;
        range = 40;
        frequency = 1;
    };
    class Unesennye_T006_Shader
    {
        samples[] = {{"data/sounds/tracks/track_006.ogg", 1}};
        volume = 0.850;
        range = 40;
        frequency = 1;
    };
};

class CfgSoundSets
{
    class Unesennye_T001_SoundSet
    {
        soundShaders[] = {"Unesennye_T001_Shader"};
        spatial = 1;
        doppler = 0;
        volumeFactor = 1.0;
        distanceFactor = 1.0;
        looped = 0;
    };
    class Unesennye_T002_SoundSet
    {
        soundShaders[] = {"Unesennye_T002_Shader"};
        spatial = 1;
        doppler = 0;
        volumeFactor = 1.0;
        distanceFactor = 1.0;
        looped = 0;
    };
    class Unesennye_T003_SoundSet
    {
        soundShaders[] = {"Unesennye_T003_Shader"};
        spatial = 1;
        doppler = 0;
        volumeFactor = 1.0;
        distanceFactor = 1.0;
        looped = 0;
    };
    class Unesennye_T004_SoundSet
    {
        soundShaders[] = {"Unesennye_T004_Shader"};
        spatial = 1;
        doppler = 0;
        volumeFactor = 1.0;
        distanceFactor = 1.0;
        looped = 0;
    };
    class Unesennye_T005_SoundSet
    {
        soundShaders[] = {"Unesennye_T005_Shader"};
        spatial = 1;
        doppler = 0;
        volumeFactor = 1.0;
        distanceFactor = 1.0;
        looped = 0;
    };
    class Unesennye_T006_SoundSet
    {
        soundShaders[] = {"Unesennye_T006_Shader"};
        spatial = 1;
        doppler = 0;
        volumeFactor = 1.0;
        distanceFactor = 1.0;
        looped = 0;
    };
};

class CfgVehicles
{
    class ItemBase;
    class UnesennyeMusicCard_Base : ItemBase
    {
        // scope = 0: базовый класс не появляется в игре; наследники задают scope = 2 явно
        scope = 0;
        displayName = "Музыкальная карта";
        descriptionShort = "Карта с альбомом Unesennye.";
        model = "\dz\gear\notebook.p3d";
        weight = 50;
        itemSize[] = {1, 1};
    };
    class UnesennyeMusicCard_Album01 : UnesennyeMusicCard_Base
    {
        scope = 2;
        displayName = "Карта: Пример: альбом 01";
        descriptionShort = "Альбом: Пример: альбом 01";
        unesennyeAlbumId = "Album01";
    };
};

class CfgUnesennyeTracks
{
    class Track_001
    {
        trackId = 1;
        title = "Пример: трек 1 (машина)";
        file = "data/sounds/tracks/track_001.ogg";
        soundSet = "Unesennye_T001_SoundSet";
        carPlaylist = 1;
    };
    class Track_002
    {
        trackId = 2;
        title = "Пример: трек 2 (машина)";
        file = "data/sounds/tracks/track_002.ogg";
        soundSet = "Unesennye_T002_SoundSet";
        carPlaylist = 1;
    };
    class Track_003
    {
        trackId = 3;
        title = "Пример: трек 3 (машина)";
        file = "data/sounds/tracks/track_003.ogg";
        soundSet = "Unesennye_T003_SoundSet";
        carPlaylist = 1;
    };
    class Track_004
    {
        trackId = 4;
        title = "Пример: трек 4 (альбом)";
        file = "data/sounds/tracks/track_004.ogg";
        soundSet = "Unesennye_T004_SoundSet";
        carPlaylist = 0;
    };
    class Track_005
    {
        trackId = 5;
        title = "Пример: трек 5 (альбом)";
        file = "data/sounds/tracks/track_005.ogg";
        soundSet = "Unesennye_T005_SoundSet";
        carPlaylist = 0;
    };
    class Track_006
    {
        trackId = 6;
        title = "Пример: трек 6 (альбом)";
        file = "data/sounds/tracks/track_006.ogg";
        soundSet = "Unesennye_T006_SoundSet";
        carPlaylist = 0;
    };
};

class CfgUnesennyeAlbums
{
    class Album01
    {
        title = "Пример: альбом 01";
        trackCount = 3;
        class Slot01 { track = 4; };
        class Slot02 { track = 5; };
        class Slot03 { track = 6; };
    };
};
