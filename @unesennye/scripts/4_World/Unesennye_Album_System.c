// Author: KRa Tos (Константин) | Project: Unesennye
// =====================================================================
// UNSENNYE MUSIC SYSTEM — МУЛЬТИ-ТРЕКОВЫЕ ФЛЕШКИ / АЛЬБОМЫ (v1.2.0, расширение)
// НОВЫЙ файл: существующие системы НЕ изменены:
//   - автомобили (Unesennye_Client_System.c, RPC 100-204) — как есть
//   - рации и старые одно-трековые флешки (Unesennye_Radio_System.c, RPC 300-305) — как есть
// Альбомные флешки работают ПАРАЛЛЕЛЬНО со старыми (обратная совместимость).
// Защита: переиспользуется существующий Handshake (AUTH_TIMEOUT_MS=4000,
// опрос каждые 500 мс, PerformLockdown -> GetGame().Disconnect()).
// Текущий трек хранится в Item Variables флешки (SetVar/GetVar) — данные
// выживают при дропе предмета и перезаходе игрока.
// Автор: KRa Tos (Константин)
// =====================================================================

// ===== RPC-протокол альбомов: ID 400-406 =====
// (ID 100-204 — авто, 300-305 — рации: НЕ ТРОГАТЬ!)
// enum UnesennyeAlbumRPC определён ниже — единый источник ID.

static const int    UNSENNYE_ALBUM_PROTO_VERSION = 1;
static const string UNSENNYE_ALBUM_AUTHOR_NAME   = "KRa Tos (Константин)"; // водяной знак
static const int    UNSENNYE_ALBUM_MAX_TRACKS    = 40;   // шаг нумерации глобальных id (>= макс. треков, без коллизий альбомов)
static const int    UNSENNYE_ALBUM_SWITCH_CD_MS  = 2000;                  // локальный анти-спам переключения

// Item Variable ключи (префикс мода — конфликтов с другими модами нет)
static const string UAS_VAR_TRACK = "Unesennye_CurrentTrack"; // int: индекс текущего трека альбома
static const string UAS_VAR_RADIO = "Unesennye_ActiveRadio";  // int: сетевой ID активной рации

// ===== RPC-протокол альбомов (тот же enum определён в серверном моде) =====
enum UnesennyeAlbumRPC
{
    ALBUM_NEXT_TRACK     = 400, // следующий трек альбома (radioID, cardNetID)
    ALBUM_PREV_TRACK     = 401, // предыдущий трек альбома (radioID, cardNetID)
    ALBUM_SET_TRACK      = 402, // конкретный трек (radioID, globalTrackID)
    ALBUM_GET_TRACKLIST  = 403, // список треков альбома (albumIdx)
    ALBUM_BROADCAST      = 404, // сервер -> клиенты: играть (radioID, trackID)
    ALBUM_TRACKLIST_RESP = 405, // сервер -> клиент: список треков
    ALBUM_REJECT         = 406  // сервер -> клиент: команда отклонена
};

// ---------- Скрытая метка авторства (альбомный модуль) ----------
static string GetAlbumModAuthor()
{
    return UNSENNYE_ALBUM_AUTHOR_NAME; // "KRa Tos (Константин)"
}

// ---------- Менеджер альбомов: чтение конфига + Item Variables ----------
class UnesennyeAlbumManager
{
    // Индекс класса альбома в CfgUnesennyeAlbums по имени (Album01 -> 0)
    static int FindAlbumIndex(string albumName)
    {
        if (albumName == "") return -1;
        int count = ConfigGetClassCount("CfgUnesennyeAlbums");
        for (int i = 0; i < count; i++)
        {
            string cname = ConfigGetClassName(i, "CfgUnesennyeAlbums");
            if (cname == albumName) return i;
        }
        return -1;
    }

    // Название альбома из класса флешки (CfgVehicles.<type>.unesennyeAlbumId)
    static string GetCardAlbumName(ItemBase card)
    {
        if (!card) return "";
        string type = card.GetType();
        return ConfigReadString("CfgVehicles\\" + type + "\\unesennyeAlbumId", "");
    }

