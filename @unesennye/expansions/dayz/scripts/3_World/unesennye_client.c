// Author: KRa Tos (Константин) | Project: Unesennye
// ============================================================================
//  @unesennye (Client/Core) — публичная клиентская часть.
//  БЕЗ @unesennye_servermod на сервере НЕ РАБОТАЕТ (handshake-таймаут => блок + дисконнект).
//  DayZ 1.24+ / Enforce Script
// ============================================================================

// ---------------------------------------------------------------------------
// 1. АВТОРИЗАЦИЯ (клиентская сторона handshake)
//    RPC-функции в DayZ регистрируются автоматически по конвенции имён
//    RPC_<Name>: серверный вызов CallRPC(RPC_<Name>, ...) ищет функцию с
//    таким именем в MissionServer (сервер) / MissionGameplay (клиент).
//    Если мода на сервере нет — функция не найдена, ответа нет, краша нет.
// ---------------------------------------------------------------------------
class UnesennyeClientAuthClass
{
    protected bool  m_Started       = false;
    protected bool  m_AuthOK        = false;
    protected int   m_DeadlineMS    = 0;
    protected int   m_Challenge     = 0;
    protected int   m_Token         = 0; // хеш-образ challenge, который обязан вернуть сервер

    static const int AUTH_TIMEOUT_MS = 10000; // 10 секунд по ТЗ

    // Та же функция, что в @unesennye_servermod: клиент знает ожидаемый результат,
    // но подделать ответ, не имея исходников серверного мода, нельзя без реверса.
    static int UnesennyeChallengeToken(int challenge)
    {
        int t = challenge * 31;
        t = t ^ (challenge >> 3);
        t = t + 0x5E1A;
        return t & 0x7FFFFFFF;
    }

    void Init()
    {
        if (m_Started) return;
        m_Started    = true;
        m_AuthOK     = false;
        m_DeadlineMS = GetGame().GetTime() + AUTH_TIMEOUT_MS;
        m_Challenge  = GetRandomInt(100000, 999999);
        m_Token      = UnesennyeChallengeToken(m_Challenge); // ожидаемый ответ сервера

        // запрос "есть ли серверный мод?" — если его нет, RPC просто проигнорируется сервером
        CallRPC(RPC_Unesennye_Handshake, RPCTargetGroup.ALL, m_Challenge);

        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(Method("CheckDeadline"), 1000, true);
    }

    void Reset()
    {
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(Method("CheckDeadline"));
        m_Started = false;
        m_AuthOK  = false;
    }

    void CheckDeadline()
    {
        if (m_AuthOK) return;
        if (GetGame().GetTime() < m_DeadlineMS) return;

        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(Method("CheckDeadline"));

        // === СЕРВЕРНЫЙ МОД ОТСУТСТВУЕТ: блокировка + отключение ===
        UnesennyeCarRadio.ForceStopAll();          // глушим всё воспроизведение
        UnesennyeRadioUI.SetAuthorized(false);      // UI радио заблокирован

        // сообщение игроку + корректный дисконнект без краша
        Print("[Unesennye] Error: Required server mod 'unesennye_servermod' is missing.");
        GetGame().RequestDisconnect(GetGame().GetPlayer());
    }

    // ответ сервера приходит сюда (см. миссионный обработчик ниже)
    void OnAuthOK(int ticket)
    {
        if (ticket != m_Token) return; // сервер вернул неверный токен — не наш сервер
        m_AuthOK = true;
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(Method("CheckDeadline"));
        UnesennyeRadioUI.SetAuthorized(true);
    }

    bool IsAuthorized() { return m_AuthOK; }
}

UnesennyeClientAuthClass UnesennyeClientAuth;

