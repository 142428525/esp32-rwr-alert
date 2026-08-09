#define BUZ_PIN 14
#define BUZ_CHAN 0

#define RANDSEED_PIN 0

#define TOUCH0_PIN 4
#define TOUCH1_PIN 13
#define TOUCH_GATE 20

#define NOTE_HIGH 554.37
#define NOTE_MIDL 466.16
#define NOTE_LOW1 370.00
#define NOTE_LOW2 392.00

#define SLSH_W 2
#define SLSH_H 2
static const uint8_t LSLSH_BITS[] = {
0x01,0x02
};
static const uint8_t RSLSH_BITS[] = {
0x02,0x01
};

#define SLSH_POSLEN 10
#define SLSH_POSDELI 4
static const uint8_t SLSH_LX[] = { 82, 72, 119, 109, 84, 87, 90, 101, 104, 107 };
static const uint8_t SLSH_LY[] = {  7, 17,  44,  54, 19, 22, 25,  36,  39,  42 };
static const uint8_t SLSH_RX[] = { 72, 82, 109, 119, 84, 87, 90, 101, 104, 107 };
static const uint8_t SLSH_RY[] = { 44, 54,   7,  17, 42, 39, 36,  25,  22,  19 };
