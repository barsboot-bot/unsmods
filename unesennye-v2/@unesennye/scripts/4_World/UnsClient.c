// Unesennye v2 — клиент: рукопожатие с сервером, блокировка при отсутствии серверного мода,
// воспроизведение звука по ответам сервера.
// Клиент не меняет состояние проигрывания сам: звук запускается только по S2C_PLAY.

class UnsClientSession
{
    static ref UnsClientSession s_Instance;

    protected bool m_Sent;
    protected bool m_Authorized;
    protected bool m_Locked;
    protected bool m_TickScheduled;
    protected int m_Nonce;
    protected int m_SentAtMs;
    protected ref map<string, ref EffectSound> m_Sounds;

    static UnsClientSession Get()
    {
        if (!s_Instance)
            s_Instance = new UnsClientSession();
        return s_Instance;
    }

    void UnsClientSession()
    {
        m_Sounds = new map<string, ref EffectSound>;
    }

    bool IsAuthorized()
    {
        return m_Authorized;
    }

    // Таймер рукопожатия (повторяющийся). Таймаут отсчитывается от отправки HELLO, а не от старта клиента,
    // поэтому долгая загрузка миссии не даёт ложной блокировки.
    void Start()
    {
        if (m_TickScheduled)
            return;
        m_TickScheduled = true;
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLaterByName(this, "Tick", UnsProto.HANDSHAKE_TICK_MS, true);
    }

    void Stop()
    {
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).RemoveByName(this, "Tick");
        m_TickScheduled = false;
        StopAll();
    }

    protected void StopTick()
    {
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).RemoveByName(this, "Tick");
        m_TickScheduled = false;
    }

    void Tick()
    {
        if (m_Locked || m_Authorized)
            return;

        if (!m_Sent)
        {
            if (!GetGame().GetPlayer())
                return;
            SendHello();
            return;
        }

        if (GetGame().GetTime() - m_SentAtMs >= UnsProto.HANDSHAKE_TIMEOUT_MS)
            Lockdown();
    }

    protected void SendHello()
    {
        m_Nonce = Math.RandomInt(1, 1000000);
        m_Sent = true;
        m_SentAtMs = GetGame().GetTime();

        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(UnsProto.C2S_HELLO);
        rpc.Write(UnsProto.PROTO_VERSION);
        rpc.Write(m_Nonce);
        rpc.Send(null, UnsProto.RPC_ID, true, null);
        Print("[Unesennye] HELLO отправлен, ждём подтверждения сервера");
    }

    protected void Lockdown()
    {
        if (m_Locked)
            return;
        m_Locked = true;
        Stop();
        UnsUi.Notify(UnsProto.MSG_MISSING_SERVER);
        GetGame().Disconnect();
    }

    // Диспетчер входящих сообщений. Вызывается из UnsClientOnRPC после проверки RPC_ID.
    void Dispatch(int op, ParamsReadContext ctx)
    {
        if (op == UnsProto.S2C_HELLO_ACK)
            OnHelloAck(ctx);
        else if (op == UnsProto.S2C_PLAY)
            OnPlay(ctx);
        else if (op == UnsProto.S2C_STOP)
            OnStop(ctx);
        else if (op == UnsProto.S2C_REJECT)
            OnReject(ctx);
    }

    protected void OnHelloAck(ParamsReadContext ctx)
    {
        int nonce = 0;
        int ok = 0;
        if (!ctx.Read(nonce) || !ctx.Read(ok))
            return;
        if (nonce != m_Nonce || ok != 1)
            return;

        m_Authorized = true;
        StopTick();
        Print("[Unesennye] сервер подтвердил протокол");
    }

    protected void OnPlay(ParamsReadContext ctx)
    {
        string key;
        float x = 0;
        float y = 0;
        float z = 0;
        int trackId = 0;
        int kind = 0;
        if (!ctx.Read(key) || !ctx.Read(x) || !ctx.Read(y) || !ctx.Read(z) || !ctx.Read(trackId) || !ctx.Read(kind))
            return;

        vector pos;
        pos[0] = x;
        pos[1] = y;
        pos[2] = z;
        Play(key, pos, trackId);
    }

    protected void OnStop(ParamsReadContext ctx)
    {
        string key;
        if (!ctx.Read(key))
            return;
        StopKey(key);
    }

    protected void OnReject(ParamsReadContext ctx)
    {
        int reason = 0;
        if (!ctx.Read(reason))
            return;
        UnsUi.Notify(UnsProto.MSG_REJECT_PREFIX + reason.ToString());
    }

    // Один источник звука на ключ (машина или карта). Новый PLAY заменяет старый.
    void Play(string key, vector pos, int trackId)
    {
        if (!m_Authorized)
            return;

        UnsTrack t = UnsRegistry.Get().GetTrack(trackId);
        if (!t)
        {
            Print("[Unesennye] неизвестный trackId " + trackId.ToString());
            return;
        }

        StopKey(key);
        EffectSound snd = SEffectManager.PlaySound(t.m_SoundSet, pos);
        if (!snd)
            return;
        snd.SetAutodestroy(true);
        m_Sounds.Set(key, snd);
    }

    void StopKey(string key)
    {
        if (!m_Sounds.Contains(key))
            return;
        EffectSound old = m_Sounds.Get(key);
        if (old)
            old.Stop();
        m_Sounds.Remove(key);
    }

    void StopAll()
    {
        for (int i = 0; i < m_Sounds.Count(); i++)
        {
            EffectSound s = m_Sounds.GetElement(i);
            if (s)
                s.Stop();
        }
        m_Sounds.Clear();
    }
}

void UnsClientOnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
{
    if (rpc_type != UnsProto.RPC_ID)
        return;
    int op = 0;
    if (!ctx.Read(op))
        return;
    UnsClientSession.Get().Dispatch(op, ctx);
}

modded class MissionGameplay
{
    override void OnInit()
    {
        super.OnInit();
        GetDayZGame().Event_OnRPC.Insert(UnsClientOnRPC);
        UnsClientSession.Get().Start();
    }

    override void OnMissionFinish()
    {
        GetDayZGame().Event_OnRPC.Remove(UnsClientOnRPC);
        UnsClientSession.Get().Stop();
        super.OnMissionFinish();
    }
}
