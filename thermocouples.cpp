#include "thermocouples.h"
#include "globals.h" // Для доступа к объектам tc... и переменным temp...

// --- MAX6675 Objects (определения здесь, объявления в globals.h) ---
MAX6675 tcHeater(SCK_PIN, CS_HEATER_PIN, MISO_PIN);
MAX6675 tcPcb1(SCK_PIN, CS_PCB1_PIN, MISO_PIN);
MAX6675 tcPcb2(SCK_PIN, CS_PCB2_PIN, MISO_PIN);

// Глобальные переменные температур (определения здесь)
double tempHeater = 0.0, tempPcb1 = 0.0, tempPcb2 = 0.0;

// Глобальные переменные для скользящего среднего (определения здесь)
double pcb1TempHistory[MOVING_AVG_SIZE];
int pcb1TempHistoryIndex = 0;
double pcb1TempMovingAvg = -999.0; 
double previousPcb1TempMovingAvg = -999.0;
bool movingAvgInitialized = false;


void initThermocouples() {
    // Можно добавить сюда специфичную инициализацию, если потребуется
    // Например, начальное заполнение pcb1TempHistory
    for (int i = 0; i < MOVING_AVG_SIZE; i++) {
        pcb1TempHistory[i] = 20.0; // Начальное значение (комнатная температура)
    }
    pcb1TempMovingAvg = 20.0;
    previousPcb1TempMovingAvg = 20.0;
}

void readTemperatures() {
  tempHeater = tcHeater.readCelsius(); delay(5); 
  double rawPcb1 = tcPcb1.readCelsius(); delay(5); 
  tempPcb2 = tcPcb2.readCelsius();
  static double sum = 0.0; 

  if (!isnan(rawPcb1)) {
    tempPcb1 = rawPcb1; 
    
    if (!movingAvgInitialized) {
        sum = 0.0; 
        for (int i = 0; i < MOVING_AVG_SIZE; i++) {
          pcb1TempHistory[i] = tempPcb1;
          sum += tempPcb1; 
        }
        pcb1TempMovingAvg = tempPcb1; 
        previousPcb1TempMovingAvg = tempPcb1; 
        movingAvgInitialized = true;
        if (Serial) Serial.println(F("Moving average initialized."));
    } else {        
        sum = sum - pcb1TempHistory[pcb1TempHistoryIndex] + tempPcb1; 
        pcb1TempHistory[pcb1TempHistoryIndex] = tempPcb1; 
        pcb1TempHistoryIndex = (pcb1TempHistoryIndex + 1) % MOVING_AVG_SIZE;       
        
        previousPcb1TempMovingAvg = pcb1TempMovingAvg; 
        pcb1TempMovingAvg = sum / MOVING_AVG_SIZE;     
    }
  } else {
    if (Serial) Serial.println(F("Warning: NaN read from PCB1 sensor."));
  }
}