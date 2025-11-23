
#include "uart_display.h"
#include <string.h>
#include <stdio.h>

void AppMode_ToString(AppMode app_mode, char *buffer) {
	switch (app_mode) {
	case APP_MODE_IDLE:
		sprintf(buffer, "idle");
		break;
	case APP_MODE_PLAYING_COMMAND:
		sprintf(buffer, "playing_command");
		break;
	case APP_MODE_CAPTURING_RESPONSE:
		sprintf(buffer, "capturing_response");
		break;
	default:
		sprintf(buffer, "unknown: %d", app_mode);
		break;
	}
}

void Command_ToString(BopItCommand command, char *buffer) {
	switch (command) {
	case COMMAND_NONE:
		sprintf(buffer, "none");
		break;
	case COMMAND_CLAP_IT:
		sprintf(buffer, "clap_it");
		break;
	case COMMAND_COVER_IT:
		sprintf(buffer, "cover_it");
		break;
	case COMMAND_BOP_IT:
		sprintf(buffer, "bop_it");
		break;
	default:
		sprintf(buffer, "unknown: %d", command);
		break;
	}
}

void UD_UpdateGameStatus(UART_HandleTypeDef *huart, AppMode app_mode, BopItCommand command) {
	if (huart == NULL) {
		return;
	}
	char buffer[200];
	char app_mode_str[25];
	char command_str[25];

	AppMode_ToString(app_mode, app_mode_str);
	Command_ToString(command, command_str);

	sprintf(buffer, "{\"app_mode\": \"%s\", \"command\": \"%s\"}\r\n", app_mode_str, command_str);
	HAL_UART_Transmit(huart, (uint8_t*)buffer, strlen(buffer), 100);
}

void UD_UpdateScore(UART_HandleTypeDef *huart, uint32_t score) {
	if (huart == NULL) {
		return;
	}
	char buffer[100];

	sprintf(buffer, "{\"score\": %lu}\r\n", score);
	HAL_UART_Transmit(huart, (uint8_t*)buffer, strlen(buffer), 100);
}
