// Author: KRa Tos (Константин) | Project: Unesennye
// =====================================================================
// UNSENNYE MUSIC SYSTEM — ПОДДЕРЖКА РАЦИЙ (v1.1.0, расширение)
// НОВЫЙ файл: существующая автомобильная система (Unesennye_Client_System.c)
// НЕ изменена. Машины и рации работают ПАРАЛЛЕЛЬНО и НЕЗАВИСИМО.
// Защита: переиспользуется существующий Handshake (AUTH_TIMEOUT_MS=4000,
// проверка каждые 500 мс, PerformLockdown -> GetGame().Disconnect()).
// Без @unesennye_servermod команды рации не проходят авторизацию.
// Автор: KRa Tos (Константин)
// =====================================================================

// ===== RPC-протокол раций: НОВЫЕ ID с 300 =====
// (ID 100-204 зарезервированы за автомобильной системой — НЕ ТРОГАТЬ!)
enum UnesennyeRadioRPC
{
    // Клиент -> Сервер
    RADIO_INSERT_CARD = 300,   // вставить флешку (radioID:int, trackID:int)
    RADIO_EJECT_CARD = 301,    // извлечь флешку (radioID:int)
    RADIO_PLAY_TRACK = 302,    // включить трек из рации (radioID:int, trackID:int)
    RADIO_STOP_TRACK = 303,    // выключить трек из рации (radioID:int)

    // Сервер -> Клиенты
    RADIO_BROADCAST  = 304     // ретрансляция (radioID:int, trackID:int, action:int)
};

static const int UNSENNYE_RADIO_PROTO_VERSION = 1;
static const string UNSENNYE_RADIO_AUTHOR_NAME = "KRa Tos (Константин)"; // водяной знак

// ---------- Менеджер портативного аудио (активные звуки по ID рации) ----------
class UnesennyePortableRadioManager
{
    private ref array<ref EffectSound> m_Sounds; // активные звуки, индекс = radioNetID
    private const int MAX_TRACKS = 64;           // резерв под треки CfgUnesennyeTracks

    void UnesennyePortableRadioManager()
    {
        m_Sounds = new array<ref EffectSound>;
        m_Sounds.Resize(MAX_TRACKS);
        for (int i = 0; i < MAX_TRACKS; i++)
            m_Sounds[i] = null;
    }

    void ~UnesennyePortableRadioManager()
    {
        StopAll();
        delete m_Sounds;
        m_Sounds = null;
    }

    // Индекс хранилища для сетевого ID рации
    private int SlotOf(int radioID)
    {
        return Math.Abs(radioID) % MAX_TRACKS;
    }

    // PlayTrackFromRadio — 3D-звук из позиции рации (только SEffectManager.PlaySound)
    void PlayTrackFromRadio(int radioID, string soundSet)
    {
        if (!soundSet || soundSet.Length() == 0) return;

        IEntity radioEnt = GetGame().FindEntity(radioID);
        vector pos;
        if (radioEnt)
            pos = radioEnt.GetPosition();
        else
            pos = GetGame().GetPlayer().GetPosition(); // рация в инвентаре — звук от игрока

        int slot = SlotOf(radioID);

        // Останавливаем предыдущий трек этой же рации — без утечек EffectSound
        if (m_Sounds[slot])
        {
            m_Sounds[slot].Stop();
            delete m_Sounds[slot];
            m_Sounds[slot] = null;
        }

        EffectSound snd = SEffectManager.PlaySound(soundSet, pos);
        if (snd)
        {
            snd.SetSoundAutodestroy(true); // движок сам освободит источник
            m_Sounds[slot] = snd;
            Print(string.Format("[Unesennye Radio] Playing '%s' at radio %d. Author: %s",
                soundSet, radioID, UNSENNYE_RADIO_AUTHOR_NAME));
        }
    }

    // StopTrackFromRadio — корректное уничтожение источника
    void StopTrackFromRadio(int radioID)
    {
        int slot = SlotOf(radioID);
        if (m_Sounds[slot])
        {
            m_Sounds[slot].Stop();
            delete m_Sounds[slot];
            m_Sounds[slot] = null;
            Print(string.Format("[Unesennye Radio] Stopped radio %d.", radioID));
        }
    }

    // Полная остановка всех звуков (lockdown / выход из игры)
    void StopAll()
    {
        for (int i = 0; i < MAX_TRACKS; i++)
        {
            if (m_Sounds[i])
            {
                m_Sounds[i].Stop();
                delete m_Sounds[i];
                m_Sounds[i] = null;
            }
        }
    }

