// header for TLC5940 class for ATMega MCUs
// https://github.com/mcous/TLC5940
// copyright 2013 by michael cousins
// released under the terms of the MIT license

#ifndef TLC5940_H
#define TLC5940_H

// headers necessary for this file
#include <avr/io.h>

// communication pins - define in makefile (avrdude) or set appropriately (arduino)
// note: DCPRG is tied high on all drivers
// greyscale clock - OCR1A on an ATMega328p is PB1
#ifndef TLC5940_GS_PORT
#define TLC5940_GS_PORT PORTB
#endif
#ifndef TLC5940_GS_PIN
#define TLC5940_GS_PIN 1
#endif
// serial clock - PD5
#ifndef TLC5940_SCK_PORT
#define TLC5940_SCK_PORT PORTD
#endif
#ifndef TLC5940_SCK_PIN
#define TLC5940_SCK_PIN 5
#endif
// latch - PD4
#ifndef TLC5940_XLAT_PORT
#define TLC5940_XLAT_PORT PORTD
#endif
#ifndef TLC5940_XLAT_PIN
#define TLC5940_XLAT_PIN 4
#endif
// programming select - PD7
#ifndef TLC5940_VPRG_PORT
#define TLC5940_VPRG_PORT PORTD
#endif
#ifndef TLC5940_VPRG_PIN
#define TLC5940_VPRG_PIN  7
#endif
// blank outputs - pullup resistor + PD3
#ifndef TLC5940_BLANK_PORT
#define TLC5940_BLANK_PORT PORTD
#endif
#ifndef TLC5940_BLANK_PIN
#define TLC5940_BLANK_PIN 3
#endif
// serial data master out slave in - PD6
#ifndef TLC5940_MOSI_PORT
#define TLC5940_MOSI_PORT PORTD
#endif
#ifndef TLC5940_MOSI_PIN
#define TLC5940_MOSI_PIN 6
#endif

#ifndef TLC5940_N
#define TLC5940_N 3
#endif

#define TLC5940_LED_N 16 * TLC5940_N


class TLC5940 {
public:
    // constructor (don't need a destructor)
    TLC5940();
    // initilize the chip by filling the dc data
    void init(void);
    // set the dot correction array
    void setDC(uint8_t led, uint8_t val);
    // set the brightness of an led in the array
    void setGS(uint8_t led, uint16_t val);
    // tell the chip to clock in the data from the GS array
    void update(void);
    // gs refresh function (lives in an ISR that fires ever 4096 greyscale clocks)
    void refreshGS(void);

private:
    // serial data function - returns if a latch is needed or not
    bool serialCycle(void);
    // dc array
    uint8_t dc[TLC5940_LED_N];
    // gs array
    uint16_t gs[TLC5940_LED_N];
    // new gs data flag
    volatile bool newData;
};

#endif