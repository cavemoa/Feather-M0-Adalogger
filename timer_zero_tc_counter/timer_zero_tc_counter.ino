/**
 * @author Markus Bader
 * @brief this program shows how to use the TC timer with interrupts on an Arduino Zero board
 * @email markus.bader@tuwien.ac.at
 */
 
int pin_ovf_led = 13;  // debug pin for overflow led 
int pin_mc0_led = 5;  // debug pin for compare led 
unsigned int loop_count = 0;
unsigned int irq_ovf_count = 0;

void setup() {

  pinMode(pin_ovf_led, OUTPUT);   // for debug leds
  digitalWrite(pin_ovf_led, LOW); // for debug leds
  pinMode(pin_mc0_led, OUTPUT);   // for debug leds
  digitalWrite(pin_mc0_led, LOW); // for debug leds
  Serial.begin(9600);

  
  // Enable clock for TC 
  REG_GCLK_CLKCTRL = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_TCC2_TC3) ;
  while ( GCLK->STATUS.bit.SYNCBUSY == 1 ); // wait for sync 

  // The type cast must fit with the selected timer mode 
  TcCount16* TC = (TcCount16*) TC3; // get timer struct

  TC->CTRLA.reg &= ~TC_CTRLA_ENABLE;   // Disable TCCx
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync 

  TC->CTRLA.reg |= TC_CTRLA_MODE_COUNT16;  // Set Timer counter Mode to 16 bits
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync 
  TC->CTRLA.reg |= TC_CTRLA_WAVEGEN_NFRQ; // Set TC as normal Normal Frq
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync 

  TC->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV2;   // Set perscaler
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync 
  
  // TC->PER.reg = 0xFF;   // Set counter Top using the PER register but the 16/32 bit timer counts allway to max  
  // while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync 

  TC->CC[0].reg = 0x1F;
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync 
  
  // Interrupts 
  TC->INTENSET.reg = 0;              // disable all interrupts
  TC->INTENSET.bit.OVF = 1;          // enable overfollow
  TC->INTENSET.bit.MC0 = 1;          // enable compare match to CC0

  // Enable InterruptVector
  NVIC_EnableIRQ(TC3_IRQn);

  // Enable TC
  TC->CTRLA.reg |= TC_CTRLA_ENABLE;
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync 

}

void loop() {
  // dummy
  delay(250);
  TcCount16* TC = (TcCount16*) TC3; // get timer struct
  delay(250);
  uint16_t counter = TC->COUNT.reg;
  while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync 
  char msg[0xFF];
  sprintf(msg, "%4u: COUNT = %6X", loop_count, counter);
  Serial.println(msg);
  loop_count++;
}

void TC3_Handler()
{
  TcCount16* TC = (TcCount16*) TC3; // get timer struct
  if (TC->INTFLAG.bit.OVF == 1) {  // A overflow caused the interrupt
    digitalWrite(pin_ovf_led, irq_ovf_count % 2); // for debug leds
    digitalWrite(pin_mc0_led, HIGH); // for debug leds
    TC->INTFLAG.bit.OVF = 1;    // writing a one clears the flag ovf flag
    irq_ovf_count++;                 // for debug leds
  }
  
  if (TC->INTFLAG.bit.MC0 == 1) {  // A compare to cc0 caused the interrupt
    digitalWrite(pin_mc0_led, LOW);  // for debug leds
    TC->INTFLAG.bit.MC0 = 1;    // writing a one clears the flag ovf flag
  }
}


