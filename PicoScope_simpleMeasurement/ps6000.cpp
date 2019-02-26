#include "pch.h"
#include <iostream>
#include<fstream>

/****************************************************************************
* Callback
* Used by PS6000 data streaming collection calls, on receipt of data.
* Used to set global flags etc checked by user routines
*
* In this example, a BUFFER_INFO structure holding pointers to the
* driver and application buffers for a device is used to allow the data
* from the driver buffers to be copied into the application buffers.
*
****************************************************************************/
void PREF4 CallBackStreaming(int16_t handle,
	uint32_t noOfSamples,
	uint32_t startIndex,
	int16_t overflow,
	uint32_t triggerAt,
	int16_t triggered,
	int16_t autoStop,
	void	*pParameter) {
	int32_t channel;
	BUFFER_INFO * bufferInfo = NULL;

	if (pParameter != NULL) {
		bufferInfo = (BUFFER_INFO *)pParameter;
	}

	// used for streaming
	g_sampleCount = noOfSamples;
	g_startIndex = startIndex;
	g_autoStopped = autoStop;
	g_overflow = overflow;
	// flag to say done reading data
	g_ready = TRUE;

	// flags to show if & where a trigger has occurred
	g_trig = triggered;
	g_trigAt = triggerAt;

	if (bufferInfo != NULL && noOfSamples) {
		for (channel = 0; channel < bufferInfo->unit->channelCount; channel++) {
			if (bufferInfo->unit->channelSettings[channel].enabled) {
				if (bufferInfo->appBuffers && bufferInfo->driverBuffers) {
					// Copy data...

					// Max buffers
					if (bufferInfo->appBuffers[channel * 2] && bufferInfo->driverBuffers[channel * 2]) {
						memcpy_s(&bufferInfo->appBuffers[channel * 2][startIndex], noOfSamples * sizeof(int16_t),
							&bufferInfo->driverBuffers[channel * 2][startIndex], noOfSamples * sizeof(int16_t));
					}

					// Min buffers
					if (bufferInfo->appBuffers[channel * 2 + 1] && bufferInfo->driverBuffers[channel * 2 + 1]) {
						memcpy_s(&bufferInfo->appBuffers[channel * 2 + 1][startIndex], noOfSamples * sizeof(int16_t),
							&bufferInfo->driverBuffers[channel * 2 + 1][startIndex], noOfSamples * sizeof(int16_t));
					}
				}
			}
		}
	}
}

