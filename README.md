# ClockDisplay
CS 122A Project 1

# Description
The user can use this program to read the time, date, temperature, and humidity. Additionally, the clock has alarm and timer functionalities (accessed by pressing the appropriate buttons).

# Hardware Requirements
1. Arduino Due
2. 5 tactile buttons
3. 1 buzzer
4. DHT11 sensor
5. 20x4 LCD I2C

# Libraries Required
> Install these libraries to the Arduino library directory
1. LCD I2C - https://github.com/MarkusLange/Arduino-Due-RTC-Library
2. DHT11 - https://github.com/adidax/dht11
3. Time - https://github.com/PaulStoffregen/Time
4. Scheduler - included with Arduino IDE

# Important Notes
B2 - Clock Display / Confirm Settings (default)  
B3 - Alarm Display  
B4 - Timer Display  
B5 - Decrease Value (when editing time/date)  
B6 - Increase Value (when editing time/date)  

> To edit the displays, press the associated display button two times.
> To progress through the settings, press B2.
