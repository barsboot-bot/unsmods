// Author: KRa Tos (Константин) | Project: Unesennye
// =====================================================================
// UNSENNYE MUSIC SYSTEM — СЕРВЕРНАЯ ЧАСТЬ (v1.0.0)
// Регистрация RPC по ID (enum), handshake-авторизация, валидация
// радио-команд, rate-limit, скрытая метка автора GetModAuthor().
// Автор: KRa Tos (Константин)
// =====================================================================

// ---------- Скрытая метка авторства ----------
static string GetModAuthor()
{
    return UNSENNYE_AUTHOR_NAME; // "KRa Tos (Константин)"
}

// ---------- Запись об игроке ----------
class UnesennyeAuthEntry
{
    int  playerID;         // сетевой ID игрока
    int  tokenA;           // секрет handshake A
    int  tokenB;           // секрет handshake B (производный)
    bool authorized;
    int  lastRadioCmdMs;   // таймстемп последней радио-команды (rate-limit)

    void UnesennyeAuthEntry()
    {
        playerID = 0;
        tokenA = 0;
        tokenB = 0;
        authorized = false;
        lastRadioCmdMs = 0;
    }
}

// ---------- Главная серверная система ----------
class UnesennyeServerSystem
{
    ref array<ref UnesennyeAuthEntry> m_AuthEntries;
    private int m_NonceCounter; // резерв (не используется в детерминированной схеме)
    private const int RATE_LIMIT_MS = 500; // мин. интервал радио-команд на игрока

    void UnesennyeServerSystem()
    {
        m_AuthEntries = new array<ref UnesennyeAuthEntry>;
        m_NonceCounter = 0;
    }

    void ~UnesennyeServerSystem()
    {
        // Явная очистка ссылок — без утечек памяти при рестарте миссии
        if (m_AuthEntries)
        {
            for (int i = 0; i < m_AuthEntries.Count(); i++)
            {
                delete m_AuthEntries[i];
            }
            m_AuthEntries.Clear();
            delete m_AuthEntries;
            m_AuthEntries = null;
        }
    }

    // ===== Инициализация: регистрация RPC + водяной знак в RPT =====
    void Init()
    {
        Print("[Unesennye] Server system initializing...");

        // Регистрация SERVER RPC по числовым ID из enum UnesennyeRPC
        // (авто-регистрация по имени НЕ используется)
        GetGame().RegisterServerRpc(UnesennyeRPC.HS_REQUEST,     "UnesennyeServerRPC", "OnHandshakeRequest");
        GetGame().RegisterServerRpc(UnesennyeRPC.TRACK_LIST_REQ, "UnesennyeServerRPC", "OnTrackListRequest");
        GetGame().RegisterServerRpc(UnesennyeRPC.RADIO_PLAY,     "UnesennyeServerRPC", "OnRadioPlay");
        GetGame().RegisterServerRpc(UnesennyeRPC.RADIO_STOP,     "UnesennyeServerRPC", "OnRadioStop");

        Print("[Unesennye] RPC handlers registered: HS_REQUEST(100), TRACK_LIST_REQ(101), RADIO_PLAY(102), RADIO_STOP(103)");

        // Водяной знак в RPT — админ видит автора даже при скрытом UI
        Print(string.Format("[Unesennye_ServerMod v%d.%d.%d] ACTIVE. Author: %s | Project: %s",
            UNSENNYE_SERVER_VERSION / 100,
            (UNSENNYE_SERVER_VERSION / 10) % 10,
            UNSENNYE_SERVER_VERSION % 10,
            GetModAuthor(), UNSENNYE_PROJECT_NAME));
    }

    // ===== Работа со списком авторизации =====
    private UnesennyeAuthEntry GetOrCreateEntry(int playerID)
    {
        foreach (UnesennyeAuthEntry entry : m_AuthEntries)
        {
            if (entry.playerID == playerID) return entry;
        }
        UnesennyeAuthEntry e = new UnesennyeAuthEntry;
        e.playerID = playerID;
        m_AuthEntries.Insert(e);
        return e;
    }

    private UnesennyeAuthEntry FindAuthorized(int playerID)
    {
        foreach (UnesennyeAuthEntry entry : m_AuthEntries)
        {
            if (entry.playerID == playerID && entry.authorized) return entry;
        }
        return null;
    }

    void RemoveEntry(int playerID)
    {
        for (int i = 0; i < m_AuthEntries.Count(); i++)
        {
            if (m_AuthEntries[i].playerID == playerID)
            {
                delete m_AuthEntries[i];
                m_AuthEntries.Remove(i);
                return;
            }
        }
    }

