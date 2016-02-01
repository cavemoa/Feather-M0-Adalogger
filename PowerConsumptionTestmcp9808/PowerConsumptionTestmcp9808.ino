/*
  Simple RTC Alarm for Adafruit Feather M0 modified from 
  RTCzero library example at https://github.com/arduino-libraries/RTCZero
  By: CaveMoa
  Date: 30/12/15
*/

#include <RTCZero.h>
#include <Wire.h>
#include "Adafruit_MCP9808.h"


/* Create an rtc object */
RTCZero rtc;
int AlarmTime;

Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

void setup()
{
  rtc.begin();
  tempsensor.begin();
  tempsensor.shutdown_wake(1);   // Don't remove this line! required before reading temp
}

void loop()
{

  // Simple indication of being awake
  digitalWrite(8, HIGH);   // turn the LED on 
  delay(100);              
  digitalWrite(8, LOW);    // turn the LED off
  delay(100);
  digitalWrite(8, HIGH);   // turn the LED on 
  delay(100);
  digitalWrite(8, LOW);    // turn the LED off
  digitalWrite(13, LOW); 

  AlarmTime += 10; // Adds 5 seconds to alarm time
  AlarmTime = AlarmTime % 60; // checks for roll over 60 seconds and corrects
  rtc.setAlarmSeconds(AlarmTime); // Wakes at next alarm time, i.e. every 5 secs
  
  rtc.enableAlarm(rtc.MATCH_SS); // Match seconds only
  rtc.attachInterrupt(alarmMatch); // Attach function to interupt
  rtc.standbyMode();    // Sleep until next alarm match
  
}

void alarmMatch() // Do something when interrupt called
{
  
}
