# SmartBumper

SmartBumper is an embedded systems project, built on an ATmega328P microcontroller (Arduino Uno board). The system measures the speed of a moving vehicle using two IR sensors and activates a physical barrier (bumper) and an audible alarm if the measured speed exceeds the 30 km/h limit.

The project is written in C (without high-level Arduino libraries), directly manipulating the microcontroller's hardware registers.

## Features and Concepts
* **GPIO:** Acoustic control of the buzzer and servomotor positioning via custom software pulse generation using general-purpose digital output pins.
* **External Interrupts (INT0, INT1):** Asynchronous and instantaneous capture of the vehicle passing in front of the IR sensors to achieve maximum timing precision, eliminating loop polling latency.
* **Timers (Timer1):** Configured in CTC (Clear Timer on Compare Match) mode to generate a stable system tick every 1ms (`systicks`), which serves as the base clock for the speed calculation formula.
* **I2C / TWI (Two-Wire Interface):** Low-level implementation of the I2C communication protocol from scratch to transmit commands and characters to the LCD screen display.

## Project Structure (File Hierarchy)
* `src/main.c` - Initialization of peripherals, Interrupt Service Routines (ISRs), and the core speed calculation and decision logic loop.
* `src/i2c.h` / `src/i2c.c` - Custom hardware driver for communication over the microcontroller's built-in TWI/I2C bus.
* `src/lcd.h` / `src/lcd.c` - Driver for the 16x2 LCD screen, managing the RS/EN control flags and data transmission in 4-bit mode.

## Hardware Connections (Pin Configuration)
* **IR Sensor 1 (Start):** Pin D2 (PD2 / INT0) -> Triggers the start timestamp capture (`t_start`).
* **IR Sensor 2 (Stop):** Pin D3 (PD3 / INT1) -> Triggers the stop timestamp capture (`t_stop`) and initiates speed processing.
* **SG90 Servomotor:** Pin D9 (PB1 / GPIO) -> Controlled for precise barrier lifting and lowering.
* **Buzzer:** Pin D12 (PB4 / GPIO) -> Driven directly via digital output for acoustic alerts.
* **I2C LCD Screen:** Pins A4 (SDA / PC4) and A5 (SCL / PC5) -> Displays real-time speed data and speed limit violation warnings.

## Installation and Deployment
1. Open the project root directory in **VS Code** with the **PlatformIO** extension installed.
2. Connect the Arduino Uno board to your computer using a USB cable.
3. If necessary, adjust the servomotor calibration constants (`PULS_INITIAL_US` and `PULS_RIDICAT_US`) located at the top of the `src/main.c` file to match your physical assembly.
4. Click the **Build** button.
5. Click the **Upload** button.