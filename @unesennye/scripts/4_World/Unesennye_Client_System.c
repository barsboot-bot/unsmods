// Author: KRa Tos (Константин) | Project: Unesennye
// =====================================================================
// UNSENNYE MUSIC SYSTEM — КЛИЕНТСКАЯ ЧАСТЬ (v1.0.0)
// Handshake-авторизация (таймаут 4000 мс, проверка каждые 500 мс),
// блокировка + Disconnect при отсутствии серверного мода,
// UI радио, проигрывание треков через SEffectManager.PlaySound().
// Автор: KRa Tos (Константин)
// =====================================================================

// ===== RPC-протокол (идентичная копия в @unesennye_servermod/scripts/1_Core/init.c) =====
enum UnesennyeRPC
{
    HS_REQUEST      = 100,
    TRACK_LIST_REQ  = 101,
    RADIO_PLAY      = 102,
    RADIO_STOP      = 103,
    HS_RESPONSE     = 200,
    AUTH_FAIL       = 201,
    TRACK_LIST_RESP = 202,
    BROADCAST_PLAY  = 203,
    BROADCAST_STOP  = 204
};

static const int    UNSENNYE_PROTO_VERSION  = 1;
static const string UNSENNYE_AUTHOR_NAME    = "KRa Tos (Константин)";
static const string UNSENNYE_PROJECT_NAME   = "Unesennye Music System";
static const string UNSENNYE_MISSING_SRV_MSG = "Error: Required server mod 'unesennye_servermod' is missing.";

// ---------- Клиентская система авторизации ----------
class UnesennyeClientAuthClass
{
    // Критические параметры защиты (по ТЗ)
    static const int AUTH_TIMEOUT_MS = 4000;  // ждём ответ сервера максимум 4 секунды
    static const int CHECK_INTERVAL_MS = 500; // период опроса таймера — 500 мс

    private int   m_ClientId;        // мой challenge id для handshake
    private bool  m_HandshakeSent;
    private bool  m_Authorized;
    private bool  m_Blocked;         // уже выполнили блокировку+disconnect
    private int   m_StartTimeMs;

    void UnesennyeClientAuthClass()
    {
        m_ClientId = 0;
        m_HandshakeSent = false;
        m_Authorized = false;
        m_Blocked = false;
        m_StartTimeMs = 0;
    }

    bool IsAuthorized() { return m_Authorized; }
    bool IsBlocked()    { return m_Blocked; }

    // Отправка handshake-запроса на сервер
    void StartHandshake()
    {
        if (m_HandshakeSent || m_Blocked) return;

        m_ClientId = MathRandom(1, 999999); // challenge
        m_HandshakeSent = true;
        m_StartTimeMs = MathFloor(GetGame().GetTime() * 0.001);

        ParamWriteBuffer req = new ParamWriteBuffer;
        req.WriteInt(m_ClientId);
        req.WriteInt(UNSENNYE_PROTO_VERSION);
        // Отправка на сервер по ID протокола (НЕ по имени функции)
        GetGame().RPCSingleParam(0, UnesennyeRPC.HS_REQUEST, req, RPCTargetGroup.ServerOnly);

        Print(string.Format("[Unesennye] Handshake sent (client=%d). Waiting max %d ms...",
            m_ClientId, AUTH_TIMEOUT_MS));

        // Планировщик проверки таймера: CallLater(this, delay, repeat, ref-имя метода),
        // интервал — 500 мс (повтор внутри метода-самопланировщика)
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(
            this, CHECK_INTERVAL_MS, false, "CheckTimeout");
    }

    // Вызывается планировщиком каждые 500 мс, пока нет ответа
    void CheckTimeout()
    {
        if (m_Authorized || m_Blocked) return; // выход из цикла — повторный план не ставим

        int elapsed = MathFloor(GetGame().GetTime() * 0.001) - m_StartTimeMs;

        if (elapsed >= AUTH_TIMEOUT_MS)
        {
            // Сервер НЕ ответил за 4 секунды => @unesennye_servermod не установлен
            PerformLockdown();
            return;
        }

        // Ещё ждём — планируем следующую проверку через 500 мс
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(
            this, CHECK_INTERVAL_MS, false, "CheckTimeout");
    }

