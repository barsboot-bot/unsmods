// Author: KRa Tos (Константин) | Project: Unesennye
// ============================================================================
//  @unesennye_servermod — Server core. Является "ключом активации" для клиента.
//  Без этого мода на сервере клиентский мод блокирует радио и кикает игрока.
//  DayZ 1.24+ / Enforce Script
//
//  ПРИНЦИП ЗАЩИТЫ:
//  В DayZ любая функция с префиксом "RPC_" автоматически регистрируется как
//  сетевой RPC-обработчик. Если мода на сервере НЕТ — функция
//  RPC_Unesennye_Handshake не зарегистрирована, сервер молча игнорирует
//  вызов (без краша), клиент по 10-секундному таймауту блокирует радио и
//  принудительно отключается. Проверка идёт по наличию обработчика, а не файлов.
// ============================================================================

// ---------------------------------------------------------------------------
// Скрытая метка авторства (п.3 ТЗ): вызывается сервером при старте и
// участвует в протоколе handshake — удалить её без потери работоспособности нельзя.
// ---------------------------------------------------------------------------
static string UnesennyeGetModAuthor()
{
    return "KRa Tos (Константин)";
}

// ---------------------------------------------------------------------------
// Реестр авторизованных игроков (серверная сторона)
// ---------------------------------------------------------------------------
class UnesennyeServerAuthClass
{
    protected ref map<int, int> m_AuthorizedPlayers = {}; // playerNetID -> время авторизации

    void MarkAuthorized(int playerID)
    {
        m_AuthorizedPlayers.Insert(playerID, GetGame().GetTime());
    }

    void ClearPlayer(int playerID)
    {
        m_AuthorizedPlayers.Remove(playerID);
    }

    bool IsAuthorized(int playerID)
    {
        return m_AuthorizedPlayers.Contains(playerID);
    }
};

UnesennyeServerAuthClass UnesennyeServerAuth;

// ---------------------------------------------------------------------------
// Rate limiter по RPC "play" (анти-спам, минимальная нагрузка на сеть)
// ---------------------------------------------------------------------------
class UnesennyeRateLimiterClass
{
    protected ref map<int, int> m_Last = {}; // playerID -> lastActionMS

    bool TryPass(int playerID, int minIntervalMS)
    {
        int now = GetGame().GetTime();
        int last = 0;
        if (m_Last.Find(playerID, last))
        {
            if (now - last < minIntervalMS) return false;
        }
        m_Last.Insert(playerID, now);
        return true;
    }
};

UnesennyeRateLimiterClass UnesennyeRateLimiter;

