/*
  Sketch for measuring pwer consumption on the Feather M0 Adalogger
  from Adafruit.

  Sketch does a set of standard tests to establish pwer consumption
  which is measured using an EEVblog
  
  By: CaveMoa
  Date: 30/12/15
*/

#include <RTCZero.h>

/* Create an rtc object */
RTCZero rtc;
int AlarmTime;

void setup()
{
  rtc.begin();
  // Base Line for LED power consumption
  digitalWrite(13, LOW);   // turn the LED on 
  digitalWrite(8, LOW);    // turn the LED off
  //digitalWrite(9, LOW);    // uSD detect pin
  
}

void loop()
{

  
  
  /*
  AlarmTime += 30; // Adds 10 seconds to alarm time
  AlarmTime = AlarmTime % 60; // checks for roll over 60 seconds and corrects
  rtc.setAlarmSeconds(AlarmTime); // Wakes at next alarm time, i.e. every 10 secs
  
  rtc.enableAlarm(rtc.MATCH_SS); // Match seconds only
  rtc.attachInterrupt(alarmMatch); // Attach function to interupt
  */
  
  rtc.standbyMode();    // Sleep until next alarm match
  
}

void alarmMatch() // Do something when interrupt called
{
  
}
