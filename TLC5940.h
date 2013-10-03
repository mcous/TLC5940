// header file for tlc5940 class for atmega chips
#ifndef TLC5940_H
#define TLC5940_H

// headers necessary for this file
#include <avr/io.h>

// communication pins - set appropriately
// greyscale clock - D2
#define DDR_GSCLK  DDRD
#define PORT_GSCLK PORTD
#define GSCLK_PIN  2
// serial clock - D4
#define DDR_SCLK  DDRD
#define PORT_SCLK PORTD
#define SCLK_PIN  4
// dc programming select - unused (pulled up)
//#define DDR_DCPRG  DDRB
//#define PORT_DCPRG PORTB
//#define DCPRG_PIN  1
// programming select - B3/D1
#define DDR_VPRG  DDRD
#define PORT_VPRG PORTD
#define VPRG_PIN  1
// latch - D3
#define DDR_XLAT  DDRD
#define PORT_XLAT PORTD
#define XLAT_PIN  3
// blank outputs - B2/D0/C6
#define DDR_BLANK  DDRC
#define PORT_BLANK PORTC
#define BLANK_PIN  6
// serial data master out slave in - D7
#define DDR_MOSI  DDRD
#define PORT_MOSI PORTD
#define MOSI_PIN  7

#define TLC5940_N 2
#define LED_N 16 * TLC5940_N


class TLC5940 {
public:
    // constructor (don't need a destructor)
    TLC5940();
    // initilize the chip by filling the dc data
    void init(void);
    // set dot correction
    void setDC(uint8_t led, uint8_t val);
    // set the brightness of an led
    void setGS(uint8_t led, uint16_t val);
    // gs refresh function (goes in main loop)
    void refreshGS(void);

private:
    // dc array
    uint8_t dc[LED_N];
    // gs array
    uint16_t gs[LED_N];
    // gs first cycle flag
    bool gsFirstCycle;
};

#endif