// Unesennye v2 — сервер: рукопожатие, проверка всех входящих аргументов, состояние проигрывания,
// рассылка PLAY/STOP.
// Правило: состояние меняется только после прохождения всех проверок. Отказ отправляется как S2C_REJECT.
//
// Ограничения (см. README): рукопожатие подтверждает наличие мода, но не аутентифицирует клиента.
// Сессии хранятся в памяти и сбрасываются при перезапуске сервера.

class UnsSession
{
    int m_PlayerId;
    bool m_Authorized;
    int m_LastCmdMs;
}

class UnsServer
{
    static ref UnsServer s_Instance;

    protected ref map<int, ref UnsSession> m_Sessions;
    protected ref map<string, int> m_CarOrdinal;   // ключ машины -> номер последнего трека плейлиста
    protected ref map<string, int> m_CardIndex;    // ключ карты -> позиция в альбоме

    static UnsServer Get()
    {
        if (!s_Instance)
        {
            s_Instance = new UnsServer();
            Print("[Unesennye] сервер: протокол " + UnsProto.PROTO_VERSION.ToString() + ", RPC " + UnsProto.RPC_ID.ToString());
            UnsRegistry.Get();
        }
        return s_Instance;
    }

    void UnsServer()
    {
        m_Sessions = new map<int, ref UnsSession>;
        m_CarOrdinal = new map<string, int>;
        m_CardIndex = new map<string, int>;
    }

    // Точка входа для всех RPC нашего ID. Все чтения из ctx проверяются.
    void Dispatch(PlayerIdentity sender, int op, ParamsReadContext ctx)
    {
        if (op == UnsProto.C2S_HELLO)
        {
            OnHello(sender, ctx);
            return;
        }

        UnsSession s = FindSession(sender);
        if (!s || !s.m_Authorized)
        {
            Reject(sender, UnsProto.R_NOT_AUTH);
            return;
        }

        int now = GetGame().GetTime();
        if (now - s.m_LastCmdMs < UnsProto.COOLDOWN_MS)
        {
            Reject(sender, UnsProto.R_COOLDOWN);
            return;
        }
        s.m_LastCmdMs = now;

        if (op == UnsProto.C2S_CAR_PLAY)
            OnCarPlay(sender, ctx);
        else if (op == UnsProto.C2S_CAR_STOP)
            OnCarStop(sender, ctx);
        else if (op == UnsProto.C2S_CARD_STEP)
            OnCardStep(sender, ctx);
        else if (op == UnsProto.C2S_CARD_STOP)
            OnCardStop(sender, ctx);
    }

    protected void OnHello(PlayerIdentity sender, ParamsReadContext ctx)
    {
        int proto = 0;
        int nonce = 0;
        if (!ctx.Read(proto) || !ctx.Read(nonce))
        {
            Reject(sender, UnsProto.R_PROTO);
            return;
        }

        if (proto != UnsProto.PROTO_VERSION)
        {
            Reject(sender, UnsProto.R_PROTO);
            return;
        }

        UnsSession s = GetOrCreateSession(sender);
        s.m_Authorized = true;

        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(UnsProto.S2C_HELLO_ACK);
        rpc.Write(nonce);
        rpc.Write(1);
        rpc.Send(null, UnsProto.RPC_ID, true, sender);
    }

    protected void OnCarPlay(PlayerIdentity sender, ParamsReadContext ctx)
    {
        Object obj = UnsNet.ReadObjectId(ctx);
        Transport car = Transport.Cast(obj);
        if (!car)
        {
            Reject(sender, UnsProto.R_BAD_TARGET);
            return;
        }

        PlayerBase player = FindPlayer(sender);
        if (!player || !IsInVehicle(player, car))
        {
            Reject(sender, UnsProto.R_NOT_IN_CAR);
            return;
        }

        UnsRegistry reg = UnsRegistry.Get();
        if (reg.GetCarTrackCount() == 0)
        {
            Reject(sender, UnsProto.R_NO_TRACKS);
            return;
        }

        // Состояние меняем только здесь, после всех проверок.
        string key = car.GetNetworkIDString();
        int ordinal = 0;
        if (m_CarOrdinal.Contains(key))
            ordinal = m_CarOrdinal.Get(key) + 1;
        m_CarOrdinal.Set(key, ordinal);

        UnsTrack track = reg.GetCarTrackByOrdinal(ordinal);
        if (!track)
        {
            Reject(sender, UnsProto.R_NO_TRACKS);
            return;
        }
        Broadcast(UnsProto.S2C_PLAY, key, car.GetPosition(), track.m_TrackId, UnsProto.KIND_CAR);
    }

    protected void OnCarStop(PlayerIdentity sender, ParamsReadContext ctx)
    {
        Object obj = UnsNet.ReadObjectId(ctx);
        Transport car = Transport.Cast(obj);
        if (!car)
        {
            Reject(sender, UnsProto.R_BAD_TARGET);
            return;
        }

        PlayerBase player = FindPlayer(sender);
        if (!player || !IsInVehicle(player, car))
        {
            Reject(sender, UnsProto.R_NOT_IN_CAR);
            return;
        }

        BroadcastStop(car.GetNetworkIDString());
    }

