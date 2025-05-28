#ifndef TIMER_LOGIC_H
#define TIMER_LOGIC_H

void initTimer(); // Не используется, но оставим
void startSoakTimer();
void stopSoakTimer();
bool updateSoakTimerAndCheckCompletion(); // Возвращает true, если таймер завершился

#endif // TIMER_LOGIC_H