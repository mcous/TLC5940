# TLC5940
AVR library for the TI TLC5940 16 led driver. uses an output compare toggle for the greyscale clock and bit banging for the serial communication. works ok up to about three drivers. doesn't work well on a 8 MHz chip.

## usage
* avr - include the files and update your makefile accordingly
* arduino - close the Arduino IDE, clone this repo into ~/Arduino/libraries, and reopen Arduino
* uses two timers that need to be set up properly in the application - see the example sketch

## todo
* modify constructor to take in pin assignments for better cross-project usability
* refactor code to use hardware spi
