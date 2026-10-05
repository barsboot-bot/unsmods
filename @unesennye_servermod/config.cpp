// Author: KRa Tos (Константин) | Project: Unesennye
class CfgPatches
{
    class unesennye_servermod
    {
        name = "Unesennye Server Mod";
        author = "KRa Tos (Константин)";
        version = "1.0.0";
        url = "";
        requiredVersion = 1.24;
        requiredAddons[] = {"DZ_Data", "DZ_Scripts"};
    };
};

class CfgMods
{
    class unesennye_servermod
    {
        name = "Unesennye Music System - SERVER v1.0.0";
        dir = "unesennye_servermod";
        action = "";
        hideName = 0;
        hideIcon = 1;
        version = "1.0.0";
        author = "KRa Tos (Константин)";
        authorID = "KRaTos";
        description = "Серверный мод автомобильного радио. Ключ активации клиента @unesennye.";
    };
};

// Структура scripts/ (1_Core, 4_World) подхватывается стандартным
// конфигом DZ_Scripts в DayZ Workbench — отдельный CfgScriptConfigs не требуется.
// Файлы компилируются в порядке слоёв: 1_Core/init.c -> 4_World/*.c