    // Кол-во треков в альбоме (из конфига, без хардкода)
    static int GetAlbumTrackCount(string albumId)
    {
        if (albumId == "") return 0;
        int n = ConfigReadInt("CfgUnesennyeAlbums\\" + albumId + "\\trackCount", 0);
        if (n > 20) n = 20; // физический лимит альбома — 20 треков
        return n;
    }

    // Класс TrackXX альбома по индексу (0-based)
    private static string TrackClassPath(string albumId, int trackIndex)
    {
        return string.Format("CfgUnesennyeAlbums\\%s\\Track%02d", albumId, trackIndex + 1);
    }

    // Путь к образцу (sample) трека
    static string GetAlbumTrackPath(string albumId, int trackIndex)
    {
        if (albumId == "" || trackIndex < 0) return "";
        return ConfigReadString(TrackClassPath(albumId, trackIndex) + "\\sample", "");
    }

    // Название трека
    static string GetAlbumTrackTitle(string albumId, int trackIndex)
    {
        if (albumId == "" || trackIndex < 0) return "";
        return ConfigReadString(TrackClassPath(albumId, trackIndex) + "\\title", "");
    }

    // Звуковой набор трека (пусто = не резолвится напрямую, нужен прогон через реестр)
    static string GetAlbumTrackSoundSet(string albumId, int trackIndex)
    {
        return ConfigReadString(TrackClassPath(albumId, trackIndex) + "\\soundSet", "");
    }

    // ----- Разрешение локального индекса альбома -> запись реестра CfgUnesennyeTracks -----
    // Глобальный id трека = albumIndex*UNSENNYE_ALBUM_MAX_TRACKS + localIndex
    // (Album01->20..29, Album02->40..54, Album03->60..79; см. config.cpp).
    static int GlobalTrackID(string albumName, int localIndex)
    {
        int baseIdx = FindAlbumIndex(albumName);
        if (baseIdx < 0) return -1;
        return baseIdx * UNSENNYE_ALBUM_MAX_TRACKS + localIndex;
    }

    // Звуковой набор трека альбома: берём из CfgUnesennyeTracks\track_<globalID>;
    // если SoundSet ещё не описан в конфиге — генерируем шейдер/сет по sample
    // альбома (уникальный класс на индекс; повторный вызов дёшев за счёт кэша).
    private static string g_ResolveCacheKey;
    private static string g_ResolveCacheValue;

    static string ResolveTrackSoundSet(string albumName, int localIndex)
    {
        string key = albumName + "#" + localIndex;
        if (key == g_ResolveCacheKey) return g_ResolveCacheValue;

        string result = "";
        int gidx = GlobalTrackID(albumName, localIndex);
        if (gidx >= 0)
        {
            string regCls = "CfgUnesennyeTracks\\track_" + gidx;
            if (ConfigIsExisting(regCls))
            {
                result = ConfigReadString(regCls + "\\soundSet", "");
            }
            if (result == "")
            {
                string sample = GetAlbumTrackPath(albumName, localIndex);
                if (sample != "")
                {
                    string gen = "Unesennye_Alb_" + gidx;
                    if (!ConfigIsExisting("CfgSoundShaders\\" + gen + "_Shader"))
                    {
                        ConfigAddClass("CfgSoundShaders", gen + "_Shader", 0);
                        ConfigAddString("CfgSoundShaders\\" + gen + "_Shader", "samples[]", "{\"" + sample + "\", 1}", 0);
                        ConfigAddInt("CfgSoundShaders\\" + gen + "_Shader", "range", 60);
                        ConfigAddFloat("CfgSoundShaders\\" + gen + "_Shader", "volume", 0.85);
                    }
                    if (!ConfigIsExisting("CfgSoundSets\\" + gen + "_SoundSet"))
                    {
                        ConfigAddClass("CfgSoundSets", gen + "_SoundSet", 0);
                        ConfigAddString("CfgSoundSets\\" + gen + "_SoundSet", "soundShaders[]", "{" + gen + "_Shader}", 0);
                        ConfigAddInt("CfgSoundSets\\" + gen + "_SoundSet", "spatial", 1);
                        ConfigAddInt("CfgSoundSets\\" + gen + "_SoundSet", "looped", 1);
                        ConfigAddFloat("CfgSoundSets\\" + gen + "_SoundSet", "volumeFactor", 0.9);
                        ConfigAddFloat("CfgSoundSets\\" + gen + "_SoundSet", "distanceFactor", 1.0);
                    }
                    result = gen + "_SoundSet";
                }
            }
        }
        g_ResolveCacheKey = key;
        g_ResolveCacheValue = result;
        return result;
    }

