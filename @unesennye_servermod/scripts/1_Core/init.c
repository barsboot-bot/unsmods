// Author: KRa Tos (Константин) | Project: Unesennye
// Точка входа серверного мода. 1_Core компилируется раньше 4_World,
// поэтому здесь только константы/протокол; классы — в 4_World.

// ===== Версия и авторство (водяные знаки) =====
static const int    UNSENNYE_SERVER_VERSION = 100;          // v1.0.0
static const string UNSENNYE_AUTHOR_NAME    = "KRa Tos (Константин)";
static const string UNSENNYE_PROJECT_NAME   = "Unesennye Music System";

// ===== RPC-протокол (идентичная копия есть в клиентском моде) =====
enum UnesennyeRPC
{
    // Клиент -> Сервер
    HS_REQUEST      = 100,   // начало handshake (clientId:int)
    TRACK_LIST_REQ  = 101,   // запрос списка треков (clientId:int)
    RADIO_PLAY      = 102,   // играть трек (carID:int, trackID:int)
    RADIO_STOP      = 103,   // остановить радио (carID:int)

    // Сервер -> Клиент
    HS_RESPONSE     = 200,   // handshake ок (clientId, tokenA, tokenB)
    AUTH_FAIL       = 201,   // отказ авторизации (clientId, reason)
    TRACK_LIST_RESP = 202,   // список треков (count, id0,name0,...)
    BROADCAST_PLAY  = 203,   // включить музыку в машине (carID, trackID)
    BROADCAST_STOP  = 204    // выключить музыку в машине (carID)
};

static const int UNSENNYE_PROTO_VERSION = 1;
