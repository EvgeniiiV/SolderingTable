#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

void initLcd();
void updateLcd();
void printLcdLine(int row, const char* strContent);

#endif // LCD_DISPLAY_H