/*
 Low Power logger for Feather M0 Adalogger

 Uses internal RTC and interrupts to put M0 into deep sleep.
 Output logged to uSD at intervals set using RTC

 Created by:  Jonathan Davies
 Date:        16/01/16
 Version:     0.4
*/

////////////////////////////////////////////////////////////
//#define ECHO_TO_SERIAL // Allows serial output if uncommented
////////////////////////////////////////////////////////////

#include <RTCZero.h>
#include <SPI.h>
// #include <SD.h>
#include <SdFat.h>
SdFat SD;             //Quick way to make SdFat work with standard SD.h sketches

#define cardSelect 4  // Set the pin used for uSD
#define RED 13 // Red LED on Pin #13
#define GREEN 8 // Green LED on Pin #8
#define VBATPIN A7    // Battery Voltage on Pin A7

#ifdef ARDUINO_SAMD_ZERO
   #define Serial SerialUSB   // re-defines USB serial from M0 chip so it appears as regular serial
#endif

extern "C" char *sbrk(int i); //  Used by FreeRAm Function

//////////////// Key Settings ///////////////////


#define SampleIntMin 00 // RTC - Sample interval in minutes
#define SampleIntSec 10 // RTC - Sample interval in seconds

#define SamplesPerCycle 60  // Number of samples to buffer before uSD card flush is called. 

// 65536 (2^16) is the maximum number of spreadsheet rows supported by Excel 97, Excel 2000, Excel 2002 and Excel 2003 
// Excel 2007, 2010 and 2013 support 1,048,576 rows (2^20)). Text files that are larger than 65536 rows 
// cannot be imported to these versions of Excel.
#define SamplesPerFile 1440 // 1 per minute = 1440 per day = 10080 per week and ¬380Kb file (assumes 38bytes per sample)

/* Change these values to set the current initial time */
const byte hours = 18;
const byte minutes = 29;
const byte seconds = 30;
/* Change these values to set the current initial date */
const byte day = 19;
const byte month = 01;
const byte year = 16;

/////////////// Global Objects ////////////////////
RTCZero rtc;          // Create RTC object
File logfile;         // Create file object
char filename[15];    // Array for file name data logged to named in setup
  
float measuredvbat;   // Variable for battery voltage
int NextAlarmSec;     // Variable to hold next alarm time seconds value
int NextAlarmMin;     // Variable to hold next alarm time minute value

unsigned int CurrentCycleCount;  // Num of smaples in current cycle, before uSD flush call
unsigned int CurrentFileCount;   // Num of samples in current file

//////////////    Setup   ///////////////////
void setup() {

  //Set board LED pins as output
  pinMode(13, OUTPUT);
  pinMode(8, OUTPUT);

  //  Setup RTC and set time
  rtc.begin();    // Start the RTC in 24hr mode
  rtc.setTime(hours, minutes, seconds);   // Set the time
  rtc.setDate(day, month, year);    // Set the date

  strcpy(filename, "ANALOG00.CSV");   // Template for file name, characters 6 & 7 get set automatically later
  CreateFile();

}  

/////////////////////   Loop    //////////////////////
void loop() {

  blink(GREEN,2);               // Quick blink to show we have a pulse
  CurrentCycleCount += 1;       //  Increment samples in current uSD flush cycle
  CurrentFileCount += 1;        //  Increment samples in current file

  #ifdef ECHO_TO_SERIAL
    SerialOutput();             // Only logs to serial if ECHO_TO_SERIAL is uncommented at start of code
  #endif
  
  WriteToSD();                  // Output to uSD card stream, will not actually be written due to buffer/page size

  //  Code to limit the number of power hungry writes to the uSD
  //  Don't sync too often - requires 2048 bytes of I/O to SD card. 512 bytes of I/O if using Fat16 library
  //  But this code forces it to sync at a fixd interval, i.e. once per hour etc depending on what is set
  if( CurrentCycleCount >= SamplesPerCycle ) {
    logfile.flush();
    CurrentCycleCount = 0;
    #ifdef ECHO_TO_SERIAL
      Serial.println("logfile.flush() called");
    #endif
  }

  // Code to increment files limiting number of lines in each hence size, close the open file first.
  if( CurrentFileCount >= SamplesPerFile ) {
    if (logfile.isOpen()) {
      logfile.close();
    }
    CreateFile();
    CurrentFileCount = 0;
    #ifdef ECHO_TO_SERIAL
      Serial.println("New log file created: ");
      Serial.println(filename);
    #endif
  }
  
  ///////// Interval Timing and Sleep Code ////////////////

  // Calculate next alarm time. Simple proces if we're just adding seconds, bit more complicated if we add minutes & seconds
  if ( SampleIntMin == 0 ) {
    NextAlarmSec = (NextAlarmSec + SampleIntSec) % 60;    // modulus to deal with rollover 65 becomes 5
    rtc.setAlarmSeconds(NextAlarmSec);                    // RTC time to wake, currently seconds only
    rtc.enableAlarm(rtc.MATCH_SS);                        // Match seconds only
  }
  else {  
    NextAlarmMin = (rtc.getMinutes() + SampleIntMin) % 60;    // modulus to deal with rollover 65 becomes 5
    NextAlarmMin = NextAlarmMin + int ((rtc.getSeconds() + SampleIntSec) / 60);    // find rollover minute, i.e. 65 secs becomes 1 minute extra
    NextAlarmSec = (rtc.getSeconds() + SampleIntSec) % 60;    // i.e. 65 becomes 5

    #ifdef ECHO_TO_SERIAL
      OutputNextAlarm();                                  // Outputs next alarm if debugging
    #endif

    rtc.setAlarmSeconds(NextAlarmSec);                    // Set RTC time to wake in seconds
    rtc.setAlarmMinutes(NextAlarmMin);                    // Set RTC time to wake in minutes
    rtc.enableAlarm(rtc.MATCH_MMSS);                      // Match minutes & seconds
  }

  rtc.attachInterrupt(alarmMatch);                        // Attaches function to be called, currently blank
  delay(5);                                               // Brief delay prior to sleeping not really sure its required
  
  rtc.standbyMode();                                      // Sleep until next alarm match
  
  // Code re-starts here after sleep !

}

