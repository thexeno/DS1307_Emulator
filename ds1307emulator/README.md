#DS1307 emulator - V 1.0

This code emulates the commercial DS1307 chip. Is it not fully compatible, for more info and usage see the page https://enricosanino.wordpress.com/2015/03/06/ds1307-real-time-clock-hacking/ where are described the various connections.
It is released under MIT licence, inside LICENSE file in https://github.com/thexeno/Firmwares

#Compilation

This code is developed for the XC8 v1.20 official Microchip compiler. It is advised to compile using MPLAB-X IDE for a full compatiblity.
The current code is tested for the PIC18F47J53 microcontroller, but can be extended with almost no effort on other PIC18FxxJ53 or, with a better analisys, on other families.

This code uses the I2C libraries on https://github.com/thexeno/Firmwares/tree/master/libraries/i2c and embed the DS1307 libraries, on files named DS_xxxx files.
Normally, if device changes, only DS_HAL.x files should need to change, provided that proper libraries like the I2C are adapted.

____

