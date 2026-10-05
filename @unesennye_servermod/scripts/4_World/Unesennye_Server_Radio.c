// Author: KRa Tos (Константин) | Project: Unesennye
// =====================================================================
// UNSENNYE MUSIC SYSTEM — СЕРВЕРНАЯ ЧАСТЬ РАЦИЙ (v1.1.0, расширение)
// НОВЫЙ файл: существующая серверная система машин
// (Unesennye_Server_System.c) НЕ изменена. RPC ID 100-204 не тронуты.
// Валидация команд раций: авторизация Handshake + существование трека
// в CfgUnesennyeTracks + близость игрока к рации + анти-спам кулдаун 5 с.
// Автор: KRa Tos (Константин)
// =====================================================================

// ===== RPC-протокол раций: НОВЫЕ ID с 300 (идентично клиенту) =====
enum UnesennyeRadioRPC
{
    RADIO_INSERT_CARD = 300,
    RADIO_EJECT_CARD = 301,
    RADIO_PLAY_TRACK = 302,
    RADIO_STOP_TRACK = 303,
    RADIO_BROADCAST  = 304,
    RADIO_BROADCAST_STOP = 305
};

static const string UNSENNYE_RADIO_AUTHOR_NAME = "KRa Tos (Константин)"; // водяной знак

// ---------- Запись о состоянии рации у игрока ----------
class UnesennyeRadioSession
{
    int playerID;       // сетевой ID игрока-владельца сессии
    int radioID;        // сетевой ID рации
    int insertedTrack;  // какой трек «во флешке» (-1 = пусто)
    int lastCmdMs;      // анти-спам кулдаун

    void UnesennyeRadioSession()
    {
        playerID = 0;
        radioID = 0;
        insertedTrack = -1;
        lastCmdMs = 0;
    }
}

// ---------- Серверный модуль раций ----------
class UnesennyeServerRadioModule
{
    private ref array<ref UnesennyeRadioSession> m_Sessions;
    private const int RADIO_COOLDOWN_MS = 5000; // анти-спам: 5 секунд на включение трека

    void UnesennyeServerRadioModule()
    {
        m_Sessions = new array<ref UnesennyeRadioSession>;
    }

    void ~UnesennyeServerRadioModule()
    {
        for (int i = 0; i < m_Sessions.Count(); i++)
            delete m_Sessions[i];
        m_Sessions.Clear();
        delete m_Sessions;
        m_Sessions = null;
    }

    // Инициализация: регистрация SERVER RPC по числовым ID (300-303)
    void Init()
    {
        GetGame().RegisterServerRpc(UnesennyeRadioRPC.RADIO_INSERT_CARD, UnesennyeServerRadioModule, "OnInsertCard");
        GetGame().RegisterServerRpc(UnesennyeRadioRPC.RADIO_EJECT_CARD,  UnesennyeServerRadioModule, "OnEjectCard");
        GetGame().RegisterServerRpc(UnesennyeRadioRPC.RADIO_PLAY_TRACK,  UnesennyeServerRadioModule, "OnPlayTrack");
        GetGame().RegisterServerRpc(UnesennyeRadioRPC.RADIO_STOP_TRACK,  UnesennyeServerRadioModule, "OnStopTrack");

        Print(string.Format("[Unesennye Radio] Server module ACTIVE (RPC 300-305). Author: %s", UNSENNYE_RADIO_AUTHOR_NAME));
    }

    // ----- Сессии -----
    private UnesennyeRadioSession GetOrCreateSession(int senderID)
    {
        for (int i = 0; i < m_Sessions.Count(); i++)
        {
            if (m_Sessions[i].playerID == senderID) return m_Sessions[i];
        }
        UnesennyeRadioSession s = new UnesennyeRadioSession;
        s.playerID = senderID;
        m_Sessions.Insert(s);
        return s;
    }

    // Анти-спам: возвращает false, если команда раньше чем через 5 с после предыдущей
    private bool CheckCooldown(int senderID)
    {
        UnesennyeRadioSession s = GetOrCreateSession(senderID);
        int now = MathFloor(GetGame().GetTime()); // GetTime() уже в мс
        if (now - s.lastCmdMs < RADIO_COOLDOWN_MS)
        {
            Print(string.Format("[Unesennye Radio] Anti-spam: player %d throttled.", senderID));
            return false;
        }
        s.lastCmdMs = now;
        return true;
    }

