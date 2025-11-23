// PRIVATE INCLUDES

#include "app.h"
#include "uart_display.h"
#include "tof.h"
#include "clap_detection.h"

// PRIVATE DEFINITIONS

#define DO_POWER_SAVING
#define NUM_COMMANDS 3
#define MIN_TIME_LIMIT 4000
#define MAX_TIME_LIMIT 8000

typedef enum {
	RESPONSE_STATUS_SUCCESS,
	RESPONSE_STATUS_FAILURE
} ResponseStatus;

typedef enum {
	COUNTDOWN_STATUS_IN_PROGRESS,
	COUNTDOWN_STATUS_OUT_OF_TIME
} CountdownStatus;

// PRIVATE VARIABLES

// Data specific to current game
volatile static AppMode app_mode = APP_MODE_IDLE;
volatile static uint32_t score = 0;

// Data specific to the current command and response
volatile static BopItCommand command = COMMAND_NONE;
volatile static uint32_t time_limit;
volatile static uint32_t time_elapsed;
ResponseStatus response_status;

// For use by countdown functions
volatile static uint32_t initial_timestamp;

// For use by rand function
volatile static uint32_t seed = 0;

// Peripheral handles
static I2C_HandleTypeDef *hi2c = NULL;
static UART_HandleTypeDef *huart = NULL;

// PRIVATE FUNCTION DECLARATIONS

void handle_capture();
void game_over_sequence();
uint8_t rand(uint8_t max_value);
uint32_t compute_time_limit(uint32_t min, uint32_t max, uint32_t score);
void start_countdown();
CountdownStatus update_countdown_and_get_result();

// EXPORTED FUNCTIONS

// Peripheral handle exchange functions
void BOPIT_Set_I2C_Handle(I2C_HandleTypeDef *hi2c_in) {
    if (hi2c_in != NULL) {
        hi2c = hi2c_in;
    }
}

void BOPIT_Set_UART_Handle(UART_HandleTypeDef *huart_in) {
    if (huart_in != NULL) {
        huart = huart_in;
    }
}

/**
  * @brief  Called to reset game data in preparation for next game.
  */
void reset_game_data() {
	app_mode = APP_MODE_IDLE;
    score = 0;
	command = COMMAND_NONE;
    time_limit = MAX_TIME_LIMIT;
    response_status = RESPONSE_STATUS_FAILURE;	// Redundant statement that should be unused
}

void BOPIT_Start() {
    reset_game_data();
    ClapDetector_Init();
    ClapDetector_Start();
    while (1) {
    		UD_UpdateGameStatus(huart, app_mode, command);
    		UD_UpdateScore(huart, score);
        switch (app_mode) {
            case APP_MODE_IDLE:
                break;
            case APP_MODE_PLAYING_COMMAND:
                command = rand(NUM_COMMANDS - 1) + 1;
                // TODO: Play command
                // AP_PlayRecording(command)
                HAL_Delay(1000);	// Add small delay between rounds
                app_mode = APP_MODE_CAPTURING_RESPONSE;
                break;
            case APP_MODE_CAPTURING_RESPONSE:
            		time_limit = compute_time_limit(MIN_TIME_LIMIT, MAX_TIME_LIMIT, score);
            		handle_capture();
                if (response_status == RESPONSE_STATUS_FAILURE) {
                    game_over_sequence();
                    reset_game_data();
                    app_mode = APP_MODE_IDLE;
					#ifdef DO_POWER_SAVING
						HAL_SuspendTick();
						HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
						HAL_ResumeTick();
						SystemClock_Config();	// Reconfigure system clock after STOP mode
					#endif
                } else {
                    score++;
                    app_mode = APP_MODE_PLAYING_COMMAND;
                }
                break;
        }
    }
}

/**
  * @brief  Must be called by GPIO button press callback.
  */
void BOPIT_Handle_Button_Press() {
    switch (app_mode) {
        case APP_MODE_IDLE:
            app_mode = APP_MODE_PLAYING_COMMAND;
            break;
        case APP_MODE_PLAYING_COMMAND:
        		__NOP();
            break;
        case APP_MODE_CAPTURING_RESPONSE:
      			if (command == COMMAND_BOP_IT) {
      					response_status = RESPONSE_STATUS_SUCCESS;
      			}
        	  break;
    }
}