    protected void OnCardStep(PlayerIdentity sender, ParamsReadContext ctx)
    {
        Object obj = UnsNet.ReadObjectId(ctx);
        int dir = 0;
        if (!ctx.Read(dir))
        {
            Reject(sender, UnsProto.R_BAD_TARGET);
            return;
        }
        // Допускаем только шаг на один трек в любую сторону.
        if (dir != 1 && dir != -1)
        {
            Reject(sender, UnsProto.R_BAD_TARGET);
            return;
        }

        UnesennyeMusicCard_Base card = UnesennyeMusicCard_Base.Cast(obj);
        if (!card)
        {
            Reject(sender, UnsProto.R_BAD_TARGET);
            return;
        }

        PlayerBase player = FindPlayer(sender);
        if (!player || card.GetHierarchyRootPlayer() != player)
        {
            Reject(sender, UnsProto.R_NOT_OWNER);
            return;
        }

        UnsRegistry reg = UnsRegistry.Get();
        string albumId = reg.GetCardAlbumId(card.GetType());
        int count = reg.GetAlbumCount(albumId);
        if (count == 0)
        {
            Reject(sender, UnsProto.R_NO_TRACKS);
            return;
        }

        string key = card.GetNetworkIDString();
        int cur = 0;
        if (m_CardIndex.Contains(key))
            cur = m_CardIndex.Get(key);
        else if (dir > 0)
            cur = -1;   // первый «Далее» даёт трек 0

        int next = ((cur + dir) % count + count) % count;
        int trackId = reg.GetAlbumTrackId(albumId, next);
        if (trackId < 0)
        {
            Reject(sender, UnsProto.R_NO_TRACKS);
            return;
        }

        m_CardIndex.Set(key, next);
        Broadcast(UnsProto.S2C_PLAY, key, player.GetPosition(), trackId, UnsProto.KIND_PORTABLE);
    }

    protected void OnCardStop(PlayerIdentity sender, ParamsReadContext ctx)
    {
        UnesennyeMusicCard_Base card = UnesennyeMusicCard_Base.Cast(UnsNet.ReadObjectId(ctx));
        if (!card)
        {
            Reject(sender, UnsProto.R_BAD_TARGET);
            return;
        }

        PlayerBase player = FindPlayer(sender);
        if (!player || card.GetHierarchyRootPlayer() != player)
        {
            Reject(sender, UnsProto.R_NOT_OWNER);
            return;
        }

        BroadcastStop(card.GetNetworkIDString());
    }

    protected UnsSession FindSession(PlayerIdentity sender)
    {
        if (!sender)
            return null;
        int pid = sender.GetPlayerId();
        if (!m_Sessions.Contains(pid))
            return null;
        return m_Sessions.Get(pid);
    }

    protected UnsSession GetOrCreateSession(PlayerIdentity sender)
    {
        UnsSession s = FindSession(sender);
        if (s)
            return s;
        s = new UnsSession();
        s.m_PlayerId = sender.GetPlayerId();
        s.m_Authorized = false;
        s.m_LastCmdMs = -UnsProto.COOLDOWN_MS * 10;
        m_Sessions.Set(s.m_PlayerId, s);
        return s;
    }

    protected PlayerBase FindPlayer(PlayerIdentity sender)
    {
        if (!sender)
            return null;
        ref array<Man> players = new array<Man>;
        GetGame().GetPlayers(players);
        for (int i = 0; i < players.Count(); i++)
        {
            PlayerBase pb = PlayerBase.Cast(players.Get(i));
            if (pb && pb.GetIdentity() && pb.GetIdentity().GetPlayerId() == sender.GetPlayerId())
                return pb;
        }
        return null;
    }

    // Игрок реально сидит в этой машине (по командам вождения, а не по радиусу).
    protected bool IsInVehicle(PlayerBase player, Transport car)
    {
        HumanCommandVehicle cmd = player.GetCommand_Vehicle();
        if (!cmd)
            return false;
        Transport tr = cmd.GetTransport();
        return tr == car;
    }

    protected void Reject(PlayerIdentity to, int reason)
    {
        if (!to)
            return;
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(UnsProto.S2C_REJECT);
        rpc.Write(reason);
        rpc.Send(null, UnsProto.RPC_ID, true, to);
    }

    protected void Broadcast(int op, string key, vector pos, int trackId, int kind)
    {
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(op);
        rpc.Write(key);
        rpc.Write(pos[0]);
        rpc.Write(pos[1]);
        rpc.Write(pos[2]);
        rpc.Write(trackId);
        rpc.Write(kind);
        rpc.Send(null, UnsProto.RPC_ID, true, null);
    }

    protected void BroadcastStop(string key)
    {
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(UnsProto.S2C_STOP);
        rpc.Write(key);
        rpc.Send(null, UnsProto.RPC_ID, true, null);
    }
}

void UnsServerOnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
{
    if (rpc_type != UnsProto.RPC_ID || !sender)
        return;
    int op = 0;
    if (!ctx.Read(op))
        return;
    UnsServer.Get().Dispatch(sender, op, ctx);
}

modded class MissionServer
{
    override void OnInit()
    {
        super.OnInit();
        UnsServer.Get();
        GetDayZGame().Event_OnRPC.Insert(UnsServerOnRPC);
    }

    override void OnMissionFinish()
    {
        GetDayZGame().Event_OnRPC.Remove(UnsServerOnRPC);
        super.OnMissionFinish();
    }
}
