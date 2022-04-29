#include "myLib.h"

#define DHT11PIN 13
#define B2 2
#define B3 3
#define B4 4
#define B5 5
#define B6 6
#define BUZZER 10
#define LED 7
#define LCD_ADDR 0x27
#define LCD_COL 20
#define LCD_ROW 4

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COL, LCD_ROW);
RTC_clock rtc_clock(XTAL);
dht11 DHT11;

// Clock Settings
int pHour, pMinute, pAmpm, pDay, pMonth, pYear;

// Alarm Settings
int aHour = 1, aMinute = 0, aAmpm = 1;
boolean alarm_flag = false;

// Timer Settings
int tHour = 0, tMinute = 0, tSecond = 0;
boolean timer_flag = false;

void setup() {
    // Initialize LCD Display
    lcd.init();
    lcd.backlight();
    lcd.clear();

    // Default Time: 12:00:00 AM January 1, 2000
    rtc_clock.init();
    // HH:MM:SS
    rtc_clock.set_time(0, 0, 0);
    // DD:MM:YYYY
    rtc_clock.set_date(1, 1, 2000);
    setSyncProvider(getArduinoDueTime);

    // Initialize IO Pins
    pinMode(B2, INPUT);
    pinMode(B3, INPUT);
    pinMode(B4, INPUT);
    pinMode(B5, INPUT);
    pinMode(B6, INPUT);
    pinMode(BUZZER, OUTPUT);

    pinMode(LED, OUTPUT);
    Scheduler.startLoop(TickFct_ButtonSwitch);
    Scheduler.startLoop(TickFct_Alarm);
    Scheduler.startLoop(TickFct_Timer);
}

void loop() {
    delay(1000);
}

void TickFct_ButtonSwitch() {
    static boolean clear_flag = false;
    static ButtonSwitch_states curDisplay = BS_ClockDisplay;
    switch (ButtonSwitch_state) {
        case BS_SMStart:
            ButtonSwitch_state = BS_ClockDisplay;
            break;
        case BS_ClockDisplay:
            if (digitalRead(B2) == LOW) {
                lcd.clear();
                ButtonSwitch_state = BS_Hour;
                curDisplay = BS_ClockDisplay;
            }
            else if (digitalRead(B3) == LOW) {
                lcd.clear();
                ButtonSwitch_state = BS_AlarmDisplay;
                curDisplay = BS_AlarmDisplay;
            }
            else if (digitalRead(B4) == LOW) {
                lcd.clear();
                ButtonSwitch_state = BS_TimerDisplay;
                curDisplay = BS_TimerDisplay;
            }
            break;
        case BS_AlarmDisplay:
            if (digitalRead(B3) == LOW) {
                lcd.clear();
                ButtonSwitch_state = BS_Hour;
            }
            else if (digitalRead(B4) == LOW) {
                lcd.clear();
                ButtonSwitch_state = BS_TimerDisplay;
                curDisplay = BS_TimerDisplay;
            }
            else if (digitalRead(B2) == LOW) {
                lcd.clear();
                ButtonSwitch_state = BS_ClockDisplay;
                curDisplay = BS_ClockDisplay;
            }
            break;
        case BS_TimerDisplay:
            if (digitalRead(B4) == LOW) {
                lcd.clear();
                ButtonSwitch_state = BS_Hour;
            }
            else if (digitalRead(B3) == LOW) {
                lcd.clear();
                ButtonSwitch_state = BS_AlarmDisplay;
                curDisplay = BS_AlarmDisplay;
            }
            else if (digitalRead(B2) == LOW) {
                lcd.clear();
                ButtonSwitch_state = BS_ClockDisplay;
                curDisplay = BS_ClockDisplay;
            }
            break;
        case BS_Hour:
            if (digitalRead(B2) == LOW) {
                lcd.clear(); 
                ButtonSwitch_state = BS_Minute;
            }
            break;
        case BS_Minute:
            if (digitalRead(B2) == LOW) {
                lcd.clear(); 
                if (curDisplay == BS_TimerDisplay) {
                    ButtonSwitch_state = BS_TimerDisplay;
                    if (tHour > 0 || tMinute > 0) timer_flag = true;
                    else if (tHour == 0 && tMinute == 0) timer_flag = false;
                }
                else ButtonSwitch_state = BS_AMPM;
            }
            break;
        case BS_AMPM:
            if (digitalRead(B2) == LOW) {
                lcd.clear(); 
                if (curDisplay == BS_AlarmDisplay) {
                    ButtonSwitch_state = BS_AlarmDisplay;
                    alarm_flag = true;
                }
                else ButtonSwitch_state = BS_Month;
            }
            break;
        case BS_Month:
            if (digitalRead(B2) == LOW) {
                lcd.clear(); 
                ButtonSwitch_state = BS_Day;
            }
            break;
        case BS_Day:
            if (digitalRead(B2) == LOW) {
                lcd.clear(); 
                ButtonSwitch_state = BS_Year;
            }
            break;
        case BS_Year:
            if (digitalRead(B2) == LOW) {
                lcd.clear(); 
                ButtonSwitch_state = BS_SetDate;
            }
            break;
        case BS_SetDate:
            ButtonSwitch_state = BS_ClockDisplay;
            break;
        default:
            ButtonSwitch_state = BS_SMStart;
            break;
    }
    switch (ButtonSwitch_state) {
        case BS_SMStart:
            break;
        case BS_ClockDisplay:
            // Initialize Clock Values
            pHour = hourFormat12();
            pMinute = minute();
            pAmpm = isAM();
            pDay = day();
            pMonth = month();
            pYear = year();
            printTime();
            printDate();
            printTemperatureAndHumidity();
            break;
        case BS_AlarmDisplay:
            printAlarm();
            break; 
        case BS_TimerDisplay:
            printTimer();
            break;
        case BS_Hour:
            if (curDisplay == BS_ClockDisplay) printHour(pHour, curDisplay);
            else if (curDisplay == BS_AlarmDisplay) printHour(aHour, curDisplay);
            else if (curDisplay == BS_TimerDisplay) printHour(tHour, curDisplay);
            break;
        case BS_Minute:
            if (curDisplay == BS_ClockDisplay) printMinute(pMinute);
            else if (curDisplay == BS_AlarmDisplay) printMinute(aMinute);
            else if (curDisplay == BS_TimerDisplay) printMinute(tMinute);
            break;
        case BS_AMPM:
            if (curDisplay == BS_ClockDisplay) printAMPM(pAmpm);
            else if (curDisplay == BS_AlarmDisplay) printAMPM(aAmpm);
            break;
        case BS_Month:
            printMonth();
            break;
        case BS_Day:
            printDay();
            break;
        case BS_Year:
            printYear();
            break;
        case BS_SetDate:
            if (pAmpm && pHour == 12) pHour = 0;
            else if (!pAmpm && pHour != 12) pHour += 12;
            rtc_clock.set_time(pHour, pMinute, 0);
            rtc_clock.set_date(pDay, pMonth, pYear);
            setSyncProvider(getArduinoDueTime);
            break;
        default:
            break;
    }
    delay(500);
}

