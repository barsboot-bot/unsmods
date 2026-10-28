class CfgPatches
{
    class unesennye_client
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 1.24;
        requiredAddons[] = {"DZ_Data", "DZ_Scripts", "unesennye_music_db"};
    };
};

class CfgMods
{
    class unesennye_client
    {
        dir = "unesennye";
        name = "Unesennye v2 (client)";
        type = "mod";
        dependencies[] = {"Game", "World", "Mission"};
        class defs
        {
            class gameScriptModule { value = ""; files[] = {"unesennye/scripts/3_Game"}; };
            class worldScriptModule { value = ""; files[] = {"unesennye/scripts/4_World"}; };
        };
    };
};