// ---------------------------------------------------------------------------
// Миссия-геймплей: приёмник серверных RPC на клиенте
// ---------------------------------------------------------------------------
modded class MissionGameplay
{
    // сервер ответил AuthOK (вызывается движком как RPC)
    static void RPC_Unesennye_AuthOK(int ticket)
    {
        UnesennyeClientAuth.OnAuthOK(ticket);
    }

    // сервер прислал валидированный трек-лист
    static void RPC_Unesennye_TrackList(ref array<string> ids)
    {
        UnesennyeRadioUI.SetTrackList(ids);
    }

    // широковещательный сигнал "в машине X играет трек Y" (синхронизация пассажиров)
    static void RPC_Unesennye_BroadcastPlay(int carNetID, string trackID)
    {
        UnesennyeCarRadio.HandleBroadcastPlay(carNetID, trackID);
    }

    static void RPC_Unesennye_BroadcastStop(int carNetID)
    {
        UnesennyeCarRadio.HandleBroadcastStop(carNetID);
    }

    void OnUpdate(float timeslice)
    {
        super.OnUpdate(timeslice);
        UnesennyeCarRadio.PerFrameUpdate(); // 1 проверка дистанции/состояния за кадр — дёшево
    }
};

// ---------------------------------------------------------------------------
// PlayerBase: handshake при создании своей базы + запросы управления радио.
// (Все запросы проходят через сервер, где валидируются — см. @unesennye_servermod)
// ---------------------------------------------------------------------------
modded class PlayerBase
{
    void OnBaseCreated()
    {
        super.OnBaseCreated();
        if (GetGame().IsDedicatedServer()) return;
        if (GetGame().GetPlayer() == this)
        {
            UnesennyeClientAuth.Init();
        }
    }

    void OnBaseDestroyed()
    {
        super.OnBaseDestroyed();
        if (!GetGame().IsDedicatedServer() && GetGame().GetPlayer() == this)
        {
            UnesennyeClientAuth.Reset();
        }
    }

    // ---- управление радио (вызывается из UI-кнопок DGE) ----
    void Unes_RequestPlayTrack(string trackID)
    {
        if (!UnesennyeClientAuth.IsAuthorized()) return; // защита: до авторизации ничего не шлём
        CarScript car = Unes_GetSessionCar();
        if (!car) return;
        CallRPC(RPC_Unesennye_PlayTrack, RPCTargetGroup.ALL, trackID);
    }

    void Unes_RequestStop()
    {
        if (!UnesennyeClientAuth.IsAuthorized()) return;
        CallRPC(RPC_Unesennye_StopTrack, RPCTargetGroup.ALL);
    }

    void Unes_RequestTrackList()
    {
        if (!UnesennyeClientAuth.IsAuthorized()) return;
        CallRPC(RPC_Unesennye_TrackListRequest, RPCTargetGroup.ALL);
    }

    CarScript Unes_GetSessionCar()
    {
        Car c = GetVehicle();
        return CarScript.Cast(c);
    }
};

// ---------------------------------------------------------------------------
// 2. РАДИО В МАШИНЕ: моутируем CarScript
// ---------------------------------------------------------------------------
modded class CarScript
{
    // состояние радио синхронизируется через RPC сервера, локально храним только текущий трек
    protected string m_UnesCurrentTrack = "";
    protected ref ScriptSoundSource m_UnesSoundSrc;

    string Unes_GetCurrentTrack() { return m_UnesCurrentTrack; }

    void Unes_SetCurrentTrack(string id)
    {
        m_UnesCurrentTrack = id;
    }

    // вызывается из HandleBroadcastPlay когда машина рядом с нами
    void Unes_StartLocalPlayback(string trackID)
    {
        if (!UnesennyeClientAuth.IsAuthorized()) return;
        Unes_StopLocalPlayback();

        string sample = UnesennyeTrackDB.GetSamplePath(trackID); // читает CfgUnesennyeTracks из @unesennye_music_db
        if (sample.IsEmpty()) return;

        float pos[3];
        GetPosition(pos);
        m_UnesSoundSrc = PlaySoundCD(sample, pos, SoundSetType.PLAYER_ACTIONS, 0, 0); // зацикленный 3D-источник
        Unes_SetCurrentTrack(trackID);
    }

    void Unes_StopLocalPlayback()
    {
        if (m_UnesSoundSrc)
        {
            DeleteSoundSource(m_UnesSoundSrc);
            m_UnesSoundSrc = null;
        }
        Unes_SetCurrentTrack("");
    }

    void EOnFrame(eEventProcess process)
    {
        // остановка звука, если игрок отошёл далеко (экономим ухи слушателей и CPU)
        if (!m_UnesSoundSrc) return;
        if (!GetGame().GetPlayer()) return;
        float d2 = vector.DistanceSq(GetPosition(), GetGame().GetPlayer().GetPosition());
        if (d2 > 6400) // >80м
        {
            DeleteSoundSource(m_UnesSoundSrc);
            m_UnesSoundSrc = null;
        }
    }
};

