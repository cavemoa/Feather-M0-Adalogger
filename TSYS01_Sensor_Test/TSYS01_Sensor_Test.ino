/*
 Modified Blue Robotics TSYS01 Library Example to run on Feather M0

Uses the 24bit TSYS01 Temperature Sensor on I2C to do a simple read test

 Created by:  Jonathan Davies
 Date:        16/07/18
 Version:     0.1
*/


#include <Wire.h>
#include "TSYS01.h"

#ifdef ARDUINO_SAMD_ZERO
   #define Serial SerialUSB   // re-defines USB serial from M0 chip so it appears as regular serial
#endif

TSYS01 sensor;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting");
  Wire.begin();
  sensor.init();
}

void loop() {
  sensor.read();
  Serial.print("Temperature: ");
  Serial.print(sensor.temperature()); 
  Serial.println(" deg C");
  delay(1000);
}
