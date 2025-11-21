#include "app.h"


#define APP_MODE_IDLE 0
#define APP_MODE_PLAYING_COMMAND 1
#define APP_MODE_CAPTURING_RESPONSE 2

#define COMMAND_NONE 0
#define COMMAND_CLAP_IT 1

volatile static uint8_t app_mode = APP_MODE_IDLE;
volatile static uint8_t command = COMMAND_NONE;
volatile static uint32_t score = 0;

void BOPIT_Start() {
    while (1) {
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
        case APP_MODE_PLAYING_COMMAND:
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
