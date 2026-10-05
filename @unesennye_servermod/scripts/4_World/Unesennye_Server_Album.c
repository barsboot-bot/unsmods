// Author: KRa Tos (Константин) | Project: Unesennye
// =====================================================================
// UNSENNYE MUSIC SYSTEM — СЕРВЕРНАЯ ЧАСТЬ АЛЬБОМНЫХ ФЛЕШОК (v1.2.0, расширение)
// НОВЫЙ файл: существующие серверные системы НЕ изменены:
//   - авто-валидация (Unesennye_Server_System.c, RPC 100-204) — как есть
//   - рации (Unesennye_Server_Radio.c, RPC 300-305) — как есть
// Валидация команд альбомов: авторизация Handshake + существование альбома
// в CfgUnesennyeAlbums + трек существует в CfgUnesennyeTracks +
// trackIndex < trackCount + анти-спам кулдаун 2 секунды на переключение.
// Автор: KRa Tos (Константин)
// =====================================================================

// ===== RPC-протокол альбомов: ID 400-406 (идентично клиенту) =====

// ===== RPC-протокол альбомов (идентично клиентскому моде) =====
enum UnesennyeAlbumRPC
{
    ALBUM_NEXT_TRACK     = 400,
    ALBUM_PREV_TRACK     = 401,
    ALBUM_SET_TRACK      = 402,
    ALBUM_GET_TRACKLIST  = 403,
    ALBUM_BROADCAST      = 404,
    ALBUM_TRACKLIST_RESP = 405,
    ALBUM_REJECT         = 406
};

static const int    UNSENNYE_ALBUM_SRV_MAX_TRACKS = 40; // шаг нумерации id (идентично клиенту, без коллизий альбомов)
static const string UNSENNYE_ALBUM_SRV_AUTHOR     = "KRa Tos (Константин)"; // водяной знак

// ---------- Скрытая метка авторства (серверный альбомный модуль) ----------
static string GetAlbumServerModAuthor()
{
    return UNSENNYE_ALBUM_SRV_AUTHOR; // "KRa Tos (Константин)"
}

// ---------- Серверный модуль альбомов ----------
class UnesennyeServerAlbumModule
{
    private const int ALBUM_SWITCH_COOLDOWN_MS = 2000; // анти-спам: 2 секунды на переключение трека

    void UnesennyeServerAlbumModule()
    {
        m_LastSwitchPerPlayer = new array<int>;
        m_PlayerIDs = new array<int>;
    }

    void ~UnesennyeServerAlbumModule()
    {
        delete m_LastSwitchPerPlayer;
        m_LastSwitchPerPlayer = null;
        delete m_PlayerIDs;
        m_PlayerIDs = null;
    }

    // Инициализация: регистрация SERVER RPC по числовым ID (400-403)
    void Init()
    {
        // Числовые ID фиксированы протоколом (enum UnesennyeAlbumRPC);
        // регистрация по имени класса — как в существующих модулях (100-204, 300-305).
        GetGame().RegisterServerRpc(400, "UnesennyeServerAlbumModule", "OnNextTrack");
        GetGame().RegisterServerRpc(401, "UnesennyeServerAlbumModule", "OnPrevTrack");
        GetGame().RegisterServerRpc(402, "UnesennyeServerAlbumModule", "OnSetTrack");
        GetGame().RegisterServerRpc(403, "UnesennyeServerAlbumModule", "OnGetTracklist");

        Print(string.Format("[Unesennye Album] Server module ACTIVE (RPC 400-406). Author: %s", UNSENNYE_ALBUM_SRV_AUTHOR));
    }

    // ----- Анти-спам: 2 секунды на переключение (per player) -----
    private int LastSwitchMs(int playerID)
    {
        for (int i = 0; i < m_PlayerIDs.Count(); i++)
            if (m_PlayerIDs[i] == playerID) return m_LastSwitchPerPlayer[i];
        return -ALBUM_SWITCH_COOLDOWN_MS * 2; // первый запрос всегда проходит
    }

    private bool CheckCooldown(int senderID)
    {
        int now = MathFloor(GetGame().GetTime()); // GetTime() уже в мс
        if (now - LastSwitchMs(senderID) < ALBUM_SWITCH_COOLDOWN_MS)
        {
            RejectRequest(senderID);
            Print(string.Format("[Unesennye Album] Anti-spam: player %d throttled (2 s cooldown).", senderID));
            return false;
        }
        int idx = -1;
        for (int i = 0; i < m_PlayerIDs.Count(); i++)
            if (m_PlayerIDs[i] == senderID) { idx = i; break; }
        if (idx >= 0) m_LastSwitchPerPlayer[idx] = now;
        else { m_PlayerIDs.Insert(senderID); m_LastSwitchPerPlayer.Insert(now); }
        return true;
    }

