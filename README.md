# TLC5940
AVR library for the TI TLC5940 16 led driver. current code based on [TI's TLC5940 Programming Flowchart](http://www.ti.com/lit/sw/slvc106/slvc106.pdf).

## usage
* avr - include the files and update your makefile accordingly
* arduino - close the Arduino IDE, clone this repo into ~/Arduino/libraries, and reopen Arduino

## todo
* modify constructor to take in pin assignments for better cross-project usability
* refactor code to use hardware spi
