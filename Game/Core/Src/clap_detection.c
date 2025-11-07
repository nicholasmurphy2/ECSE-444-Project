/*
 * 	clap_detection.c
 *
 *  Created on: Nov 7, 2025
 *  Author: Nico
 */

#include "clap_detection.h"

#define CLAP_THRESHOLD  1500000
#define CLAP_WINDOW_MS  1000
#define CLAP_BUF_SIZE   64000   // DFSDM buffer size for detection

extern DFSDM_Filter_HandleTypeDef hdfsdm1_filter0; // from main.c

static int32_t clapBuf[CLAP_BUF_SIZE];
static volatile uint8_t clapRecordingDone = 0;

uint8_t ClapDetector_CheckForClap(void) {
    clapRecordingDone = 0;

    // Start DMA recording
    HAL_DFSDM_FilterRegularStart_DMA(&hdfsdm1_filter0, clapBuf, CLAP_BUF_SIZE);

    // Wait until DMA completes or timeout
    uint32_t start = HAL_GetTick();
    while (!clapRecordingDone) {
        if (HAL_GetTick() - start > CLAP_WINDOW_MS) {
            HAL_DFSDM_FilterRegularStop_DMA(&hdfsdm1_filter0);
            break;
        }
    }

    // Analyze buffer
    int32_t maxAmp = 0;
    for (int i = 0; i < CLAP_BUF_SIZE; i++) {
        int32_t val = clapBuf[i];
        if (val < 0) val = -val;
        if (val > maxAmp) maxAmp = val;
    }

    return (maxAmp > CLAP_THRESHOLD);
}

void ClapDetector_DMA_Complete_Callback(void) {
	HAL_DFSDM_FilterRegularStop_DMA(&hdfsdm1_filter0);
	clapRecordingDone = 1;
}

