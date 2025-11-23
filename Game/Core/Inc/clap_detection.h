/*
 * clap_detection.h
 *
 *  Created on: Nov 7, 2025
 *      Author: Nico
 */

#ifndef INC_CLAP_DETECTION_H_
#define INC_CLAP_DETECTION_H_

#include "main.h" // for HAL types and DFSDM handle

// Configuration
#define CLAP_THRESHOLD   1500000
#define CLAP_BUF_SIZE    4800     // Smaller buffer (~100ms if 48kHz)
#define CLAP_DEBOUNCE_MS 300      // Ignore multiple claps within 300 ms

void ClapDetector_Init(void);
void ClapDetector_Start(void);
uint8_t ClapDetector_WasClapDetected(void);

// Internal callbacks (called by HAL)
void ClapDetector_ProcessSamples(int32_t *samples, uint32_t len);

#endif /* INC_CLAP_DETECTION_H_ */
