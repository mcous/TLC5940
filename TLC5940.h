// header file for tlc5940 class for atmega chips
#ifndef TLC5940_H
#define TLC5940_H

// headers necessary for this file
#include <avr/io.h>


// communication pins - define in makefile or set appropriately
// greyscale clock
#ifndef TLC5940_GS_PORT
#define TLC5940_GS_PORT PORTB
#endif
#ifndef TLC5940_GS_PIN
#define TLC5940_GS_PIN 6
#endif
// serial clock - D4
#ifndef TLC5940_SCK_PORT
#define TLC5940_SCK_PORT PORTD
#endif
#ifndef TLC5940_SCK_PIN
#define TLC5940_SCK_PIN 4
#endif
// latch - D3
#ifndef TLC5940_XLAT_PORT
#define TLC5940_XLAT_PORT PORTD
#endif
#ifndef TLC5940_XLAT_PIN
#define TLC5940_XLAT_PIN 3
#endif
// programming select - D1
#ifndef TLC5940_VPRG_PORT
#define TLC5940_VPRG_PORT PORTD
#endif
#ifndef TLC5940_VPRG_PIN
#define TLC5940_VPRG_PIN  1
#endif
// blank outputs - C6
#ifndef TLC5940_BLANK_PORT
#define TLC5940_BLANK_PORT PORTC
#endif
#ifndef TLC5940_BLANK_PIN
#define TLC5940_BLANK_PIN 6
#endif
// serial data master out slave in - D7
#ifndef TLC5940_MOSI_PORT
#define TLC5940_MOSI_PORT PORTD
#endif
#ifndef TLC5940_MOSI_PIN
#define TLC5940_MOSI_PIN 7
#endif

/*
// pin defines
// greyscale clock - B1
#define TLC5940_GS_PORT PORTB
#define TLC5940_GS_PIN 1
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
*/

// number of drivers
//#define TLC5940_N 1

#ifndef TLC5940_N
#define TLC5940_N 2
#endif

#define TLC5940_LED_N 16 * TLC5940_N


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
    // gs refresh function (goes in ISR)
    void refreshGS(void);
    // serial data function - returns if a latch is needed or not
    bool serialCycle(void);
    // set the new data flag after new data input is done
    void update(void);

private:
    // dc array
    uint8_t dc[TLC5940_LED_N];
    // gs array
    uint16_t gs[TLC5940_LED_N];
    // new gs data flag
    volatile bool newData;
};

#endif