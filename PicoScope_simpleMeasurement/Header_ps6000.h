#include <stdio.h>
#include <tuple>
/* Headers for Windows */
#ifdef _WIN32
#include "windows.h"
#include <conio.h>
#include "ps6000Api.h"
#else
#include <sys/types.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#include <libps6000-1.4/ps6000Api.h>
#ifndef PICO_STATUS
#include <libps6000-1.4/PicoStatus.h>
#endif

#define Sleep(a) usleep(1000*a)
#define scanf_s scanf
#define fscanf_s fscanf
#define memcpy_s(a,b,c,d) memcpy(a,c,d)

typedef enum enBOOL { FALSE, TRUE } BOOL;

/* A function to detect a keyboard press on Linux */
int32_t _getch()
{
	struct termios oldt, newt;
	int32_t ch;
	int32_t bytesWaiting;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	setbuf(stdin, NULL);
	do {
		ioctl(STDIN_FILENO, FIONREAD, &bytesWaiting);
		if (bytesWaiting)
			getchar();
	} while (bytesWaiting);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return ch;
}

int32_t _kbhit()
{
	struct termios oldt, newt;
	int32_t bytesWaiting;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	setbuf(stdin, NULL);
	ioctl(STDIN_FILENO, FIONREAD, &bytesWaiting);

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return bytesWaiting;
}

int32_t fopen_s(FILE ** a, const int8_t * b, const int8_t * c)
{
	FILE * fp = fopen(b, c);
	*a = fp;
	return (fp > 0) ? 0 : -1;
}

/* A function to get a single character on Linux */
#define max(a,b) ((a) > (b) ? a : b)
#define min(a,b) ((a) < (b) ? a : b)
#endif


#define VERSION		1
#define ISSUE		3

static int32_t cycles = 0;

#define BUFFER_SIZE 	10000 // Used for block and streaming mode examples

// AWG Parameters
#define	AWG_DAC_FREQUENCY		200e6
#define	AWG_PHASE_ACCUMULATOR	4294967296.0

typedef enum {
	MODEL_NONE = 0,
	MODEL_PS6402 = 0x6402, //Bandwidth: 350MHz, Memory: 32MS, AWG
	MODEL_PS6402A = 0xA402, //Bandwidth: 250MHz, Memory: 128MS, FG
	MODEL_PS6402B = 0xB402, //Bandwidth: 250MHz, Memory: 256MS, AWG
	MODEL_PS6402C = 0xC402, //Bandwidth: 350MHz, Memory: 256MS, AWG
	MODEL_PS6402D = 0xD402, //Bandwidth: 350MHz, Memory: 512MS, AWG
	MODEL_PS6403 = 0x6403, //Bandwidth: 350MHz, Memory: 1GS, AWG
	MODEL_PS6403A = 0xA403, //Bandwidth: 350MHz, Memory: 256MS, FG
	MODEL_PS6403B = 0xB403, //Bandwidth: 350MHz, Memory: 512MS, AWG
	MODEL_PS6403C = 0xC403, //Bandwidth: 350MHz, Memory: 512MS, AWG
	MODEL_PS6403D = 0xD403, //Bandwidth: 350MHz, Memory: 1GS, AWG
	MODEL_PS6404 = 0x6404, //Bandwidth: 500MHz, Memory: 1GS, AWG
	MODEL_PS6404A = 0xA404, //Bandwidth: 500MHz, Memory: 512MS, FG
	MODEL_PS6404B = 0xB404, //Bandwidth: 500MHz, Memory: 1GS, AWG
	MODEL_PS6404C = 0xC404, //Bandwidth: 350MHz, Memory: 1GS, AWG
	MODEL_PS6404D = 0xD404, //Bandwidth: 350MHz, Memory: 2GS, AWG
	MODEL_PS6407 = 0x6407, //Bandwidth: 1GHz,	 Memory: 2GS, AWG

} MODEL_TYPE;

typedef struct
{
	int16_t DCcoupled;
	int16_t range;
	int16_t enabled;
}CHANNEL_SETTINGS;

typedef struct tTriggerDirections
{
	enum enPS6000ThresholdDirection channelA;
	enum enPS6000ThresholdDirection channelB;
	enum enPS6000ThresholdDirection channelC;
	enum enPS6000ThresholdDirection channelD;
	enum enPS6000ThresholdDirection ext;
	enum enPS6000ThresholdDirection aux;
}TRIGGER_DIRECTIONS;

typedef struct tPwq
{
	struct tPS6000PwqConditions * conditions;
	int16_t nConditions;
	enum enPS6000ThresholdDirection direction;
	uint32_t lower;
	uint32_t upper;
	PS6000_PULSE_WIDTH_TYPE type;
}PWQ;

typedef struct
{
	int16_t handle;
	MODEL_TYPE				model;
	int8_t					modelString[8];
	int8_t					serial[10];
	int16_t					complete;
	int16_t					openStatus;
	int16_t					openProgress;
	PS6000_RANGE			firstRange;
	PS6000_RANGE			lastRange;
	int16_t					channelCount;
	BOOL					AWG;
	CHANNEL_SETTINGS		channelSettings[PS6000_MAX_CHANNELS];
	int32_t					awgBufferSize;
}UNIT;

static uint32_t	timebase = 8;
static int16_t		oversample = 1;
int32_t      scaleVoltages = TRUE;

static uint16_t inputRanges[PS6000_MAX_RANGES] = { 10,
												20,
												50,
												100,
												200,
												500,
												1000,
												2000,
												5000,
												10000,
												20000,
												50000 };
static BOOL        g_ready = FALSE;
static int64_t		g_times[PS6000_MAX_CHANNELS];
static int16_t     g_timeUnit;
static uint32_t    g_sampleCount;
static uint32_t	g_startIndex;
static int16_t     g_autoStopped;
static int16_t     g_trig = 0;
static uint32_t	g_trigAt = 0;
static int16_t		g_overflow;
static int8_t      BlockFile[20] = "block.txt";
static int8_t      ETSBlockFile[20] = "ETS_block.txt";
static int8_t      StreamFile[20] = "stream.txt";

typedef struct tBufferInfo
{
	UNIT * unit;
	int16_t **driverBuffers;
	int16_t **appBuffers;

} BUFFER_INFO;

void StreamDataHandler(UNIT * unit, uint32_t preTrigger, char * fname);

int32_t adc_to_mv(int32_t raw, int32_t ch);
void SetDefaults(UNIT * unit);
PICO_STATUS HandleDevice(UNIT * unit);
PICO_STATUS OpenDevice(UNIT *unit, int8_t *serial);
void CloseDevice(UNIT *unit);