    // Нелинейная смесь для challenge-response токенов
    private int MakeToken(int seed, int nonce)
    {
        int mixed = (seed * 2654435761) ^ (nonce * 40503) ^ (UNSENNYE_PROTO_VERSION * 99991);
        return mixed & 0x7FFFFFFF;
    }

    private int NowMs()
    {
        return MathFloor(GetGame().GetTime() * 0.001);
    }

    // ===== HANDSHAKE: если код здесь — серверный мод установлен =====
    void HandleHandshake(RPCParamContext context, ParamReadBuffer buf)
    {
        int clientId = buf.ReadInt();
        int protoVer = buf.ReadInt();
        int senderID = context.GetSenderID();

        UnesennyeAuthEntry e = GetOrCreateEntry(senderID);
        if (protoVer != UNSENNYE_PROTO_VERSION)
        {
            // Несовместимая версия протокола — вежливый отказ без краша
            ParamWriteBuffer fail = new ParamWriteBuffer;
            fail.WriteInt(clientId);
            fail.WriteInt(2); // reason: version mismatch
            GetGame().RPCSingleParam(senderID, UnesennyeRPC.AUTH_FAIL, fail, RPCTargetGroup.Self);
            return;
        }

        // Детерминированный nonce: клиент может пересчитать tokenA для сверки образа
        int nonce = clientId + UNSENNYE_PROTO_VERSION;
        e.tokenA = MakeToken(clientId, nonce);
        e.tokenB = MakeToken(e.tokenA, clientId ^ 0xA5A5);
        e.authorized = true;

        // Адресный ответ клиенту: (clientId, tokenA, tokenB)
        ParamWriteBuffer resp = new ParamWriteBuffer;
        resp.WriteInt(clientId);
        resp.WriteInt(e.tokenA);
        resp.WriteInt(e.tokenB);
        GetGame().RPCSingleParam(senderID, UnesennyeRPC.HS_RESPONSE, resp, RPCTargetGroup.Self);

        Print(string.Format("[Unesennye] Auth OK playerID=%d client=%d | Author: %s",
            senderID, clientId, GetModAuthor()));
    }

    // ===== СПИСОК ТРЕКОВ: читается из конфига @unesennye_music_db =====
    void HandleTrackListRequest(RPCParamContext context, ParamReadBuffer buf)
    {
        int senderID = context.GetSenderID();
        if (!FindAuthorized(senderID)) return; // не авторизован — молча игнор (без краша)

        int clientId = buf.ReadInt();

        // Формат ответа: clientId, count, [trackID, title]*count
        ref array<int>    ids    = new array<int>;
        ref array<string> titles = new array<string>;

        Man player = GetGame().GetPlayerByID(senderID);
        if (player)
        {
            TNewScriptDataContext ctx = TNewScriptDataContext.Cast(
                GetGame().CreateContext(TNewScriptDataContext, player));
            int classes = ConfigGetClassCount("CfgUnesennyeTracks");
            for (int i = 0; i < classes; i++)
            {
                string clsName = ConfigGetClassName(i, "CfgUnesennyeTracks");
                if (clsName == "") continue;
                string file  = ConfigReadString(clsName + "\\file", "", ctx);
                string title = ConfigReadString(clsName + "\\title", clsName, ctx);
                if (file.Length() == 0) continue;
                ids.Insert(i);
                titles.Insert(title);
            }
        }

        ParamWriteBuffer final = new ParamWriteBuffer;
        final.WriteInt(clientId);
        final.WriteInt(ids.Count());
        for (int j = 0; j < ids.Count(); j++)
        {
            final.WriteInt(ids[j]);
            final.WriteString(titles[j]);
        }

        GetGame().RPCSingleParam(senderID, UnesennyeRPC.TRACK_LIST_RESP, final, RPCTargetGroup.Self);
    }

    // ===== PLAY: полная валидация перед ретрансляцией =====
    void HandleRadioPlay(RPCParamContext context, ParamReadBuffer buf)
    {
        int senderID = context.GetSenderID();
        UnesennyeAuthEntry e = FindAuthorized(senderID);
        if (!e) return; // клиент без handshake — отбрасываем

        int carID   = buf.ReadInt();
        int trackID = buf.ReadInt();

        // Rate limit (защита от флуда)
        int now = NowMs();
        if (now - e.lastRadioCmdMs < RATE_LIMIT_MS) return;
        e.lastRadioCmdMs = now;

        // Валидация 1: трек существует в CfgUnesennyeTracks?
        Man player = GetGame().GetPlayerByID(senderID);
        if (!player) return;
        TNewScriptDataContext ctx = TNewScriptDataContext.Cast(
            GetGame().CreateContext(TNewScriptDataContext, player));
        string trackCls = "CfgUnesennyeTracks\\track_" + trackID.ToString();
        if (!ConfigIsExist(trackCls, ctx)) return;

        // Валидация 2: машина существует и отправитель реально в ней сидит?
        CarScript car = Cast<CarScript>(GetEntityFromID(carID));
        if (!car) return;
        if (!IsPlayerInVehicle(player, car)) return;

        // Ретрансляция всем клиентам — пассажиры слышат синхронно
        ParamWriteBuffer bcast = new ParamWriteBuffer;
        bcast.WriteInt(carID);
        bcast.WriteInt(trackID);
        GetGame().RPCSingleParam(senderID, UnesennyeRPC.BROADCAST_PLAY, bcast, RPCTargetGroup.All);

        Print(string.Format("[Unesennye] Radio PLAY car=%d track=%d by player=%d (Author: %s)",
            carID, trackID, senderID, GetModAuthor()));
    }