    // ----- Item Variables: текущий трек хранится НА ПРЕДМЕТЕ (DayZ Wiki best practice) -----
    static int GetCurrentTrack(ItemBase card)
    {
        if (!card) return -1;
        int v = -1;
        if (card.GetVar(UAS_VAR_TRACK, v)) return v;
        return 0; // трек не устанавливался — начинаем с первого
    }

    static void SetCurrentTrack(ItemBase card, int trackIndex)
    {
        if (!card) return;
        card.SetVar(UAS_VAR_TRACK, trackIndex);
    }

    static int GetActiveRadioID(ItemBase card)
    {
        if (!card) return 0;
        int v = 0;
        card.GetVar(UAS_VAR_RADIO, v);
        return v;
    }

    static void SetActiveRadioID(ItemBase card, int radioID)
    {
        if (!card) return;
        card.SetVar(UAS_VAR_RADIO, radioID);
    }
};

// ---------- Исходящие запросы + приёмник broadcast для альбомов ----------
class UnesennyeAlbumClient
{
    // Анти-спам на клиенте: не спамим сервер переключениями (сервер держит свои 2 с)
    private static int s_LastSwitchMs = 0;

    static bool CanSwitch()
    {
        int now = MathFloor(GetGame().GetTime()); // GetTime() уже в мс
        if (now - s_LastSwitchMs < UNSENNYE_ALBUM_SWITCH_CD_MS) return false;
        s_LastSwitchMs = now;
        return true;
    }

    static void InitRpc()
    {
        // Регистрация CLIENT RPC по числовым ID (авто-регистрация по имени не используется)
        GetGame().RegisterClientRpc(UnesennyeAlbumRPC.ALBUM_TRACKLIST_RESP, UnesennyeAlbumClient, "OnTrackListResp");
        GetGame().RegisterClientRpc(UnesennyeAlbumRPC.ALBUM_REJECT, UnesennyeAlbumClient, "OnReject");
        Print(string.Format("[Unesennye Album] Client RPC registered (400-406). Author: %s", UNSENNYE_ALBUM_AUTHOR_NAME));
    }

    static bool Authorized()
    {
        // Общая защита: без успешного handshake (4000 мс / опрос 500 мс) команды не отправляем
        return UnesennyeClientRPC.GetAuth().IsAuthorized();
    }

    static void RequestNext(int radioID, int cardNetID)
    {
        if (!Authorized()) return;
        GetGame().RPCSingleParam(0, UnesennyeAlbumRPC.ALBUM_NEXT_TRACK, new Param2<int, int>(radioID, cardNetID), RPCTargetGroup.ServerOnly);
    }

    static void RequestPrev(int radioID, int cardNetID)
    {
        if (!Authorized()) return;
        GetGame().RPCSingleParam(0, UnesennyeAlbumRPC.ALBUM_PREV_TRACK, new Param2<int, int>(radioID, cardNetID), RPCTargetGroup.ServerOnly);
    }

    // Переключение +/-: сервер читает текущий трек из Item Variable флешки (cardNetID)
    static void RequestSwitch(int radioID, int cardNetID, bool forward)
    {
        if (!Authorized()) return;
        int rpcId = forward ? UnesennyeAlbumRPC.ALBUM_NEXT_TRACK : UnesennyeAlbumRPC.ALBUM_PREV_TRACK;
        GetGame().RPCSingleParam(0, rpcId, new Param2<int, int>(radioID, cardNetID), RPCTargetGroup.ServerOnly);
    }

    static void RequestSetTrack(int radioID, int trackIndex)
    {
        if (!Authorized()) return;
        GetGame().RPCSingleParam(0, UnesennyeAlbumRPC.ALBUM_SET_TRACK, new Param2<int, int>(radioID, trackIndex), RPCTargetGroup.ServerOnly);
    }

