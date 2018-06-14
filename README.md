# Code for Feather M0 Adalogger 
#### All code is primarily for [Feather M0 Hardware] (https://learn.adafruit.com/adafruit-feather-m0-adalogger/) by Adafruit which handily runs the SAMD21 chip (i.e. Arduino Zero) which has an an inbuilt RTC. The adafruit incantation has an onboard uSD card slot and lovely small form factor. 
Repo for the code  as I play with the new Feather M0 Adalogger from Adafruit. Aim will be to develop a standard logger code primarily for I2C devices but also ADC if you want. That makes use of the compact Feather M0, its RTC and deepsleep/low power modes.

### SimpleLogger

First pass at a simple logger that saves ADC (in this case battery voltage) to uSD, it uses the RTC for a time stamp but not to set logging interval. Does not sleep and uses logfile.flush() to ensure every reading is saved. So is reasonably heavy on power. Much of the code comes from the Adafruit tutorial.

### SimpleLogger2
Similar to simplelogger but with code in functions and minor revisions/tweaks. Still uses delay() and not RTC

### SimpleSleep
Basic code using the RTCzero library to set the Feathers RTC into deep sleep every 5 seconds and blink the red LED twice to show its awake. Just a proof of concept not terribly useful on its own.

### SimpleSleepUSB
Basic sketch to demonstate genrating serial during repeated awake/sleep cycles. It detaches the boards USB, puts the Feather M0 to sleep. When it wakes 10 seconds later it re-attaches the USB and allows serial output.

The USBDevice.detach(); -> rtc.standbyMode(); -> after RTC interupt -> USBDevice.attach(); was the crucial sequence it getting the Feather M0 to talk after sleeping.

> Note: You do need to remember to close the serial moniotr when the Feather goes into deepsleep as it won't just start outputing to the same window when it wakes. You have to open another one once you hear the USB re-attach or see the LED's blink.

### LowPowerLogger
Basic logger that utilizes the Feather M0 Adalogger. It sets the RTC then wakes at regular intervals (15 secs by default) to write to the uSD card. Currently just logs battery voltage but could of course log anything with additional code.
It has basic error code for uSD startup, flushes after every SD card write (so not the most efficient) and flashes the green LED each sample.