void TickFct_Alarm() {
    switch (Alarm_state) {
        case A_SMStart:
            Alarm_state = A_Sleep;
            break;
        case A_Sleep:
            if (alarm_flag) {
                if (hourFormat12() == aHour && minute() == aMinute && isAM() == aAmpm) Alarm_state = A_Activate;
            }
            break;
        case A_Activate:
            if (digitalRead(B3) == LOW) {
                Alarm_state = A_Sleep;
                alarm_flag = false;
            }
            break;
        default:
            Alarm_state = A_SMStart;
            break;
    }
    switch (Alarm_state) {
        case A_SMStart:
            break; 
        case A_Sleep:
            if (!timer_flag) analogWrite(BUZZER, 0);
            break;
        case A_Activate:
            analogWrite(BUZZER, 127);
            break;
        default:
            break;
    }
    delay(1000);
}

void TickFct_Timer() {
    static int count = 0;
    switch (Timer_state) {
        case T_SMStart:
            Timer_state = T_Sleep;
            break;
        case T_Sleep:
            if (timer_flag) {
                if (tHour > 0 && tMinute == 0) {
                    tHour--;
                    tMinute = 59;
                }
                if (tMinute > 0 && count == 0) {
                    tMinute--;
                    count = 59;
                }
                else if (tHour == 0 && tMinute == 0 && count == 0) {
                    Timer_state = T_Activate;
                }
                else {
                    count--;
                }
            }
            break;
        case T_Activate:
            if (digitalRead(B4) == LOW) {
                Timer_state = T_Sleep;
                timer_flag = false;
            }
            break;
        default:
            Timer_state = T_SMStart;
            break;
    }
    switch (Timer_state) {
        case T_SMStart:
            break; 
        case T_Sleep:  
            if (!alarm_flag) analogWrite(BUZZER, 0);
            break; 
        case T_Activate:
            analogWrite(BUZZER, 127);
            break;
        default:
            break;
    }
    tSecond = count;
    delay(1000);
}

void printTime() {
    char buffer[40];
    char _day[16];
    int _hour = hourFormat12();
    int _minute = minute();
    int _second = second();
    char _ampm[3];
    lcd.setCursor(2,0);
    strcpy(_day, dayStr(weekday()));
    if (isAM()) strcpy(_ampm, "AM");
    else strcpy(_ampm, "PM");
    sprintf(buffer, "%.3s %02d:%02d:%02d %s", 
            _day, _hour, _minute, _second, _ampm);
    lcd.print(buffer);
}

void printDate() {
    static int month_num = 1;
    // Checks if the month has changed
    if (month_num != month()) {
        lcd.setCursor(0,1);
        // "Deletes" line by replacing text with spaces
        lcd.print("                    ");
        month_num = month();
    }
    char buffer[40];
    int _day = day();
    char _month[16];
    int _year = year();
    int _month_len;
    uint8_t i = 5;
    strcpy(_month, monthStr(month()));
    _month_len = strlen(_month) / 2;
    i -= _month_len;
    if (i%2 == 1) i++;
    lcd.setCursor(i,1);
    sprintf(buffer, "%02d %s %d", _day, _month, _year);
    lcd.print(buffer);
}