    static void RequestTrackList(int albumIdx)
    {
        if (!Authorized()) return;
        GetGame().RPCSingleParam(0, UnesennyeAlbumRPC.ALBUM_GET_TRACKLIST, new Param1<int>(albumIdx), RPCTargetGroup.ServerOnly);
    }

    // ===== 404 ALBUM_BROADCAST =====
    // Формат параметров (radioID:int, trackID:int) идентичен RADIO_BROADCAST(304),
    // поэтому его штатно обрабатывает СУЩЕСТВУЮЩИЙ слушатель
    // UnesennyeRadioClient.OnBroadcastPlay (зарегистрирован на ID 304).
    // Второй слушатель намеренно НЕ регистрируется — нет дублирующего кода звука
    // и двойного воспроизведения (обратная совместимость сохранена).

    // ===== 405: список треков альбома (для UI) =====
    static void OnTrackListResp(RPCParamContext context, ParamsReadContext buf)
    {
        if (!Authorized()) return;
        ParamArray p;
        if (!buf.ReadObject(p)) return;
        array<string> vals = p.GetArray();
        if (!vals || vals.Count() < 2) return;

        string albumName = vals[0];
        int count = vals[1].ToInt();
        Print(string.Format("[Unesennye Album] Tracklist '%s': %d tracks.", albumName, count));
        for (int i = 0; i < count && (2 + i) < vals.Count(); i++)
        {
            Print(string.Format("  #%d: %s", i, vals[2 + i]));
        }
    }

    // ===== 406: сервер отклонил команду =====
    static void OnReject(RPCParamContext context, ParamsReadContext buf)
    {
        if (!Authorized()) return;
        ShowMessageOnScreen("Unesennye Album: команда отклонена сервером (лимит/валидация).");
    }

    private static void ShowMessageOnScreen(string msg)
    {
        ChatBase chat = ChatBase.Cast(GetGame().GetGUIController().GetElementByName("ChatBase"));
        if (chat) chat.AddChatSender(msg);
    }
};

