// example / testing script for TLC5940 library

// pin defines
// greyscale clock - D2
#define TLC5940_GS_PORT PORTD
#define TLC5940_GS_PIN 2
// serial clock - D4
#define TLC5940_SCK_PORT PORTD
#define TLC5940_SCK_PIN 4
// latch - D3
#define TLC5940_XLAT_PORT PORTD
#define TLC5940_XLAT_PIN 3
// programming select - D1
#define TLC5940_VPRG_PORT PORTD
#define TLC5940_VPRG_PIN  1
// blank outputs - C6
#define TLC5940_BLANK_PORT PORTC
#define TLC5940_BLANK_PIN 6
// serial data master out slave in - D7
#define TLC5940_MOSI_PORT PORTD
#define TLC5940_MOSI_PIN 7
// number of drivers
#define TLC5940_N 3

#include "TLC5940.h"

// tlc object
TLC5940 tlc;

void setup(void) {
  Serial.begin(9600);

  Serial.println("initializing tlc");
  tlc.init();
}

void loop(void) {

}

// ISR for serial data input into TLC5940
// run in non-blocking mode so that the greyscale cycle continues regardless of serial data being clocked in
ISR(TIMER0_COMPA_vect) {
  tlc.serialCycle();
}

// ISR for greyscale clock on TLC5940
ISR(TIMER1_COMPA_vect) {
  tlc.gsCycle();
}
