# BGA Preheater Project

## Description
This project is a DIY heating table for soldering SMD QFN/BGA components, operating from 220V AC. It uses an Arduino Nano for temperature control and user interface.

<!-- Optional: Add a photo here -->
<!-- ![Preheater Photo](images/preheater_photo1.jpg) -->

## Features
- Target temperature setting for PCB preheating.
- Real-time display of setpoint, actual PCB temperature, and heater temperature.
- Optional soak timer with auto-off functionality.
- Temperature monitoring with three K-type thermocouples (MAX6675).
- User interface via rotary encoder and LCD 2004 display.
- Power control using an SSR (Solid State Relay) with zero-crossing detection.
- Hardware over-temperature protection (KSD9700 thermostat).

## Main Components
* Arduino Nano
* 500W 220V Aluminum Heater (e.g., 160x100mm with ceramic element)
* 3x MAX6675 Thermocouple Modules
* 3x K-Type Thermocouples
* LCD 2004 I2C Display
* Rotary Encoder with Push Button
* SSR-5FA (or similar 5A+ DC-AC SSR with Zero Crossing)
* KSD9700 Thermostat (e.g., 250°C NC)
* HLK-PM01 (or similar 220V AC to 5V DC power module)

## Software
The project uses an Arduino sketch that implements:
* PID algorithm for temperature control.
* User interface logic for settings and operation.
* EEPROM storage for user settings.
* Libraries: `Wire.h`, `LiquidCrystal_I2C.h`, `max6675.h`, `Encoder.h`, `PID_v1.h`, `EEPROM.h`.

## Wiring
Refer to the schematic diagram provided in the `schematic/` folder.
**CAUTION: This project involves 220V AC. Ensure all high-voltage connections are properly insulated, secured, and that the device chassis is properly grounded for safety.**

## Usage
1.  Power on the device.
2.  Use the rotary encoder to navigate the setup menu:
    *   Set Target PCB Temperature.
    *   Set Soak Time.
    *   Enable/Disable Timer.
3.  Confirm settings by pressing the encoder button.
4.  Once "READY" is displayed, press the encoder button to start the heating process.
5.  The preheater will reach and maintain the target temperature. If the timer is enabled, it will count down the soak time.
6.  A long press on the encoder button at any active stage (Heating, Soaking, Ready) will stop the process.

## Disclaimer
This is a DIY project involving high voltages. Build and operate at your own risk. The author(s) are not responsible for any damage or injury.