// ---------- Модификация базового класса флешки: ТОЛЬКО новые методы ----------
// Старые одно-трековые предметы UnesennyeMusicCard0..4 наследуются от этого же
// базового класса и продолжают работать через Unesennye_Radio_System.c (без изменений):
// их ActionUse остаётся в радио-файле. Альбомные методы доступны всем флешкам,
// но у старых просто не заполнен unesennyeAlbumId -> безопасный no-op.
modded class UnesennyeMusicCard_Base
{
    // Максимум треков на этой флешке (0 = старая одно-трековая карточка)
    int UnesennyeGetTrackCount()
    {
        string album = UnesennyeAlbumManager::GetCardAlbumName(this);
        return UnesennyeAlbumManager::GetAlbumTrackCount(album);
    }

    // Следующий трек альбома (циклический переход)
    void UnesennyeNextTrack()
    {
        SwitchTrack(true);
    }

    // Предыдущий трек альбома (циклический переход)
    void UnesennyePrevTrack()
    {
        SwitchTrack(false);
    }

    // Установить конкретный трек (0 .. trackCount-1)
    void UnesennyeSetTrack(int trackIndex)
    {
        string album = UnesennyeAlbumManager::GetCardAlbumName(this);
        int max = UnesennyeAlbumManager::GetAlbumTrackCount(album);
        if (max <= 0 || trackIndex < 0 || trackIndex >= max) return;
        if (!UnesennyeAlbumClient::CanSwitch()) return;

        int radioID = UnesennyeAlbumManager::GetActiveRadioID(this);
        if (radioID == 0) return; // флешка не активна в рации

        UnesennyeAlbumManager::SetCurrentTrack(this, trackIndex);
        UnesennyeAlbumClient::RequestSetTrack(radioID, UnesennyeAlbumManager::GlobalTrackID(album, trackIndex));
        AnnounceTrack(album, trackIndex);
    }

    // Активировать альбомную флешку: воспроизвести текущий трек на существующем
    // протоколе раций. radioID берётся из Item Variable (ставится при использовании
    // старой ActionUse) либо из ближайшей рации — без дублирования радио-логики.
    void UnesennyeActivateAlbum()
    {
        if (GetGame().IsDedicated()) return;
        if (!UnesennyeAlbumClient::Authorized()) return;

        string album = UnesennyeAlbumManager::GetCardAlbumName(this);
        int max = UnesennyeAlbumManager::GetAlbumTrackCount(album);
        if (max <= 0) return; // старая карточка — ею управляет Unesennye_Radio_System.c

        int cur = UnesennyeAlbumManager::GetCurrentTrack(this);
        if (cur < 0 || cur >= max) cur = 0;

        int radioID = UnesennyeAlbumManager::GetActiveRadioID(this);
        if (radioID == 0)
        {
            PlayerBase player = GetGame().GetPlayer();
            if (!player) return;
            vector pos = player.GetPosition();
            ref array<IEntity> items = new array<IEntity>;
            GetGame().GetObjectsAtPosition(pos, 2.0, 2.0, 2.0, items, null, null);
            for (int i = 0; i < items.Count(); i++)
            {
                RadioBase rb = Cast<RadioBase>(items[i]);
                if (rb) { radioID = rb.GetID(); break; }
            }
            delete items;
        }
        if (radioID == 0)
        {
            UnesennyeAlbumShowMsg("Unesennye Album: рядом нет рации.");
            return;
        }
        UnesennyeAlbumManager::SetActiveRadioID(this, radioID);

        // Существующий RPC раций: RADIO_PLAY_TRACK(302), формат (radioID, trackID)
        UnesennyeAlbumClient::RequestSetTrack(radioID, UnesennyeAlbumManager::GlobalTrackID(album, cur));
        AnnounceTrack(album, cur);
    }

    // ----- внутреннее -----
    private void SwitchTrack(bool forward)
    {
        string album = UnesennyeAlbumManager::GetCardAlbumName(this);
        int max = UnesennyeAlbumManager::GetAlbumTrackCount(album);
        if (max <= 0) return; // старая одно-трековая флешка — не наш случай

        int radioID = UnesennyeAlbumManager::GetActiveRadioID(this);
        if (radioID == 0)
        {
            UnesennyeAlbumShowMsg("Unesennye Album: сначала используйте флешку рядом с рацией.");
            return;
        }
        if (!UnesennyeAlbumClient::CanSwitch()) return;

        int cur = UnesennyeAlbumManager::GetCurrentTrack(this);
        if (cur < 0 || cur >= max) cur = 0;

        int next = forward ? (cur + 1) % max : (cur - 1 + max) % max;
        UnesennyeAlbumManager::SetCurrentTrack(this, next); // Item Variable — сохранится при дропе/логине

        // Сервер вычислит новый индекс из Item Variable флешки, провалидирует трек
        // по CfgUnesennyeTracks и ретранслирует в стандартном формате (radioID, trackID)
        UnesennyeAlbumClient::RequestSwitch(radioID, GetID(), forward);
        AnnounceTrack(album, next);
    }

    private void AnnounceTrack(string album, int idx)
    {
        string title = UnesennyeAlbumManager::GetAlbumTrackTitle(album, idx);
        int max = UnesennyeAlbumManager::GetAlbumTrackCount(album);
        UnesennyeAlbumShowMsg(string.Format("Unesennye Album [%s]: Треки: %d/%d — %s | Author: %s",
            album, idx + 1, max, title, UNSENNYE_ALBUM_AUTHOR_NAME));
    }
};

// ---------- Хелпер вывода сообщений (общий для альбомного модуля) ----------
void UnesennyeAlbumShowMsg(string msg)
{
    ChatBase chat = ChatBase.Cast(GetGame().GetGUIController().GetElementByName("ChatBase"));
    if (chat) chat.AddChatSender(msg);
}

// ---------- Запоминаем активную рацию при использовании флешки ----------
// Существующая логика ActionUse/вставки НЕ меняется — только запись Item Variable.
modded class RadioBase
{
    override void OnUnesennyeCardUsed(ItemBase card)
    {
        super.OnUnesennyeCardUsed(card);
        if (card) UnesennyeAlbumManager::SetActiveRadioID(card, GetID());
    }
};

// ---------- Точка регистрации: расширяем MissionGameplay БЕЗ правки других файлов ----------
modded class MissionGameplay
{
    override void OnInit()
    {
        super.OnInit(); // существующая инициализация авто- и радио-систем сохраняется
        UnesennyeAlbumClient.InitRpc();
    }
}
