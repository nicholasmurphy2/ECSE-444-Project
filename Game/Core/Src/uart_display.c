
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

void UD_UpdateAppMode(UART_HandleTypeDef *huart, AppMode app_mode) {
	if (huart == NULL) {
		return;
	}
	char buffer[100];
	char app_mode_str[25];

	AppMode_ToString(app_mode, app_mode_str);

	sprintf(buffer, "{\"app_mode\": \"%s\"}\r\n", app_mode_str);
	HAL_UART_Transmit(huart, (uint8_t*)buffer, strlen(buffer), 250);
}

void UD_UpdateCommand(UART_HandleTypeDef *huart, BopItCommand command) {
	if (huart == NULL) {
		return;
	}
	char buffer[100];
	char command_str[25];

	Command_ToString(command, command_str);

	sprintf(buffer, "{\"command\": \"%s\"}\r\n", command_str);
	HAL_UART_Transmit(huart, (uint8_t*)buffer, strlen(buffer), 250);
}

void UD_UpdateScore(UART_HandleTypeDef *huart, uint32_t score) {
	if (huart == NULL) {
		return;
	}
	char buffer[100];

	sprintf(buffer, "{\"score\": %lu}\r\n", score);
	HAL_UART_Transmit(huart, (uint8_t*)buffer, strlen(buffer), 250);
}

void UD_CommandDone(UART_HandleTypeDef *huart, bool success) {
	if (huart == NULL) {
		return;
	}

	char buffer[100];
	if (success) {
		sprintf(buffer, "{\"command_done_success\": true}\r\n");
	} else {
		sprintf(buffer, "{\"command_done_success\": false}\r\n");
	}
	HAL_UART_Transmit(huart, (uint8_t*)buffer, strlen(buffer), 250);
}

void UD_UpdateLowPowerMode(UART_HandleTypeDef *huart, bool lowPower) {
	if (huart == NULL) {
		return;
	}

	char buffer[100];
	if (lowPower) {
		sprintf(buffer, "{\"low_power\": true}\r\n");
	} else {
		sprintf(buffer, "{\"low_power\": false}\r\n");
	}
	HAL_UART_Transmit(huart, (uint8_t*)buffer, strlen(buffer), 250);
}

void UD_UpdateTimeLeft(UART_HandleTypeDef *huart, uint32_t timeLeft) {
	if (huart == NULL) {
		return;
	}
	char buffer[100];

	sprintf(buffer, "{\"time_left\": %lu}\r\n", timeLeft);
	HAL_UART_Transmit(huart, (uint8_t*)buffer, strlen(buffer), 250);
}

void UD_SendEmpty(UART_HandleTypeDef *huart) {
	if (huart == NULL) {
		return;
	}
	char buffer[10];

	sprintf(buffer, "\r\n");
	HAL_UART_Transmit(huart, (uint8_t*)buffer, strlen(buffer), 250);
}
