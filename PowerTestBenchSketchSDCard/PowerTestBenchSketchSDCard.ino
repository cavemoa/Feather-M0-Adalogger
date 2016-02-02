/*
  Power test bench sketch for looking at the power used by the
  Adafruit Feather M0 Adalogger.

  Sketch does a set of standard tests to establish power consumption
  which is measured using an EEVblog.

  Note: Will only work with SAMD21 based arduino's that have RTC built in.
  
  By: CaveMoa
  Date: 3/2/15
*/
#include<SPI.h>
#include <RTCZero.h>
#include <SdFat.h>
#define RED 13 // Red LED on Pin #13
#define GREEN 8 // Green LED on Pin #8

SdFat SD;
File logfile;

#define cardSelect 4  // Set the pin used for uSD

/* Create an rtc object */
RTCZero rtc;
int AlarmTime;
int n = 0;
void setup()
{
  rtc.begin();
  digitalWrite(8, LOW); 
}

void loop()
{

  ++n;
  switch (n) {
    case 1:
      delay (10000);
      break;
    case 2:
      blink(RED,1);
    case 3:
      blink(RED,2);
      SD.begin(cardSelect);
      break;
    case 4:
      blink(RED,3);
      logfile = SD.open("ANALOG00.CSV", FILE_WRITE);
      break;
    case 5:
      blink(RED,4); 
      logfile.close();
      break;
    case 6:
      blink(RED,5);
      digitalWrite(4, LOW);
      rtc.standbyMode();    // Sleep until next alarm match
      break;
    default: 
      break;
  }


  AlarmTime += 15; // Adds 10 seconds to alarm time
  AlarmTime = AlarmTime % 60; // checks for roll over 60 seconds and corrects
  rtc.setAlarmSeconds(AlarmTime); // Wakes at next alarm time, i.e. every 10 secs
  rtc.enableAlarm(rtc.MATCH_SS); // Match seconds only
  rtc.attachInterrupt(alarmMatch); // Attach function to interupt
  rtc.standbyMode();    // Sleep until next alarm match
  
}

// Function called by RTC Alarm match
void alarmMatch() 
{
  
}

// blink out an error code, Red on pin #13 or Green on pin #8
void blink(uint8_t LED, uint8_t flashes) {
  uint8_t i;
  for (i=0; i<flashes; i++) {
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(200);
  }
}

