/************************************************************************************
* USB接続のオシロスコープ「PicoScope」を使って計測を行うためのプログラム
* 実行を確認したデバイスは「PicoScope 6402C」
* 
* 作成者：上田 龍幸
* 更新日：2019/02/26
*
**************************************************************************************/

#include "pch.h"
#include <iostream>

int main()
{
	PICO_STATUS status = PICO_OK;
	UNIT unit;
	int32_t timeIndisposed;
	uint32_t sampleCount = BUFFER_SIZE;
	uint32_t maxSamples;
	uint32_t segmentIndex = 0;
	ps6000BlockReady lpReady = 0;
	int16_t * buffers[PS6000_MAX_CHANNEL_BUFFERS];

	float timeInterval = 0.00f;

	do {
		memset(&unit, 0, sizeof(UNIT));
		status = OpenDevice(&unit, NULL);
		if (status == PICO_OK || status == PICO_USB3_0_DEVICE_NON_USB3_0_PORT) {
			unit.openStatus = status;
			break;
		}
	} while (status != PICO_NOT_FOUND);

	printf("Found one device, opening...\n\n");
	status = unit.openStatus;
	std::cout << unit.openStatus << std::endl;

	if (status == PICO_OK || status == PICO_USB3_0_DEVICE_NON_USB3_0_PORT) {
		status = HandleDevice(&unit);
	}

	if (status != PICO_OK) {
		printf("Picoscope devices open failed, error code 0x%x\n", (uint32_t)status);
		return 1;
	}

	//ポートA，B，C，Dの設定を行う（第5引数の主に計測レンジの設定を行う）
	status = ps6000SetChannel(unit.handle, (PS6000_CHANNEL)(PS6000_CHANNEL_A),
		unit.channelSettings[PS6000_CHANNEL_A].enabled,
		(PS6000_COUPLING)unit.channelSettings[PS6000_CHANNEL_A].DCcoupled,
		PS6000_50MV, 0, PS6000_BW_FULL);
	status = ps6000SetChannel(unit.handle, (PS6000_CHANNEL)(PS6000_CHANNEL_B),
		unit.channelSettings[PS6000_CHANNEL_B].enabled,
		(PS6000_COUPLING)unit.channelSettings[PS6000_CHANNEL_B].DCcoupled,
		PS6000_1V, 0, PS6000_BW_FULL);
	status = ps6000SetChannel(unit.handle, (PS6000_CHANNEL)(PS6000_CHANNEL_C),
		unit.channelSettings[PS6000_CHANNEL_C].enabled,
		(PS6000_COUPLING)unit.channelSettings[PS6000_CHANNEL_C].DCcoupled,
		PS6000_50MV, 0, PS6000_BW_FULL);
	status = ps6000SetChannel(unit.handle, (PS6000_CHANNEL)(PS6000_CHANNEL_D),
		unit.channelSettings[PS6000_CHANNEL_D].enabled,
		(PS6000_COUPLING)unit.channelSettings[PS6000_CHANNEL_D].DCcoupled,
		PS6000_50MV, 0, PS6000_BW_FULL);

	char fname[50];		//出力するファイル名
	sprintf_s(fname, "./output/out.csv");

	StreamDataHandler(&unit, 0, fname);		//計測
	Sleep(1000);

	CloseDevice(&unit);						//デバイス終了
	printf("Exit...\n");

	return 0;
}