    // Очистка записи игрока при выходе (без утечек и роста массивов)
    void RemovePlayer(int playerID)
    {
        for (int i = 0; i < m_PlayerIDs.Count(); i++)
        {
            if (m_PlayerIDs[i] == playerID)
            {
                m_PlayerIDs.Remove(i);
                m_LastSwitchPerPlayer.Remove(i);
                return;
            }
        }
    }

    // ----- Валидация -----
    // Игрок авторизован handshake'ем? (существующий реестр 100-204, логика не изменена)
    private Man ValidateAuthorized(RPCParamContext context)
    {
        int senderID = context.GetSenderID();
        if (!UnesennyeServerRPC.g_UnesennyeServer || !UnesennyeServerRPC.g_UnesennyeServer.IsPlayerAuthorized(senderID))
            return null;
        return GetGame().GetPlayerByID(senderID);
    }

    // Альбом существует в CfgUnesennyeAlbums?
    private bool AlbumExists(string albumName)
    {
        if (albumName == "") return false;
        return ConfigIsExisting("CfgUnesennyeAlbums\\" + albumName);
    }

    // Кол-во треков альбома из конфига
    private int AlbumTrackCount(string albumName)
    {
        int n = ConfigReadInt("CfgUnesennyeAlbums\\" + albumName + "\\trackCount", 0);
        if (n > 20) n = 20; // физический лимит альбома — 20 треков
        return n;
    }

    // Трек существует в базе CfgUnesennyeTracks (глобальный индекс)?
    private bool TrackExists(int trackID)
    {
        string clsName = ConfigGetClassName(trackID, "CfgUnesennyeTracks");
        if (clsName == "") return false;
        return ConfigReadString(clsName + "\\file", "").Length() > 0;
    }

    // Усиленная проверка для альбомов: запись реестра есть И её file совпадает
    // со sample соответствующего трека альбома (защита от подмены индекса).
    private bool AlbumTrackValid(int globalTrackID)
    {
        if (!TrackExists(globalTrackID)) return false;
        int baseIdx = globalTrackID / UNSENNYE_ALBUM_SRV_MAX_TRACKS;
        int localIdx = globalTrackID % UNSENNYE_ALBUM_SRV_MAX_TRACKS;
        string albumName = AlbumNameByIndex(baseIdx);
        if (albumName == "" || !AlbumExists(albumName)) return false;
        if (localIdx >= AlbumTrackCount(albumName)) return false;
        string regFile = ConfigReadString("CfgUnesennyeTracks\\track_" + globalTrackID + "\\file", "");
        string sample  = ConfigReadString(string.Format("CfgUnesennyeAlbums\\%s\\Track%02d\\sample", albumName, localIdx + 1), "");
        return (regFile != "" && regFile == sample);
    }

    // Рация существует и это RadioBase, игрок рядом (<=8 м)?
    private bool ValidateRadio(Man player, int radioID)
    {
        IEntity radioEnt = GetGame().FindEntity(radioID);
        if (!radioEnt || !Cast<RadioBase>(radioEnt)) return false;
        vector pos = player.GetPosition();
        vector rpos = radioEnt.GetPosition();
        if (vector.DistanceSq(pos, rpos) > 64.0) return false;
        ref array<IEntity> items = new array<IEntity>;
        GetGame().GetObjectsAtPosition(rpos, 3.0, 3.0, 3.0, items, null, null);
        for (int i = 0; i < items.Count(); i++)
            if (items[i] == player) { delete items; return true; }
        delete items;
        return false;
    }

    // Отклонение команды (клиент покажет сообщение)
    private void RejectRequest(int senderID)
    {
        GetGame().RPCSingleParam(senderID, UnesennyeAlbumRPC.ALBUM_REJECT,
            new Param1<int>(senderID), RPCTargetGroup.Self);
    }

    // Ретрансляция воспроизведения всем клиентам.
    // ВАЖНО: формат (radioID, trackID) идентичен RADIO_BROADCAST(304), поэтому
    // старые клиенты принимают его штатным обработчиком раций — обратная совместимость.
    private void BroadcastPlay(int senderID, int radioID, int trackID)
    {
        GetGame().RPCSingleParam(senderID, UnesennyeAlbumRPC.ALBUM_BROADCAST,
            new Param2<int, int>(radioID, trackID), RPCTargetGroup.All);
    }