    // Трек существует в базе @unesennye_music_db?
    private bool TrackExists(int trackID)
    {
        string clsName = ConfigGetClassName(trackID, "CfgUnesennyeTracks");
        if (clsName == "") return false;
        return ConfigReadString(clsName + "\\file", "").Length() > 0;
    }

    // Игрок рядом с рацией? (GetObjectsAtPosition — стандартный движковый API)
    private bool IsPlayerNearRadio(Man player, IEntity radio)
    {
        vector pos = player.GetPosition();
        vector rpos = radio.GetPosition();
        if (vector.DistanceSq(pos, rpos) > 64.0) return false; // дальше 8 м — не его рация

        ref array<IEntity> items = new array<IEntity>;
        GetGame().GetObjectsAtPosition(rpos, 3.0, 3.0, 3.0, items, null, null);
        for (int i = 0; i < items.Count(); i++)
        {
            if (items[i] == player) return true;
        }
        return false;
    }

    // Общая проверка: авторизация Handshake + игрок + рация рядом
    private Man ValidateCommon(RPCParamContext context, int radioID, out IEntity radioOut)
    {
        radioOut = null;
        int senderID = context.GetSenderID();

        // КЛЮЧЕВАЯ ЗАЩИТА: без успешного handshake (таймаут 4000 мс) — молча игнор
        if (!UnesennyeServerRPC.g_UnesennyeServer || !UnesennyeServerRPC.g_UnesennyeServer.IsPlayerAuthorized(senderID)) return null;

        Man player = GetGame().GetPlayerByID(senderID);
        if (!player) return null;

        IEntity radioEnt = GetGame().FindEntity(radioID);
        if (!radioEnt || !Cast<RadioBase>(radioEnt)) return null;
        if (!IsPlayerNearRadio(player, radioEnt)) return null;

        radioOut = radioEnt;
        return player;
    }

    // ===== 300: вставить флешку =====
    void OnInsertCard(RPCParamContext context, ParamsReadContext buf)
    {
        Param2<int, int> p;
        if (!buf.ReadObject(p)) return;
        int radioID = p.arg1;
        int trackID = p.arg2;

        IEntity radioEnt;
        if (!ValidateCommon(context, radioID, radioEnt)) return;
        if (!CheckCooldown(context.GetSenderID())) return;
        if (!TrackExists(trackID)) return;

        UnesennyeRadioSession s = GetOrCreateSession(context.GetSenderID());
        s.radioID = radioID;
        s.insertedTrack = trackID;

        Print(string.Format("[Unesennye Radio] Card insert OK radio=%d track=%d player=%d | Author: %s",
            radioID, trackID, context.GetSenderID(), UNSENNYE_RADIO_AUTHOR_NAME));
    }

    // ===== 301: извлечь флешку =====
    void OnEjectCard(RPCParamContext context, ParamsReadContext buf)
    {
        Param1<int> p;
        if (!buf.ReadObject(p)) return;
        int radioID = p.param;

        IEntity radioEnt;
        if (!ValidateCommon(context, radioID, radioEnt)) return;

        UnesennyeRadioSession s = GetOrCreateSession(context.GetSenderID());
        s.insertedTrack = -1;

        // Останавливаем звучание у всех клиентов (broadcast stop = 305)
        GetGame().RPCSingleParam(context.GetSenderID(), UnesennyeRadioRPC.RADIO_BROADCAST_STOP,
            new Param1<int>(radioID), RPCTargetGroup.All);
    }