void printTemperatureAndHumidity() {
    int chk = DHT11.read(DHT11PIN);
    lcd.setCursor(0, 2);
    lcd.print("Temperature: ");
    lcd.setCursor(13, 2);
    lcd.print(((float)DHT11.temperature * 9.0/5.0) + 32);
    lcd.print((char)223);
    lcd.print("F");
    lcd.setCursor(0, 3);
    lcd.print("Humidity: ");
    lcd.setCursor(13, 3);
    lcd.print(((float)DHT11.humidity));
    lcd.print("%");
}

void printAlarm() {
    char buffer[40];
    char ampm[3];
    char *line0 = "Alarm Settings";
    lcd.setCursor(3,0);
    lcd.print(line0);
    if (aAmpm) strcpy(ampm, "AM");
    else strcpy(ampm, "PM");
    sprintf(buffer, "%02d:%02d %s", aHour, aMinute, ampm);
    lcd.setCursor(6,2);
    lcd.print(buffer);
}

void printTimer() {
    char buffer[40];
    char *line0 = "Timer Settings";
    lcd.setCursor(3,0);
    lcd.print(line0);
    sprintf(buffer, "%02d:%02d:%02d", tHour, tMinute, tSecond);
    lcd.setCursor(6,2);
    lcd.print(buffer);
}

void printHour(int &hr, ButtonSwitch_states state) {
    char buffer[40];
    char *line1 = "Set Hour:";
    lcd.setCursor(5, 1);
    lcd.print(line1);
    if (digitalRead(B5) == LOW) hr--;
    else if (digitalRead(B6) == LOW) hr++;
    if (state == BS_TimerDisplay) {
        if (hr < 0) hr = 59;
        else if (hr >= 60) hr = 0;
    }
    else if (hr == 0) hr = 12;
    else if (hr == 13) hr = 1;
    lcd.setCursor(9, 2);
    sprintf(buffer, "%02d", hr);
    lcd.print(buffer);
}

void printMinute(int &min) {
    char buffer[40];
    char *line1 = "Set Minute:";
    lcd.setCursor(4, 1);
    lcd.print(line1);
    if (min == 0) min = 60;
    if (digitalRead(B5) == LOW) min--;
    else if (digitalRead(B6) == LOW) min++;
    if (min == 60) min = 0;
    if (min == 61) min = 1;
    lcd.setCursor(9, 2);
    sprintf(buffer, "%02d", min);
    lcd.print(buffer);
}

void printAMPM(int &ap) {
    char buffer[40];
    char *line1 = "Set AM/PM:";
    lcd.setCursor(4, 1);
    lcd.print(line1);
    if (digitalRead(B5) == LOW) ap = !ap;
    if (ap) sprintf(buffer, "%s", "AM");
    else sprintf(buffer, "%s", "PM");   
    lcd.setCursor(9, 2);
    lcd.print(buffer);
}

void printDay() {
    char buffer[40];
    char *line1 = "Set Day:";
    lcd.setCursor(6, 1);
    lcd.print(line1);
    if (digitalRead(B5) == LOW) pDay--;
    else if (digitalRead(B6) == LOW) pDay++;
    if (pMonth == 2) {
        if (LEAP_YEAR(pMonth)) {
            if (pDay == 30) pDay = 1;
            else if (pDay == 0) pDay = 29;
        }
        else {
            if (pDay == 29) pDay = 1;
            else if (pDay == 0) pDay = 28;
        }
    }
    else if (pMonth == 1 || pMonth == 3 || pMonth == 5 || pMonth == 7 || pMonth == 8 || pMonth == 10 || pMonth == 12) {
        if (pDay == 0) pDay = 31;
        else if (pDay == 32) pDay = 1;
    }
    else {
        if (pDay == 0) pDay = 30;
        else if (pDay == 31) pDay = 1;
    }
    lcd.setCursor(9, 2);
    sprintf(buffer, "%02d", pDay);
    lcd.print(buffer);
}

void printMonth() {
    char buffer[40];
    char *line1 = "Set Month:";
    lcd.setCursor(5, 1);
    lcd.print(line1);
    if (digitalRead(B5) == LOW) pMonth--;
    else if (digitalRead(B6) == LOW) pMonth++;
    if (pMonth == 0) pMonth = 12;
    if (pMonth == 13) pMonth = 1;
    lcd.setCursor(9, 2);
    sprintf(buffer, "%02d", pMonth);
    lcd.print(buffer);
}

void printYear() {
    char buffer[40];
    char *line1 = "Set Year:";
    lcd.setCursor(5, 1);
    lcd.print(line1);
    if (digitalRead(B5) == LOW) pYear--;
    else if (digitalRead(B6) == LOW) pYear++;
    if (pYear == 0) pYear = 3999;
    if (pYear == 4000) pYear = 0000;
    lcd.setCursor(7, 2);
    sprintf(buffer, "%d", pYear);
    lcd.print(buffer);
}

time_t getArduinoDueTime() {
    return rtc_clock.unixtime();
}