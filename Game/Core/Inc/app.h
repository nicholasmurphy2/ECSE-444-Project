
#ifndef INC_APP_H_
#define INC_APP_H_

#include "main.h"


typedef enum {
	APP_MODE_IDLE,
	APP_MODE_PLAYING_COMMAND,
	APP_MODE_CAPTURING_RESPONSE
} AppMode;

typedef enum {
	COMMAND_NONE,
	COMMAND_CLAP_IT
} BopItCommand;


void BOPIT_Start(UART_HandleTypeDef *huart);

void BOPIT_Handle_Button_Press();

void handle_capture();


#endif
