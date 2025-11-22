/*
 * tof.h
 *
 *  Created on: Nov 13, 2025
 *      Author: Justin
 */

#ifndef INC_TOF_H_
#define INC_TOF_H_

#include "stm32l4xx_hal.h"

#define TOF_DISTANCE_UNKNOWN -1
#define TOF_SENSITIVITY 5				// Larger value -> Less sensitive

typedef enum
{
	TOF_SUCCESS,
	TOF_ERROR,
	TOF_PENDING
} TOF_Status;

HAL_StatusTypeDef TOF_get_distance(I2C_HandleTypeDef *hi2c, int *distance_p);
TOF_Status TOF_run_task(I2C_HandleTypeDef *hi2c);

#endif /* INC_TOF_H_ */
