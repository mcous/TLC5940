// example / testing script for TLC5940 library

#include "TLC5940.h"
#include "avr/interrupt.h"

// tlc object
TLC5940 tlc;
// loop counter
uint16_t count;
int8_t dir;

void setup(void) {
  Serial.begin(9600);

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
  // OCR1A = 0 -> f=F_CPU/2 = 8000000
  // gs edge gets sent every 32*2=64 clock ticks
  OCR1A = 31;
  // put the timer in CTC mode and start timer with no prescaler
  TCCR1B |= ( (1 << WGM12) | (1 << CS10) );
  // enable interrupt on output compare A match
  //TIMSK1 |= (1 << OCIE1A);
  
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
  Serial.println("loop");

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
  count += dir*100;

  // delay
  //_delay_ms(100);
  //delay(100);
}


// ISR for serial data input into TLC5940
// run in non-blocking mode so that the greyscale cycle continues regardless of serial data being clocked in
ISR(TIMER0_COMPA_vect) {
  tlc.refreshGS();
}

/*
// ISR for greyscale clock on TLC5940
ISR(TIMER1_COMPA_vect) {
  tlc.refreshGS();
}
*/