    // Разрешение глобального индекса трека: base*MAX + local (инвариант клиента)
    private int ResolveGlobalTrack(int senderID, int radioID, int globalTrackID)
    {
        if (!CheckCooldown(senderID)) return -1;
        if (!AlbumTrackValid(globalTrackID)) { RejectRequest(senderID); return -1; }
        if (!ValidateRadio(GetGame().GetPlayerByID(senderID), radioID)) { RejectRequest(senderID); return -1; }
        return globalTrackID;
    }

    // ===== 400: следующий трек =====
    void OnNextTrack(RPCParamContext context, ParamsReadContext buf)
    {
        Param2<int, int> p; // radioID, cardNetID
        if (!buf.ReadObject(p)) return;
        Man player = ValidateAuthorized(context);
        if (!player) return;

        // Вычисляем новый индекс из Item Variable флешки на СЕРВЕРЕ (предмет синхронизирован)
        IEntity cardEnt = GetGame().FindEntity(p.arg2);
        ItemBase card = cardEnt ? EntityToType(ItemBase, cardEnt) : null;
        if (!card) { RejectRequest(context.GetSenderID()); return; }

        string album = ConfigReadString("CfgVehicles\\" + card.GetType() + "\\unesennyeAlbumId", "");
        if (!AlbumExists(album)) { RejectRequest(context.GetSenderID()); return; }
        int max = AlbumTrackCount(album);
        if (max <= 0) { RejectRequest(context.GetSenderID()); return; }

        int cur = 0;
        card.GetVar("Unesennye_CurrentTrack", cur);
        if (cur < 0 || cur >= max) cur = -1;
        int next = (cur + 1) % max;
        card.SetVar("Unesennye_CurrentTrack", next); // authoritative запись на предмете

        int baseIdx = AlbumIndexOf(album);
        if (baseIdx < 0) { RejectRequest(context.GetSenderID()); return; }
        int gidx = baseIdx * UNSENNYE_ALBUM_SRV_MAX_TRACKS + next;
        if (ResolveGlobalTrack(context.GetSenderID(), p.arg1, gidx) < 0) return;

        BroadcastPlay(context.GetSenderID(), p.arg1, gidx);
        Print(string.Format("[Unesennye Album] NEXT '%s' track %d/%d radio=%d player=%d | Author: %s",
            album, next + 1, max, p.arg1, context.GetSenderID(), UNSENNYE_ALBUM_SRV_AUTHOR));
    }

    // ===== 401: предыдущий трек =====
    void OnPrevTrack(RPCParamContext context, ParamsReadContext buf)
    {
        Param2<int, int> p; // radioID, cardNetID
        if (!buf.ReadObject(p)) return;
        Man player = ValidateAuthorized(context);
        if (!player) return;

        IEntity cardEnt = GetGame().FindEntity(p.arg2);
        ItemBase card = cardEnt ? EntityToType(ItemBase, cardEnt) : null;
        if (!card) { RejectRequest(context.GetSenderID()); return; }

        string album = ConfigReadString("CfgVehicles\\" + card.GetType() + "\\unesennyeAlbumId", "");
        if (!AlbumExists(album)) { RejectRequest(context.GetSenderID()); return; }
        int max = AlbumTrackCount(album);
        if (max <= 0) { RejectRequest(context.GetSenderID()); return; }

        int cur = 0;
        card.GetVar("Unesennye_CurrentTrack", cur);
        if (cur < 0 || cur >= max) cur = 0;
        int prev = (cur - 1 + max) % max;
        card.SetVar("Unesennye_CurrentTrack", prev);

        int baseIdx = AlbumIndexOf(album);
        if (baseIdx < 0) { RejectRequest(context.GetSenderID()); return; }
        int gidx = baseIdx * UNSENNYE_ALBUM_SRV_MAX_TRACKS + prev;
        if (ResolveGlobalTrack(context.GetSenderID(), p.arg1, gidx) < 0) return;

        BroadcastPlay(context.GetSenderID(), p.arg1, gidx);
        Print(string.Format("[Unesennye Album] PREV '%s' track %d/%d radio=%d player=%d | Author: %s",
            album, prev + 1, max, p.arg1, context.GetSenderID(), UNSENNYE_ALBUM_SRV_AUTHOR));
    }

