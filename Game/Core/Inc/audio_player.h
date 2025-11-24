///**
// ******************************************************************************
// * @file    audio_player.h
// * @brief   Audio Player/Recorder Module for STM32L4
// ******************************************************************************
// *
// * Audio recording and playback module using DFSDM (mic input), DAC (output),
// * and QSPI flash (storage). Records clips in the loading procedure and saves them on the QSPI memory.
// * Can be ran as an application to load recordings and also used as a library to play loaded recordings.
// *
// * Initialization:
// * - Initialize with AP_Init() with the DAC and DFSDM handles before use.
// * - Configure button and microphone interrupts to call AP_HandleButtonPress() and AP_HandleRecordingFull() respectively.
// *
// * Usage:
// * - As loading procedure:
// *   - Call the app entry point AP_StartLoadingProcedure(n) to record n clips.
// *   - Press button to start recording, and subsequent presses to record multiple clips.
// *
// * - As playback library:
// *   - AP_PlayRecording(recording_number) (sync) or AP_PlayRecording_Async(recording_number) (async).
// *   - Check AP_IsRecording, AP_IsPlaying, AP_RecordingReady status flags.
// *
// * - As playback library:
// *   - Call AP_StopRecording() to stop the recording.
// *
// * @note
// * - Each recording uses 256 KB QSPI memory (4 blocks). Uses DMA for I/O.
// * - The recording can be accessed by the recording number, 0-indexed (same order as in the loading procedure).
// * - AP_StopRecording() can be called at any time to stop the recording (must be called when using AP_PlayRecording_Async()).
// *
// ******************************************************************************
// */
//
#ifndef INC_AUDIO_PLAYER_H_
#define INC_AUDIO_PLAYER_H_

#include "main.h"

extern volatile uint8_t AP_RecordingReady;
extern volatile uint8_t AP_IsRecording;
extern volatile uint8_t AP_IsPlaying;

void AP_Init(DAC_HandleTypeDef *d, DFSDM_Filter_HandleTypeDef *f);

void AP_HandleRecordingFull();

void AP_HandleButtonPress();

void AP_StartLoadingProcedure(uint8_t n);

void AP_PlayRecording(int rec_num);

void AP_PlayRecording_Async(int rec_num);

void AP_StopRecording();

#endif /* INC_AUDIO_PLAYER_H_ */
