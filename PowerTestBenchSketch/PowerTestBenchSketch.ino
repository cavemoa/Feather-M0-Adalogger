/*
  Power test bench sketch for looking at the power used by the
  Adafruit Feather M0 Adalogger.

  Sketch does a set of standard tests to establish power consumption
  which is measured using an EEVblog.

  Note: Will only work with SAMD21 based arduino's that have RTC built in.
  
  By: CaveMoa
  Date: 3/2/15
*/

#include <RTCZero.h>
#define RED 13 // Red LED on Pin #13
#define GREEN 8 // Green LED on Pin #8

/* Create an rtc object */
RTCZero rtc;
int AlarmTime;
int n = 0;
void setup()
{
  rtc.begin();
  
}

void loop()
{

  ++n;
  switch (n) {
    case 1:
      blink(RED,1);
      delay (5000);
      break;
    case 2:
      blink(RED,2); 
      digitalWrite(13, LOW);
      break;
    case 3:
      blink(RED,3); 
      digitalWrite(8, LOW);
      break;

    case 4:
      blink(RED,4); 
      digitalWrite(8, LOW);
      digitalWrite(13, LOW);
      break;
      
    default: 
      rtc.standbyMode();
    break;
  }


  AlarmTime += 10; // Adds 10 seconds to alarm time
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


