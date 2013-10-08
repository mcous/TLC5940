 // low level tlc5940 class for atmega microcontrollers

#include "TLC5940.h"
//#include "arduino.h"

// DDR from PORT macro
#define DDR(port) (*(&port-1))

// give the variables some default values
TLC5940::TLC5940(void) {
  // initialize variables at all leds off for safety and dot correction to full brightness
  for (uint8_t i=0; i<(16 * TLC5940_N); i++) {
    setDC(i, 63);
  }
  for (uint8_t i=0; i<(16 * TLC5940_N); i++) {
    setGS(i, 0);
  }
  //gsFirstCycle = false;
  //needLatch = false;
  newData = false;
  //gsCount = 0;
  //dataCount = 0;
}

// initialize the pins and set dot correction
void TLC5940::init(void) {
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

  // set vprg to 1 (program dc data)
  TLC5940_VPRG_PORT |= (1 << TLC5940_VPRG_PIN);
  // set serial data to high (setting dc to 1)
  TLC5940_MOSI_PORT |= (1 << TLC5940_MOSI_PIN);

  // pulse the serial clock (96 * number-of-drivers) times to write in dc data
  for (uint8_t i=0; i<(96 * TLC5940_N); i++) {
    // get the bit the tlc5940 is expecting from the gs array (tlc expects msb first)
    uint8_t data = (dc[((96 * TLC5940_N) - 1 - i)/6]) & (1 << ((96 * TLC5940_N) - 1 - i)%6);
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

  // pulse xlat to latch the data
  TLC5940_XLAT_PORT |= (1 << TLC5940_XLAT_PIN);
  TLC5940_XLAT_PORT &= ~(1 << TLC5940_XLAT_PIN);

  // enable leds
  TLC5940_BLANK_PORT &= ~(1 << TLC5940_BLANK_PIN);
}

// refresh the led display and data
void TLC5940::refreshGS(void) {
  bool gsFirstCycle = false;
  static bool needLatch = false;

  // disable leds before latching in new data
  TLC5940_BLANK_PORT |= (1 << TLC5940_BLANK_PIN);

  // check if vprg is still high
  if ( TLC5940_VPRG_PORT & (1 << TLC5940_VPRG_PIN) ) {
    // pull VPRG low and set the first cycle flag
    TLC5940_VPRG_PORT &= ~(1 << TLC5940_VPRG_PIN);
    gsFirstCycle = true;
  }

  // check if we need a latch
  if (needLatch) {
    needLatch = false;
    TLC5940_XLAT_PORT |= (1 << TLC5940_XLAT_PIN);
    TLC5940_XLAT_PORT &= ~(1 << TLC5940_XLAT_PIN);
  }

  // check if this was the first gs cycle after a dc cycle
  if (gsFirstCycle) {
    gsFirstCycle = false;
    // pulse serial clock once if it is (because the datasheet tells us to)
    TLC5940_SCK_PORT |= (1 << TLC5940_SCK_PIN);
    TLC5940_SCK_PORT &= ~(1 << TLC5940_SCK_PIN);
  }

  // enable leds
  TLC5940_BLANK_PORT &= ~(1 << TLC5940_BLANK_PIN);
  
  // clock in new gs data
  needLatch = serialCycle();
}


bool TLC5940::serialCycle(void) {
  // if there's data to clock in
  if (newData) {
    for (uint16_t dataCount=0; dataCount<192*TLC5940_N; dataCount++) {
      // get the bit the tlc5940 is expecting from the gs array (tlc expects msb first)
      uint16_t data = (gs[((192 * TLC5940_N) - 1 - dataCount)/12]) & (1 << ((192 * TLC5940_N) - 1 - dataCount)%12);
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
    }
    return true;
  }
  return false;
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
    newData = true;
  }
}