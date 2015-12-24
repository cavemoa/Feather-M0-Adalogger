/*
  Simple Logger using internal RTC for Arduino Zero

 Created by:  Jonathan DAvies
 Date:        21 Oct 2015
 Version:     0.1
*/


#include <SPI.h>
#include <SD.h>
#include <RTCZero.h>


#define cardSelect 4  // Set the pins used
#define VBATPIN A7    // Battery Voltage on Pin A7
#ifdef ARDUINO_SAMD_ZERO
   #define Serial SerialUSB
#endif

File logfile;   // Create file object

RTCZero rtc;    // Create RTC object
/* Change these values to set the current initial time */
const byte hours = 0;
const byte minutes = 0;
const byte seconds = 0;


//////////////    Setup   ///////////////////
void setup() {

  rtc.begin();    // Start the RTC
  
  // while (! Serial); // Wait until Serial is ready
  // Serial.begin(115200);
  // Serial.println("\r\nAnalog logger test");
  
  pinMode(13, OUTPUT);


  // see if the card is present and can be initialized:
  if (!SD.begin(cardSelect)) {
    Serial.println("Card init. failed!");
    error(2);
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
  Serial.println("Ready!");
}

uint8_t i=0;

///////////////   Loop    //////////////////
void loop() {
  digitalWrite(8, HIGH);

  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  
  /* Print RTC Time
  Serial.print(rtc.getHours());
  Serial.print(":");
  Serial.print(rtc.getMinutes());
  Serial.print(":");
  Serial.print(rtc.getSeconds());
  Serial.print(",");
  Serial.println(measuredvbat);   // Print battery voltage
  */
  
  // Print Data to SD card
  logfile.print(rtc.getHours());
  logfile.print(":");
  logfile.print(rtc.getMinutes());
  logfile.print(":");
  logfile.print(rtc.getSeconds());
  logfile.print(", ");
  logfile.println(measuredvbat);   // Print battery voltage
  logfile.flush();
  
  digitalWrite(8, LOW);
  
  delay(10000);
}

///////////////   Functions   //////////////////
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
