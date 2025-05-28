#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <max6675.h>
#include <Encoder.h>
#include <PID_v1.h>
#include "config.h" // Включаем для SystemState и Settings

// LCD
extern LiquidCrystal_I2C lcd;

// Thermocouples
extern MAX6675 tcHeater;
extern MAX6675 tcPcb1;
extern MAX6675 tcPcb2;
extern double tempHeater, tempPcb1, tempPcb2;

// Moving Average for PCB1
extern double pcb1TempHistory[MOVING_AVG_SIZE];
extern int pcb1TempHistoryIndex;
extern double pcb1TempMovingAvg;
extern double previousPcb1TempMovingAvg;
extern bool movingAvgInitialized;

// Encoder
extern Encoder knob;
extern long encoderOldPosition;

// Button
extern volatile bool g_encoderButtonSignal; 
extern unsigned long buttonDownStartTime;    
extern bool buttonIsBeingProcessed;      

// PID
extern double pidSetpoint, pidInput, pidOutput;
extern PID preheaterPID;
extern unsigned long windowStartTime;

// System State
extern SystemState currentState;
extern SystemState previousStateDisplay;

// Settings
extern Settings currentSettings;

// Timer
extern unsigned long soakTimerStartTime, remainingSoakTimeMillis;
extern bool soakTimerRunning;

// Blinking
extern unsigned long lastBlinkTime;
extern bool blinkState;

// General Timing
extern unsigned long lastTempReadTime;
extern unsigned long lastLcdUpdateTime;

#endif // GLOBALS_H