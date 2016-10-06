##DS1307 Emulator library V1.2AD - Arduino Version - DS1307 emulation

### This library is tested on Arduino Uno and any Arduino version which uses the Atmega328P with 16MHz crystal oscillator.

It is directly forked from the ![RTC Emulator combo-lib](https://github.com/thexeno/DS1307_Emulator/tree/master/DS1307emulator_combo/combo-lib). The versions are usually aligned.

## How to use 

### Arduino environment

Import the EmuDS1307 lib, the required header file shall be written automagically. Then you have some API that let you use the emulator.

Are provided mainly 6 user functions:

-   **rtcProtocol_init()** -> this call initialize all the system variables and hardware. Must be called once before use any functionality.
-   **rtcProtocol_freezeUserData()** -> prepare the data safely avoiding rollover
-   **rtcProtocol_writeUserData(unsigned char byte)** -> write one byte in the RTC
-   **unsigned char data = rtcProtocol_readUserData()** -> return one byte to data
-   **rtcProtocol_setUserData()** -> need to be called in order to apply RTC the data adjustment
-   **rtcProtocol_tickIncrementISR()** -> Every HALF SECOND, this call is issued automatically and will increment the internal RTC tick

Follows an example on using it.

Inizialize the emulator calling in the **void setup()** the:

 rtcProtocol_init();

If you need to be used also as a slave (it is not exclusive) from another device (i.e. a Raspberry Pi), then you shall also use the I2C bus, by issuing:

 rtcProtocol_i2cInit();

Then you can read/write data anywhere in the sketch by following the functions described here below.

**Reading the time/date** Read RTC safely by issuing APIs in this order:

1.    rtcProtocol_freezeUserData();
2.    rtcProtocol_writeUserData(address);
3.    rtcProtocol_setUserData();
4.    rtcProtocol_freezeUserData();
5.    var = rtcProtocol_readUserData();
6.    recall point 5 as many times is needed to read all the RTC bytes
7.    rtcProtocol_setUserData();

**Writing the time/date** Write RTC data safely by issuing APIs in this order:

1.    rtcProtocol_freezeUserData();
2.    rtcProtocol_writeUserData(address);
3.    rtcProtocol_writeUserData(data);
4.    recall point 3 as many times is needed to write all the required RTC bytes
5.    rtcProtocol_setUserData();

The reset state of the RTC Protocol is the same as stated in the DS1307 chip. As a conclusion, for details of when and what write the address and write/read data you can read the official page at [Maxim Integrated](https://www.maximintegrated.com/en/products/digital/real-time-clocks/DS1307.html). Learn how to use the DS1307, and you can use the emulator!
