// Author: KRa Tos (Константин) | Project: Unesennye
// ============================================================================
//  Архитектура системы "Музыка в автомобилях — Унесённые" (DayZ 1.24+)
// ============================================================================
//
//  Три мода:
//   @unesennye            — клиент (публичный). UI, радио, RPC-запросы.
//   @unesennye_servermod  — сервер (приватный). КЛЮЧ АКТИВАЦИИ + валидация RPC.
//   @unesennye_music_db   — ассеты (.ogg + CfgSoundSets/CfgUnesennyeTracks),
//                           ставится и на клиент, и на сервер.
//
//  ПОТОК HANDSHAKE (защита):
//   PlayerBase::OnBaseCreated (клиент)
//        └─> CallRPC(RPC_Unesennye_Handshake, ALL, challenge)
//                ├─ сервер с @unesennye_servermod:
//                │     MissionServer::RPC_Unesennye_Handshake(challenge)
//                │        └─> CallRPC(RPC_Unesennye_AuthOK, playerID, token(challenge))
//                │              └─> клиент сверяет token -> снимает блок радио
//                └─ сервер БЕЗ мода: функция не зарегистрирована -> тишина
//                      └─> таймер 10с -> блокировка радио + RequestDisconnect
//                          с сообщением об отсутствии unesennye_servermod
//
//  ПОТОК PLAYBACK:
//   Игрок в машине жмёт "Play trackID" в UI
//        └─> RPC_Unesennye_PlayTrack(trackID)          [клиент -> сервер]
//              Сервер валидирует: авторизован? трек в конфиге? игрок в CarScript?
//              кулдаун 5 сек? └─> BroadcastPlay(carNetID, trackID) всем клиентам
//                    └─> каждый клиент: машина рядом (<90м)? -> PlaySoundCD(set)
//   Остановки/смена трека синхронизируются так же через сервер.
//
//  ВОДЯНЫЕ ЗНАКИ:
//   - author="KRa Tos (Константин)" во всех mod.cpp и CfgMods/CfgPatches
//   - заголовок "// Author: ..." в каждом .c/.cpp
//   - MissionServer::GetModAuthor() + Print в RPT при OnInit
//   - версия v1.0.0 в имени мода (видно в лаунчере)
// ============================================================================
