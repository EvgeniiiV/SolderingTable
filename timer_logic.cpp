#include "timer_logic.h"
#include "globals.h" // currentSettings, переменным таймера
#include <Arduino.h> // millis

// Timer variables 
unsigned long soakTimerStartTime = 0, remainingSoakTimeMillis = 0;
bool soakTimerRunning = false;


void startSoakTimer() {
  if (currentSettings.isTimerEnabled) {
    remainingSoakTimeMillis = (unsigned long)currentSettings.soakMinutes * 60000UL + 
                              (unsigned long)currentSettings.soakSeconds * 1000UL;
    if (remainingSoakTimeMillis > 0) { 
        soakTimerStartTime = millis(); soakTimerRunning = true;
        if (Serial) { Serial.print(F("Soak timer started for: ")); Serial.println(remainingSoakTimeMillis); }
    } else { soakTimerRunning = false; }
  }
}

void stopSoakTimer() {
  soakTimerRunning = false; remainingSoakTimeMillis = 0; 
}

bool updateSoakTimerAndCheckCompletion() {
  if (soakTimerRunning && currentSettings.isTimerEnabled) {
    unsigned long el = millis() - soakTimerStartTime;
    unsigned long totalDur = (unsigned long)currentSettings.soakMinutes*60000UL + 
                             (unsigned long)currentSettings.soakSeconds*1000UL;
    if (el >= totalDur) { 
        remainingSoakTimeMillis = 0; 
        return true; // Timer finished
    } else { 
        remainingSoakTimeMillis = totalDur - el; 
        return false; // Timer is still running
    }
  }
  return false; // Timer off
}