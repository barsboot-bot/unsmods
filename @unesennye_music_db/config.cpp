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

    // ===== v1.2.0: слоты шейдеров под треки альбомов (Track06..Track79) =====
    class Unesennye_Track06_Shader
    {
        samples[] = {{"data/sounds/tracks/track_06.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track07_Shader
    {
        samples[] = {{"data/sounds/tracks/track_07.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track08_Shader
    {
        samples[] = {{"data/sounds/tracks/track_08.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track09_Shader
    {
        samples[] = {{"data/sounds/tracks/track_09.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track10_Shader
    {
        samples[] = {{"data/sounds/tracks/track_10.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track11_Shader
    {
        samples[] = {{"data/sounds/tracks/track_11.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track12_Shader
    {
        samples[] = {{"data/sounds/tracks/track_12.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track13_Shader
    {
        samples[] = {{"data/sounds/tracks/track_13.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track14_Shader
    {
        samples[] = {{"data/sounds/tracks/track_14.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track15_Shader
    {
        samples[] = {{"data/sounds/tracks/track_15.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track16_Shader
    {
        samples[] = {{"data/sounds/tracks/track_16.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track17_Shader
    {
        samples[] = {{"data/sounds/tracks/track_17.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track18_Shader
    {
        samples[] = {{"data/sounds/tracks/track_18.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track19_Shader
    {
        samples[] = {{"data/sounds/tracks/track_19.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track20_Shader
    {
        samples[] = {{"data/sounds/tracks/track_20.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track21_Shader
    {
        samples[] = {{"data/sounds/tracks/track_21.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track22_Shader
    {
        samples[] = {{"data/sounds/tracks/track_22.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track23_Shader
    {
        samples[] = {{"data/sounds/tracks/track_23.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track24_Shader
    {
        samples[] = {{"data/sounds/tracks/track_24.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track25_Shader
    {
        samples[] = {{"data/sounds/tracks/track_25.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track26_Shader
    {
        samples[] = {{"data/sounds/tracks/track_26.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track27_Shader
    {
        samples[] = {{"data/sounds/tracks/track_27.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track28_Shader
    {
        samples[] = {{"data/sounds/tracks/track_28.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track29_Shader
    {
        samples[] = {{"data/sounds/tracks/track_29.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track30_Shader
    {
        samples[] = {{"data/sounds/tracks/track_30.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track31_Shader
    {
        samples[] = {{"data/sounds/tracks/track_31.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track32_Shader
    {
        samples[] = {{"data/sounds/tracks/track_32.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track33_Shader
    {
        samples[] = {{"data/sounds/tracks/track_33.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track34_Shader
    {
        samples[] = {{"data/sounds/tracks/track_34.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track35_Shader
    {
        samples[] = {{"data/sounds/tracks/track_35.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track36_Shader
    {
        samples[] = {{"data/sounds/tracks/track_36.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track37_Shader
    {
        samples[] = {{"data/sounds/tracks/track_37.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track38_Shader
    {
        samples[] = {{"data/sounds/tracks/track_38.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track39_Shader
    {
        samples[] = {{"data/sounds/tracks/track_39.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track40_Shader
    {
        samples[] = {{"data/sounds/tracks/track_40.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track41_Shader
    {
        samples[] = {{"data/sounds/tracks/track_41.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track42_Shader
    {
        samples[] = {{"data/sounds/tracks/track_42.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track43_Shader
    {
        samples[] = {{"data/sounds/tracks/track_43.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track44_Shader
    {
        samples[] = {{"data/sounds/tracks/track_44.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track45_Shader
    {
        samples[] = {{"data/sounds/tracks/track_45.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track46_Shader
    {
        samples[] = {{"data/sounds/tracks/track_46.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track47_Shader
    {
        samples[] = {{"data/sounds/tracks/track_47.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track48_Shader
    {
        samples[] = {{"data/sounds/tracks/track_48.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track49_Shader
    {
        samples[] = {{"data/sounds/tracks/track_49.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track50_Shader
    {
        samples[] = {{"data/sounds/tracks/track_50.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track51_Shader
    {
        samples[] = {{"data/sounds/tracks/track_51.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track52_Shader
    {
        samples[] = {{"data/sounds/tracks/track_52.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track53_Shader
    {
        samples[] = {{"data/sounds/tracks/track_53.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track54_Shader
    {
        samples[] = {{"data/sounds/tracks/track_54.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track55_Shader
    {
        samples[] = {{"data/sounds/tracks/track_55.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track56_Shader
    {
        samples[] = {{"data/sounds/tracks/track_56.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track57_Shader
    {
        samples[] = {{"data/sounds/tracks/track_57.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track58_Shader
    {
        samples[] = {{"data/sounds/tracks/track_58.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track59_Shader
    {
        samples[] = {{"data/sounds/tracks/track_59.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track60_Shader
    {
        samples[] = {{"data/sounds/tracks/track_60.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track61_Shader
    {
        samples[] = {{"data/sounds/tracks/track_61.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track62_Shader
    {
        samples[] = {{"data/sounds/tracks/track_62.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track63_Shader
    {
        samples[] = {{"data/sounds/tracks/track_63.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track64_Shader
    {
        samples[] = {{"data/sounds/tracks/track_64.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track65_Shader
    {
        samples[] = {{"data/sounds/tracks/track_65.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track66_Shader
    {
        samples[] = {{"data/sounds/tracks/track_66.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track67_Shader
    {
        samples[] = {{"data/sounds/tracks/track_67.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track68_Shader
    {
        samples[] = {{"data/sounds/tracks/track_68.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track69_Shader
    {
        samples[] = {{"data/sounds/tracks/track_69.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track70_Shader
    {
        samples[] = {{"data/sounds/tracks/track_70.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track71_Shader
    {
        samples[] = {{"data/sounds/tracks/track_71.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track72_Shader
    {
        samples[] = {{"data/sounds/tracks/track_72.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track73_Shader
    {
        samples[] = {{"data/sounds/tracks/track_73.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track74_Shader
    {
        samples[] = {{"data/sounds/tracks/track_74.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track75_Shader
    {
        samples[] = {{"data/sounds/tracks/track_75.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track76_Shader
    {
        samples[] = {{"data/sounds/tracks/track_76.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track77_Shader
    {
        samples[] = {{"data/sounds/tracks/track_77.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track78_Shader
    {
        samples[] = {{"data/sounds/tracks/track_78.ogg", 1}};
        range = 60;
        volume = 0.85;
    };
    class Unesennye_Track79_Shader
    {
        samples[] = {{"data/sounds/tracks/track_79.ogg", 1}};
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

    // ===== v1.2.0: слоты сетов под треки альбомов (Track06..Track79) =====
    class Unesennye_Track06_SoundSet
    {
        soundShaders[] = {"Unesennye_Track06_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track07_SoundSet
    {
        soundShaders[] = {"Unesennye_Track07_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track08_SoundSet
    {
        soundShaders[] = {"Unesennye_Track08_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track09_SoundSet
    {
        soundShaders[] = {"Unesennye_Track09_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track10_SoundSet
    {
        soundShaders[] = {"Unesennye_Track10_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track11_SoundSet
    {
        soundShaders[] = {"Unesennye_Track11_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track12_SoundSet
    {
        soundShaders[] = {"Unesennye_Track12_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track13_SoundSet
    {
        soundShaders[] = {"Unesennye_Track13_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track14_SoundSet
    {
        soundShaders[] = {"Unesennye_Track14_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track15_SoundSet
    {
        soundShaders[] = {"Unesennye_Track15_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track16_SoundSet
    {
        soundShaders[] = {"Unesennye_Track16_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track17_SoundSet
    {
        soundShaders[] = {"Unesennye_Track17_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track18_SoundSet
    {
        soundShaders[] = {"Unesennye_Track18_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track19_SoundSet
    {
        soundShaders[] = {"Unesennye_Track19_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track20_SoundSet
    {
        soundShaders[] = {"Unesennye_Track20_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track21_SoundSet
    {
        soundShaders[] = {"Unesennye_Track21_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track22_SoundSet
    {
        soundShaders[] = {"Unesennye_Track22_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track23_SoundSet
    {
        soundShaders[] = {"Unesennye_Track23_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track24_SoundSet
    {
        soundShaders[] = {"Unesennye_Track24_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track25_SoundSet
    {
        soundShaders[] = {"Unesennye_Track25_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track26_SoundSet
    {
        soundShaders[] = {"Unesennye_Track26_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track27_SoundSet
    {
        soundShaders[] = {"Unesennye_Track27_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track28_SoundSet
    {
        soundShaders[] = {"Unesennye_Track28_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track29_SoundSet
    {
        soundShaders[] = {"Unesennye_Track29_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track30_SoundSet
    {
        soundShaders[] = {"Unesennye_Track30_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track31_SoundSet
    {
        soundShaders[] = {"Unesennye_Track31_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track32_SoundSet
    {
        soundShaders[] = {"Unesennye_Track32_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track33_SoundSet
    {
        soundShaders[] = {"Unesennye_Track33_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track34_SoundSet
    {
        soundShaders[] = {"Unesennye_Track34_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track35_SoundSet
    {
        soundShaders[] = {"Unesennye_Track35_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track36_SoundSet
    {
        soundShaders[] = {"Unesennye_Track36_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track37_SoundSet
    {
        soundShaders[] = {"Unesennye_Track37_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track38_SoundSet
    {
        soundShaders[] = {"Unesennye_Track38_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track39_SoundSet
    {
        soundShaders[] = {"Unesennye_Track39_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track40_SoundSet
    {
        soundShaders[] = {"Unesennye_Track40_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track41_SoundSet
    {
        soundShaders[] = {"Unesennye_Track41_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track42_SoundSet
    {
        soundShaders[] = {"Unesennye_Track42_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track43_SoundSet
    {
        soundShaders[] = {"Unesennye_Track43_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track44_SoundSet
    {
        soundShaders[] = {"Unesennye_Track44_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track45_SoundSet
    {
        soundShaders[] = {"Unesennye_Track45_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track46_SoundSet
    {
        soundShaders[] = {"Unesennye_Track46_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track47_SoundSet
    {
        soundShaders[] = {"Unesennye_Track47_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track48_SoundSet
    {
        soundShaders[] = {"Unesennye_Track48_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track49_SoundSet
    {
        soundShaders[] = {"Unesennye_Track49_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track50_SoundSet
    {
        soundShaders[] = {"Unesennye_Track50_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track51_SoundSet
    {
        soundShaders[] = {"Unesennye_Track51_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track52_SoundSet
    {
        soundShaders[] = {"Unesennye_Track52_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track53_SoundSet
    {
        soundShaders[] = {"Unesennye_Track53_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track54_SoundSet
    {
        soundShaders[] = {"Unesennye_Track54_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track55_SoundSet
    {
        soundShaders[] = {"Unesennye_Track55_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track56_SoundSet
    {
        soundShaders[] = {"Unesennye_Track56_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track57_SoundSet
    {
        soundShaders[] = {"Unesennye_Track57_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track58_SoundSet
    {
        soundShaders[] = {"Unesennye_Track58_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track59_SoundSet
    {
        soundShaders[] = {"Unesennye_Track59_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track60_SoundSet
    {
        soundShaders[] = {"Unesennye_Track60_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track61_SoundSet
    {
        soundShaders[] = {"Unesennye_Track61_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track62_SoundSet
    {
        soundShaders[] = {"Unesennye_Track62_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track63_SoundSet
    {
        soundShaders[] = {"Unesennye_Track63_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track64_SoundSet
    {
        soundShaders[] = {"Unesennye_Track64_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track65_SoundSet
    {
        soundShaders[] = {"Unesennye_Track65_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track66_SoundSet
    {
        soundShaders[] = {"Unesennye_Track66_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track67_SoundSet
    {
        soundShaders[] = {"Unesennye_Track67_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track68_SoundSet
    {
        soundShaders[] = {"Unesennye_Track68_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track69_SoundSet
    {
        soundShaders[] = {"Unesennye_Track69_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track70_SoundSet
    {
        soundShaders[] = {"Unesennye_Track70_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track71_SoundSet
    {
        soundShaders[] = {"Unesennye_Track71_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track72_SoundSet
    {
        soundShaders[] = {"Unesennye_Track72_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track73_SoundSet
    {
        soundShaders[] = {"Unesennye_Track73_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track74_SoundSet
    {
        soundShaders[] = {"Unesennye_Track74_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track75_SoundSet
    {
        soundShaders[] = {"Unesennye_Track75_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track76_SoundSet
    {
        soundShaders[] = {"Unesennye_Track76_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track77_SoundSet
    {
        soundShaders[] = {"Unesennye_Track77_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track78_SoundSet
    {
        soundShaders[] = {"Unesennye_Track78_Shader"};
        spatial = 1;
        looped = 1;
        volumeFactor = 0.9;
        distanceFactor = 1.0;
    };
    class Unesennye_Track79_SoundSet
    {
        soundShaders[] = {"Unesennye_Track79_Shader"};
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

    // =================================================================
    // ДОБАВЛЕНО v1.2.0: МУЛЬТИ-ТРЕКОВЫЕ ФЛЕШКИ-АЛЬБОМЫ (10-20 треков).
    // Старые одно-трековые карточки выше НЕ удалены — обратная совместимость.
    // Метаданные unesennyeAlbumId/unesennyeMaxTracks читаются скриптами
    // (Unesennye_Album_System.c / Unesennye_Server_Album.c) через Config API.
    // =================================================================
    class UnesennyeMusicCard_Album : UnesennyeMusicCard_Base
    {
        displayName = "Unesennye Music Album Card";
        descriptionShort = "Хранит до 20 треков одного альбома. Переключение треков: используйте флешку рядом с рацией. Author: KRa Tos (Константин)";
        unesennyeAlbumId = "";      // базовый класс: альбом не назначен
        unesennyeMaxTracks = 0;
        unesennyeAuthor = "KRa Tos (Константин)"; // водяной знак предмета
    };

    // Альбом 1: «Русский Рок» (10 треков)
    class UnesennyeMusicCard_Album01 : UnesennyeMusicCard_Album
    {
        displayName = "Кассета: Русский Рок (10 треков)";
        descriptionShort = "Альбом «Русский Рок», 10 треков. Автор записи: KRa Tos (Константин)";
        unesennyeAlbumId = "Album01";
        unesennyeMaxTracks = 10;
        unesennyeAuthor = "KRa Tos (Константин)";
    };

    // Альбом 2: «Электроника» (15 треков)
    class UnesennyeMusicCard_Album02 : UnesennyeMusicCard_Album
    {
        displayName = "Флешка: Электроника (15 треков)";
        descriptionShort = "Альбом «Электроника», 15 треков. Автор записи: KRa Tos (Константин)";
        unesennyeAlbumId = "Album02";
        unesennyeMaxTracks = 15;
        unesennyeAuthor = "KRa Tos (Константин)";
    };

    // Альбом 3: «Сборник Unesennye» (20 треков)
    class UnesennyeMusicCard_Album03 : UnesennyeMusicCard_Album
    {
        displayName = "Флешка: Сборник Unesennye (20 треков)";
        descriptionShort = "Альбом «Сборник Unesennye», 20 треков. Автор записи: KRa Tos (Константин)";
        unesennyeAlbumId = "Album03";
        unesennyeMaxTracks = 20;
        unesennyeAuthor = "KRa Tos (Константин)";
    };
};

// =====================================================================
// ДОБАВЛЕНО v1.2.0: АЛЬБОМЫ (CfgUnesennyeAlbums)
// Один трек на флешке = TrackXX (1-based). Поле sample ССЫЛАЕТСЯ на файл
// из существующего CfgUnesennyeTracks — база треков НЕ дублируется.
// Глобальный RPC id трека = indexAlbuma*20 + localIndex (см. скрипты).
// Новый трек добавляется ТОЛЬКО правкой этого конфига + .ogg — без
// перекомпиляции скриптов.
// =====================================================================
class CfgUnesennyeAlbums
{
    class Album01 // «Русский Рок» — 10 треков
    {
        displayName = "Русский Рок";
        trackCount = 10;
        class Track01 { sample = "data/sounds/tracks/track_01.ogg"; title = "Кино - Перемен"; };
        class Track02 { sample = "data/sounds/tracks/track_02.ogg"; title = "ДДТ - Это всё"; };
        class Track03 { sample = "data/sounds/tracks/track_03.ogg"; title = "Ария - Штиль"; };
        class Track04 { sample = "data/sounds/tracks/track_04.ogg"; title = "Король и Шут - Кукла колдуна"; };
        class Track05 { sample = "data/sounds/tracks/track_05.ogg"; title = "Сплин - Выхода нет"; };
        class Track06 { sample = "data/sounds/tracks/album_01_06.ogg"; title = "Мумий Тролль - Владивосток 2000"; };
        class Track07 { sample = "data/sounds/tracks/album_01_07.ogg"; title = "Nautilus Pompilius - Крылья"; };
        class Track08 { sample = "data/sounds/tracks/album_01_08.ogg"; title = "Би-2 - Полки"; };
        class Track09 { sample = "data/sounds/tracks/album_01_09.ogg"; title = "Земфира - Искала"; };
        class Track10 { sample = "data/sounds/tracks/album_01_10.ogg"; title = "Агата Кристи - Как на войне"; };
    };
    class Album02 // «Электроника» — 15 треков
    {
        displayName = "Электроника";
        trackCount = 15;
        class Track01 { sample = "data/sounds/tracks/album_02_01.ogg"; title = "Vox Samana - Signal"; };
        class Track02 { sample = "data/sounds/tracks/album_02_02.ogg"; title = "Neon Drive - Midnight Run"; };
        class Track03 { sample = "data/sounds/tracks/album_02_03.ogg"; title = "Pulse Array - Core"; };
        class Track04 { sample = "data/sounds/tracks/album_02_04.ogg"; title = "Iono - Drift"; };
        class Track05 { sample = "data/sounds/tracks/album_02_05.ogg"; title = "Circuit Bloom - Fade"; };
        class Track06 { sample = "data/sounds/tracks/album_02_06.ogg"; title = "Vector Sky - Ascent"; };
        class Track07 { sample = "data/sounds/tracks/album_02_07.ogg"; title = "Lumen - Grid"; };
        class Track08 { sample = "data/sounds/tracks/album_02_08.ogg"; title = "Phase Six - Echo Chamber"; };
        class Track09 { sample = "data/sounds/tracks/album_02_09.ogg"; title = "Static Field - Rain"; };
        class Track10 { sample = "data/sounds/tracks/album_02_10.ogg"; title = "Modular Heart - Beat"; };
        class Track11 { sample = "data/sounds/tracks/album_02_11.ogg"; title = "Aurora Bus - Nightline"; };
        class Track12 { sample = "data/sounds/tracks/album_02_12.ogg"; title = "Kelvin - Cold Start"; };
        class Track13 { sample = "data/sounds/tracks/album_02_13.ogg"; title = "Oscilla - Waveform"; };
        class Track14 { sample = "data/sounds/tracks/album_02_14.ogg"; title = "Datamoth - Swarm"; };
        class Track15 { sample = "data/sounds/tracks/album_02_15.ogg"; title = "Null Sector - Horizon"; };
    };
    class Album03 // «Сборник Unesennye» — 20 треков
    {
        displayName = "Сборник Unesennye";
        trackCount = 20;
        class Track01 { sample = "data/sounds/tracks/track_01.ogg"; title = "Сборник #1 (кросс-трек)"; };
        class Track02 { sample = "data/sounds/tracks/track_02.ogg"; title = "Сборник #2 (кросс-трек)"; };
        class Track03 { sample = "data/sounds/tracks/track_03.ogg"; title = "Сборник #3 (кросс-трек)"; };
        class Track04 { sample = "data/sounds/tracks/track_04.ogg"; title = "Сборник #4 (кросс-трек)"; };
        class Track05 { sample = "data/sounds/tracks/track_05.ogg"; title = "Сборник #5 (кросс-трек)"; };
        class Track06 { sample = "data/sounds/tracks/album_03_06.ogg"; title = "Сборник #6"; };
        class Track07 { sample = "data/sounds/tracks/album_03_07.ogg"; title = "Сборник #7"; };
        class Track08 { sample = "data/sounds/tracks/album_03_08.ogg"; title = "Сборник #8"; };
        class Track09 { sample = "data/sounds/tracks/album_03_09.ogg"; title = "Сборник #9"; };
        class Track10 { sample = "data/sounds/tracks/album_03_10.ogg"; title = "Сборник #10"; };
        class Track11 { sample = "data/sounds/tracks/album_03_11.ogg"; title = "Сборник #11"; };
        class Track12 { sample = "data/sounds/tracks/album_03_12.ogg"; title = "Сборник #12"; };
        class Track13 { sample = "data/sounds/tracks/album_03_13.ogg"; title = "Сборник #13"; };
        class Track14 { sample = "data/sounds/tracks/album_03_14.ogg"; title = "Сборник #14"; };
        class Track15 { sample = "data/sounds/tracks/album_03_15.ogg"; title = "Сборник #15"; };
        class Track16 { sample = "data/sounds/tracks/album_03_16.ogg"; title = "Сборник #16"; };
        class Track17 { sample = "data/sounds/tracks/album_03_17.ogg"; title = "Сборник #17"; };
        class Track18 { sample = "data/sounds/tracks/album_03_18.ogg"; title = "Сборник #18"; };
        class Track19 { sample = "data/sounds/tracks/album_03_19.ogg"; title = "Сборник #19"; };
        class Track20 { sample = "data/sounds/tracks/album_03_20.ogg"; title = "Сборник #20"; };
    };
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

    // ===== ДОБАВЛЕНО v1.2.0: слоты реестра для треков альбомов =====
    // Инвариант скриптов: globalID = albumIndex*UNSENNYE_ALBUM_MAX_TRACKS(40) + localIndex
    //   Album01 -> track_20..29 | Album02 -> track_40..54 | Album03 -> track_60..79
    // Слоты 0..4 (старые одно-трековые флешки) НЕ изменены — обратная совместимость.
    // Для записей без готового SoundSet клиент генерирует шейдер/сет через ConfigAddClass.
    class track_20
    {
        title = "Русский Рок — трек 1 (кросс-запись)";
        file = "data/sounds/tracks/track_01.ogg";
        soundSet = "Unesennye_Track21_SoundSet";
    };
    class track_21
    {
        title = "Русский Рок — трек 2 (кросс-запись)";
        file = "data/sounds/tracks/track_02.ogg";
        soundSet = "Unesennye_Track22_SoundSet";
    };
    class track_22
    {
        title = "Русский Рок — трек 3 (кросс-запись)";
        file = "data/sounds/tracks/track_03.ogg";
        soundSet = "Unesennye_Track23_SoundSet";
    };
    class track_23
    {
        title = "Русский Рок — трек 4 (кросс-запись)";
        file = "data/sounds/tracks/track_04.ogg";
        soundSet = "Unesennye_Track24_SoundSet";
    };
    class track_24
    {
        title = "Русский Рок — трек 5 (кросс-запись)";
        file = "data/sounds/tracks/track_05.ogg";
        soundSet = "Unesennye_Track25_SoundSet";
    };
    class track_25
    {
        title = "Русский Рок — трек 6";
        file = "data/sounds/tracks/album_01_06.ogg";
        soundSet = "Unesennye_Track26_SoundSet";
    };
    class track_26
    {
        title = "Русский Рок — трек 7";
        file = "data/sounds/tracks/album_01_07.ogg";
        soundSet = "Unesennye_Track27_SoundSet";
    };
    class track_27
    {
        title = "Русский Рок — трек 8";
        file = "data/sounds/tracks/album_01_08.ogg";
        soundSet = "Unesennye_Track28_SoundSet";
    };
    class track_28
    {
        title = "Русский Рок — трек 9";
        file = "data/sounds/tracks/album_01_09.ogg";
        soundSet = "Unesennye_Track29_SoundSet";
    };
    class track_29
    {
        title = "Русский Рок — трек 10";
        file = "data/sounds/tracks/album_01_10.ogg";
        soundSet = "Unesennye_Track30_SoundSet";
    };
    class track_40
    {
        title = "Электроника — трек 1";
        file = "data/sounds/tracks/album_02_01.ogg";
        soundSet = "Unesennye_Track41_SoundSet";
    };
    class track_41
    {
        title = "Электроника — трек 2";
        file = "data/sounds/tracks/album_02_02.ogg";
        soundSet = "Unesennye_Track42_SoundSet";
    };
    class track_42
    {
        title = "Электроника — трек 3";
        file = "data/sounds/tracks/album_02_03.ogg";
        soundSet = "Unesennye_Track43_SoundSet";
    };
    class track_43
    {
        title = "Электроника — трек 4";
        file = "data/sounds/tracks/album_02_04.ogg";
        soundSet = "Unesennye_Track44_SoundSet";
    };
    class track_44
    {
        title = "Электроника — трек 5";
        file = "data/sounds/tracks/album_02_05.ogg";
        soundSet = "Unesennye_Track45_SoundSet";
    };
    class track_45
    {
        title = "Электроника — трек 6";
        file = "data/sounds/tracks/album_02_06.ogg";
        soundSet = "Unesennye_Track46_SoundSet";
    };
    class track_46
    {
        title = "Электроника — трек 7";
        file = "data/sounds/tracks/album_02_07.ogg";
        soundSet = "Unesennye_Track47_SoundSet";
    };
    class track_47
    {
        title = "Электроника — трек 8";
        file = "data/sounds/tracks/album_02_08.ogg";
        soundSet = "Unesennye_Track48_SoundSet";
    };
    class track_48
    {
        title = "Электроника — трек 9";
        file = "data/sounds/tracks/album_02_09.ogg";
        soundSet = "Unesennye_Track49_SoundSet";
    };
    class track_49
    {
        title = "Электроника — трек 10";
        file = "data/sounds/tracks/album_02_10.ogg";
        soundSet = "Unesennye_Track50_SoundSet";
    };
    class track_50
    {
        title = "Электроника — трек 11";
        file = "data/sounds/tracks/album_02_11.ogg";
        soundSet = "Unesennye_Track51_SoundSet";
    };
    class track_51
    {
        title = "Электроника — трек 12";
        file = "data/sounds/tracks/album_02_12.ogg";
        soundSet = "Unesennye_Track52_SoundSet";
    };
    class track_52
    {
        title = "Электроника — трек 13";
        file = "data/sounds/tracks/album_02_13.ogg";
        soundSet = "Unesennye_Track53_SoundSet";
    };
    class track_53
    {
        title = "Электроника — трек 14";
        file = "data/sounds/tracks/album_02_14.ogg";
        soundSet = "Unesennye_Track54_SoundSet";
    };
    class track_54
    {
        title = "Электроника — трек 15";
        file = "data/sounds/tracks/album_02_15.ogg";
        soundSet = "Unesennye_Track55_SoundSet";
    };
    class track_60
    {
        title = "Сборник Unesennye — трек 1 (кросс-запись)";
        file = "data/sounds/tracks/track_01.ogg";
        soundSet = "Unesennye_Track61_SoundSet";
    };
    class track_61
    {
        title = "Сборник Unesennye — трек 2 (кросс-запись)";
        file = "data/sounds/tracks/track_02.ogg";
        soundSet = "Unesennye_Track62_SoundSet";
    };
    class track_62
    {
        title = "Сборник Unesennye — трек 3 (кросс-запись)";
        file = "data/sounds/tracks/track_03.ogg";
        soundSet = "Unesennye_Track63_SoundSet";
    };
    class track_63
    {
        title = "Сборник Unesennye — трек 4 (кросс-запись)";
        file = "data/sounds/tracks/track_04.ogg";
        soundSet = "Unesennye_Track64_SoundSet";
    };
    class track_64
    {
        title = "Сборник Unesennye — трек 5 (кросс-запись)";
        file = "data/sounds/tracks/track_05.ogg";
        soundSet = "Unesennye_Track65_SoundSet";
    };
    class track_65
    {
        title = "Сборник Unesennye — трек 6";
        file = "data/sounds/tracks/album_03_06.ogg";
        soundSet = "Unesennye_Track66_SoundSet";
    };
    class track_66
    {
        title = "Сборник Unesennye — трек 7";
        file = "data/sounds/tracks/album_03_07.ogg";
        soundSet = "Unesennye_Track67_SoundSet";
    };
    class track_67
    {
        title = "Сборник Unesennye — трек 8";
        file = "data/sounds/tracks/album_03_08.ogg";
        soundSet = "Unesennye_Track68_SoundSet";
    };
    class track_68
    {
        title = "Сборник Unesennye — трек 9";
        file = "data/sounds/tracks/album_03_09.ogg";
        soundSet = "Unesennye_Track69_SoundSet";
    };
    class track_69
    {
        title = "Сборник Unesennye — трек 10";
        file = "data/sounds/tracks/album_03_10.ogg";
        soundSet = "Unesennye_Track70_SoundSet";
    };
    class track_70
    {
        title = "Сборник Unesennye — трек 11";
        file = "data/sounds/tracks/album_03_11.ogg";
        soundSet = "Unesennye_Track71_SoundSet";
    };
    class track_71
    {
        title = "Сборник Unesennye — трек 12";
        file = "data/sounds/tracks/album_03_12.ogg";
        soundSet = "Unesennye_Track72_SoundSet";
    };
    class track_72
    {
        title = "Сборник Unesennye — трек 13";
        file = "data/sounds/tracks/album_03_13.ogg";
        soundSet = "Unesennye_Track73_SoundSet";
    };
    class track_73
    {
        title = "Сборник Unesennye — трек 14";
        file = "data/sounds/tracks/album_03_14.ogg";
        soundSet = "Unesennye_Track74_SoundSet";
    };
    class track_74
    {
        title = "Сборник Unesennye — трек 15";
        file = "data/sounds/tracks/album_03_15.ogg";
        soundSet = "Unesennye_Track75_SoundSet";
    };
    class track_75
    {
        title = "Сборник Unesennye — трек 16";
        file = "data/sounds/tracks/album_03_16.ogg";
        soundSet = "Unesennye_Track76_SoundSet";
    };
    class track_76
    {
        title = "Сборник Unesennye — трек 17";
        file = "data/sounds/tracks/album_03_17.ogg";
        soundSet = "Unesennye_Track77_SoundSet";
    };
    class track_77
    {
        title = "Сборник Unesennye — трек 18";
        file = "data/sounds/tracks/album_03_18.ogg";
        soundSet = "Unesennye_Track78_SoundSet";
    };
    class track_78
    {
        title = "Сборник Unesennye — трек 19";
        file = "data/sounds/tracks/album_03_19.ogg";
        soundSet = "Unesennye_Track79_SoundSet";
    };
    class track_79
    {
        title = "Сборник Unesennye — трек 20";
        file = "data/sounds/tracks/album_03_20.ogg";
        soundSet = "Unesennye_Track80_SoundSet";
    };
};
