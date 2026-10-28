// Unesennye v2 — только чтение реестра треков и альбомов из config.cpp.
// Конфиг генерируется tools/build_config.py; здесь он только читается.
// Вызовы конфига: GetGame().ConfigGetInt / ConfigGetText / ConfigGetChildrenCount / ConfigGetChildName.
// (Форма подтверждена исходниками dayzexplorer: liquid.c, dayzgame.c. См. README, раздел «Что не подтверждено».)

class UnsTrack
{
    int m_TrackId;
    string m_Title;
    string m_File;
    string m_SoundSet;
    bool m_Car;
}

class UnsAlbum
{
    string m_Id;
    string m_Title;
    ref array<int> m_TrackIds;
}

class UnsRegistry
{
    static ref UnsRegistry s_Instance;

    protected ref map<int, ref UnsTrack> m_Tracks;
    protected ref map<string, ref UnsAlbum> m_Albums;
    protected ref array<int> m_CarTrackIds;

    static UnsRegistry Get()
    {
        if (!s_Instance)
        {
            s_Instance = new UnsRegistry();
            s_Instance.Load();
        }
        return s_Instance;
    }

    void UnsRegistry()
    {
        m_Tracks = new map<int, ref UnsTrack>;
        m_Albums = new map<string, ref UnsAlbum>;
        m_CarTrackIds = new array<int>;
    }

    protected void Load()
    {
        LoadTracks();
        LoadAlbums();
        Print(string.Format("[Unesennye] реестр: треков %1, машинных %2, альбомов %3", m_Tracks.Count(), m_CarTrackIds.Count(), m_Albums.Count()));
    }

    protected void LoadTracks()
    {
        int count = GetGame().ConfigGetChildrenCount("CfgUnesennyeTracks");
        for (int i = 0; i < count; i++)
        {
            string cls;
            GetGame().ConfigGetChildName("CfgUnesennyeTracks", i, cls);
            string p = "CfgUnesennyeTracks " + cls;

            int id = GetGame().ConfigGetInt(p + " trackId");
            string title;
            GetGame().ConfigGetText(p + " title", title);
            string file;
            GetGame().ConfigGetText(p + " file", file);
            string soundSet;
            GetGame().ConfigGetText(p + " soundSet", soundSet);
            bool car = GetGame().ConfigGetInt(p + " carPlaylist") == 1;

            if (id <= 0 || m_Tracks.Contains(id) || soundSet == "")
            {
                Print(string.Format("[Unesennye] пропуск трека %1: неверный id или soundSet", cls));
                continue;
            }

            UnsTrack t = new UnsTrack();
            t.m_TrackId = id;
            t.m_Title = title;
            t.m_File = file;
            t.m_SoundSet = soundSet;
            t.m_Car = car;
            m_Tracks.Set(id, t);

            if (car)
                m_CarTrackIds.Insert(id);
        }
    }

    protected void LoadAlbums()
    {
        int count = GetGame().ConfigGetChildrenCount("CfgUnesennyeAlbums");
        for (int i = 0; i < count; i++)
        {
            string albumId;
            GetGame().ConfigGetChildName("CfgUnesennyeAlbums", i, albumId);
            string ap = "CfgUnesennyeAlbums " + albumId;

            string title;
            GetGame().ConfigGetText(ap + " title", title);

            UnsAlbum a = new UnsAlbum();
            a.m_Id = albumId;
            a.m_Title = title;
            a.m_TrackIds = new array<int>;

            int slots = GetGame().ConfigGetChildrenCount(ap);
            for (int j = 0; j < slots; j++)
            {
                string slot;
                GetGame().ConfigGetChildName(ap, j, slot);
                int tid = GetGame().ConfigGetInt(ap + " " + slot + " track");
                if (!m_Tracks.Contains(tid))
                {
                    Print(string.Format("[Unesennye] альбом %1: трек %2 не найден", albumId, tid));
                    continue;
                }
                if (a.m_TrackIds.Count() < UnsProto.MAX_ALBUM_TRACKS)
                    a.m_TrackIds.Insert(tid);
            }

            if (a.m_TrackIds.Count() == 0)
                continue;

            m_Albums.Set(albumId, a);
        }
    }

    UnsTrack GetTrack(int trackId)
    {
        if (!m_Tracks.Contains(trackId))
            return null;
        return m_Tracks.Get(trackId);
    }

    int GetCarTrackCount()
    {
        return m_CarTrackIds.Count();
    }

    // Машинный плейлист: треки с carPlaylist = 1, в порядке id. ordinal берётся по модулю.
    UnsTrack GetCarTrackByOrdinal(int ordinal)
    {
        int n = m_CarTrackIds.Count();
        if (n == 0 || ordinal < 0)
            return null;
        return GetTrack(m_CarTrackIds.Get(ordinal % n));
    }

    bool HasAlbum(string albumId)
    {
        return m_Albums.Contains(albumId);
    }

    int GetAlbumCount(string albumId)
    {
        if (!m_Albums.Contains(albumId))
            return 0;
        return m_Albums.Get(albumId).m_TrackIds.Count();
    }

    // Возвращает trackId для позиции альбома или -1.
    int GetAlbumTrackId(string albumId, int index)
    {
        if (!m_Albums.Contains(albumId))
            return -1;
        UnsAlbum a = m_Albums.Get(albumId);
        if (index < 0 || index >= a.m_TrackIds.Count())
            return -1;
        return a.m_TrackIds.Get(index);
    }

    // Альбом карты берётся из конфига класса карты (unesennyeAlbumId). Пусто — карта не наша.
    string GetCardAlbumId(string cardType)
    {
        string albumId;
        GetGame().ConfigGetText("CfgVehicles " + cardType + " unesennyeAlbumId", albumId);
        return albumId;
    }
}
