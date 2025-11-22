
#ifndef INC_APP_H_
#define INC_APP_H_

#include "main.h"

// EXPORTED DEFINITIONS

#ifndef DEBUG
#define DEBUG
#endif

#ifndef DO_POWER_SAVING
// #define DO_POWER_SAVING
#endif

typedef enum {
	APP_MODE_IDLE,
	APP_MODE_PLAYING_COMMAND,
	APP_MODE_CAPTURING_RESPONSE
} AppMode;

typedef enum {
	COMMAND_NONE,
	COMMAND_CLAP_IT,
	COMMAND_COVER_IT,
	COMMAND_BOP_IT
} BopItCommand;

// EXPORTED FUNCTION DECLARATIONS

void BOPIT_Set_I2C_Handle(I2C_HandleTypeDef *hi2c_in);
void BOPIT_Set_UART_Handle(UART_HandleTypeDef *huart_in);
void BOPIT_Start();
void BOPIT_Handle_Button_Press();
void handle_capture();		// why?

#endif
