// header file for tlc5940 class for atmega chips
#ifndef TLC5940_H
#define TLC5940_H

// headers necessary for this file
#include <avr/io.h>

// communication pins - set appropriately
// greyscale clock - B0 - D3
#define DDR_GSCLK  DDRB
#define PORT_GSCLK PORTB
#define GSCLK_PIN  0
// serial clock - B5
#define DDR_SCLK  DDRB
#define PORT_SCLK PORTB
#define SCLK_PIN  5
// dc programming select - D4
#define DDR_DCPRG  DDRD
#define PORT_DCPRG PORTD
#define DCPRG_PIN  4
// programming select - D5
#define DDR_VPRG  DDRD
#define PORT_VPRG PORTD
#define VPRG_PIN  5
// latch - B2 - B1
#define DDR_XLAT  DDRB
#define PORT_XLAT PORTB
#define XLAT_PIN  2
// blank outputs - B1 - B2
#define DDR_BLANK  DDRB
#define PORT_BLANK PORTB
#define BLANK_PIN  1
// serial data master out slave in - B3
#define DDR_MOSI  DDRB
#define PORT_MOSI PORTB
#define MOSI_PIN  3

#define TLC5940_N 1
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