    // Ответ сервера получен — сверяем challenge/response токен
    void OnAuthResponse(RPCParamContext context, ParamReadBuffer buf)
    {
        if (m_Blocked) return;

        int clientId = buf.ReadInt();
        int tokenA   = buf.ReadInt();
        int tokenB   = buf.ReadInt();

        if (clientId != m_ClientId) return; // нам пришёл чужой ответ — игнор
        // Сверяем образ секрета: tokenA должен совпасть с ожидаемым по нашему challenge
        if (tokenA != MakeToken(m_ClientId, ServerNonceHint())) return; // подделка — игнор

        m_Authorized = true;
        Print(string.Format("[Unesennye] Auth OK from server. Author: %s", UNSENNYE_AUTHOR_NAME));

        // Разрешаем UI радио
        UnesennyeRadioUI.GetOrCreate().SetEnabled(true);
    }

    // Блокировка всех функций + принудительное отключение от сервера
    private void PerformLockdown()
    {
        if (m_Blocked) return;
        m_Blocked = true;
        m_Authorized = false;

        Print("[Unesennye] LOCKDOWN: " + UNSENNYE_MISSING_SRV_MSG);

        // 1. Гасим всё локальное звучание радио
        UnesennyeCarRadio radio = UnesennyeCarRadio.GetOrCreate();
        radio.ForceStopAll();

        // 2. Полностью режем UI
        UnesennyeRadioUI ui = UnesennyeRadioUI.GetOrCreate();
        ui.SetEnabled(false);
        ui.Close();

        // 3. Принудительный disconnect с сообщением об ошибке
        ShowMessageOnScreen(UNSENNYE_MISSING_SRV_MSG);
        GetGame().Disconnect(); // клиентское отключение без краша сервера
    }

    // Простой on-screen вывод (через стандартный Chat/RPT-канал клиента)
    private void ShowMessageOnScreen(string msg)
    {
        Print("[Unesennye][FATAL] " + msg);
        // Дублируем в игровой чат если он доступен
        ChatBase chat = ChatBase.Cast(GetGame().GetGUIController().GetElementByName("ChatBase"));
        if (chat)
        {
            chat.AddChatSender(msg);
        }
    }

    // Тот же алгоритм токена, что на сервере (для сверки образа)
    private int MakeToken(int seed, int nonce)
    {
        int mixed = (seed * 2654435761) ^ (nonce * 40503) ^ (UNSENNYE_PROTO_VERSION * 99991);
        return mixed & 0x7FFFFFFF;
    }

    // Сервер использует nonce = clientId + PROTO_VERSION (детерминированно, см. сервер)
    private int ServerNonceHint()
    {
        return m_ClientId + UNSENNYE_PROTO_VERSION;
    }
}

// ---------- Реестр активных звуковых источников радио ----------
class UnesennyeRadioSource
{
    int carID;
    int trackID;
    ref SoundSource source; // активный источник SEffectManager

    void ~UnesennyeRadioSource()
    {
        Stop();
    }

    void Stop()
    {
        if (source)
        {
            source.Stop();
            delete source;
            source = null;
        }
    }
}

// ---------- Логика воспроизведения авто-радио ----------
class UnesennyeCarRadio
{
    static ref UnesennyeCarRadio s_Instance;

    ref array<ref UnesennyeRadioSource> m_Sources; // по одному источнику на машину

    static UnesennyeCarRadio GetOrCreate()
    {
        if (!s_Instance) s_Instance = new UnesennyeCarRadio;
        return s_Instance;
    }

    static void DestroyInstance()
    {
        if (s_Instance)
        {
            s_Instance.ForceStopAll();
            delete s_Instance;
            s_Instance = null;
        }
    }

    void UnesennyeCarRadio()
    {
        m_Sources = new array<ref UnesennyeRadioSource>;
    }

    void ~UnesennyeCarRadio()
    {
        ForceStopAll();
        delete m_Sources;
        m_Sources = null;
    }

    private UnesennyeRadioSource FindByCar(int carID)
    {
        foreach (UnesennyeRadioSource s : m_Sources)
        {
            if (s.carID == carID) return s;
        }
        return null;
    }

    // Запуск трека в машине (вызывается из broadcast-обработчика)
    void PlayTrackInCar(int carID, int trackID)
    {
        CarScript car = Cast<CarScript>(GetEntityFromID(carID));
        if (!car) return;

        // Уже играет этот трек — ничего не делаем
        UnesennyeRadioSource existing = FindByCar(carID);
        if (existing && existing.trackID == trackID && existing.source) return;
        if (existing) existing.Stop();

        // Конфиг трека: N-й класс CfgUnesennyeTracks (track_<N>) -> soundSet.
        // Клиент читает тот же config.cpp мода @unesennye_music_db.
        string clsName = ConfigGetClassName(trackID, "CfgUnesennyeTracks");
        if (clsName == "") return;
        string soundSet = ConfigReadString(clsName + "\\soundSet", "");
        if (soundSet.Length() == 0) return;

        // 3D-звук из позиции машины — ТОЛЬКО SEffectManager.PlaySound (движковый API)
        vector pos = car.GetPosition();
        SoundSource snd = SEffectManager.PlaySound(soundSet, pos[0], pos[1], pos[2]);
        if (!snd) return;
        snd.SetSoundAutodestroy(true);

        UnesennyeRadioSource src = new UnesennyeRadioSource;
        src.carID = carID;
        src.trackID = trackID;
        src.source = snd;
        m_Sources.Insert(src);
    }

