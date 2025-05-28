#include "lcd_display.h"
#include "globals.h" // currentSettings, temperature, state ...
#include <Arduino.h> // dtostrf, snprintf, strlen, strcat, strcpy, memmove

// LCD Object 
LiquidCrystal_I2C lcd(LCD_I2C_ADDR, LCD_COLS, LCD_ROWS);

// Global variables for LCD 
unsigned long lastBlinkTime = 0;
bool blinkState = false;

void initLcd() {
    lcd.init();
    lcd.backlight();
    lcd.clear();
}

void printLcdLine(int row, const char* strContent) {
    lcd.setCursor(0, row);
    int len = strlen(strContent);
    lcd.print(strContent);
    for (int i = len; i < LCD_COLS; i++) {
        lcd.print(" ");
    }
}

void updateLcd() {
  char tempStr[8];    
  char paramBuffer[12]; 
  char lineBuffer[LCD_COLS + 1]; 

  // String 1
  dtostrf(currentSettings.targetPcbTemp, 4, 0, tempStr); 
  char* pSet = tempStr; while (*pSet == ' ' && *(pSet+1) != '\0') pSet++; 
  char pcb1DisplayStr[8]; 
  if (isnan(tempPcb1)) { strcpy(pcb1DisplayStr, "ERR"); } 
  else {
    dtostrf(tempPcb1, 4, 0, pcb1DisplayStr);
    char* pPcb1 = pcb1DisplayStr; while (*pPcb1 == ' ' && *(pPcb1+1) != '\0') pPcb1++;
    if (pPcb1 != pcb1DisplayStr) { memmove(pcb1DisplayStr, pPcb1, strlen(pPcb1) + 1); }
  }
  char statusIcon = ' ';
  if (currentState == STATE_HEATING && movingAvgInitialized && !isnan(pcb1TempMovingAvg)) {
    if (pcb1TempMovingAvg > previousPcb1TempMovingAvg + 0.05) statusIcon = '^'; 
    else if (pcb1TempMovingAvg < previousPcb1TempMovingAvg - 0.05) statusIcon = 'v';
    else statusIcon = '=';
  } else if (currentState == STATE_SOAKING) { statusIcon = '='; }
  snprintf(lineBuffer, sizeof(lineBuffer), "Set:%sC  PCB1:%sC %c", pSet, pcb1DisplayStr, statusIcon);
  printLcdLine(0, lineBuffer);

  // String 2
  char heaterDisplayStr[8];
  if(isnan(tempHeater)) { strcpy(heaterDisplayStr, "ERR"); } 
  else {
      dtostrf(tempHeater, 4, 0, heaterDisplayStr);
      char* pHeat = heaterDisplayStr; while (*pHeat == ' ' && *(pHeat+1) != '\0') pHeat++;
      if (pHeat != heaterDisplayStr) { memmove(heaterDisplayStr, pHeat, strlen(pHeat) + 1); }
  }
  char pcb2DisplayStr[8];
  if(isnan(tempPcb2)) { strcpy(pcb2DisplayStr, "ERR"); } 
  else {
      dtostrf(tempPcb2, 4, 0, pcb2DisplayStr);
      char* pPcb2 = pcb2DisplayStr; while (*pPcb2 == ' ' && *(pPcb2+1) != '\0') pPcb2++;
      if (pPcb2 != pcb2DisplayStr) { memmove(pcb2DisplayStr, pPcb2, strlen(pPcb2) + 1); }
  }
  snprintf(lineBuffer, sizeof(lineBuffer), "Heat:%sC  PCB2:%sC", heaterDisplayStr, pcb2DisplayStr);
  printLcdLine(1, lineBuffer);

  // String 3
  bool blinkParam = false;
  lineBuffer[0] = '\0'; 
  strcat(lineBuffer, "Mode: "); 
  switch (currentState) {
    case STATE_OFF: strcat(lineBuffer, "OFF"); break;
    case STATE_SETUP_TEMP:
      strcat(lineBuffer, "Set Temp ");
      dtostrf(currentSettings.targetPcbTemp, 4, 0, tempStr);
      pSet = tempStr; while (*pSet == ' ' && *(pSet+1) != '\0') pSet++;
      snprintf(paramBuffer, sizeof(paramBuffer), "[%s]C", pSet);
      blinkParam = true; break;
    case STATE_SETUP_TIME:
      strcat(lineBuffer, "Set Time ");
      snprintf(paramBuffer, sizeof(paramBuffer), "%02d:%02d", currentSettings.soakMinutes, currentSettings.soakSeconds);
      blinkParam = true; break;
    case STATE_SETUP_TIMER_EN:
      strcat(lineBuffer, "Timer EN?");
      snprintf(paramBuffer, sizeof(paramBuffer), "[%s]", currentSettings.isTimerEnabled ? "YES" : "NO ");
      blinkParam = true; break;
    case STATE_READY: strcat(lineBuffer, "READY"); break;
    case STATE_HEATING: strcat(lineBuffer, "HEATING"); break;
    case STATE_SOAKING:
      strcat(lineBuffer, currentSettings.isTimerEnabled ? "SOAKING" : "SOAK (Manual)"); break;
    case STATE_DONE: strcat(lineBuffer, "DONE!"); break;
    case STATE_STOPPED: strcat(lineBuffer, "STOPPED"); break;
    case STATE_ERROR: strcat(lineBuffer, "ERROR!"); break;
    default: strcat(lineBuffer, "???"); break;
  }
  if (blinkParam) {
    if (millis() - lastBlinkTime > BLINK_INTERVAL_MS) {
      blinkState = !blinkState; lastBlinkTime = millis();
    }
    if (blinkState) { 
        if (strlen(lineBuffer) + strlen(paramBuffer) < sizeof(lineBuffer)) strcat(lineBuffer, paramBuffer); 
        else strncat(lineBuffer, paramBuffer, sizeof(lineBuffer) - strlen(lineBuffer) - 1);
    } else { 
      for (size_t i = 0; i < strlen(paramBuffer); ++i) {
        if (strlen(lineBuffer) < sizeof(lineBuffer) - 1) strcat(lineBuffer, " "); else break;
      }
    }
  }
  printLcdLine(2, lineBuffer);

  // String 4
  lineBuffer[0] = '\0'; 
  if (currentState == STATE_SOAKING && currentSettings.isTimerEnabled && soakTimerRunning) {
    unsigned int m = remainingSoakTimeMillis/60000, s = (remainingSoakTimeMillis%60000)/1000;
    snprintf(lineBuffer, sizeof(lineBuffer), "Time: %02u:%02u BTN=RST", m, s);
  } else if (currentState == STATE_SOAKING && !currentSettings.isTimerEnabled) {
    strcpy(lineBuffer, "Time: --:-- HLD=STOP"); 
  } else if (currentState == STATE_HEATING) {
    strcpy(lineBuffer, "           HLD=STOP"); 
  } else if (currentState == STATE_READY) {
    strcpy(lineBuffer, "Press BTN to START");
  } else if (currentState == STATE_OFF || currentState == STATE_DONE || currentState == STATE_STOPPED) {
    strcpy(lineBuffer, "Press BTN for SETUP"); 
  } else if (currentState == STATE_ERROR) {
    strcpy(lineBuffer, "Check Sensors!");
  } else if (currentState == STATE_SETUP_TEMP || currentState == STATE_SETUP_TIME || currentState == STATE_SETUP_TIMER_EN) {
    strcpy(lineBuffer, "Adjust & Push Button");
  }
  printLcdLine(3, lineBuffer);
}