    // ===== 302: включить трек =====
    void OnPlayTrack(RPCParamContext context, ParamsReadContext buf)
    {
        Param2<int, int> p;
        if (!buf.ReadObject(p)) return;
        int radioID = p.arg1;
        int trackID = p.arg2;

        // 1) Авторизация handshake + игрок + рация рядом
        IEntity radioEnt;
        if (!ValidateCommon(context, radioID, radioEnt)) return;

        // 2) Анти-спам: кулдаун 5 секунд на включение трека
        if (!CheckCooldown(context.GetSenderID())) return;

        // 3) Трек существует в CfgUnesennyeTracks
        if (!TrackExists(trackID)) return;

        // 4) Флешка с этим треком реально вставлена
        UnesennyeRadioSession s = GetOrCreateSession(context.GetSenderID());
        if (s.insertedTrack != trackID) return;

        // Ретрансляция всем клиентам (вещание от позиции рации)
        GetGame().RPCSingleParam(context.GetSenderID(), UnesennyeRadioRPC.RADIO_BROADCAST,
            new Param2<int, int>(radioID, trackID), RPCTargetGroup.All);

        Print(string.Format("[Unesennye Radio] PLAY OK radio=%d track=%d by player=%d | Author: %s",
            radioID, trackID, context.GetSenderID(), UNSENNYE_RADIO_AUTHOR_NAME));
    }

    // Удаление сессии игрока (выход/деспаун)
    void RemovePlayer(int playerID)
    {
        for (int i = 0; i < m_Sessions.Count(); i++)
        {
            if (m_Sessions[i].playerID == playerID)
            {
                delete m_Sessions[i];
                m_Sessions.Remove(i);
                return;
            }
        }
    }

    // ===== 303: выключить трек (рассылка остановки всем) =====
    void OnStopTrack(RPCParamContext context, ParamsReadContext buf)
    {
        Param1<int> p;
        if (!buf.ReadObject(p)) return;
        int radioID = p.param;

        IEntity radioEnt;
        if (!ValidateCommon(context, radioID, radioEnt)) return;

        // Клиенты вызывают StopTrackFromRadio (broadcast stop = 305)
        GetGame().RPCSingleParam(context.GetSenderID(), UnesennyeRadioRPC.RADIO_BROADCAST_STOP,
            new Param1<int>(radioID), RPCTargetGroup.All);
    }
};

// ---------- Проверка авторизации (переиспользуем существующий реестр handshake) ----------
// UnesennyeServerSystem.IsPlayerAuthorized() добавлена в v1.1.0 как геттер;
// логика 100-204 не изменена. Без @unesennye_servermod этот код не выполняется вовсе.

// ---------- Диспетчер-адаптер для RegisterServerRpc ----------
class UnesennyeRadioRPCServer
{
    static ref UnesennyeServerRadioModule g_RadioModule;

    static void Init()
    {
        if (!g_RadioModule)
            g_RadioModule = new UnesennyeServerRadioModule;
        g_RadioModule.Init();
    }

    static void OnInsertCard(RPCParamContext context, ParamsReadContext buf)
    {
        if (g_RadioModule) g_RadioModule.OnInsertCard(context, buf);
    }

    static void OnEjectCard(RPCParamContext context, ParamsReadContext buf)
    {
        if (g_RadioModule) g_RadioModule.OnEjectCard(context, buf);
    }

    static void OnPlayTrack(RPCParamContext context, ParamsReadContext buf)
    {
        if (g_RadioModule) g_RadioModule.OnPlayTrack(context, buf);
    }

    static void OnStopTrack(RPCParamContext context, ParamsReadContext buf)
    {
        if (g_RadioModule) g_RadioModule.OnStopTrack(context, buf);
    }
};

// ---------- Очистка сессий при выходе игрока (без утечек) ----------
modded class PlayerBase
{
    override void OnBaseDestroyed()
    {
        super.OnBaseDestroyed();
        if (UnesennyeRadioRPCServer.g_RadioModule)
        {
            UnesennyeRadioRPCServer.g_RadioModule.RemovePlayer(GetID());
        }
    }
}

// ---------- Точка старта: расширяем MissionServer БЕЗ правки Server_System.c ----------
modded class MissionServer
{
    override void OnInit()
    {
        super.OnInit(); // вызывает и существующую инициализацию авто-системы тоже
        UnesennyeRadioRPCServer.Init();
    }

    override void OnMissionFinish()
    {
        super.OnMissionFinish();
        if (UnesennyeRadioRPCServer.g_RadioModule)
        {
            delete UnesennyeRadioRPCServer.g_RadioModule;
            UnesennyeRadioRPCServer.g_RadioModule = null;
        }
    }
}
