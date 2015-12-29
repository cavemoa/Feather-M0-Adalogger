/*
  Simple RTC Alarm for Adafruit Feather M0 modified from 
  RTCzero library example by Arturo Guadalupi
  By: CaveMoa
  Date: 19/12/15
*/
#define Serial SerialUSB

#include <RTCZero.h>


/* Create an rtc object */
RTCZero rtc;

void setup()
{
  rtc.begin();
  Serial.begin(9600);
  while (! Serial); // Wait until Serial is ready
  Serial.println("Ready!");
  Serial.end();
}

void loop()
{
  
  rtc.setAlarmSeconds(30); // Wakes on the 30th second of the minute NOT every 30 secs!
  // alarm.time.second += 5;
  // alarm.time.second = alarm.time.second % 60;

  rtc.enableAlarm(rtc.MATCH_SS); // Match seconds only
  rtc.attachInterrupt(alarmMatch);
  
  Serial.end();
  USBDevice.detach(); // Safely detach the USB prior to sleeping
  //delay(10000);
  
  rtc.standbyMode();    // Sleep until next alarm match

  // Simple indication of being awake
  digitalWrite(13, HIGH);   // turn the LED on 
  delay(100);              
  digitalWrite(13, LOW);    // turn the LED off
  delay(100);
  digitalWrite(13, HIGH);   // turn the LED on 
  delay(100);              
  digitalWrite(13, LOW);    // turn the LED off
 
  USBDevice.attach();   // Re-attach the USB, audible sound on windows machines

  delay(5000);
  
  digitalWrite(13, HIGH);   // turn the LED on 
  delay(250);              
  digitalWrite(13, LOW);    // turn the LED off

  Serial.begin(9600);
  while (! Serial); // Wait until Serial is ready
  Serial.println("Awake");
  Serial.end();
  
}

void alarmMatch() // Do something when interrupt called
{
  
}
