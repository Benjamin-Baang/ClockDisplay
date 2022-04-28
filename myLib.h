#ifndef __myLib__H
#define __myLib__H

#include <LiquidCrystal_I2C.h>
#include "TimeLib.h"
#include "rtc_clock.h"
#include <Scheduler.h>
#include <dht11.h>

#define LEAP_YEAR(Y)     ( ((1970+(Y))>0) && !((1970+(Y))%4) && ( ((1970+(Y))%100) || !((1970+(Y))%400) ) )

enum ButtonSwitch_states {BS_SMStart, BS_ClockDisplay, BS_AlarmDisplay, BS_TimerDisplay, 
                            BS_Hour, BS_Minute, BS_AMPM, BS_Month, BS_Day, BS_Year, BS_SetDate} ButtonSwitch_state;
void TickFct_ButtonSwitch();

enum Alarm_states {A_SMStart, A_Sleep, A_Activate} Alarm_state;
void TickFct_Alarm();

enum Timer_states {T_SMStart, T_Sleep, T_Activate} Timer_state;
void TickFct_Timer();

void printTime();
void printDate();
void printTemperatureAndHumidity();
void printAlarm();
void printTimer();
void printHour(int &, ButtonSwitch_states);
void printMinute(int &);
void printAmPm(int &);
void printDay();
void printMonth();
void printYear();

time_t getArduinoDueTime();

#endif // __myLib__H