/****************************************************************************
* Initialise unit' structure with Variant specific defaults
****************************************************************************/
void set_info(UNIT * unit) {
	int16_t i = 0;
	int16_t r = 20;
	int8_t line[20];
	int32_t variant;

	int8_t description[11][25] = { "Driver Version",
		"USB Version",
		"Hardware Version",
		"Variant Info",
		"Serial",
		"Cal Date",
		"Kernel",
		"Digital H/W",
		"Analogue H/W",
		"Firmware 1",
		"Firmware 2" };

	if (unit->handle) {
		for (i = 0; i < 11; i++) {
			ps6000GetUnitInfo(unit->handle, line, sizeof(line), &r, i);

			if (i == 3) {
				// info = 3 - PICO_VARIANT_INFO

				variant = atoi((const char*)line);
				memcpy(&(unit->modelString), line, sizeof(unit->modelString) == 7 ? 7 : sizeof(unit->modelString));

				//To identify A or B model variants.....
				if (strlen((const char*)line) == 4)							// standard, not A, B, C or D, convert model number into hex i.e 6402 -> 0x6402
				{
					variant += 0x4B00;
				}
				else {
					if (strlen((const char*)line) == 5)						// A, B, C or D variant unit 
					{
						line[4] = toupper(line[4]);

						switch (line[4]) {
						case 65: // i.e 6402A -> 0xA402
							variant += 0x8B00;
							break;
						case 66: // i.e 6402B -> 0xB402
							variant += 0x9B00;
							break;
						case 67: // i.e 6402C -> 0xC402
							variant += 0xAB00;
							break;
						case 68: // i.e 6402D -> 0xD402
							variant += 0xBB00;
							break;
						default:
							break;
						}
					}
				}
			}

			if (i == 4) {
				// info = 4 - PICO_BATCH_AND_SERIAL
				ps6000GetUnitInfo(unit->handle, unit->serial, sizeof(unit->serial), &r, PICO_BATCH_AND_SERIAL);
			}

			printf("%s: %s\n", description[i], line);
		}

		switch (variant) {
		case MODEL_PS6402:
			unit->model = MODEL_PS6402;
			unit->firstRange = PS6000_50MV;
			unit->lastRange = PS6000_20V;
			unit->channelCount = 4;
			unit->AWG = TRUE;
			unit->awgBufferSize = MAX_SIG_GEN_BUFFER_SIZE;

			for (i = 0; i < PS6000_MAX_CHANNELS; i++) {
				unit->channelSettings[i].range = PS6000_5V;
				unit->channelSettings[i].DCcoupled = PS6000_DC_1M;
				unit->channelSettings[i].enabled = TRUE;
			}
			break;

		case MODEL_PS6402A:
			unit->model = MODEL_PS6402A;
			unit->firstRange = PS6000_50MV;
			unit->lastRange = PS6000_20V;
			unit->channelCount = 4;
			unit->AWG = FALSE;
			unit->awgBufferSize = 0;

			for (i = 0; i < PS6000_MAX_CHANNELS; i++) {
				unit->channelSettings[i].range = PS6000_5V;
				unit->channelSettings[i].DCcoupled = PS6000_DC_1M;
				unit->channelSettings[i].enabled = TRUE;
			}
			break;

		case MODEL_PS6402B:
			unit->model = MODEL_PS6402B;
			unit->firstRange = PS6000_50MV;
			unit->lastRange = PS6000_20V;
			unit->channelCount = 4;
			unit->AWG = TRUE;
			unit->awgBufferSize = MAX_SIG_GEN_BUFFER_SIZE;

			for (i = 0; i < PS6000_MAX_CHANNELS; i++) {
				unit->channelSettings[i].range = PS6000_5V;
				unit->channelSettings[i].DCcoupled = PS6000_DC_1M;
				unit->channelSettings[i].enabled = TRUE;
			}
			break;

		case MODEL_PS6402C:
			unit->model = MODEL_PS6402C;
			unit->firstRange = PS6000_50MV;
			unit->lastRange = PS6000_20V;
			unit->channelCount = 4;
			unit->AWG = FALSE;
			unit->awgBufferSize = 0;

			for (i = 0; i < PS6000_MAX_CHANNELS; i++) {
				unit->channelSettings[i].range = PS6000_5V;
				unit->channelSettings[i].DCcoupled = PS6000_DC_1M;
				unit->channelSettings[i].enabled = TRUE;
			}
			break;

		case MODEL_PS6402D:
			unit->model = MODEL_PS6402D;
			unit->firstRange = PS6000_50MV;
			unit->lastRange = PS6000_20V;
			unit->channelCount = 4;
			unit->AWG = TRUE;
			unit->awgBufferSize = PS640X_C_D_MAX_SIG_GEN_BUFFER_SIZE;

			for (i = 0; i < PS6000_MAX_CHANNELS; i++) {
				unit->channelSettings[i].range = PS6000_5V;
				unit->channelSettings[i].DCcoupled = PS6000_DC_1M;
				unit->channelSettings[i].enabled = TRUE;
			}
			break;

		case MODEL_PS6403:
			unit->model = MODEL_PS6403;
			unit->firstRange = PS6000_50MV;
			unit->lastRange = PS6000_20V;
			unit->channelCount = 4;
			unit->AWG = TRUE;
			unit->awgBufferSize = MAX_SIG_GEN_BUFFER_SIZE;

			for (i = 0; i < PS6000_MAX_CHANNELS; i++) {
				unit->channelSettings[i].range = PS6000_5V;
				unit->channelSettings[i].DCcoupled = PS6000_DC_1M;
				unit->channelSettings[i].enabled = TRUE;
			}
			break;

		case MODEL_PS6403A:
			unit->model = MODEL_PS6403;
			unit->firstRange = PS6000_50MV;
			unit->lastRange = PS6000_20V;
			unit->channelCount = 4;
			unit->AWG = FALSE;
			unit->awgBufferSize = 0;

			for (i = 0; i < PS6000_MAX_CHANNELS; i++) {
				unit->channelSettings[i].range = PS6000_5V;
				unit->channelSettings[i].DCcoupled = PS6000_DC_1M;
				unit->channelSettings[i].enabled = TRUE;
			}
			break;

		case MODEL_PS6403B:
			unit->model = MODEL_PS6403B;
			unit->firstRange = PS6000_50MV;
			unit->lastRange = PS6000_20V;
			unit->channelCount = 4;
			unit->AWG = TRUE;
			unit->AWG = MAX_SIG_GEN_BUFFER_SIZE;

			for (i = 0; i < PS6000_MAX_CHANNELS; i++) {
				unit->channelSettings[i].range = PS6000_5V;
				unit->channelSettings[i].DCcoupled = PS6000_DC_1M;
				unit->channelSettings[i].enabled = TRUE;
			}
			break;

		case MODEL_PS6403C:
			unit->model = MODEL_PS6403C;
			unit->firstRange = PS6000_50MV;
			unit->lastRange = PS6000_20V;
			unit->channelCount = 4;
			unit->AWG = FALSE;
			unit->awgBufferSize = 0;

			for (i = 0; i < PS6000_MAX_CHANNELS; i++) {
				unit->channelSettings[i].range = PS6000_5V;
				unit->channelSettings[i].DCcoupled = PS6000_DC_1M;
				unit->channelSettings[i].enabled = TRUE;
			}
			break;

		case MODEL_PS6403D:
			unit->model = MODEL_PS6403D;
			unit->firstRange = PS6000_50MV;
			unit->lastRange = PS6000_20V;
			unit->channelCount = 4;
			unit->AWG = TRUE;
			unit->awgBufferSize = PS640X_C_D_MAX_SIG_GEN_BUFFER_SIZE;

			for (i = 0; i < PS6000_MAX_CHANNELS; i++) {
				unit->channelSettings[i].range = PS6000_5V;
				unit->channelSettings[i].DCcoupled = PS6000_DC_1M;
				unit->channelSettings[i].enabled = TRUE;
			}
			break;

		case MODEL_PS6404:
			unit->model = MODEL_PS6404;
			unit->firstRange = PS6000_50MV;
			unit->lastRange = PS6000_20V;
			unit->channelCount = 4;
			unit->AWG = TRUE;
			unit->awgBufferSize = MAX_SIG_GEN_BUFFER_SIZE;

			for (i = 0; i < PS6000_MAX_CHANNELS; i++) {
				unit->channelSettings[i].range = PS6000_5V;
				unit->channelSettings[i].DCcoupled = PS6000_DC_1M;
				unit->channelSettings[i].enabled = TRUE;
			}
			break;

		case MODEL_PS6404A:
			unit->model = MODEL_PS6404;
			unit->firstRange = PS6000_50MV;
			unit->lastRange = PS6000_20V;
			unit->channelCount = 4;
			unit->AWG = FALSE;
			unit->awgBufferSize = 0;

			for (i = 0; i < PS6000_MAX_CHANNELS; i++) {
				unit->channelSettings[i].range = PS6000_5V;
				unit->channelSettings[i].DCcoupled = PS6000_DC_1M;
				unit->channelSettings[i].enabled = TRUE;
			}
			break;

		case MODEL_PS6404B:
			unit->model = MODEL_PS6404B;
			unit->firstRange = PS6000_50MV;
			unit->lastRange = PS6000_20V;
			unit->channelCount = 4;
			unit->AWG = TRUE;
			unit->awgBufferSize = MAX_SIG_GEN_BUFFER_SIZE;

			for (i = 0; i < PS6000_MAX_CHANNELS; i++) {
				unit->channelSettings[i].range = PS6000_5V;
				unit->channelSettings[i].DCcoupled = PS6000_DC_1M;
				unit->channelSettings[i].enabled = TRUE;
			}
			break;

		case MODEL_PS6404C:
			unit->model = MODEL_PS6404C;
			unit->firstRange = PS6000_50MV;
			unit->lastRange = PS6000_20V;
			unit->channelCount = 4;
			unit->AWG = TRUE;
			unit->awgBufferSize = 0;

			for (i = 0; i < PS6000_MAX_CHANNELS; i++) {
				unit->channelSettings[i].range = PS6000_5V;
				unit->channelSettings[i].DCcoupled = PS6000_DC_1M;
				unit->channelSettings[i].enabled = TRUE;
			}
			break;

		case MODEL_PS6404D:
			unit->model = MODEL_PS6404D;
			unit->firstRange = PS6000_50MV;
			unit->lastRange = PS6000_20V;
			unit->channelCount = 4;
			unit->AWG = TRUE;
			unit->awgBufferSize = PS640X_C_D_MAX_SIG_GEN_BUFFER_SIZE;

			for (i = 0; i < PS6000_MAX_CHANNELS; i++) {
				unit->channelSettings[i].range = PS6000_5V;
				unit->channelSettings[i].DCcoupled = PS6000_DC_1M;
				unit->channelSettings[i].enabled = TRUE;
			}
			break;

		case MODEL_PS6407:
			unit->model = MODEL_PS6407;
			unit->firstRange = PS6000_100MV;
			unit->lastRange = PS6000_100MV;
			unit->channelCount = 4;
			unit->AWG = TRUE;

			for (i = 0; i < PS6000_MAX_CHANNELS; i++) {
				unit->channelSettings[i].range = PS6000_100MV;
				unit->channelSettings[i].DCcoupled = PS6000_DC_50R;
				unit->channelSettings[i].enabled = TRUE;
			}
			break;

		default:
			break;
		}

	}
}


