
#ifndef INC_UART_DISPLAY_H_
#define INC_UART_DISPLAY_H_

#include "app.h"
#include <stdbool.h>

void UD_UpdateAppMode(UART_HandleTypeDef *huart, AppMode app_mode);

void UD_UpdateCommand(UART_HandleTypeDef *huart, BopItCommand command);

void UD_UpdateScore(UART_HandleTypeDef *huart, uint32_t score);

void UD_CommandDone(UART_HandleTypeDef *huart, bool success);

void UD_UpdateLowPowerMode(UART_HandleTypeDef *huart, bool lowPower);

void UD_UpdateTimeLeft(UART_HandleTypeDef *huart, uint32_t timeLeft);

void UD_SendEmpty(UART_HandleTypeDef *huart);


#endif /* INC_UART_DISPLAY_H_ */
