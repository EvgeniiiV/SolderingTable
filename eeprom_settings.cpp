#include "eeprom_settings.h"
#include "globals.h" // Для доступа к currentSettings
#include <EEPROM.h>
#include <Arduino.h> // Для Serial

// Settings object 
Settings currentSettings;

void loadSettingsFromEEPROM() {
  Settings tempSettings; EEPROM.get(EEPROM_ADDR_SETTINGS, tempSettings);
  if (tempSettings.checksum == EEPROM_CHECKSUM_VALID) {
    currentSettings = tempSettings; 
    if (Serial) Serial.println(F("Settings loaded from EEPROM."));
  } else {
    if (Serial) Serial.println(F("Invalid EEPROM/first run. Defaults loaded."));
    currentSettings.targetPcbTemp = 120.0; currentSettings.soakMinutes = 5;
    currentSettings.soakSeconds = 0; currentSettings.isTimerEnabled = true;
    // Не сохраняем по умолчанию сразу, даем пользователю настроить
  }
}

void saveSettingsToEEPROM() {
  currentSettings.checksum = EEPROM_CHECKSUM_VALID;
  EEPROM.put(EEPROM_ADDR_SETTINGS, currentSettings);
  if (Serial) Serial.println(F("Settings saved to EEPROM."));
  #if (defined(ESP32) || defined(ESP8266))
    EEPROM.commit();
  #endif
}