/**
  * @brief  Blocking function that updates response status before returning
  */
void handle_capture() {
    switch (command) {
        case COMMAND_NONE:
						#ifdef DEBUG
        		Error_Handler();
						#endif
            break;
        case COMMAND_CLAP_IT:
        	response_status = RESPONSE_STATUS_FAILURE;
        	ClapDetector_WasClapDetected();
        	start_countdown();

        	for (;;) {
        		if (update_countdown_and_get_result() == COUNTDOWN_STATUS_OUT_OF_TIME) {
        			response_status = RESPONSE_STATUS_FAILURE;
        	        break;
        	    }

        	    if (ClapDetector_WasClapDetected()) {
        	    	response_status = RESPONSE_STATUS_SUCCESS;
        	        break;
        	    }
        	}
        	break;
        case COMMAND_COVER_IT:
            start_countdown();
            for (;;) {
            		if (update_countdown_and_get_result() == COUNTDOWN_STATUS_OUT_OF_TIME) {
            			response_status = RESPONSE_STATUS_FAILURE;
            			break;
            		} else if (TOF_run_task(hi2c) == TOF_SUCCESS) {
            			response_status = RESPONSE_STATUS_SUCCESS;
            			break;
            		}
            		// Otherwise, TOF_status = TOF_PENDING so we loop around to try again
        	  }
        	  break;
        case COMMAND_BOP_IT:
        		// Sets the response status to failure at beginning
        	  response_status = RESPONSE_STATUS_FAILURE;
        	  start_countdown();
        		for (;;) {
        				if ((update_countdown_and_get_result() == COUNTDOWN_STATUS_OUT_OF_TIME) ||
        						(response_status == RESPONSE_STATUS_SUCCESS)) {
        					  break;
        				}
        		}
        		// When button is pressed, the interrupt will call BOPIT_Handle_Button_Press to update
        		// the response status to RESPONSE_STATUS_SUCCESS.
        		break;
        default:
						#ifdef DEBUG
						Error_Handler();
						#endif
            break;
    }
}

// PRIVATE FUNCTION IMPLEMENTATIONS

/**
  * @brief  Indicates to the user that the game has ended.
  */
void game_over_sequence() {
		// TODO
		__NOP();
}

/**
  * @brief  Returns a pseudorandom integer in the range [0, max_value].
  */
uint8_t rand(uint8_t max_value) {
		if (seed == 0) {
				seed = (uint32_t)HAL_GetTick();
		}
		seed = seed * 1103515245 + 12345;
		return  (uint8_t)((seed / 65536) % 32768) % (max_value + 1);
}

/**
  * @brief  Computes time limit for next command based on score.
  * @param min  Minimum time limit in milliseconds.
  * @param max	Maximum time limit milliseconds.
  * @retval Computed time limit.
  */
uint32_t compute_time_limit(uint32_t min, uint32_t max, uint32_t score) {
    if (score < 3) {
        return max;
    } else if (score < 20) {
        return max - ((score - 3) * ((max - min) / (20 - 3)));
    } else {
        return min;
    }
}

/**
  * @brief  Starts countdown for user to correctly respond to command
  */
void start_countdown() {
  	initial_timestamp = HAL_GetTick();
}

/**
  * @brief  Updates/checks the countdown. If used, this function must be called periodically
  * 				in order to function.
  * @retval Returns COUNTDOWN_STATUS_OUT_OF_TIME if countdown has completed. Otherwise returns
  * 				COUNTDOWN_STATUS_IN_PROGRESS if there is still time remaining.
  */
CountdownStatus update_countdown_and_get_result() {
	  // Updates global time elapsed variable
		time_elapsed = HAL_GetTick() - initial_timestamp;

		// This is a good place to send game data or debug messages over UART to the console

	  if (time_elapsed > time_limit) {
	  	  return COUNTDOWN_STATUS_OUT_OF_TIME;
	  }
	  // Else
	  return COUNTDOWN_STATUS_IN_PROGRESS;
}
