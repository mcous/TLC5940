// example / testing script for TLC5940 library
// https://github.com/mcous/TLC5940
// copyright 2013 by michael cousins
// released under the terms of the MIT license
//
// 16 MHz Arduino recommended
// configured for Arduino Uno with:
// DCPRG - N/A - N/A   - tied high
// GSCLK - PB1 - pin 9
// BLANK - PD3 - pin 3 - 10k pullup to VCC
// XLAT  - PD4 - pin 4
// SCLK  - PD5 - pin 5
// SIN   - PD6 - pin 6
// VPRG  - PD7 - pin 7
//
// change these pins in TLC5940.h
// GSCLK must be plugged into a OC (PWM) output and a timer must be properly configured to toggle that OC
//
// serial ISR uses timer 0, which wipes out the Arduino delay() function; this sketch uses _delay_ms() from avr/delay.h instead

#include "TLC5940.h"
#include "avr/interrupt.h"
#include "avr/delay.h"

// tlc object
TLC5940 tlc;
// loop counter
uint16_t count;
int8_t dir;

void setup(void) {
  Serial.begin(9600);
  Serial.println("send any character to begin");
  while(!Serial.available());
  Serial.read();

  count = 200;
  dir = 1;

  // set DC to full
  for (uint8_t i=0; i<TLC5940_LED_N; i++) {
    tlc.setDC(i, 63);
  }

  // initialize the led driver
  Serial.println("initializing tlc");
  tlc.init();

  cli();

  // user timer 1 to toggle the gs clock pin
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1C = 0;
  TIMSK1 = 0;
  // toggle OC1A (pin B1) on compare match event
  TCCR1A |= (1 << COM1A0);
  // set the top of the timer
  // PS = 1, F_CPU = 16 MHz, F_OC = F_CPU/(2 * PS * (OCR1A+1)
  // gs edge gets sent every 32*2=64 clock ticks
  OCR1A = 31;
  // put the timer in CTC mode and start timer with no prescaler
  TCCR1B |= ( (1 << WGM12) | (1 << CS10) );

  // set up an isr for the serial cycle to live in
  // let it live in timer 0
  TCCR0A = 0;
  TCCR0B = 0;
  TIMSK0 = 0;
  // set waveform generation bit to put the timer into CTC mode
  TCCR0A |= (1 << WGM01);
  // set the top of the timer - want this to happen every 4096 * gs clocks = every 8192 clock ticks
  // set top to 255 for an interrupt every 256 * 1024 = 64 * 4096 clock ticks
  OCR0A = 255;
  // start the timer with a 1024 prescaler
  TCCR0B |= ( (1 << CS02) | (1 << CS00) );
  // enable the interrupt of output compare A match
  TIMSK0 |= (1 << OCIE0A);

  sei();
}

void loop(void) {
  //Serial.println("loop");

  // give it some new data
  for (uint8_t i=0; i<TLC5940_LED_N; i++) {
    tlc.setGS(i, count);
  }
  // tell the driver to update
  tlc.update();

  // set loop direction
  if (dir==1 && count>=4000) {
    dir = -1;
  }
  else if (dir==-1 && count<=300) {
    dir = 1;
  }
  // increment counter
  count += dir*100;

  // delay
  _delay_ms(50);
}

// ISR for serial data input into TLC5940
// run in non-blocking mode so that the greyscale cycle continues regardless of serial data being clocked in
ISR(TIMER0_COMPA_vect) {
  tlc.refreshGS();
}
