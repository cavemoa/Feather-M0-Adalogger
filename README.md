# Code for Feather M0 Adalogger 
### All code is primarily for [Feather M0 Hardware] (https://learn.adafruit.com/adafruit-feather-m0-adalogger/) by Adafruit
Repo for the code  as I play with the new Feather M0 Adalogger from Adafruit. Aim will be to develop a standard logger code primarily for I2C devices but also ADC if you want. That makes use of the compact Feather M0, its RTC and deepsleep/low power modes.

##SimpleLogger
First pass at a simple logger that saves ADC (in this case battery voltage) to uSD, it uses the RTC for a time stamp but not to set logging interval. Does not sleep and uses logfile.flush() to ensure every reading is saved. So is reasonably heavy on power. Much of the code comes from the Adafruit tutorial.

##SimpleLogger2
Similar to simplelogger but with code in functions and minor revisions/tweaks. Still uses delay() and not RTC

##SimpleSleep
Basic code using the RTCzero library to set the Feathers RTC to wake ont the 30th second of every minute. It just uses a seconds match only. It then disconnects the USB and puts itself to sleep. Once awake it reconnects the USB and waits for you to re-open a serial terminal to display the fact its awake.

The USBDevice.detach(); -> rtc.standbyMode(); -> after RTC interupt -> USBDevice.attach(); was the crucial sequence it gettingt he Feather M0 to talk after sleeping


