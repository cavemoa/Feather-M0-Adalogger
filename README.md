# Code for Feather M0 Adalogger Hardware by Adafruit
Repo for the code  as I play with the new Feather M0 Adalogger from Adafruit. Aim will be to develop a standard logger code primarily for I2C devices but also ADC if you want. That makes use of the compact Feather M0, its RTC and deepsleep/low power modes.

##SimpleLogger
First pass at a simple logger that saves ADC (in this case battery voltage) to uSD, it uses the RTC for a time stamp but not to set logging interval. Does not sleep and uses logfile.flush() to ensure every reading is saved. So is reasonably heavy on power.
