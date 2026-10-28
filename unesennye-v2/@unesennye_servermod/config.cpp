class CfgPatches
{
    class unesennye_servermod
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 1.24;
        requiredAddons[] = {"DZ_Data", "DZ_Scripts", "unesennye_music_db"};
    };
};

class CfgMods
{
    class unesennye_servermod
    {
        dir = "unesennye_servermod";
        name = "Unesennye v2 (server)";
        type = "mod";
        dependencies[] = {"Game", "World", "Mission"};
        class defs
        {
            class gameScriptModule { value = ""; files[] = {"unesennye_servermod/scripts/3_Game"}; };
            class worldScriptModule { value = ""; files[] = {"unesennye_servermod/scripts/4_World"}; };
        };
    };
};
