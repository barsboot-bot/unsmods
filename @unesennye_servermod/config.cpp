// Author: KRa Tos (Константин) | Project: Unesennye
class CfgPatches
{
    class Unesennye_ServerMod
    {
        author      = "KRa Tos (Константин)";
        name        = "Unesennye Server Mod";
        version     = "1.0.0";
        requiredAddons[] = {"DayZ_Core"};
        requiredVersion = 1.24;
        units[]     = {};
        weapons[]   = {};
    };
};

class CfgMods
{
    class Unesennye_ServerMod
    {
        id          = "unesennye_servermod";
        dir         = "@unesennye_servermod";
        name        = "Unesennye Music [SERVER]";
        author      = "KRa Tos (Константин)";
        version     = "1.0.0";
        type        = "server";
        hideName    = 0;
        hideIcon    = 0;
        action      = "";
    };
};
