/*
  Simple RTC Alarm for Adafruit Feather M0 modified
  with repeated serial output each time the board wakes
  By: CaveMoa
  Date: 30/12/15
*/
#define Serial SerialUSB
#include <RTCZero.h>


/* Create an rtc object */
RTCZero rtc;
int AlarmTime;

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
  
  AlarmTime = rtc.getSeconds()+10; // Adds 10 seconds to alarm time
  AlarmTime = AlarmTime % 60; // checks for roll over 60 seconds and corrects
  Serial.print("Next Alarm Time:");
  Serial.println(AlarmTime);
  
  rtc.setAlarmSeconds(AlarmTime); // Wakes at next alarm time
  rtc.enableAlarm(rtc.MATCH_SS); // Match seconds only
  rtc.attachInterrupt(alarmMatch);
  
  Serial.end();
  USBDevice.detach(); // Safely detach the USB prior to sleeping
  
  rtc.standbyMode();    // Sleep until next alarm match

  USBDevice.attach();   // Re-attach the USB, audible sound on windows machines

  // Simple indication of being awake
  digitalWrite(13, HIGH);   // turn the LED on 
  delay(100);              
  digitalWrite(13, LOW);    // turn the LED off
  delay(100);
  digitalWrite(13, HIGH);   // turn the LED on 
  delay(100);              
  digitalWrite(13, LOW);    // turn the LED off

  delay(1000);  // Delay added to make serial more reliable
  
  Serial.begin(9600);
  while (! Serial); // Wait until Serial is ready
  Serial.println("Awake");  
}

void alarmMatch() // Do something when interrupt called
{
  
}
