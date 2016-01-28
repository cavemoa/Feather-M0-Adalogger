/*
  Simple Logger using internal RTC for Arduino Zero

 Created by:  Jonathan DAvies
 Date:        26 Dec 2015
 Version:     0.4
 Switched to RTCInt library from Arduino after problem with RTCZero
*/

////////////////////////////////////////////////////////////
//#define ECHO_TO_SERIAL // Allows serial output if uncommented
////////////////////////////////////////////////////////////

#include <SPI.h>
#include <SD.h>
#include <RTCZero.h> // Uses GabrielNotman's version of RTCZero which correctly sets 24Hr mode compared to main branch

#define cardSelect 4  // Set the pins used
#define VBATPIN A7    // Battery Voltage on Pin A7
#ifdef ARDUINO_SAMD_ZERO
   #define Serial SerialUSB   // re-defines USB serial from M0 chip so it appears as regular serial
#endif

//////////////// Key Settings ///////////////////

const int SampleIntSeconds = 5000;   //Sample interval in ms i.e. 1000 = 1 sec

/* Change these values to set the current initial time */
const byte hours = 13;
const byte minutes = 32;
const byte seconds = 0;
/* Change these values to set the current initial date */
const byte day = 27;
const byte month = 12;
const byte year = 15;

/////////////// Global Objects ////////////////////
RTCZero rtc;    // Create RTC object
File logfile;   // Create file object
float measuredvbat;   // Variable for battery voltage





//////////////    Setup   ///////////////////
void setup() {

  rtc.begin();    // Start the RTC in 24hr mode
  rtc.setTime(hours, minutes, seconds);   // Set the time
  rtc.setDate(day, month, year);    // Set the date
   
  #ifdef ECHO_TO_SERIAL
    while (! Serial); // Wait until Serial is ready
    Serial.begin(115200);
    Serial.println("\r\nFeather M0 Analog logger");
  #endif
  
  pinMode(13, OUTPUT);


  // see if the card is present and can be initialized:
  if (!SD.begin(cardSelect)) {
    Serial.println("Card init. failed! or Card not present");
    error(2);     // Two red flashes means no card or card init failed.
  }
  char filename[15];
  strcpy(filename, "ANALOG00.CSV");
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
  Serial.print("Writing to "); 
  Serial.println(filename);

  pinMode(13, OUTPUT);
  pinMode(8, OUTPUT);
  Serial.println("Logging ....");
}

///////////////   Loop    //////////////////
void loop() {
  digitalWrite(8, HIGH);  // Turn the green LED on

  measuredvbat = analogRead(VBATPIN);   //Measure the battery voltage at pin A7
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
 
  SdOutput();       // Output to uSD card
  
  #ifdef ECHO_TO_SERIAL
    SerialOutput();   // Only logs to serial if ECHO_TO_SERIAL is uncommented at start of code
  #endif
  
  digitalWrite(8, LOW);   // Turn the green LED off 
  delay(SampleIntSeconds);   // Interval set by const in header
}

///////////////   Functions   //////////////////

// Debbugging output of time/date and battery voltage
void SerialOutput() {
  
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
  Serial.println(measuredvbat);   // Print battery voltage  
}

// Print data and time followed by battery voltage to SD card
void SdOutput() {

  /* Force data to SD and update the directory entry to avoid data loss.
  if (!file.sync() || file.getWriteError()) {
    error("write error");
    error(3);     // Three red flashes means write failed.
  }
  */

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
  logfile.println(measuredvbat);   // Print battery voltage
  logfile.flush();

}

// Write data header.
void writeHeader() {

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
