/*
 * tof.c
 *
 *  Created on: Nov 13, 2025
 *      Author: Justin
 */

#include "tof.h"
#include "string.h"

// DEFINTIONS

#ifdef USING_OS
    #define SYSTEM_DELAY(t) osDelay(t)
#else
    #define SYSTEM_DELAY(t) HAL_Delay(t)
#endif

// Constants for VL350X I2C Interface
const uint8_t WRITE_ADDRESS = 0x52;
const uint8_t READ_ADDRESS = 0x53;
const uint8_t MAX_BUFFER_SIZE = 2;
const uint8_t START_REQUEST [] = {0x00, 0x01};		// Must respect MAX_BUFFER_SIZE
const uint8_t GET_DATA_REQUEST[] = {0x1e};			// Must respect MAX_BUFFER_SIZE

// Magic numbers
const int MIN_VALUE = 1;
const int MAX_VALUE = 200;

// State variables
static int count = 0;

HAL_StatusTypeDef TOF_get_distance(I2C_HandleTypeDef *hi2c, int *distance_p)
{
	if (hi2c == NULL)
	{
		return HAL_ERROR;
	}
	if (distance_p == NULL)
	{
		return HAL_ERROR;
	}
	HAL_StatusTypeDef status = HAL_ERROR;
	uint8_t buffer[MAX_BUFFER_SIZE];
	// "Start" procedure
	SYSTEM_DELAY(1);
	memcpy(buffer, START_REQUEST, sizeof(START_REQUEST));
	status = HAL_I2C_Master_Transmit(hi2c, WRITE_ADDRESS, buffer, sizeof(START_REQUEST), 1);
	if (status != HAL_OK)
	{
		return status;
	}
	// Get distance
	SYSTEM_DELAY(1);
	memcpy(buffer, GET_DATA_REQUEST, sizeof(GET_DATA_REQUEST));
	status = HAL_I2C_Master_Transmit(hi2c, WRITE_ADDRESS, buffer, sizeof(GET_DATA_REQUEST), 1);
	if (status != HAL_OK)
	{
		return status;
	}
	SYSTEM_DELAY(1);
	status = HAL_I2C_Master_Receive(hi2c, READ_ADDRESS, buffer, sizeof(buffer), 1);
	if (status != HAL_OK)
	{
		return status;
	}
	*distance_p = (buffer[0] << 8) + buffer[1] - 20;
	if (*distance_p < MIN_VALUE || *distance_p > MAX_VALUE)
	{
		*distance_p = TOF_DISTANCE_UNKNOWN;
	}
	return HAL_OK;
}

TOF_Status TOF_run_task(I2C_HandleTypeDef *hi2c)
{
	//HAL_Delay(10);
	int distance = TOF_DISTANCE_UNKNOWN;
	if (TOF_get_distance(hi2c, &distance) != HAL_OK)
	{
		return TOF_ERROR;
	}
	if (distance == TOF_DISTANCE_UNKNOWN)
	{
		count = 0;
	}
	else
	{
		count++;
		if (count == TOF_SENSITIVITY)
		{
			return TOF_SUCCESS;
		}
	}
	return TOF_PENDING;
}
