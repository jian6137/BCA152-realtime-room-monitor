# Real-Time Room Environment Monitor

> *View the full project breakdown, story, and tutorial on Hackster.io!*  
> [**Real-Time Room Environment Monitor with FreeRTOS on ESP32**](https://www.hackster.io/555143/real-time-room-environment-monitor-with-freertos-on-esp32-e8939b)

A FreeRTOS based environmental monitoring system for the ESP32. It tracks temperature, humidity, ambient light, and motion, while using an event-driven architecture to manage an OLED display, rotary encoder, and alarm buzzer.

## How to Try It Out

You can easily clone and run this project using the PlatformIO IDE.

**1. Clone the repository:**
```bash
git clone https://github.com/jian6137/BCA152-realtime-room-monitor.git
cd BCA152-realtime-room-monitor
```

**2. Open the project:**
Make sure you have the [PlatformIO IDE extension](https://platformio.org/install/ide?install=vscode) installed in VS Code. Open the `BCA152-realtime-room-monitor` folder. PlatformIO will automatically download the required ESP-IDF frameworks.

**3. Build and upload to your ESP32:**
Connect your ESP32 via USB and run:
```bash
pio run -e esp32dev -t upload
```

*(You can also run this project in the browser using the Wokwi simulator by loading the provided `diagram.json` and `wokwi.toml` files).*

## Documentation & Laboratory Reports

All extended documentation, including the laboratory reports, static analysis findings, and Hackster.io drafts can be found in the [`/docs`](./docs) directory of this repository.

## Hardware Pinout

| Component | ESP32 Pin | Details |
| :--- | :--- | :--- |
| DHT22 | GPIO 4 | Digital (Bit-Banging) |
| LDR | GPIO 34 | Analog (ADC1_CHANNEL_6) |
| PIR Sensor | GPIO 14 | Digital Input |
| Rotary Encoder (CLK) | GPIO 26 | Digital Input (Hardware Interrupt) |
| Rotary Encoder (DT) | GPIO 27 | Digital Input |
| SSD1306 OLED (SDA) | GPIO 21 | I2C |
| SSD1306 OLED (SCL) | GPIO 22 | I2C |
| Active Buzzer | GPIO 13 | Digital Output |

![Hardware Pinout Diagram](https://res.cloudinary.com/dezykjpfz/image/upload/v1790400051/c2662811-6e8f-4603-b15b-b30f5d4fe578.png)

## Software Architecture

The system uses FreeRTOS to divide operations into 6 separate tasks.

### Tasks
1. **sensor_task:** Reads DHT22 and LDR every 2 seconds. Evaluates alarm limits.
2. **motion_task:** Reads PIR sensor every 500ms.
3. **state_task:** Manages the 15-second auto-sleep timer.
4. **input_task:** Deep sleeps until awoken by a hardware interrupt from the rotary encoder. Changes display modes.
5. **alarm_task:** Controls the buzzer if temperature thresholds are breached.
6. **display_task:** Updates the OLED screen only when new data arrives or states change.

### FreeRTOS IPC (Inter-Process Communication)
- **Queue (`sensorQueue`):** Safely passes sensor readings from `sensor_task` to `display_task`.
- **Event Group (`systemEventGroup`):** Contains `EVENT_MOTION`, `EVENT_ACTIVE`, and `EVENT_ALARM`. Used to wake up sleeping tasks instantly without polling loops.
- **Mutex (`serialMutex`):** Locks the UART Serial Console to prevent corrupted text when multiple tasks print at the same time.
- **Task Notifications:** Used inside the hardware ISR to instantly wake up the `input_task` from a 0% CPU state.

![Software Architecture Diagram](https://res.cloudinary.com/dezykjpfz/image/upload/v1790400076/softarch_nost8s.png)

## State Machine Logic

The system operates in two main states to save power:
- **ACTIVE:** The OLED display is on. The rotary encoder can switch between Temperature, Humidity, Light, and Motion screens. The buzzer will sound if the temperature drops below 18°C or goes above 30°C.
- **INACTIVE (Sleep):** If the PIR sensor detects no motion for exactly 15 seconds, the OLED screen turns off, the rotary encoder is ignored, and the buzzer is disabled. Any new motion instantly returns the system to ACTIVE.

![State Machine Diagram](https://res.cloudinary.com/dezykjpfz/image/upload/v1790400107/state_machine_ohy6e4.png)

## Code Overview

The firmware is modularized to separate hardware drivers from business logic, making it highly testable:

- `src/main.cpp`: Entry point. Initializes FreeRTOS Tasks, IPC objects (Queues, Mutexes, Event Groups), and the Rotary Encoder Hardware ISR.
- `src/sensors.cpp`: Custom hardware drivers. Implements precise microsecond bit-banging for the DHT22 and one-shot ADC reads for the LDR.
- `src/display.cpp`: Handles OLED rendering via I2C, mapping system state and sensor data to the physical screen.
- `src/alarm.cpp`: Contains pure logic for temperature limit evaluations.
- `src/input.cpp`: Manages the circular navigation math for the display menus.
- `src/system_state.cpp`: Contains the pure logic for the 15-second timeout and sleep evaluation.

## Unit Testing

The project includes native hardware mocked unit tests for the alarm limits, menu navigation, and state machine timeout logic.

**To run the automated unit test cases:**
```bash
pio test -e native
```

![Unit Tests Output](https://res.cloudinary.com/dezykjpfz/image/upload/v1790400324/4d3790b6-1a1d-4588-b334-3c5977d37aa6.png)

## Static Analysis

Static analysis is configured using `clangtidy` to enforce security (`cert-*`), logic (`bugprone-*`), and optimization (`performance-*`) checks.

**To run static analysis:**
```bash
pio check -e esp32dev
```

![Static Analysis Output](https://res.cloudinary.com/dezykjpfz/image/upload/v1790403026/3face549-0f3b-433a-8fe4-1b23393a1734.png)
