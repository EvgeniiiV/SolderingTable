#include "encoder_control.h"
#include "globals.h"    // knob, state, currentSettings и т.д.
#include "timer_logic.h" // startSoakTimer, stopSoakTimer
#include "eeprom_settings.h" // saveSettingsToEEPROM
#include <Arduino.h>    // pinMode, digitalWrite, attachInterrupt, millis, delay

// Encoder Object 
Encoder knob(ENCODER_CLK_PIN, ENCODER_DT_PIN);
long encoderOldPosition = -999;

// Button variables 
volatile bool g_encoderButtonSignal = false; 
unsigned long buttonDownStartTime = 0;    
bool buttonIsBeingProcessed = false;

void initEncoderAndButton() {
    pinMode(ENCODER_SW_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ENCODER_SW_PIN), encoderButton_ISR_CHANGE, CHANGE);
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
    encoderOldPosition = knob.read() / 4; 
}

void shortBeep() {
  digitalWrite(BUZZER_PIN, HIGH); delay(100); digitalWrite(BUZZER_PIN, LOW);  
}
void longBeepSequence() {
  digitalWrite(BUZZER_PIN, HIGH); delay(150); digitalWrite(BUZZER_PIN, LOW);
  delay(100); 
  digitalWrite(BUZZER_PIN, HIGH); delay(150); digitalWrite(BUZZER_PIN, LOW);
}

void encoderButton_ISR_CHANGE() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterruptTime > BUTTON_DEBOUNCE_TIME_MS) { 
    g_encoderButtonSignal = true; 
  }
  lastInterruptTime = interruptTime;
}

void handleEncoderRotation() {
  long newPosition = knob.read() / 4; 
  if (newPosition != encoderOldPosition) {
    int change = newPosition - encoderOldPosition; encoderOldPosition = newPosition;
    switch (currentState) {
      case STATE_SETUP_TEMP:
        currentSettings.targetPcbTemp += change * 1.0; 
        if (currentSettings.targetPcbTemp < 50) currentSettings.targetPcbTemp = 50;
        if (currentSettings.targetPcbTemp > 200) currentSettings.targetPcbTemp = 200;
        break;
      case STATE_SETUP_TIME:
        { int ts = currentSettings.soakMinutes*60+currentSettings.soakSeconds;
          ts += change * 10; 
          if (ts < 0) ts = 0; if (ts > 3600) ts = 3600;
          currentSettings.soakMinutes = ts / 60; currentSettings.soakSeconds = ts % 60;
        } break;
      case STATE_SETUP_TIMER_EN:
        if (change != 0) { currentSettings.isTimerEnabled = !currentSettings.isTimerEnabled; }
        break;
      default: break;
    }
  }
}

void handleButton() {
    if (g_encoderButtonSignal) { 
        g_encoderButtonSignal = false; 
        bool buttonIsCurrentlyDown = (digitalRead(ENCODER_SW_PIN) == LOW);
        if (buttonIsCurrentlyDown && !buttonIsBeingProcessed) {
            buttonIsBeingProcessed = true; buttonDownStartTime = millis(); 
            if (Serial) Serial.println(F("Button PRESSED, processing started."));
        } else if (!buttonIsCurrentlyDown && buttonIsBeingProcessed) {
            if (Serial) Serial.println(F("Button RELEASED - SHORT press.")); 
            shortBeep();
            switch (currentState) {
                case STATE_OFF: case STATE_DONE: case STATE_STOPPED: case STATE_ERROR:
                    currentState = STATE_SETUP_TEMP;
                    knob.write(currentSettings.targetPcbTemp * 4);
                    encoderOldPosition = currentSettings.targetPcbTemp;
                    break;
                case STATE_SETUP_TEMP:
                    currentState = STATE_SETUP_TIME;
                    { int ts = currentSettings.soakMinutes*60+currentSettings.soakSeconds;
                      knob.write(ts / 10 * 4); encoderOldPosition = ts / 10; }
                    break;
                case STATE_SETUP_TIME:
                    currentState = STATE_SETUP_TIMER_EN;
                    knob.write(currentSettings.isTimerEnabled ? 0 : 4);
                    encoderOldPosition = currentSettings.isTimerEnabled ? 0 : 1;
                    break;
                case STATE_SETUP_TIMER_EN:
                    saveSettingsToEEPROM(); currentState = STATE_READY; break;
                case STATE_READY:
                    currentState = STATE_HEATING; windowStartTime = millis(); break; // windowStartTime in globals.h
                case STATE_SOAKING:
                    if(currentSettings.isTimerEnabled && soakTimerRunning) { // soakTimerRunning in globals.h
                        startSoakTimer(); 
                        if (Serial) Serial.println(F("Timer restarted by short press"));
                    } break;
                case STATE_HEATING: break; 
            }
            buttonIsBeingProcessed = false; 
        }
    }
    if (buttonIsBeingProcessed && (digitalRead(ENCODER_SW_PIN) == LOW)) {
        if (millis() - buttonDownStartTime > LONG_PRESS_DURATION_MS) {
            if (Serial) Serial.println(F("Button HELD - LONG press.")); 
            longBeepSequence();
            if (currentState == STATE_HEATING || currentState == STATE_SOAKING || currentState == STATE_READY) {
                currentState = STATE_STOPPED; digitalWrite(SSR_PIN, LOW); stopSoakTimer();
                if (Serial) Serial.println(F("System STOPPED by long press"));
            }
            buttonIsBeingProcessed = false; 
            while(digitalRead(ENCODER_SW_PIN) == LOW) { delay(10); } 
            g_encoderButtonSignal = false; 
        }
    }
}