///////////////   Functions   //////////////////

// Create new file on uSD incrementing file name as required
void CreateFile()
{
  #ifdef ECHO_TO_SERIAL
    while (! Serial); // Wait until Serial is ready
    Serial.begin(115200);
    Serial.println("\r\nFeather M0 Analog logger");
  #endif
  
  // see if the card is present and can be initialized:
  if (!SD.begin(cardSelect)) {
    Serial.println("Card init. failed! or Card not present");
    error(2);     // Two red flashes means no card or card init failed.
  }
  
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = '0' + i/10;
    filename[7] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }  

  logfile = SD.open(filename, FILE_WRITE);
  if( ! logfile ) {
    Serial.print("Couldnt create "); 
    Serial.println(filename);
    error(3);
  }
  #ifdef ECHO_TO_SERIAL
    Serial.print("Writing to "); 
    Serial.println(filename);
    Serial.println("Logging ....");
  #endif

}

// Write data header to file of uSD.
void writeHeader() {
  logfile.println("DD:MM:YYYY hh:mm:ss, Battery Voltage");
}

// Print data and time followed by battery voltage to SD card
void WriteToSD() {

  // Formatting for file out put dd/mm/yyyy hh:mm:ss, [sensor output]  
  logfile.print(rtc.getDay());
  logfile.print("/");
  logfile.print(rtc.getMonth());
  logfile.print("/");
  logfile.print(rtc.getYear()+2000);
  logfile.print(" ");
  logfile.print(rtc.getHours());
  logfile.print(":");
  if(rtc.getMinutes() < 10)
    logfile.print('0');      // Trick to add leading zero for formatting
  logfile.print(rtc.getMinutes());
  logfile.print(":");
  if(rtc.getSeconds() < 10)
    logfile.print('0');      // Trick to add leading zero for formatting
  logfile.print(rtc.getSeconds());
  logfile.print(",");
  logfile.println(BatteryVoltage ());   // Print battery voltage
}



// Debbugging output of time/date and battery voltage
void SerialOutput() {

  //Serial.print(CurrentCycleCount);
  //Serial.print(":");
  //Serial.print(freeram ());
  //Serial.print("-");
  Serial.print("Current RTC time: ");
  Serial.print(rtc.getDay());
  Serial.print("/");
  Serial.print(rtc.getMonth());
  Serial.print("/");
  Serial.print(rtc.getYear()+2000);
  Serial.print(" ");
  Serial.print(rtc.getHours());
  Serial.print(":");
  if(rtc.getMinutes() < 10)
    Serial.print('0');      // Trick to add leading zero for formatting
  Serial.print(rtc.getMinutes());
  Serial.print(":");
  if(rtc.getSeconds() < 10)
    Serial.print('0');      // Trick to add leading zero for formatting
  Serial.print(rtc.getSeconds());
  Serial.print(",");
  Serial.println(BatteryVoltage ());   // Print battery voltage  
}

// Debugging output to set what next alarm value is
void OutputNextAlarm() {
  Serial.print("Next alarm time set: ");
  Serial.print(rtc.getHours());
  Serial.print(":");
  if(NextAlarmMin < 10)
    Serial.print('0');      // Trick to add leading zero for formatting
  Serial.print(NextAlarmMin);
  Serial.print(":");
  if(NextAlarmSec < 10)
    Serial.print('0');      // Trick to add leading zero for formatting
  Serial.println(NextAlarmSec);  
}


// blink out an error code
void error(uint8_t errno) {
  while(1) {
    uint8_t i;
    for (i=0; i<errno; i++) {
      digitalWrite(13, HIGH);
      delay(100);
      digitalWrite(13, LOW);
      delay(100);
    }
    for (i=errno; i<10; i++) {
      delay(200);
    }
  }
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

// Measure battery voltage using divider on Feather M0 - Only works on Feathers !!
float BatteryVoltage () {
  measuredvbat = analogRead(VBATPIN);   //Measure the battery voltage at pin A7
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  return measuredvbat;
}



void alarmMatch() // Do something when interrupt called
{
  
}

// Small ARM free RAM function for use during debugging
int freeram () {
  char stack_dummy = 0;
  return &stack_dummy - sbrk(0);
}