/****************************************************************************
*
* Select timebase, set oversample to on and time units as nano seconds
*
****************************************************************************/
void SetTimebase(UNIT * unit) {
	float timeInterval = 0.00f;
	uint32_t maxSamples;
	PICO_STATUS status;

	do {
		printf("Specify timebase: ");
		fflush(stdin);
		scanf_s("%lud", &timebase);

		status = ps6000GetTimebase2(unit->handle, timebase, BUFFER_SIZE, &timeInterval, 1, &maxSamples, 0);

		if (status == PICO_INVALID_TIMEBASE) {
			printf("Invalid timebase\n\n");
			break;
		}

	} while (status == PICO_INVALID_TIMEBASE);

	printf("Timebase %lu - %.2f ns\n", timebase, timeInterval);
	oversample = TRUE;
}

/****************************************************************************
* Stream Data Handler
* - Used by the two stream data examples - untriggered and triggered
* Inputs:
* - unit - the unit to sample on
* - preTrigger - the number of samples in the pre-trigger phase
*					(0 if no trigger has been set)
***************************************************************************/
void StreamDataHandler(UNIT * unit, uint32_t preTrigger, char * fname) {
	uint32_t i, j;
	uint32_t sampleCount = BUFFER_SIZE; /*  Make sure buffer large enough to collect data on each iteration */
	FILE * fp;
	int16_t * buffers[PS6000_MAX_CHANNEL_BUFFERS];
	int16_t * appBuffers[PS6000_MAX_CHANNEL_BUFFERS]; // Application buffers to copy data into
	PICO_STATUS status;
	uint32_t sampleInterval = 1;
	uint32_t totalSamples;
	uint32_t previousTotal = 0;
	int16_t autoStop = TRUE;
	uint32_t postTrigger = 100000;	//計測トリガ数
	uint32_t downsampleRatio = 2;	//postTriggerをどれくらいでダウンサンプルするか
	uint32_t triggeredAt = 0;

	BUFFER_INFO bufferInfo;

	for (i = PS6000_CHANNEL_A; (int32_t)i < unit->channelCount; i++) // create data buffers
	{
		if (unit->channelSettings[i].enabled) {
			buffers[i * 2] = (int16_t*)calloc(sampleCount, sizeof(int16_t));
			buffers[i * 2 + 1] = (int16_t*)calloc(sampleCount, sizeof(int16_t));

			status = ps6000SetDataBuffers(unit->handle, (PS6000_CHANNEL)i, buffers[i * 2], buffers[i * 2 + 1],
				sampleCount, PS6000_RATIO_MODE_AGGREGATE);

			appBuffers[i * 2] = (int16_t*)calloc(sampleCount, sizeof(int16_t));
			appBuffers[i * 2 + 1] = (int16_t*)calloc(sampleCount, sizeof(int16_t));
		}
	}

	// Set information in structure
	bufferInfo.unit = unit;
	bufferInfo.driverBuffers = buffers;
	bufferInfo.appBuffers = appBuffers;

	if (autoStop) {
		printf("\nStreaming Data for %lu samples", postTrigger / downsampleRatio);

		// we pass 0 for preTrigger if we're not setting up a trigger
		printf(preTrigger ? " after the trigger occurs\nNote: %lu Pre Trigger samples before Trigger arms\n\n" : "\n\n", preTrigger / downsampleRatio);
	}
	else {
		printf("\nStreaming Data continually...\n\n");
	}

	g_autoStopped = FALSE;

	status = ps6000RunStreaming(unit->handle, &sampleInterval, PS6000_US, preTrigger, postTrigger - preTrigger,
		autoStop, downsampleRatio, PS6000_RATIO_MODE_AGGREGATE, sampleCount);

	printf(status ? "\nps6000RunStreaming status = 0x%x\n" : "", status);

	std::ofstream outputfile(fname);

	totalSamples = 0;

	while (!_kbhit() && !g_autoStopped) {
		/* Poll until data is received. Until then, GetStreamingLatestValues wont call the callback */
		Sleep(1);
		g_ready = FALSE;
		status = ps6000GetStreamingLatestValues(unit->handle, CallBackStreaming, &bufferInfo);

		if (status != PICO_OK && status != PICO_BUSY) {
			printf("Streaming status return 0x%x\n", status);
			break;
		}

		if (g_ready && g_sampleCount > 0) /* can be ready and have no data, if autoStop has fired */
		{
			if (g_trig) {
				triggeredAt = totalSamples += g_trigAt;		// calculate where the trigger occurred in the total samples collected
			}

			previousTotal = totalSamples;
			totalSamples += g_sampleCount;

			printf("\nCollected %3li samples, index = %5lu, Total: %6d samples ", g_sampleCount, g_startIndex, totalSamples);

			int32_t* ChA = new int32_t[totalSamples]; //要素数がtotalSamples個のint32_t型の配列のためのメモリを動的に確保

			if (g_trig) {
				printf("Trig. at index %lu Total at trigger: %lu", triggeredAt, previousTotal + (triggeredAt - g_startIndex + 1));	// show where trigger occurred
			}

			//データ取得
			for (i = g_startIndex; i < (g_startIndex + g_sampleCount); i++) {
				for (j = PS6000_CHANNEL_A; (int32_t)j < unit->channelCount; j++) {
					if (unit->channelSettings[j].enabled) {
						outputfile << (adc_to_mv(appBuffers[j * 2][i], unit->channelSettings[PS6000_CHANNEL_A + j].range) + adc_to_mv(appBuffers[j * 2 + 1][i], unit->channelSettings[PS6000_CHANNEL_A + j].range)) / 2 << ", ";
					}
				}
				outputfile << "\n";
			}
		}
	}
	printf("\n\n");
	outputfile.close();

	ps6000Stop(unit->handle);

	if (!g_autoStopped) {
		printf("data collection aborted\n");
		_getch();
	}

	for (i = PS6000_CHANNEL_A; (int32_t)i < unit->channelCount; i++) {
		if (unit->channelSettings[i].enabled) {
			free(buffers[i * 2]);
			free(buffers[i * 2 + 1]);
		}
	}
}



