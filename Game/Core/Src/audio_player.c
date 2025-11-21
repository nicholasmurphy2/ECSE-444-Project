
#include "stm32l4s5i_iot01_qspi.h"
#include "audio_player.h"

#define REC_BUFFER_SIZE 64000
#define WAIT_TIME 100
#define REC_DURATION_MS 2000

#define BLOCKS_PER_RECORDING 4

#define APP_MODE_IDLE 0
#define APP_MODE_RECORDING 1
#define APP_MODE_PLAYBACK 2


int32_t recBuf[REC_BUFFER_SIZE];
uint32_t playBuf[REC_BUFFER_SIZE];

volatile uint8_t AP_RecordingReady = 0;
volatile uint8_t AP_IsRecording = 0;
volatile uint8_t AP_IsPlaying = 0;

DAC_HandleTypeDef *dac;
DFSDM_Filter_HandleTypeDef *filter;

volatile uint8_t app_mode = 0;
volatile uint8_t recording_number = 0;

// Initializes the audio player.
void AP_Init(DAC_HandleTypeDef *d, DFSDM_Filter_HandleTypeDef *f) {
	dac = d;
	filter = f;
}

// Handles the recording full interrupt.
void AP_HandleRecordingFull() {
	int32_t maxVal = -2147483648;
	int32_t minVal = 2147483647;

	HAL_DFSDM_FilterRegularStop_DMA(filter);
	for(int i = 0; i < REC_BUFFER_SIZE; i++){
		recBuf[i] = recBuf[i] >> 8;
		if(recBuf[i] < minVal){
			minVal = recBuf[i];
		}
		if(recBuf[i] > maxVal){
			maxVal = recBuf[i];
		}
	}

	if(minVal < 0) minVal = -1 * minVal;

	float temp = (float)((float)4095/((float)maxVal+(float)minVal));

	for(int j = 0; j < REC_BUFFER_SIZE; j++){
		recBuf[j] = recBuf[j] + minVal;
		playBuf[j] = temp * recBuf[j];
	}

	uint32_t start_address = recording_number * REC_BUFFER_SIZE * 4;
	if (BSP_QSPI_Write((uint8_t *)playBuf, start_address, REC_BUFFER_SIZE * 4) != QSPI_OK){
		Error_Handler();
	}

	for(int i = 0; i < REC_BUFFER_SIZE; i++){
		playBuf[i] = 0;
	}

	AP_IsRecording = 0;
	AP_RecordingReady = 1;
}

// Internal function to start the microphone recording.
void start_recording() {
	HAL_DFSDM_FilterRegularStart_DMA(filter, recBuf, REC_BUFFER_SIZE);
	AP_IsRecording = 1;
}

// Internal function to play the recorded buffer.
void play_recording() {
	uint32_t start_address = recording_number * REC_BUFFER_SIZE * 4;
	if (BSP_QSPI_Read((uint8_t *)playBuf, start_address, REC_BUFFER_SIZE * 4) != QSPI_OK){
		Error_Handler();
	}
	HAL_DAC_Start_DMA(dac, DAC_CHANNEL_1, (uint32_t *)playBuf, REC_BUFFER_SIZE, DAC_ALIGN_12B_R);
	AP_IsPlaying = 1;
}

// Internal function to stop the playing of the recording.
void stop_recording() {
	HAL_DAC_Stop_DMA(dac, DAC_CHANNEL_1);
	AP_IsPlaying = 0;
}

// Handles the button press interrupt.
void AP_HandleButtonPress() {
	if (AP_IsRecording) {
		return;
	}

	switch (app_mode) {
		case APP_MODE_IDLE:
			app_mode = APP_MODE_RECORDING;
			break;
		case APP_MODE_RECORDING:
			recording_number++;
			if (AP_IsPlaying) {
				stop_recording();
			}
			break;
		case APP_MODE_PLAYBACK:
			break;
	}
}

// Plays the recording synchronously.
void AP_PlayRecording(int rec_num) {
	recording_number = rec_num;
	play_recording();
	uint32_t start_time = HAL_GetTick();
	while (HAL_GetTick() - start_time < REC_DURATION_MS) {}
	stop_recording();
}

// Plays the recording asynchronously.
void AP_PlayRecording_Async(int rec_num) {
	recording_number = rec_num;
	play_recording();
}

// Stops the recording.
void AP_StopRecording() {
	stop_recording();
}

// Starts loading n recordings and saving them on the QSPI memory.
void AP_StartLoadingProcedure(uint8_t n) {
	// erase blocks needed for the recordings
	for(int i = 0; i < n * BLOCKS_PER_RECORDING; i++){
		if (BSP_QSPI_Erase_Block(i * 0x10000) != QSPI_OK){
			Error_Handler();
		}
	}

	// signal ready to start loading
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);

	while (1) {
		if (AP_IsRecording == 1) {
			HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
			HAL_Delay(WAIT_TIME);
		}

		switch (app_mode) {
			case APP_MODE_IDLE:
				break;
			case APP_MODE_RECORDING:
				if (recording_number == n) {
					app_mode = APP_MODE_PLAYBACK;
					continue;
				}

				if (!AP_IsRecording && !AP_RecordingReady && !AP_IsPlaying) {
					start_recording();
				}

				if (AP_RecordingReady) {
					AP_RecordingReady = 0;
					play_recording();
				}
				break;
			case APP_MODE_PLAYBACK:
				for (int i = 0; i < n; i++) {
					AP_PlayRecording(i);
				}
				break;
		}
	}
	
}
