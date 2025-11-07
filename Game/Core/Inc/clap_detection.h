/*
 * clap_detection.h
 *
 *  Created on: Nov 7, 2025
 *      Author: Nico
 */

#ifndef INC_CLAP_DETECTION_H_
#define INC_CLAP_DETECTION_H_

#include "main.h" // for HAL types and DFSDM handle

// Checks for a clap within the defined window
// Returns 1 if clap detected, 0 otherwise
uint8_t ClapDetector_CheckForClap(void);

// DMA callback to signal buffer is filled
void ClapDetector_DMA_Complete_Callback(void);

#endif /* INC_CLAP_DETECTION_H_ */
