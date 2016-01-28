#include <ZeroRegs.h>

void setup() {
  // put your setup code here, to run once:
  SerialUSB.begin(9600);
    while (! SerialUSB) {}  // wait for serial monitor to attach
    ZeroRegOptions opts = { SerialUSB, false };
    printZeroRegs(opts);
}

void loop() {
  // put your main code here, to run repeatedly:

}
