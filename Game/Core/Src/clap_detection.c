/*
 * 	clap_detection.c
 *
 *  Created on: Nov 7, 2025
 *  Author: Nico
 */

#include "clap_detection.h"

extern DFSDM_Filter_HandleTypeDef hdfsdm1_filter0; // from main.c

static int32_t clapBuf[CLAP_BUF_SIZE];
static volatile uint8_t clapDetected = 0;
static uint32_t lastClapTime = 0;

// ---------- Initialization ----------
void ClapDetector_Init(void) {
    clapDetected = 0;
    lastClapTime = 0;
}

// ---------- Start Continuous Recording ----------
void ClapDetector_Start(void) {
	HAL_DFSDM_FilterRegularStart_DMA(&hdfsdm1_filter0, clapBuf, CLAP_BUF_SIZE);
}

// ---------- Callback: Half buffer filled ----------
void HAL_DFSDM_FilterRegConvHalfCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter) {
    ClapDetector_ProcessSamples(clapBuf, CLAP_BUF_SIZE / 2);
}

// ---------- Callback: Full buffer filled ----------
//void HAL_DFSDM_FilterRegConvCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter) {
//    ClapDetector_ProcessSamples(&clapBuf[CLAP_BUF_SIZE / 2], CLAP_BUF_SIZE / 2);
//}

// ---------- Main loop polling function ----------
uint8_t ClapDetector_WasClapDetected(void) {
    if (clapDetected) {
        clapDetected = 0;  // reset flag
        return 1;
    }
    return 0;
}

// ---------- Process samples and detect claps ----------
void ClapDetector_ProcessSamples(int32_t *samples, uint32_t len) {
    int32_t maxAmp = 0;
    for (uint32_t i = 0; i < len; i++) {
        int32_t val = samples[i];
        if (val < 0) val = -val;
        if (val > maxAmp) maxAmp = val;
    }

    if (maxAmp > CLAP_THRESHOLD) {
        uint32_t now = HAL_GetTick();
        if (now - lastClapTime > CLAP_DEBOUNCE_MS) {
            clapDetected = 1;
            lastClapTime = now;
        }
    }
}
