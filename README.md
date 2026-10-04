# Unesennye — Музыка в автомобилях для DayZ (1.24+)

**Автор: KRa Tos (Константин) | Проект: Unesennye**

Система автомобильного радио из трёх модов с серверной защитой (handshake-RPC).

## Структура проекта

```
unsmods/
├── @unesennye/                       # КЛИЕНТ (публичный)
│   ├── mod.cpp
│   ├── config.cpp                    # CfgPatches / CfgMods
│   └── expansions/dayz/scripts/3_World/
│       └── unesennye_client.c        # handshake, UI-каркас, CarScript-радио, TrackDB
│
├── @unesennye_servermod/             # СЕРВЕР (приватный, ключ активации)
│   ├── mod.cpp
│   ├── config.cpp
│   └── expansions/dayz/scripts/3_World/
│       └── unesennye_server.c        # RPC-обработчики, валидация, rate-limit, лог автора
│
├── @unesennye_music_db/              # АССЕТЫ (клиент + сервер)
│   ├── mod.cpp
│   ├── config.cpp                    # CfgSoundSets / CfgSoundShaders / CfgUnesennyeTracks
│   └── data/sounds/tracks/           # demo_01.ogg, demo_02.ogg, ...
│
└── docs/
    ├── ARCHITECTURE.c                # схема потоков RPC
    └── ADDING_TRACKS.md              # инструкция по добавлению музыки
```

## Как это работает

### Защита (Dependency Check & Blocking)
1. При спавне персонажа клиент шлёт `CallRPC(RPC_Unesennye_Handshake, ALL, challenge)`.
2. **Если `@unesennye_servermod` установлен:** `MissionServer::RPC_Unesennye_Handshake`
   регистрирует игрока как авторизованного и отвечает `RPC_Unesennye_AuthOK(playerID, token(challenge))`.
   Клиент сверяет токен → радио разблокировано.
3. **Если серверного мода нет:** функция не зарегистрирована → сервер молча игнорирует
   вызов (без краша) → через 10 секунд клиент блокирует радио и выполняет
   `RequestDisconnect` с сообщением:
   `Error: Required server mod 'unesennye_servermod' is missing.`

### Воспроизведение
- Все команды (play/stop/tracklist) идут **через сервер**, который валидирует:
  трек существует в `CfgUnesennyeTracks`, игрок реально сидит в `CarScript`,
  анти-спам кулдаун 5 сек.
- Сервер ретранслирует `BroadcastPlay(carNetID, trackID)` — звук играет локально
  у каждого клиента в радиусе 90 м от машины (`PlaySoundCD`, 3D-позиционирование).
- Никакого стриминга аудио по сети: треки лежат в `@unesennye_music_db` у всех.

## Установка на сервер

Порядок в `serverDZ.cfg` (`mod=`):

```
mod=@unesennye;@unesennye_servermod;@unesennye_music_db
```

Клиенты: `@unesennye` + `@unesennye_music_db` (серверный мод клиенту **не выдаётся** —
в этом и смысл защиты).

## Добавление треков
См. [docs/ADDING_TRACKS.md](docs/ADDING_TRACKS.md) — только правка `config.cpp`
и копирование `.ogg`, перекомпиляция скриптов не нужна.

## Водяные знаки
- `author = "KRa Tos (Константин)"` во всех `mod.cpp` / `CfgMods` / `CfgPatches`;
- заголовок `// Author: KRa Tos (Константин) | Project: Unesennye` в каждом исходнике;
- `MissionServer::GetModAuthor()` + запись в RPT при старте сервера;
- версия `v1.0.0` в названии мода (отображается в лаунчере).
