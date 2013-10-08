// example / testing script for TLC5940 library

/*
// pin defines
// greyscale clock - D2
#define TLC5940_GS_PORT PORTD
#define TLC5940_GS_PIN 2
// serial clock - D5
#define TLC5940_SCK_PORT PORTD
#define TLC5940_SCK_PIN 5
// latch - D4
#define TLC5940_XLAT_PORT PORTD
#define TLC5940_XLAT_PIN 4
// programming select - D7
#define TLC5940_VPRG_PORT PORTD
#define TLC5940_VPRG_PIN  7
// blank outputs - D3
#define TLC5940_BLANK_PORT PORTD
#define TLC5940_BLANK_PIN 3
// serial data master out slave in - D6
#define TLC5940_MOSI_PORT PORTD
#define TLC5940_MOSI_PIN 6
// number of drivers
#define TLC5940_N 1
*/

#include "TLC5940.h"

// tlc object
TLC5940 tlc;
// loop counter
uint16_t count;
int8_t dir;

void setup(void) {
  Serial.begin(9600);
  Serial.println(TLC5940_LED_N);

  count = 200;
  dir = 1;

  // set DC to full
  for (uint8_t i=0; i<TLC5940_LED_N; i++) {
    tlc.setDC(i, 63);
  }

  // initialize the led driver
  Serial.println("initializing tlc");
  tlc.init();
  Serial.println("initialized; starting loop");
}

void loop(void) {
  Serial.print("refresh at "); Serial.println(count);
  tlc.refreshGS();

  for (uint8_t i=0; i<TLC5940_LED_N; i++) {
    tlc.setGS(i, count);
  }

  // set loop direction
  if (dir==1 && count>=4000) {
    dir = -1;
  }
  else if (dir==-1 && count<=300) {
    dir = 1;
  }
  // increment counter
  count += dir*200;

}

/*
// ISR for serial data input into TLC5940
// run in non-blocking mode so that the greyscale cycle continues regardless of serial data being clocked in
ISR(TIMER0_COMPA_vect) {
  tlc.serialCycle();
}

// ISR for greyscale clock on TLC5940
ISR(TIMER1_COMPA_vect) {
  tlc.gsCycle();
}
*/
