# ECSE 444: Embedded "Bop It!" Game (STM32)

## 🚀 Project Goal

This project is an interactive, voice-prompted game similar to "Bop It!", built entirely on an **STM32L4 Discovery IoT Node (B-L4S5I-IOT01A)**. The player must quickly respond to random commands using the board's sensors and buttons before a timer runs out.

The game features **progressive difficulty** (the response time gets shorter as you score higher) and a critical **power optimization** mode for battery efficiency.

## ✨ Quick Demo (Screenshots)

### Game Running on the Host Program

A Python GUI displays the score, the current command, and the remaining time.

> **Screenshot Placeholder:** Add a screenshot of the PC terminal/GUI displaying the game state (score, time, current command).
> `![Game Screenshot](assets/game_screenshot.png)`

### Hardware Setup

The complete hardware setup showing the STM32 board, the speaker, and the ToF sensor location.

> **Screenshot Placeholder:** Add a photo of your B-L4S5I-IOT01A board with all connected peripherals (speaker, etc.).
> `![Hardware Setup](assets/hardware_photo.jpg)`

## 🕹️ Implemented Commands

The game uses the following actions, based on the board's integrated peripherals:

| Command | Action Required | Hardware Input Used | 
| :--- | :--- | :--- | 
| "Bop it!" | Press the blue User Button. | GPIO | 
| "Clap it!" | Clap your hands near the board. | ST-MEMS Microphone (via DFSDM + Circular DMA) | 
| "Cover it!" | Cover the device with your hand. | Time-of-Flight (ToF) Sensor (VL53L0X) |

## ⚙️ Key Technical Features

* **Voice Prompts:** Commands are played via a DAC-connected speaker using audio data stored on the board's QSPI Flash memory.

* **Real-time Feedback:** Game data (score, command) is sent via **UART** to a PC running a custom **Python GUI (Pygame-CE)** for clear visualization. Data is transferred using JSON encoding.

* **Extreme Power Saving:** Between games, the microcontroller enters **HAL STOP Mode**, reducing current draw from \~30mA to **\~0.1mA** for maximum efficiency.

## 🛠️ Setup and Installation

### Prerequisites

* STM32L4 Discovery Kit IoT Node (B-L4S5I-IOT01A)

* STM32CubeIDE

* Python 3 (with `pyserial` and `pygame-ce`)

### Flashing the Firmware

1. Clone this repository.

2. Open the project in **STM32CubeIDE**, build the code, and flash it to the board.

### Running the Host Display

1. Connect the board and identify the **Virtual COM Port** (e.g., `COM3` on Windows, `/dev/ttyACM0` on Linux/Mac).

2. Navigate to the `HostProgram/` directory (where the Python script is).

3. Run the display script, replacing `COM_PORT` with your port:

### How to Play

1. Start the game by pressing the **User Button**.

2. Listen for the voice command and execute the corresponding physical action before the timer hits zero.

3. Have fun!
