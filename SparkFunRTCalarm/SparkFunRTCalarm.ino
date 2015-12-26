#include <RTCZero.h> // Include RTC library - make sure it's installed!

RTCZero rtc; // Create an RTC object
byte lastSecond = 60;
byte alarmMinute = 1; // Minutes after clock starts to sound alarm
bool alarmTriggered = false;

void setup() 
{
  SerialUSB.begin(9600);
  while (!SerialUSB) ; // Wait for Serial monitor to open

  byte hour = prompt("Hour", 0, 23); // Get the hour
  byte minute = prompt("Minute", 0, 59); // Get the minute
  byte second = prompt("Second", 0, 59); // Get the second
  byte day = prompt("Day", 0, 31); // Get the day
  byte month = prompt("Month", 0, 12); // Get the month
  byte year = prompt("Year (YY)", 0, 99); // Get the year

  SerialUSB.println("Press any key to begin");
  while (!SerialUSB.available()) ; // Wait for keypress to start clock

  rtc.begin(); // To use the RTC, first begin it
  rtc.setTime(hour, minute, second); // Then set the time
  rtc.setDate(day, month, year); // And the date
  SerialUSB.println("RTC Started!");

  SerialUSB.println("Setting alarm for " + String(alarmMinute) + " minute(s).");
  SerialUSB.println();
  byte alarmHour = hour + ((alarmMinute + minute) / 60);
  alarmMinute = (alarmMinute + minute) % 60;

  // To set an alarm, use the setAlarmTime function.
  rtc.setAlarmTime(alarmHour, alarmMinute, second);
  // After the time is set, enable the alarm, configuring
  // which time values you want to trigger the alarm
  rtc.enableAlarm(rtc.MATCH_HHMMSS); // Alarm when hours, minute, & second match
  // When the alarm triggers, alarmMatch will be called:
  rtc.attachInterrupt(alarmMatch);
}

void loop() 
{
  // If the second value is different:
  if (lastSecond != rtc.getSeconds())
  {
    printTime(); // Print the time
    lastSecond = rtc.getSeconds(); // Update lastSecond

    if (alarmTriggered) // If the alarm has been triggered
    {
      SerialUSB.println("Alarm!"); // Print alarm!
    }
  }
}

void printTime()
{
  // Use rtc.getDay(), .getMonth(), and .getYear()
  // To get the numerical values for the date.
  SerialUSB.print(rtc.getDay()); // Print day
  SerialUSB.print("/");
  SerialUSB.print(rtc.getMonth()); // Print Month
  SerialUSB.print("/");
  SerialUSB.print(rtc.getYear()); // Print year
  SerialUSB.print("\t");

  // Use rtc.getHours, .getMinutes, and .getSeconds()
  // to get time values:
  SerialUSB.print(rtc.getHours()); // Print hours
  SerialUSB.print(":");
  if (rtc.getMinutes() < 10) 
    SerialUSB.print('0'); // Pad the 0
  SerialUSB.print(rtc.getMinutes()); // Print minutes
  SerialUSB.print(":");
  if (rtc.getSeconds() < 10) 
    SerialUSB.print('0'); // Pad the 0
  SerialUSB.print(rtc.getSeconds()); // Print seconds
  SerialUSB.println();
}

void alarmMatch()
{
  // This function is called when the alarm values match
  // and the alarm is triggered.
  alarmTriggered = true; // Set the global triggered flag
}

// Helper function to prompt for a value, and return
// it if it's within a valid range.
byte prompt(String ask, int mini, int maxi)
{
  SerialUSB.print(ask + "? ");
  while (!SerialUSB.available()) ; // Wait for numbers to come in
  byte rsp = SerialUSB.parseInt();
  if ((rsp >= mini) && (rsp <= maxi))
  {
    SerialUSB.println(rsp);
    return rsp;
  }
  else
  {
    SerialUSB.println("Invalid.");
    return mini;
  }
}

