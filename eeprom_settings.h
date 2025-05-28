#ifndef EEPROM_SETTINGS_H
#define EEPROM_SETTINGS_H

#include <Arduino.h>

#include "config.h" // Для Settings



void loadSettingsFromEEPROM();
void saveSettingsToEEPROM();

#endif // EEPROM_SETTINGS_H