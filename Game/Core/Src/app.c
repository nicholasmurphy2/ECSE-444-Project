#include "app.h"
#include "uart_display.h"

volatile static AppMode app_mode = APP_MODE_IDLE;
volatile static BopItCommand command = COMMAND_NONE;
volatile static uint32_t score = 0;

void BOPIT_Start(UART_HandleTypeDef *huart) {
    while (1) {
    	UD_UpdateGameStatus(huart, app_mode, command);
    	UD_UpdateScore(huart, score);

        switch (app_mode) {
            case APP_MODE_IDLE:
                break;
            case APP_MODE_PLAYING_COMMAND:

                command = COMMAND_CLAP_IT;
                // PLAY COMMAND HERE...
                // AP_PlayRecording(command)
                
                app_mode = APP_MODE_CAPTURING_RESPONSE;
                break;
            case APP_MODE_CAPTURING_RESPONSE:
                handle_capture();
                break;
        }
    }
}

void BOPIT_Handle_Button_Press() {
    switch (app_mode) {
        case APP_MODE_IDLE:
            app_mode = APP_MODE_PLAYING_COMMAND;
            break;
        default:
            break;
    }
}

void handle_capture() {
    switch (command) {
        case COMMAND_NONE:
            break;
        case COMMAND_CLAP_IT:
            // CAPTURE RESPONSE HERE...
            break;
    }
}
