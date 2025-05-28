#include "pid_control.h"
#include "globals.h" // To access PID objects, SSR_PIN, state
#include <Arduino.h> // DigitalWrite, millis

// PID Object & variables 
double pidSetpoint, pidInput, pidOutput;
PID preheaterPID(&pidInput, &pidOutput, &pidSetpoint, KP_DEFAULT, KI_DEFAULT, KD_DEFAULT, DIRECT);
unsigned long windowStartTime;

void initPid() {
    preheaterPID.SetMode(AUTOMATIC);
    preheaterPID.SetOutputLimits(0, PID_WINDOW_SIZE_MS);
    windowStartTime = millis();
}

void controlHeater() { 
    unsigned long now = millis();
    if (now - windowStartTime >= PID_WINDOW_SIZE_MS) { 
        windowStartTime += PID_WINDOW_SIZE_MS;
    }
    if (pidOutput > (now - windowStartTime)) { digitalWrite(SSR_PIN, HIGH); } 
    else { digitalWrite(SSR_PIN, LOW); }
}

void runPidAndControlHeater() {
    // PID uses row tempPcb1
    pidSetpoint = currentSettings.targetPcbTemp; 
    pidInput = tempPcb1; 
    
    if (!isnan(pidInput)) { 
        preheaterPID.Compute(); 
        controlHeater(); 
    } else {         
        digitalWrite(SSR_PIN, LOW); // Safe state when sensor error occurs
        if (Serial) Serial.println(F("PID: NaN input, heater OFF"));
    }
}