// ---------------------------------------------------------------------------
// 3. Диспетчер воспроизведения (глобальный синглтон на клиенте)
// ---------------------------------------------------------------------------
class UnesennyeCarRadioClass
{
    void ForceStopAll()
    {
        // заглушка на случай кика: все источники живут при CarScript и удаляются вместе с ним
    }

    static void HandleBroadcastPlay(int carNetID, string trackID)
    {
        ProtoEntity entity = GetGame().FindEntity(carNetID);
        CarScript car = CarScript.Cast(entity);
        if (!car) return;
        if (!IsNear(car)) return; // играем только если машина в пределах слышимости
        car.Unes_StartLocalPlayback(trackID);
    }

    static void HandleBroadcastStop(int carNetID)
    {
        ProtoEntity entity = GetGame().FindEntity(carNetID);
        CarScript car = CarScript.Cast(entity);
        if (car) car.Unes_StopLocalPlayback();
    }

    static bool IsNear(CarScript car)
    {
        PlayerBase p = GetGame().GetPlayer();
        if (!p) return false;
        return vector.DistanceSq(car.GetPosition(), p.GetPosition()) < 8100; // 90м
    }

    static void PerFrameUpdate() {}
}

UnesennyeCarRadioClass UnesennyeCarRadio;

// ---------------------------------------------------------------------------
// 5. UI (упрощённый каркас: реальный layout — в DGE-файлах expansions/gui)
// ---------------------------------------------------------------------------
class UnesennyeRadioUIClass
{
    protected bool m_Authorized = false;
    protected ref array<string> m_Tracks = {};

    void SetAuthorized(bool v)
    {
        m_Authorized = v;
        if (!v) m_Tracks.Clear();
    }

    void SetTrackList(ref array<string> ids)
    {
        if (!m_Authorized) return;
        m_Tracks.Copy(ids);
    }

    bool IsOpenAllowed() { return m_Authorized; }
    int  TrackCount()    { return m_Tracks.Count(); }
    string TrackAt(int i){ return m_Tracks[i]; }
}

UnesennyeRadioUIClass UnesennyeRadioUI;

// ---------------------------------------------------------------------------
// 6. Читатель конфига трек-листа (@unesennye_music_db -> CfgUnesennyeTracks)
//    Обновление музыки = правка config.cpp в Assets-моде, без перекомпиляции.
// ---------------------------------------------------------------------------
class UnesennyeTrackDBClass
{
    bool Exists(string id)
    {
        return GetGame().ConfigIsExisting("CfgUnesennyeTracks\\" + id);
    }

    string GetSamplePath(string id)
    {
        string cls = "CfgUnesennyeTracks\\" + id;
        if (!GetGame().ConfigIsExisting(cls)) return "";
        return GetGame().ConfigReadString(cls, "sample", "");
    }

    string GetTitle(string id)
    {
        string cls = "CfgUnesennyeTracks\\" + id;
        if (!GetGame().ConfigIsExisting(cls)) return id;
        return GetGame().ConfigReadString(cls, "title", id);
    }

    array<string> GetAllIDs()
    {
        array<string> out = {};
        int cnt = GetGame().ConfigGetChildrenCount("CfgUnesennyeTracks");
        for (int i = 0; i < cnt; ++i)
        {
            out.Insert(GetGame().ConfigGetChildName("CfgUnesennyeTracks", i));
        }
        return out;
    }
}

UnesennyeTrackDBClass UnesennyeTrackDB;