/****************************************************************************
* OpenDevice
* Parameters
* - unit        pointer to the UNIT structure, where the handle will be stored
* - serial		pointer to the int8_t array containing serial number
*
* Returns
* - PICO_STATUS to indicate success, or if an error occurred
***************************************************************************/
PICO_STATUS OpenDevice(UNIT *unit, int8_t *serial) {
	PICO_STATUS status;

	if (serial == NULL) {
		status = ps6000OpenUnit(&unit->handle, NULL);
	}
	else {
		status = ps6000OpenUnit(&unit->handle, serial);
	}

	unit->openStatus = status;
	unit->complete = 1;

	return status;
}

/****************************************************************************
* SetDefaults - restore default settings
****************************************************************************/
void SetDefaults(UNIT * unit) {
	PICO_STATUS status;
	int32_t i;

	status = ps6000SetEts(unit->handle, PS6000_ETS_OFF, 0, 0, NULL); // Turn off ETS

	for (i = 0; i < unit->channelCount; i++) // reset channels to most recent settings
	{
		status = ps6000SetChannel(unit->handle, (PS6000_CHANNEL)(PS6000_CHANNEL_A + i),
			unit->channelSettings[PS6000_CHANNEL_A + i].enabled,
			(PS6000_COUPLING)unit->channelSettings[PS6000_CHANNEL_A + i].DCcoupled,
			(PS6000_RANGE)unit->channelSettings[PS6000_CHANNEL_A + i].range, 0, PS6000_BW_FULL);
	}

}

