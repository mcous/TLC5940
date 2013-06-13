 // low level tlc5940 class for atmega microcontrollers

#include "TLC5940.h"

// initilize pin modes and values in constructor
TLC5940::TLC5940(void) {
    // set control pins to outputs and initial values
    // gsclk
    DDR_GSCLK |= (1 << GSCLK_PIN);
    PORT_GSCLK &= ~(1 << GSCLK_PIN);
    // sclk
    DDR_SCLK |= (1 << SCLK_PIN);
    PORT_SCLK &= ~(1 << SCLK_PIN);
    // dcprg
    DDR_DCPRG |= (1 << DCPRG_PIN);
    PORT_DCPRG &= ~(1 << DCPRG_PIN);
    // vprg
    DDR_VPRG |= (1 << VPRG_PIN);
    PORT_VPRG |= (1 << VPRG_PIN);
    // xlat
    DDR_XLAT |= (1 << XLAT_PIN);
    PORT_XLAT &= ~(1 << XLAT_PIN);
    // blank
    DDR_BLANK |= (1 << BLANK_PIN);
    PORT_BLANK |= (1 << BLANK_PIN);
    // serial data master out slave in
    DDR_MOSI |= (1 << MOSI_PIN);
    PORT_MOSI &= ~(1 << MOSI_PIN);

    // initialize variables at all leds off for safety
    for (uint8_t i=0; i<(16 * TLC5940_N); i++) {
        setDC(i, 0);
    }
    for (uint8_t i=0; i<(16 * TLC5940_N); i++) {
        setGS(i, 0);
    }
    gsFirstCycle = false;
}

// initialize the dot correction
void TLC5940::init(void) {
    // set dcprg to 1 (don't use eeprom data)
    PORT_DCPRG |= (1 << DCPRG_PIN);
    // set vprg to 1 (program dc data)
    PORT_VPRG |= (1 << VPRG_PIN);
    // set serial data to high (setting dc to 1)
    PORT_MOSI |= (1 << MOSI_PIN);
    // pulse the serial clock 96 times to write in dc data
    for (uint8_t i=0; i<(96 * TLC5940_N); i++) {
        // get the bit the tlc5940 is expecting from the gs array (tlc expects msb first)
        uint8_t data = (dc[((96 * TLC5940_N) - 1 - i)/6]) & (1 << ((96 * TLC5940_N) - 1 - i)%6);
        // set mosi if bit is high, clear if bit is low
        if (data) {
            PORT_MOSI |= (1 << MOSI_PIN);
        }
        else {
            PORT_MOSI &= ~(1 << MOSI_PIN);
        }
        PORT_SCLK |= (1 << SCLK_PIN);
        PORT_SCLK &= ~(1 << SCLK_PIN);
    }
    // pulse xlat to latch the data
    PORT_XLAT |= (1 << XLAT_PIN);
    PORT_XLAT &= ~(1 << XLAT_PIN);
}

// refresh the led display and data
void TLC5940::refreshGS(void) {
    // if vprg is high
    if (PORT_VPRG & (1 << VPRG_PIN)) {
        // set it low
        PORT_VPRG &= ~(1 << VPRG_PIN);
        // set the first cycle flag (just came out of a dc cycle)
        gsFirstCycle = true;
    }

    // enable leds
    PORT_BLANK &= ~(1 << BLANK_PIN);
    // loop through a gs cycle and post-increment gs counter
    for(uint16_t gsCount = 0; gsCount < 4096; gsCount++) {
        // if there's data to clock in
        if (gsCount < (192 * TLC5940_N)) {
            // get the bit the tlc5940 is expecting from the gs array (tlc expects msb first)
            uint16_t data = (gs[((192 * TLC5940_N) - 1 - gsCount)/12]) & (1 << ((192 * TLC5940_N) - 1 - gsCount)%12);
            // set mosi if bit is high, clear if bit is low
            if (data) {
                PORT_MOSI |= (1 << MOSI_PIN);
            }
            else {
                PORT_MOSI &= ~(1 << MOSI_PIN);
            }
            // pulse the serial clock
            PORT_SCLK |= (1 << SCLK_PIN);
            PORT_SCLK &= ~(1 << SCLK_PIN);
            // increment data counter
        }
        // pulse the gs clock
        PORT_GSCLK |= (1 << GSCLK_PIN);
        PORT_GSCLK &= ~(1 << GSCLK_PIN);
    }
    // disable leds before latching in new data
    PORT_BLANK |= (1 << BLANK_PIN);
    // pulse xlat to save new gs data
    PORT_XLAT |= (1 << XLAT_PIN);
    PORT_XLAT &= ~(1 << XLAT_PIN);

    // check if this was the first gs cycle after a dc cycle
    if (gsFirstCycle) {
        // pulse serial clock once if it is (because the datasheet tells us to)
        PORT_SCLK |= (1 << SCLK_PIN);
        PORT_SCLK &= ~(1 << SCLK_PIN);
    }
    gsFirstCycle = false;
}

// set the brightness of an individual led
void TLC5940::setDC(uint8_t led, uint8_t val) {
    // basic parameter checking
    // check if led is inbounds
    if (led < (16 * TLC5940_N)) {
        // if value is out of bounds, set to max
        if (val < 64) {
            dc[led] = val;
        }
        else {
            dc[led] = 63;
        }
    }
}

// set the brightness of an individual led
void TLC5940::setGS(uint8_t led, uint16_t val) {
    // basic parameter checking
    // check if led is inbounds
    if (led < (16 * TLC5940_N)) {
        // if value is out of bounds, set to max
        if (val < 4096) {
            gs[led] = val;
        }
        else {
            gs[led] = 4095;
        }
    }
}