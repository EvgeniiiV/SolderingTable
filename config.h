#ifndef CONFIG_H
#define CONFIG_H

// --- Pin Definitions ---
const int SCK_PIN = 13;
const int MISO_PIN = 12;
const int CS_HEATER_PIN = 10;
const int CS_PCB1_PIN = 9;
const int CS_PCB2_PIN = 8;

const int ENCODER_CLK_PIN = 4; 
const int ENCODER_DT_PIN = 3;
const int ENCODER_SW_PIN = 2;

const int SSR_PIN = 7;
const int BUZZER_PIN = 5;

// --- LCD Configuration ---
const uint8_t LCD_I2C_ADDR = 0x27; 
const int LCD_COLS = 20;
const int LCD_ROWS = 4;

// --- PID Controller Constants ---
const double KP_DEFAULT = 15.0; 
const double KI_DEFAULT = 0.008;  
const double KD_DEFAULT = 20.0; 
const unsigned long PID_WINDOW_SIZE_MS = 2000; 

// --- System States ---
enum SystemState {
  STATE_OFF, STATE_SETUP_TEMP, STATE_SETUP_TIME, STATE_SETUP_TIMER_EN,
  STATE_READY, STATE_HEATING, STATE_SOAKING, STATE_DONE, STATE_STOPPED, STATE_ERROR
};

// --- User Settings Structure ---
struct Settings {
  double targetPcbTemp; 
  int soakMinutes; 
  int soakSeconds; 
  bool isTimerEnabled; 
  unsigned int checksum;    
};

// --- EEPROM ---
const int EEPROM_ADDR_SETTINGS = 0; 
const unsigned int EEPROM_CHECKSUM_VALID = 0xABCD; 

// --- Timing & Misc Constants ---
const unsigned long BUTTON_DEBOUNCE_TIME_MS = 50;
const unsigned long LONG_PRESS_DURATION_MS = 1500; 
const int BLINK_INTERVAL_MS = 300;
const int TEMP_READ_INTERVAL_MS = 500;
const int LCD_UPDATE_INTERVAL_MS = 300; 
const int MOVING_AVG_SIZE = 10;

#endif // CONFIG_H