    // ===== STOP =====
    void HandleRadioStop(RPCParamContext context, ParamReadBuffer buf)
    {
        int senderID = context.GetSenderID();
        UnesennyeAuthEntry e = FindAuthorized(senderID);
        if (!e) return;

        int carID = buf.ReadInt();

        int now = NowMs();
        if (now - e.lastRadioCmdMs < RATE_LIMIT_MS) return;
        e.lastRadioCmdMs = now;

        Man player = GetGame().GetPlayerByID(senderID);
        if (!player) return;
        CarScript car = Cast<CarScript>(GetEntityFromID(carID));
        if (!car) return;
        if (!IsPlayerInVehicle(player, car)) return;

        ParamWriteBuffer bcast = new ParamWriteBuffer;
        bcast.WriteInt(carID);
        GetGame().RPCSingleParam(senderID, UnesennyeRPC.BROADCAST_STOP, bcast, RPCTargetGroup.All);
    }

    // Присутствие игрока в машине проверяем через GetObjectsAtPosition
    // (PlayerBase.GetVehicle() доступен не в каждом серверном контексте)
    private bool IsPlayerInVehicle(Man player, CarScript car)
    {
        vector pos = player.GetPosition();
        // Быстрая проверка: игрок в габарите кузова машины
        vector carPos = car.GetPosition();
        if (vector.DistanceSq(pos, carPos) > 25.0) return false; // >5 м — точно мимо

        ref array<Man> nearby = new array<Man>;
        GetGame().GetObjectsAtPosition(pos, 3.0, 3.0, 3.0, nearby, null, null);
        foreach (Man m : nearby)
        {
            if (m == player) return true;
        }
        return false;
    }
};

// ===== Диспетчер: статические методы-адаптеры для RegisterServerRpc =====
class UnesennyeServerRPC
{
    static ref UnesennyeServerSystem g_UnesennyeServer;

    static void Init()
    {
        if (!g_UnesennyeServer)
        {
            g_UnesennyeServer = new UnesennyeServerSystem;
        }
        g_UnesennyeServer.Init();
    }

    static void OnHandshakeRequest(RPCParamContext context, ParamReadBuffer buf)
    {
        if (g_UnesennyeServer) g_UnesennyeServer.HandleHandshake(context, buf);
    }

    static void OnTrackListRequest(RPCParamContext context, ParamReadBuffer buf)
    {
        if (g_UnesennyeServer) g_UnesennyeServer.HandleTrackListRequest(context, buf);
    }

    static void OnRadioPlay(RPCParamContext context, ParamReadBuffer buf)
    {
        if (g_UnesennyeServer) g_UnesennyeServer.HandleRadioPlay(context, buf);
    }

    static void OnRadioStop(RPCParamContext context, ParamReadBuffer buf)
    {
        if (g_UnesennyeServer) g_UnesennyeServer.HandleRadioStop(context, buf);
    }
};

// ===== Очистка записи при выходе/деспауне игрока =====
modded class PlayerBase
{
    override void OnPlayerRemoved()
    {
        super.OnPlayerRemoved();
        if (UnesennyeServerRPC.g_UnesennyeServer)
        {
            UnesennyeServerRPC.g_UnesennyeServer.RemoveEntry(GetID());
        }
    }
}

// ===== Точка старта сервера =====
modded class MissionServer
{
    override void OnInit()
    {
        super.OnInit(); // базовая инициализация обязательна
        UnesennyeServerRPC.Init();
    }

    override void OnMissionFinish()
    {
        super.OnMissionFinish();
        // Освобождение системы — никаких висячих ссылок
        if (UnesennyeServerRPC.g_UnesennyeServer)
        {
            delete UnesennyeServerRPC.g_UnesennyeServer;
            UnesennyeServerRPC.g_UnesennyeServer = null;
        }
    }
}
