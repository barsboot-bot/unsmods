Author: KRa Tos (Константин) | Project: Unesennye

СТРУКТУРА ПАПКИ @unesennye_music_db:

  @unesennye_music_db/
  ├── mod.cpp                       <- описание мода (author = KRa Tos)
  ├── config.cpp                    <- CfgSoundShaders / CfgSoundSets / CfgUnesennyeTracks
  └── data/
      └── sounds/
          └── tracks/               <- СЮДА КЛАСТЬ .ogg ФАЙЛЫ
              ├── track_01.ogg
              ├── track_02.ogg
              └── ...

ТРЕБОВАНИЯ К ФАЙЛАМ:
  - Формат: .ogg (Vorbis), 44.1 kHz, моно или стерео, <= 10 МБ на трек.
  - Имена: только латиница, цифры, подчёркивание (track_06.ogg).

КАК ДОБАВИТЬ НОВЫЙ ТРЕК (без перекомпиляции скриптов!):
  1. Положить файл в data/sounds/tracks/ (например track_06.ogg).
  2. В config.cpp добавить в CfgSoundShaders:
        class Unesennye_Track06_Shader
        {
            samples[] = {{"data/sounds/tracks/track_06.ogg", 1}};
            range = 60;
            volume = 0.85;
        };
  3. В CfgSoundSets:
        class Unesennye_Track06_SoundSet
        {
            soundShaders[] = {"Unesennye_Track06_Shader"};
            spatial = 1; looped = 1; volumeFactor = 0.9; distanceFactor = 1.0;
        };
  4. В CfgUnesennyeTracks (trackID = следующий номер подряд):
        class track_5
        {
            title = "Moё название трека";
            file = "data/sounds/tracks/track_06.ogg";
            soundSet = "Unesennye_Track06_SoundSet";
        };
  5. Пересобрать мод в Workbench (Scripts НЕ трогаем!) и обновить на
     клиенте и сервере. Трек сразу появится в списке радио.
