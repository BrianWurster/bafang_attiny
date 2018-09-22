#ifndef BAFANG_H_
#define BAFANG_H_

#define STATE_WAITING			(0)
#define STATE_LENGTH			(1)
#define STATE_DATA				(2)
#define STATE_HANDLE			(3)
#define STATE_WRITE_RESP		(4)

#define BAFANG_READ				(0x11)
#define BAFANG_WRITE			(0x16)

#define BAFANG_STATE_IDLE		(0)
#define BAFANG_STATE_PEDALR		(2)
#define BAFANG_STATE_PEDALW		(3)

#define CMD_PEDAL				(0x53)

#define BAFANG_HEADER_SIZE		(2)

extern uint8_t packet[BUFFER_SIZE];
extern volatile uint8_t state;
extern volatile uint8_t bfState;
extern volatile uint8_t len;

typedef struct bafangHeader {
	uint8_t cmd;
	uint8_t len;
} bafangHeader_t;

typedef struct bafangPacket {
	bafangHeader_t header;
	uint8_t data[BUFFER_SIZE - BAFANG_HEADER_SIZE];
} bafangPacket_t;

typedef struct bfReadPedalCmd {
	bafangHeader_t header;
	uint8_t pedalType;				// (0x03) 0x00: None, 0x01: DH-Sensor-12, 0x02: BB-Sensor-32, 0x03: DoubleSignal-24
	uint8_t designatedAssist;		// (0xFF) 0x00-0x09: Assist Mode No., 0xFF: By Display's Command
	uint8_t speedLimit;				// (0xFF) 0x0F-0x28: Speed Limit in km/h, 0xFF: By Display's Command
	uint8_t startCurrentPercent;	// (0x64 -> 100) 0x00-0x64
	uint8_t slowStartMode;			// (06 -> 6) 0x01-0x08: Mode Number
	uint8_t startUpDegree;			// (Signal No.) (0x14 -> 20) Integer: Number of Signal before start
	uint8_t workMode;				// (0x0A -> 10) 0x0A-0x50: Angular Speed of pedal/wheel*10 0xFF: Undetermined
	uint8_t timeOfStop;				// (0x19 -> 25) Integer: *10ms
	uint8_t currentDecay;			// (0x08 -> 8) 0x01-0x08: Current Decay
	uint8_t stopDecay;				// (0x14 -> 20) Integer: *10ms
	uint8_t keepCurrentPercent;		// (0x14 -> 20)
	uint8_t checkSum;				// (0x27)
} bfReadPedalCmd_t;

uint8_t calcCheckSum( uint8_t *data, uint8_t len );
void sendReadCmd( uint8_t cmd );
void bafangIdle();

#endif /* BAFANG_H_ */