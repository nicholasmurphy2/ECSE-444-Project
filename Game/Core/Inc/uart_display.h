
#ifndef INC_UART_DISPLAY_H_
#define INC_UART_DISPLAY_H_

#include "app.h"

void UD_UpdateGameStatus(UART_HandleTypeDef *huart, AppMode app_mode, BopItCommand command);

void UD_UpdateScore(UART_HandleTypeDef *huart, uint32_t score);


#endif /* INC_UART_DISPLAY_H_ */