    bool IsPlaying(int radioID)
    {
        return m_Sounds[SlotOf(radioID)] != null;
    }
};

// ---------- Синглтон менеджера + исходящие RPC раций ----------
class UnesennyeRadioClient
{
    static ref UnesennyePortableRadioManager g_PortableRadioManager;

    static UnesennyePortableRadioManager GetManager()
    {
        if (!g_PortableRadioManager)
            g_PortableRadioManager = new UnesennyePortableRadioManager;
        return g_PortableRadioManager;
    }

    static void InitRpc()
    {
        // Регистрация CLIENT RPC по числовому ID (авто-регистрация по имени не используется)
        GetGame().RegisterClientRpc(UnesennyeRadioRPC.RADIO_BROADCAST, UnesennyeRadioClient, "OnBroadcastPlay");
        Print(string.Format("[Unesennye Radio] Client RPC registered (300-304). Author: %s", UNSENNYE_RADIO_AUTHOR_NAME));
    }

    // ===== Исходящие запросы (Param1/Param2 вместо буферов) =====
    static void RequestInsertCard(int radioID, int trackID)
    {
        // Защита: без успешного handshake (4000 мс / опрос 500 мс) ничего не отправляем
        if (!UnesennyeClientRPC.GetAuth().IsAuthorized()) return;
        GetGame().RPCSingleParam(null, UnesennyeRadioRPC.RADIO_INSERT_CARD, new Param2<int, int>(radioID, trackID));
    }

    static void RequestEjectCard(int radioID)
    {
        if (!UnesennyeClientRPC.GetAuth().IsAuthorized()) return;
        GetGame().RPCSingleParam(null, UnesennyeRadioRPC.RADIO_EJECT_CARD, new Param1<int>(radioID));
    }

    static void RequestPlayTrack(int radioID, int trackID)
    {
        if (!UnesennyeClientRPC.GetAuth().IsAuthorized()) return;
        GetGame().RPCSingleParam(null, UnesennyeRadioRPC.RADIO_PLAY_TRACK, new Param2<int, int>(radioID, trackID));
    }

    static void RequestStopTrack(int radioID)
    {
        if (!UnesennyeClientRPC.GetAuth().IsAuthorized()) return;
        GetGame().RPCSingleParam(null, UnesennyeRadioRPC.RADIO_STOP_TRACK, new Param1<int>(radioID));
    }

    // ===== Входящий SERVER->CLIENT broadcast (action: 1=play, 0=stop) =====
    static void OnBroadcastPlay(RPCParamContext context, ParamsReadContext buf)
    {
        // Общая защита системы: без авторизации игнорируем (handshake обязателен)
        if (!UnesennyeClientRPC.GetAuth().IsAuthorized()) return;

        Param2<int, int> p = new Param2<int, int>;
        if (!buf.Read(p)) return;
        int radioID = p.arg1;
        int trackID = p.arg2;

        if (trackID == 0)
        {
            GetManager().StopTrackFromRadio(radioID);
            return;
        }

        string clsName = ConfigGetClassName(trackID, "CfgUnesennyeTracks");
        if (clsName == "") return;
        string soundSet = ConfigReadString(clsName + "\\soundSet", "");
        if (soundSet == "") return;

        GetManager().PlayTrackFromRadio(radioID, soundSet);
    }
};

