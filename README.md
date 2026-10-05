# Unesennye Music System v1.0.0
**Автор: KRa Tos (Константин)** | DayZ Standalone 1.24+

Система автомобильного радио из трёх модов:

| Мод | Роль | Установка |
|---|---|---|
| `@unesennye` | Клиент: UI, handshake, воспроизведение | Игрок + сервер |
| `@unesennye_servermod` | Сервер: RPC-валидация, защита | Только сервер (приватно) |
| `@unesennye_music_db` | Ассеты: .ogg + CfgUnesennyeTracks | Игрок + сервер |

## Защита
Клиент при заходе шлёт `HS_REQUEST(id=100)`. Ждёт ответ **4000 мс**, опрос таймера каждые **500 мс**.
Нет ответа (серверный мод не установлен → обработчик RPC не зарегистрирован) → блокировка радио/UI → `GetGame().Disconnect()` с сообщением *"Error: Required server mod 'unesennye_servermod' is missing."* Сервер при этом не крашится.

## Сборка в DayZ Workbench
1. File → Add Mod → `@unesennye`, указать скрипты: `scripts/` (префиксы 1_Core/4_World стандартные DZ_Scripts). Build → @ExpackPak → `@unesennye` (Scripts Only).
2. Аналогично `@unesennye_servermod` (scripts/1_Core/init.c + scripts/4_World/*.c).
3. `@unesennye_music_db`: Scripts НЕ содержит — Build → Data&Config Only после добавления .ogg.
4. На сервере в load-mods порядок: `@DayZ-Epoch/DZ_...;@unesennye_music_db;@unesennye;@unesennye_servermod`.
5. Проверка авторства в RPT: `[Unesennye_ServerMod v1.0.0] ACTIVE. Author: KRa Tos (Константин)`.

## Добавление треков
См. `@unesennye_music_db/data/sounds/tracks/README.txt` — правка только config.cpp + .ogg, скрипты не перекомпилируются.
