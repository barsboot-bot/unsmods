// Unesennye v2 — общая логика, нужная и клиенту, и серверу:
//   - отправка запросов серверу (клиентская сторона действий);
//   - чтение/запись ID объектов;
//   - класс музыкальной карты и действия (Далее / Назад / Стоп) для карт и машин.
// Действия ничего не проигрывают локально: они только отправляют запрос. Проигрывание делает клиентский мод
// по ответу сервера (S2C_PLAY), так что клиент не меняет состояние до подтверждения.

class UnsUi
{
    // Сообщение игроку. Если MessageStatus недоступен в сборке, остаётся Print в RPT.
    static void Notify(string text)
    {
        Print("[Unesennye] " + text);
        PlayerBase p = PlayerBase.Cast(GetGame().GetPlayer());
        if (p)
            p.MessageStatus(text);
    }
}

class UnsNet
{
    static void WriteObjectId(ScriptRPC rpc, Object obj)
    {
        int lo = 0;
        int hi = 0;
        obj.GetNetworkID(lo, hi);
        rpc.Write(lo);
        rpc.Write(hi);
    }

    static Object ReadObjectId(ParamsReadContext ctx)
    {
        int lo = 0;
        int hi = 0;
        if (!ctx.Read(lo) || !ctx.Read(hi))
            return null;
        return GetGame().GetObjectByNetworkId(lo, hi);
    }

    static void SendCarPlay(Object car)
    {
        if (!car)
            return;
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(UnsProto.C2S_CAR_PLAY);
        WriteObjectId(rpc, car);
        rpc.Send(null, UnsProto.RPC_ID, true, null);
    }

    static void SendCarStop(Object car)
    {
        if (!car)
            return;
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(UnsProto.C2S_CAR_STOP);
        WriteObjectId(rpc, car);
        rpc.Send(null, UnsProto.RPC_ID, true, null);
    }

    static void SendCardStep(Object card, int dir)
    {
        if (!card)
            return;
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(UnsProto.C2S_CARD_STEP);
        WriteObjectId(rpc, card);
        rpc.Write(dir);
        rpc.Send(null, UnsProto.RPC_ID, true, null);
    }

    static void SendCardStop(Object card)
    {
        if (!card)
            return;
        ScriptRPC rpc = new ScriptRPC();
        rpc.Write(UnsProto.C2S_CARD_STOP);
        WriteObjectId(rpc, card);
        rpc.Send(null, UnsProto.RPC_ID, true, null);
    }
}

// Музыкальная карта. Конкретные альбомы — классы UnesennyeMusicCard_<AlbumId> в config.cpp.
class UnesennyeMusicCard_Base extends ItemBase
{
    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionUnsCardNext);
        AddAction(ActionUnsCardPrev);
        AddAction(ActionUnsCardStop);
    }
}

// Базовое действие: только условие цели и отправка запроса. Без локального проигрывания.
class ActionUnsCarPlay : ActionSingleUseBase
{
    void ActionUnsCarPlay()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
        m_Text = "Включить музыку (Unesennye)";
    }

    override void CreateConditionComponents()
    {
        m_ConditionItem = new CCINone();
        m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        return CarScript.Cast(target.GetObject()) != null;
    }

    override void OnExecuteClient(ActionData action_data)
    {
        UnsNet.SendCarPlay(action_data.m_Target.GetObject());
    }
}

class ActionUnsCarStop : ActionSingleUseBase
{
    void ActionUnsCarStop()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
        m_Text = "Выключить музыку (Unesennye)";
    }

    override void CreateConditionComponents()
    {
        m_ConditionItem = new CCINone();
        m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        return CarScript.Cast(target.GetObject()) != null;
    }

    override void OnExecuteClient(ActionData action_data)
    {
        UnsNet.SendCarStop(action_data.m_Target.GetObject());
    }
}

class ActionUnsCardBase : ActionSingleUseBase
{
    void ActionUnsCardBase()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
    }

    override void CreateConditionComponents()
    {
        m_ConditionItem = new CCINone();
        m_ConditionTarget = new CCTNone();
    }

    // Карта должна быть в руках/инвентаре того же игрока (проверка повторяется на сервере).
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        UnesennyeMusicCard_Base card = UnesennyeMusicCard_Base.Cast(target.GetObject());
        if (!card)
            return false;
        return card.GetHierarchyRootPlayer() == player;
    }
}

class ActionUnsCardNext : ActionUnsCardBase
{
    void ActionUnsCardNext()
    {
        m_Text = "Следующий трек (Unesennye)";
    }

    override void OnExecuteClient(ActionData action_data)
    {
        UnsNet.SendCardStep(action_data.m_Target.GetObject(), 1);
    }
}

class ActionUnsCardPrev : ActionUnsCardBase
{
    void ActionUnsCardPrev()
    {
        m_Text = "Предыдущий трек (Unesennye)";
    }

    override void OnExecuteClient(ActionData action_data)
    {
        UnsNet.SendCardStep(action_data.m_Target.GetObject(), -1);
    }
}

class ActionUnsCardStop : ActionUnsCardBase
{
    void ActionUnsCardStop()
    {
        m_Text = "Остановить музыку (Unesennye)";
    }

    override void OnExecuteClient(ActionData action_data)
    {
        UnsNet.SendCardStop(action_data.m_Target.GetObject());
    }
}

modded class CarScript
{
    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionUnsCarPlay);
        AddAction(ActionUnsCarStop);
    }
}
