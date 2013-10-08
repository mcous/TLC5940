 // low level tlc5940 class for atmega microcontrollers

#include "TLC5940.h"
#include "arduino.h"

// DDR from PORT macro
#define DDR(port) (*(&port-1))

// initilize pin modes and values in constructor
TLC5940::TLC5940(void) {

  // initialize pins
  // gsclk - output set low initially
  DDR(TLC5940_GS_PORT) |= (1 << TLC5940_GS_PIN);
  TLC5940_GS_PORT &= ~(1 << TLC5940_GS_PIN);
  // sclk - output set low
  DDR(TLC5940_SCK_PORT) |= (1 << TLC5940_SCK_PIN);
  TLC5940_SCK_PORT &= ~(1 << TLC5940_SCK_PIN);
  // xlat - output set low
  DDR(TLC5940_XLAT_PORT) |= (1 << TLC5940_XLAT_PIN);
  TLC5940_XLAT_PORT &= ~(1 << TLC5940_XLAT_PIN);
  // blank - output set high (active high pin blanks output when high)
  DDR(TLC5940_BLANK_PORT) |= (1 << TLC5940_BLANK_PIN);
  TLC5940_BLANK_PORT &= ~(1 << TLC5940_BLANK_PIN);
  // serial data MOSI - output set low
  DDR(TLC5940_MOSI_PORT) |= (1 << TLC5940_MOSI_PIN);
  TLC5940_MOSI_PORT &= ~(1 << TLC5940_MOSI_PIN);
  // programming select - output set high
  DDR(TLC5940_VPRG_PORT) |= (1 << TLC5940_VPRG_PIN);
  TLC5940_VPRG_PORT |= (1 << TLC5940_VPRG_PIN);
    
  /*  
    // set control pins to outputs and initial values
    // gsclk
    DDR_GSCLK |= (1 << GSCLK_PIN);
    TLC5940_GS_PORT &= ~(1 << GSCLK_PIN);
    // sclk
    DDR_SCLK |= (1 << SCLK_PIN);
    TLC5940_SCK_PORT &= ~(1 << SCLK_PIN);
    // vprg
    DDR_VPRG |= (1 << VPRG_PIN);
    TLC5940_VPRG_PORT |= (1 << VPRG_PIN);
    // xlat
    DDR_XLAT |= (1 << XLAT_PIN);
    TLC5940_XLAT_PORT &= ~(1 << XLAT_PIN);
    // blank
    DDR_BLANK |= (1 << BLANK_PIN);
    TLC5940_BLANK_PORT |= (1 << BLANK_PIN);
    // serial data master out slave in
    DDR_MOSI |= (1 << MOSI_PIN);
    TLC5940_MOSI_PORT &= ~(1 << MOSI_PIN);
  */

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
  Serial.println("TLC5940 initialization started");
  // set vprg to 1 (program dc data)
  TLC5940_VPRG_PORT |= (1 << TLC5940_VPRG_PIN);
  // set serial data to high (setting dc to 1)
  TLC5940_MOSI_PORT |= (1 << TLC5940_MOSI_PIN);

  Serial.print("Writing dc data for "); Serial.print(96*TLC5940_N); Serial.println("bits");

  // pulse the serial clock 96 times to write in dc data
  for (uint8_t i=0; i<(96 * TLC5940_N); i++) {
    //Serial.print("Writing dc bit: "); Serial.println(i);
    // get the bit the tlc5940 is expecting from the gs array (tlc expects msb first)
    uint8_t data = (dc[((96 * TLC5940_N) - 1 - i)/6]) & (1 << ((96 * TLC5940_N) - 1 - i)%6);
    //Serial.print("dc bit =: "); Serial.println(data);
    // set mosi if bit is high, clear if bit is low
    if (data) {
      TLC5940_MOSI_PORT |= (1 << TLC5940_MOSI_PIN);
    }
    else {
      TLC5940_MOSI_PORT &= ~(1 << TLC5940_MOSI_PIN);
    }
    TLC5940_SCK_PORT |= (1 << TLC5940_SCK_PIN);
    TLC5940_SCK_PORT &= ~(1 << TLC5940_SCK_PIN);
  }

  Serial.println("Latching dc data");
  // pulse xlat to latch the data
  TLC5940_XLAT_PORT |= (1 << TLC5940_XLAT_PIN);
  TLC5940_XLAT_PORT &= ~(1 << TLC5940_XLAT_PIN);
}

// refresh the led display and data
void TLC5940::refreshGS(void) {
    // if vprg is high
    if (TLC5940_VPRG_PORT & (1 << TLC5940_VPRG_PIN)) {
        // set it low
        TLC5940_VPRG_PORT &= ~(1 << TLC5940_VPRG_PIN);
        // set the first cycle flag (just came out of a dc cycle)
        gsFirstCycle = true;
    }

    // enable leds
    TLC5940_BLANK_PORT &= ~(1 << TLC5940_BLANK_PIN);
    // loop through a gs cycle and post-increment gs counter
    for(uint16_t gsCount = 0; gsCount < 4096; gsCount++) {
        // if there's data to clock in
        if (gsCount < (192 * TLC5940_N)) {
            // get the bit the tlc5940 is expecting from the gs array (tlc expects msb first)
            uint16_t data = (gs[((192 * TLC5940_N) - 1 - gsCount)/12]) & (1 << ((192 * TLC5940_N) - 1 - gsCount)%12);
            // set mosi if bit is high, clear if bit is low
            if (data) {
                TLC5940_MOSI_PORT |= (1 << TLC5940_MOSI_PIN);
            }
            else {
                TLC5940_MOSI_PORT &= ~(1 << TLC5940_MOSI_PIN);
            }
            // pulse the serial clock
            TLC5940_SCK_PORT |= (1 << TLC5940_SCK_PIN);
            TLC5940_SCK_PORT &= ~(1 << TLC5940_SCK_PIN);
            // increment data counter
        }
        // pulse the gs clock
        TLC5940_GS_PORT |= (1 << TLC5940_GS_PIN);
        TLC5940_GS_PORT &= ~(1 << TLC5940_GS_PIN);
    }
    // disable leds before latching in new data
    TLC5940_BLANK_PORT |= (1 << TLC5940_BLANK_PIN);
    // pulse xlat to save new gs data
    TLC5940_XLAT_PORT |= (1 << TLC5940_XLAT_PIN);
    TLC5940_XLAT_PORT &= ~(1 << TLC5940_XLAT_PIN);

    // check if this was the first gs cycle after a dc cycle
    if (gsFirstCycle) {
        // pulse serial clock once if it is (because the datasheet tells us to)
        TLC5940_SCK_PORT |= (1 << TLC5940_SCK_PIN);
        TLC5940_SCK_PORT &= ~(1 << TLC5940_SCK_PIN);
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