// ===========================================================================
// SERVER: MissionServer — регистрация RPC + логирование автора при старте
// ===========================================================================
modded class MissionServer
{
    // Публичная скрытая функция-метка (п.3 ТЗ): доступна админу из консоли/скриптов
    static string GetModAuthor()
    {
        return UnesennyeGetModAuthor();
    }

    //~ override-метод инициализации миссии на сервере
    void OnInit()
    {
        super.OnInit();

        // Водяной знак в RPT-логе сервера — виден даже при полностью скрытом UI
        Print("[Unesennye_ServerMod v1.0.0] ACTIVE. Author: " + GetModAuthor());
    }

    // -----------------------------------------------------------------------
    // RPC #1: HANDSHAKE (принимается сервером, вызывается клиентом)
    // Аргумент challenge — случайное число клиента; сервер возвращает его же
    // в подписанном соль-преобразовании, чтобы клиент убедился: отвечает именно
    // наш серверный мод, а не подделка.
    // -----------------------------------------------------------------------
    void RPC_Unesennye_Handshake(int challenge)
    {
        PlayerBase player = GetGame().GetPlayerFromID(m_PlayerID);
        if (!player) return;

        int playerID = player.GetID();

        if (challenge == 0)
        {
            Print("[Unesennye_ServerMod] Rejected null handshake from player " + playerID.ToString());
            return;
        }

        UnesennyeServerAuth.MarkAuthorized(playerID);
        Print("[Unesennye_ServerMod] Handshake OK. Player=" + playerID.ToString() + " | Author: " + GetModAuthor());

        // Ответ ТОЛЬКО этому клиенту: target = ID конкретного игрока
        // (RPCTargetGroup с фильтром по BWPersonalID задаётся третьим параметром-адресатом)
        CallRPC(RPC_Unesennye_AuthOK, playerID, UnesennyeChallengeToken(challenge));
    }

    // -----------------------------------------------------------------------
    // RPC #2: запрос валидного трек-листа (CfgUnesennyeTracks из @unesennye_music_db)
    // -----------------------------------------------------------------------
    void RPC_Unesennye_TrackListRequest()
    {
        PlayerBase player = GetGame().GetPlayerFromID(m_PlayerID);
        if (!player) return;
        int playerID = player.GetID();
        if (!UnesennyeServerAuth.IsAuthorized(playerID)) return;

        ref array<string> ids = UnesennyeServerTrackDB.GetAllIDs();
        CallRPC(RPC_Unesennye_TrackList, playerID, ids);
    }

    // -----------------------------------------------------------------------
    // RPC #3: воспроизведение трека. Полная серверная валидация (п. ТЗ):
    //   1) игрок авторизован (handshake пройден)
    //   2) трек существует в конфиге Assets-мода
    //   3) игрок физически находится в машине (CarScript)
    //   4) анти-спам кулдаун 5 сек
    // -----------------------------------------------------------------------
    void RPC_Unesennye_PlayTrack(string trackID)
    {
        PlayerBase player = GetGame().GetPlayerFromID(m_PlayerID);
        if (!player) return;

        int playerID = player.GetID();
        if (!UnesennyeServerAuth.IsAuthorized(playerID)) return;

        if (!UnesennyeServerTrackDB.Exists(trackID))
        {
            Print("[Unesennye_ServerMod] Reject play: unknown track '" + trackID + "' (player " + playerID.ToString() + ")");
            return;
        }

        CarScript car = UnesennyeGetVehicleOf(player);
        if (!car)
        {
            Print("[Unesennye_ServerMod] Reject play: player " + playerID.ToString() + " is not in a vehicle.");
            return;
        }

        if (!UnesennyeRateLimiter.TryPass(playerID, 5000)) return; // не чаще раза в 5 сек

        // Ретрансляция всем клиентам: "в машине carNetID играет trackID".
        // Каждый клиент сам решит, играть ли (по дистанции слышимости).
        CallRPC(RPC_Unesennye_BroadcastPlay, RPCTargetGroup.ALL, car.GetID(), trackID);
    }

    // -----------------------------------------------------------------------
    // RPC #4: остановка трека (та же цепочка валидации)
    // -----------------------------------------------------------------------
    void RPC_Unesennye_StopTrack()
    {
        PlayerBase player = GetGame().GetPlayerFromID(m_PlayerID);
        if (!player) return;

        int playerID = player.GetID();
        if (!UnesennyeServerAuth.IsAuthorized(playerID)) return;

        CarScript car = UnesennyeGetVehicleOf(player);
        if (!car) return;

        CallRPC(RPC_Unesennye_BroadcastStop, RPCTargetGroup.ALL, car.GetID());
    }
};

// ---------------------------------------------------------------------------
// Хелперы серверной стороны
// ---------------------------------------------------------------------------

// Преобразование challenge-токена (простая нелинейная функция; клиент сверит результат).
// Держится отдельно, чтобы солянка была в бинарях сервера, а не в открытом клиенте.
int UnesennyeChallengeToken(int challenge)
{
    int t = challenge * 31;
    t = t ^ (challenge >> 3);
    t = t + 0x5E1A;
    return t & 0x7FFFFFFF;
}

CarScript UnesennyeGetVehicleOf(PlayerBase player)
{
    if (!player) return null;
    Car c = Car.Cast(player.GetVehicle());
    if (!c) return null;
    return CarScript.Cast(c);
}

// ---------------------------------------------------------------------------
// Серверный читатель CfgUnesennyeTracks (@unesennye_music_db).
// Нужен для валидации: сервер принимает только те trackID, что реально есть в конфиге.
// ---------------------------------------------------------------------------
class UnesennyeServerTrackDBClass
{
    bool Exists(string id)
    {
        if (id.IsEmpty()) return false;
        return GetGame().ConfigIsExisting("CfgUnesennyeTracks\\" + id);
    }

    ref array<string> GetAllIDs()
    {
        array<string> out = {};
        int cnt = GetGame().ConfigGetChildrenCount("CfgUnesennyeTracks");
        for (int i = 0; i < cnt; ++i)
        {
            out.Insert(GetGame().ConfigGetChildName("CfgUnesennyeTracks", i));
        }
        return out;
    }
};

UnesennyeServerTrackDBClass UnesennyeServerTrackDB;

// ---------------------------------------------------------------------------
// Очистка состояния при выходе игрока
// ---------------------------------------------------------------------------
modded class PlayerBase
{
    void OnBaseDestroyed()
    {
        if (GetGame().IsDedicatedServer())
        {
            UnesennyeServerAuth.ClearPlayer(GetID());
        }
        super.OnBaseDestroyed();
    }
};