    // ===== 402: установить конкретный трек =====
    void OnSetTrack(RPCParamContext context, ParamsReadContext buf)
    {
        Param2<int, int> p; // radioID, GLOBAL trackID (base*20+local)
        if (!buf.ReadObject(p)) return;
        Man player = ValidateAuthorized(context);
        if (!player) return;

        int radioID = p.arg1;
        int gidx = p.arg2;

        // Валидация: трек существует + index в пределах своего альбома
        if (!AlbumTrackValid(gidx)) { RejectRequest(context.GetSenderID()); return; }

        int baseIdx = gidx / UNSENNYE_ALBUM_SRV_MAX_TRACKS;
        int localIdx = gidx % UNSENNYE_ALBUM_SRV_MAX_TRACKS;
        string albumName = AlbumNameByIndex(baseIdx);
        if (albumName == "" || !AlbumExists(albumName)) { RejectRequest(context.GetSenderID()); return; }
        if (localIdx >= AlbumTrackCount(albumName)) { RejectRequest(context.GetSenderID()); return; } // trackIndex < trackCount

        if (ResolveGlobalTrack(context.GetSenderID(), radioID, gidx) < 0) return;

        BroadcastPlay(context.GetSenderID(), radioID, gidx);
        Print(string.Format("[Unesennye Album] SET '%s' track %d/%d radio=%d player=%d | Author: %s",
            albumName, localIdx + 1, AlbumTrackCount(albumName), radioID, context.GetSenderID(), UNSENNYE_ALBUM_SRV_AUTHOR));
    }

    // ===== 403: список треков альбома =====
    void OnGetTracklist(RPCParamContext context, ParamsReadContext buf)
    {
        Param1<int> p; // индекс альбома
        if (!buf.ReadObject(p)) return;
        Man player = ValidateAuthorized(context);
        if (!player) return;

        string albumName = AlbumNameByIndex(p.param);
        if (albumName == "" || !AlbumExists(albumName)) { RejectRequest(context.GetSenderID()); return; }

        int count = AlbumTrackCount(albumName);
        ParamArray arr = new ParamArray;
        array<string> vals = arr.GetArray(); // ParamArray внутренне владеет массивом
        vals.Insert(albumName);
        vals.Insert(count.ToString());
        for (int i = 0; i < count; i++)
        {
            string title = ConfigReadString(string.Format("CfgUnesennyeAlbums\\%s\\Track%02d\\title", albumName, i + 1), "?");
            vals.Insert(title);
        }
        GetGame().RPCSingleParam(context.GetSenderID(), UnesennyeAlbumRPC.ALBUM_TRACKLIST_RESP,
            arr, RPCTargetGroup.Self);
    }

    // ----- helpers имени/индекса альбома -----
    private int AlbumIndexOf(string albumName)
    {
        int count = ConfigGetClassCount("CfgUnesennyeAlbums");
        for (int i = 0; i < count; i++)
            if (ConfigGetClassName(i, "CfgUnesennyeAlbums") == albumName) return i;
        return -1;
    }

    private string AlbumNameByIndex(int idx)
    {
        if (idx < 0) return "";
        return ConfigGetClassName(idx, "CfgUnesennyeAlbums");
    }

    private ref array<int> m_LastSwitchPerPlayer;
    private ref array<int> m_PlayerIDs;
};

// ---------- Очистка записей при выходе игрока (без утечек) ----------
modded class PlayerBase
{
    override void OnBaseDestroyed(IEntity base)
    {
        super.OnBaseDestroyed(base);
        if (g_UnesennyeAlbumModule)
        {
            g_UnesennyeAlbumModule.RemovePlayer(GetID());
        }
    }
}

// ---------- Глобальный экземпляр + точка старта ----------
// Расширяем MissionServer БЕЗ правки существующих файлов мода.
ref UnesennyeServerAlbumModule g_UnesennyeAlbumModule;

modded class MissionServer
{
    override void OnInit()
    {
        super.OnInit(); // существующая инициализация авто- и радио-систем сохраняется
        if (!g_UnesennyeAlbumModule)
            g_UnesennyeAlbumModule = new UnesennyeServerAlbumModule;
        g_UnesennyeAlbumModule.Init();
    }

    override void OnMissionFinish()
    {
        super.OnMissionFinish();
        if (g_UnesennyeAlbumModule)
        {
            delete g_UnesennyeAlbumModule;
            g_UnesennyeAlbumModule = null;
        }
    }
}