// ---------- Модификация RadioBase: методы работы с музыкальной флешкой ----------
// Только НОВые методы с префиксом Unesennye — стандартная логика рации не затронута.
modded class RadioBase
{
    ref array<int> UnesennyeCards; // индексы треков CfgUnesennyeTracks во флешках

    // Инициализируем хранилище карточек при создании рации (на обеих сторонах)
    override void EInit()
    {
        super.EInit();
        if (!UnesennyeCards)
            UnesennyeCards = new array<int>;
    }

    // Вставить флешку с треком (trackIndex = индекс класса track_N)
    void UnesennyeInsertCard(int trackIndex)
    {
        if (!UnesennyeCards) UnesennyeCards = new array<int>;
        // Не более 5 карточек на рацию
        if (UnesennyeCards.Count() >= 5) return;
        // Валидация локально: трек существует в базе?
        string clsName = ConfigGetClassName(trackIndex, "CfgUnesennyeTracks");
        if (clsName == "" || ConfigReadString(clsName + "\\file", "").Length() == 0) return;

        UnesennyeCards.Insert(trackIndex);
        // Просим сервер подтвердить/зарегистрировать (анти-спам и авторизация на сервере)
        UnesennyeRadioClient.RequestInsertCard(GetID(), trackIndex);
    }

    // Извлечь все флешки (останавливает воспроизведение)
    void UnesennyeEjectCard()
    {
        if (UnesennyeCards) UnesennyeCards.Clear();
        UnesennyeRadioClient.RequestEjectCard(GetID());
        UnesennyeRadioClient.GetManager().StopTrackFromRadio(GetID());
    }

    // Включить музыку: играет последняя вставленная карточка
    void UnesennyePlay()
    {
        if (!UnesennyeCards || UnesennyeCards.Count() == 0) return;
        int trackIdx = UnesennyeCards[UnesennyeCards.Count() - 1];
        UnesennyeRadioClient.RequestPlayTrack(GetID(), trackIdx);
    }

    // Выключить музыку
    void UnesennyeStop()
    {
        UnesennyeRadioClient.RequestStopTrack(GetID());
    }

    // Уничтожение рации — гасим её звук, никаких висячих EffectSound
    override void EOnDestroy(IEntity data0, IEntity data1)
    {
        super.EOnDestroy(data0, data1);
        if (!GetGame().IsDedicated())
        {
            UnesennyeRadioClient.GetManager().StopTrackFromRadio(GetID());
        }
    }
};

// ---------- Предмет «Музыкальная флешка» (сторона клиента) ----------
// Класс описан в config.cpp мода @unesennye_music_db (CfgVehicles.UnesennyeMusicCard_Base).
// Модинг базового класса покрывает все 5 карточек UnesennyeMusicCard0..4.
modded class UnesennyeMusicCard_Base
{
    // Использование флешки рядом с рацией: ищем RadioBase через GetObjectsAtPosition
    override void ActionAttach(out UserContext context)
    {
        super.ActionAttach(context);
        if (GetGame().IsDedicated()) return;
        if (!UnesennyeClientRPC.GetAuth().IsAuthorized()) return;

        PlayerBase player = GetGame().GetPlayer();
        if (!player) return;
        vector pos = player.GetPosition();

        ref array<IEntity> items = new array<IEntity>;
        GetGame().GetObjectsAtPosition(pos, 2.0, 2.0, 2.0, items, null, null);
        for (int i = 0; i < items.Count(); i++)
        {
            RadioBase rb = Cast<RadioBase>(items[i]);
            if (rb)
            {
                // Индекс трека = порядковый номер класса track_<N>
                int trackIdx = GetConfigTrackIndex();
                rb.UnesennyeInsertCard(trackIdx);
                rb.UnesennyePlay();
                ShowMessageOnScreen(string.Format("Unesennye: флешка вставлена, трек #%d. Author: %s",
                    trackIdx, UNSENNYE_RADIO_AUTHOR_NAME));
                break;
            }
        }
    }

    // Индекс трека этой флешки из конфига (без хардкода — обновляется только конфигом)
    private int GetConfigTrackIndex()
    {
        string name = type(this).GetName(); // UnesennyeMusicCard0 ... UnesennyeMusicCard4
        for (int i = 0; i < 5; i++)
        {
            if (name == "UnesennyeMusicCard" + i) return i;
        }
        return 0;
    }

    private void ShowMessageOnScreen(string msg)
    {
        ChatBase chat = ChatBase.Cast(GetGame().GetGUIController().GetElementByName("ChatBase"));
        if (chat) chat.AddChatSender(msg);
    }
};

// ---------- Очистка и регистрация ----------
modded class PlayerBase
{
    override void OnBaseDestroyed()
    {
        super.OnBaseDestroyed();
        // Освобождение звуков раций при выходе — без утечек памяти
        if (UnesennyeRadioClient.g_PortableRadioManager)
        {
            UnesennyeRadioClient.g_PortableRadioManager.StopAll();
            delete UnesennyeRadioClient.g_PortableRadioManager;
            UnesennyeRadioClient.g_PortableRadioManager = null;
        }
    }
}

modded class MissionGameplay
{
    override void OnInit()
    {
        super.OnInit();
        // Регистрация RPC раций поверх существующей инициализации авто-системы
        UnesennyeRadioClient.InitRpc();
    }
}