    void StopTrackInCar(int carID)
    {
        UnesennyeRadioSource s = FindByCar(carID);
        if (s)
        {
            s.Stop();
            for (int i = 0; i < m_Sources.Count(); i++)
            {
                if (m_Sources[i].carID == carID)
                {
                    delete m_Sources[i];
                    m_Sources.Remove(i);
                    break;
                }
            }
        }
    }

    void ForceStopAll()
    {
        for (int i = 0; i < m_Sources.Count(); i++)
        {
            m_Sources[i].Stop();
            delete m_Sources[i];
        }
        m_Sources.Clear();
    }
}

// ---------- Минимальный UI радио (управление в машине) ----------
class UnesennyeRadioUI
{
    static ref UnesennyeRadioUI s_Instance;

    private bool m_Enabled;      // false до успешного handshake
    private bool m_Open;
    private Widget m_RootWidget;

    static UnesennyeRadioUI GetOrCreate()
    {
        if (!s_Instance) s_Instance = new UnesennyeRadioUI;
        return s_Instance;
    }

    static void DestroyInstance()
    {
        if (s_Instance)
        {
            s_Instance.Close();
            delete s_Instance;
            s_Instance = null;
        }
    }

    bool IsEnabled() { return m_Enabled; }

    void SetEnabled(bool enable)
    {
        m_Enabled = enable;
        if (!enable && m_RootWidget)
        {
            Close();
        }
    }

    void Open()
    {
        if (!m_Enabled) return; // до авторизации UI недоступен
        if (m_Open) return;
        m_Open = true;

        // Создаём панель через стандартный GUI controller DayZ
        m_RootWidget = GetGame().GetGUIController().CreateWidget(Button, "UnesennyeRadioPanel", "Button", "Full");
        if (m_RootWidget)
        {
            GetGame().GetGUIController().AddChild(GetGame().GetGUIController().GetRootComponent(3, 0), m_RootWidget);
            m_RootWidget.SetText("Unesennye Radio | v1.0.0 | by KRa Tos");
        }
    }

    void Close()
    {
        m_Open = false;
        if (m_RootWidget)
        {
            GetGame().GetGUIController().RemoveChild(GetGame().GetGUIController().GetRootComponent(3, 0), m_RootWidget);
            m_RootWidget = null;
        }
    }

    // Команда игроку: играть трек (только после авторизации!)
    void RequestPlayTrack(int carID, int trackID)
    {
        if (!m_Enabled) return;
        ParamWriteBuffer req = new ParamWriteBuffer;
        req.WriteInt(carID);
        req.WriteInt(trackID);
        GetGame().RPCSingleParam(0, UnesennyeRPC.RADIO_PLAY, req, RPCTargetGroup.ServerOnly);
    }

    void RequestTrackList(int carID)
    {
        if (!m_Enabled) return;
        ParamWriteBuffer req = new ParamWriteBuffer;
        req.WriteInt(MathRandom(1, 999999)); // clientId запроса
        GetGame().RPCSingleParam(0, UnesennyeRPC.TRACK_LIST_REQ, req, RPCTargetGroup.ServerOnly);
    }

    void RequestStopRadio(int carID)
    {
        if (!m_Enabled) return;
        ParamWriteBuffer req = new ParamWriteBuffer;
        req.WriteInt(carID);
        GetGame().RPCSingleParam(0, UnesennyeRPC.RADIO_STOP, req, RPCTargetGroup.ServerOnly);
    }
}

// ---------- Глобальный обработчик входящих CLIENT RPC ----------
class UnesennyeClientRPC
{
    static ref UnesennyeClientAuthClass g_Auth;

    static UnesennyeClientAuthClass GetAuth()
    {
        if (!g_Auth) g_Auth = new UnesennyeClientAuthClass;
        return g_Auth;
    }

