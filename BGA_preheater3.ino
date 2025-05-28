#include "config.h"
#include "globals.h"
#include "thermocouples.h"
#include "lcd_display.h"
#include "encoder_control.h"
#include "pid_control.h"
#include "timer_logic.h"
#include "eeprom_settings.h"


// System State 
SystemState currentState = STATE_OFF;
SystemState previousStateDisplay = STATE_ERROR;

// General Timing 
unsigned long lastTempReadTime = 0;
unsigned long lastLcdUpdateTime = 0;


void setup() {
  Serial.begin(115200);
  Serial.println(F("BGA Preheater Starting..."));

  initLcd(); 
  lcd.print(F("Preheater v1.0"));
  lcd.setCursor(0, 1); lcd.print(F("Initializing..."));

  loadSettingsFromEEPROM(); // eeprom_settings.cpp
  initThermocouples();      // thermocouples.cpp
  initEncoderAndButton();   // encoder_control.cpp
  initPid();                // pid_control.cpp  
  
  pinMode(SSR_PIN, OUTPUT);             
  digitalWrite(SSR_PIN, LOW); // Turn off SSR at the start


  readTemperatures(); 
  if (isnan(tempHeater) || isnan(tempPcb1) || isnan(tempPcb2)) {
    currentState = STATE_ERROR; Serial.println(F("Thermocouple Error on Startup!"));
  } else {
    currentState = STATE_OFF; Serial.println(F("Thermocouples OK."));
  }
  
  // Установка начальной позиции энкодера ПОСЛЕ загрузки настроек
  if (currentState != STATE_ERROR) { // Только если нет ошибки датчиков
      knob.write(currentSettings.targetPcbTemp * 4);
      encoderOldPosition = currentSettings.targetPcbTemp;
  }
  
  updateLcd(); 
}

void loop() {
  if (millis() - lastTempReadTime >= TEMP_READ_INTERVAL_MS) {
    readTemperatures(); lastTempReadTime = millis();
  }
  handleEncoderRotation();
  handleButton();

  switch (currentState) {
    case STATE_OFF: 
    case STATE_SETUP_TEMP: 
    case STATE_SETUP_TIME: 
    case STATE_SETUP_TIMER_EN: 
    case STATE_READY:
      digitalWrite(SSR_PIN, LOW); stopSoakTimer(); break;
    case STATE_HEATING:
      runPidAndControlHeater();
      if (tempPcb1 >= currentSettings.targetPcbTemp - 0.5 && !isnan(tempPcb1)) { 
        currentState = STATE_SOAKING;
        shortBeep(); 
        if (currentSettings.isTimerEnabled) { startSoakTimer(); }
        windowStartTime = millis(); // Сброс окна PID для Soak
      }
      break;
    case STATE_SOAKING:
      runPidAndControlHeater();
      if (currentSettings.isTimerEnabled) {
        if (updateSoakTimerAndCheckCompletion()) { // Timer stopped
          currentState = STATE_DONE;
          longBeepSequence(); 
          stopSoakTimer(); digitalWrite(SSR_PIN, LOW);
        }
      }
      break;
    case STATE_DONE: 
    case STATE_STOPPED: 
    case STATE_ERROR:
      digitalWrite(SSR_PIN, LOW); stopSoakTimer(); break;
  }

  if (millis() - lastLcdUpdateTime >= LCD_UPDATE_INTERVAL_MS || currentState != previousStateDisplay) {
    updateLcd(); lastLcdUpdateTime = millis(); previousStateDisplay = currentState;
  }
}