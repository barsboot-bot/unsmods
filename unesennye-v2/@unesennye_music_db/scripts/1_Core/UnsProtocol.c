// Unesennye v2 — общие константы протокола (клиент и сервер).
// Только константы: без вызовов GetGame(), т.к. слой 1_Core загружается раньше.
// Все значения — внутри класса, чтобы не загрязнять глобальное пространство имён.

class UnsProto
{
    // Версия протокола. Меняется при любом изменении формата сообщений.
    static const int PROTO_VERSION = 2;

    // Единый ID RPC. Диапазон 91xxx выбран вне ванильных ERPC и вне v1 (100–204, 300–305, 400–406).
    static const int RPC_ID = 91337;

    // Клиент -> сервер (первый Int в сообщении — код операции)
    static const int C2S_HELLO = 1;       // proto:int, nonce:int
    static const int C2S_CAR_PLAY = 2;    // carLow:int, carHigh:int
    static const int C2S_CAR_STOP = 3;    // carLow:int, carHigh:int
    static const int C2S_CARD_STEP = 4;   // cardLow:int, cardHigh:int, dir:int (+1 / -1)
    static const int C2S_CARD_STOP = 5;   // cardLow:int, cardHigh:int

    // Сервер -> клиент
    static const int S2C_HELLO_ACK = 101; // nonce:int, ok:int
    static const int S2C_PLAY = 102;      // key:string, x:float, y:float, z:float, trackId:int, kind:int
    static const int S2C_STOP = 103;      // key:string
    static const int S2C_REJECT = 199;    // reason:int

    // Причины отказа (S2C_REJECT)
    static const int R_PROTO = 1;         // версия протокола не совпала
    static const int R_NOT_AUTH = 2;      // сессия не подтверждена рукопожатием
    static const int R_COOLDOWN = 3;      // слишком частые запросы
    static const int R_BAD_TARGET = 4;    // объект не найден или не нужного типа
    static const int R_NOT_IN_CAR = 5;    // игрок не в этой машине
    static const int R_NOT_OWNER = 6;     // карта не в руках игрока
    static const int R_NO_TRACKS = 7;     // нет треков для проигрывания

    // Тип источника звука (для будущих эффектов; сейчас влияет только на логи)
    static const int KIND_CAR = 1;
    static const int KIND_PORTABLE = 2;

    // Тайминги (миллисекунды)
    static const int COOLDOWN_MS = 1000;
    static const int HANDSHAKE_TICK_MS = 500;
    static const int HANDSHAKE_TIMEOUT_MS = 4000;

    static const int MAX_ALBUM_TRACKS = 20;

    static const string MSG_MISSING_SERVER = "Unesennye: на сервере не установлен @unesennye_servermod. Подключение разорвано.";
    static const string MSG_REJECT_PREFIX = "Unesennye: сервер отклонил запрос, код ";
}