    static void InitRpc()
    {
        // Регистрация CLIENT RPC по ID из enum UnesennyeRPC
        GetGame().RegisterClientRpc(UnesennyeRPC.HS_RESPONSE,     "UnesennyeClientRPC", "OnHSResponse");
        GetGame().RegisterClientRpc(UnesennyeRPC.AUTH_FAIL,       "UnesennyeClientRPC", "OnAuthFail");
        GetGame().RegisterClientRpc(UnesennyeRPC.TRACK_LIST_RESP, "UnesennyeClientRPC", "OnTrackList");
        GetGame().RegisterClientRpc(UnesennyeRPC.BROADCAST_PLAY,  "UnesennyeClientRPC", "OnBroadcastPlay");
        GetGame().RegisterClientRpc(UnesennyeRPC.BROADCAST_STOP,  "UnesennyeClientRPC", "OnBroadcastStop");
        Print(string.Format("[Unesennye] Client RPC registered. Author: %s", UNSENNYE_AUTHOR_NAME));
    }

    static void OnHSResponse(RPCParamContext context, ParamReadBuffer buf)
    {
        GetAuth().OnAuthResponse(context, buf);
    }

    static void OnAuthFail(RPCParamContext context, ParamReadBuffer buf)
    {
        Print("[Unesennye] Server refused auth.");
        GetAuth().StartHandshake(); // одна повторная попытка
    }

    static void OnTrackList(RPCParamContext context, ParamReadBuffer buf)
    {
        int clientId = buf.ReadInt();
        int count = buf.ReadInt();
        for (int i = 0; i < count; i++)
        {
            int trackID = buf.ReadInt();
            string title = buf.ReadString();
            Print(string.Format("[Unesennye] Track #%d: %s", trackID, title));
        }
    }

    static void OnBroadcastPlay(RPCParamContext context, ParamReadBuffer buf)
    {
        if (!GetAuth().IsAuthorized()) return; // защита: игнорируем без авторизации
        int carID = buf.ReadInt();
        int trackID = buf.ReadInt();
        UnesennyeCarRadio.GetOrCreate().PlayTrackInCar(carID, trackID);
    }

    static void OnBroadcastStop(RPCParamContext context, ParamReadBuffer buf)
    {
        int carID = buf.ReadInt();
        UnesennyeCarRadio.GetOrCreate().StopTrackInCar(carID);
    }
}

// ---------- Модификация CarScript: точка входа радио в машинах ----------
modded class CarScript
{
    // Вызывается из PlayerBase.UnesennyeCarRadioHook при посадке локального игрока
    void UnesennyeOnPlayerEnters()
    {
        if (GetGame().IsDedicated()) return;
        if (!UnesennyeClientRPC.GetAuth().IsAuthorized()) return;
        UnesennyeRadioUI ui = UnesennyeRadioUI.GetOrCreate();
        ui.Open();
        ui.RequestTrackList(GetID());
    }

    // Уничтожение машины — гасим звуковой источник, никаких висячих SoundSource
    override void EOnDestroy(IEntity data0, IEntity data1)
    {
        super.EOnDestroy(data0, data1);
        if (!GetGame().IsDedicated())
        {
            UnesennyeCarRadio.GetOrCreate().StopTrackInCar(GetID());
        }
    }
}


// ---------- Хук входа в машину: официальный экшен DayZ ----------
modded class ActionEntryToCar
{
    override void OnExecuteSuccess(PlayerBase player)
    {
        super.OnExecuteSuccess(player);
        if (GetGame().IsDedicated() || !player || player != GetGame().GetPlayer()) return;
        // Машина, в которую сел локальный игрок — ищем рядом через GetObjectsAtPosition
        ref array<IEntity> items = new array<IEntity>;
        vector pos = player.GetPosition();
        GetGame().GetObjectsAtPosition(pos, 2.0, 2.0, 2.0, items, null, null);
        for (int i = 0; i < items.Count(); i++)
        {
            CarScript cs = Cast<CarScript>(items[i]);
            if (cs)
            {
                cs.UnesennyeOnPlayerEnters();
                break;
            }
        }
    }
}

// ---------- Инициализация клиента при заходе на сервер ----------
modded class PlayerBase
{
    override void OnBaseCreated()
    {
        super.OnBaseCreated();
        if (GetGame().IsMultiplayer() && !GetGame().IsDedicated())
        {
            // Стартуем handshake сразу при появлении базы игрока
            UnesennyeClientRPC.GetAuth().StartHandshake();
        }
    }

    override void OnPlayerDestroyed()
    {
        super.OnPlayerDestroyed();
        // Очистка ресурсов при выходе с сервера — без утечек
        UnesennyeCarRadio.DestroyInstance();
        UnesennyeRadioUI.DestroyInstance();
    }
}

// ---------- Регистрация client RPC при старте приложения ----------
modded class MissionGameplay
{
    override void OnInit()
    {
        super.OnInit();
        UnesennyeClientRPC.InitRpc();
    }
}