/****************************************************************************
* adc_to_mv
*
* Convert an 16-bit ADC count into millivolts
****************************************************************************/
int32_t adc_to_mv(int32_t raw, int32_t ch) {
	//cout << "raw:" << raw << "  ch:" << ch << "  ret:" << (raw * inputRanges[ch]) / PS6000_MAX_VALUE << endl;
	return (raw * inputRanges[ch]) / PS6000_MAX_VALUE;
}

/****************************************************************************
* HandleDevice
* Parameters
* - unit        pointer to the UNIT structure, where the handle will be stored
*
* Returns
* - PICO_STATUS to indicate success, or if an error occurred
***************************************************************************/
PICO_STATUS HandleDevice(UNIT * unit) {
	int16_t value = 0;

	struct tPwq pulseWidth;
	struct tTriggerDirections directions;

	printf("Handle: %d\n", unit->handle);
	if (unit->openStatus != PICO_OK) {
		printf("Unable to open device\n");
		printf("Error code : 0x%08x\n", (uint32_t)unit->openStatus);
		while (!_kbhit());
		std::cout << "bbb\n";

		exit(99); // exit program
	}

	printf("Device opened successfully, cycle %d\n\n", ++cycles);
	// setup device info - unless it's set already
	if (unit->model == MODEL_NONE) {
		set_info(unit);
	}
	timebase = 1;

	memset(&directions, 0, sizeof(struct tTriggerDirections));
	memset(&pulseWidth, 0, sizeof(struct tPwq));

	std::cout << "a\n";

	SetDefaults(unit);

	/* Trigger disabled	*/
	//SetTrigger(unit->handle, NULL, 0, NULL, 0, &directions, &pulseWidth, 0, 0, 0);

	return unit->openStatus;
}
void CloseDevice(UNIT *unit) {
	ps6000CloseUnit(unit->handle);
}