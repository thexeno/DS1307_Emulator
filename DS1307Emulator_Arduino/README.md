##DS1307 Emulator library V1.21 - Arduino Version - DS1307 emulation

### This library is tested on Arduino Uno and any Arduino version which uses the Atmega328P with 16MHz crystal oscillator.

It is directly forked from the ![RTC Emulator combo-lib](https://github.com/thexeno/DS1307_Emulator/tree/master/DS1307emulator_combo/combo-lib). The versions are usually aligned.

## How to use 

### Arduino environment

Import the DS1307Emulator lib, the required header file shall be written automagically. Then you have some API that let you use the emulator.
It depends on the ["HardWire library"](https://github.com/thexeno/HardWire-Arduino-Library), so make sure it is present in your library database. If not, download it from the previous link and add the library to the Arduino IDE.

Are provided mainly 6 user functions:

-   **DS1307Emulator.init(pin)** -> this call initialize all the system variables and hardware. Must be called once before use any functionality. The **pin** can be any Arduino pin, which will be used as output, like the output of the DS1307 chip.
-   **DS1307Emulator.bufferUserData()** -> prepare the data safely avoiding rollover
-   **DS1307Emulator.writeUserData(unsigned char byte)** -> write one byte in the RTC
-   **unsigned char data = DS1307Emulator.readUserData()** -> return one byte from the last pointed data in the RTC memory
-   **DS1307Emulator.setUserData()** -> need to be called in order to apply RTC the data adjustment
-   **DS1307Emulator.tickIncrementISR()** -> Every HALF SECOND, this call is issued automatically and will increment the internal RTC tick

Follows an example on using it.

Inizialize the emulator calling in the **void setup()** the:

- **DS1307Emulator.init(pin);**

Then the RTC is up and running. Any configuration shall be issued as required from the original DS1307 chip.
Then you can read/write data anywhere in the sketch by following the functions described here below.

**Master controlled**

When connected to a I2C bus, the master shall read and write by following the instructions from the [original Maxim device](https://www.maximintegrated.com/en/products/digital/real-time-clocks/DS1307.html).


###**Reading the time/date inside the Arduino sketch** 

Ofcourse the RTC data can be accessed also inside your own sketch. Basically you need to follow the DS1307 protocol.
Issue the APIs in *this* order:

1.    DS1307Emulator.bufferUserData();
2.    DS1307Emulator.writeUserData(address);
3.    DS1307Emulator.setUserData();
4.    DS1307Emulator.freezeUserData();
5.    var = DS1307Emulator.readUserData();
6.    recall point 5 as many times is needed to read all the RTC bytes
7.    DS1307Emulator.setUserData();

This sequence is triggered automatically when a Master requires to read data.

###**Writing the time/date** 

The data can be modified by following the DS1307 protocol in your sketch. 
Issue the APIs in *this* order:

1.    DS1307Emulator.bufferUserData();
2.    DS1307Emulator.writeUserData(address);
3.    DS1307Emulator.writeUserData(data);
4.    recall point 3 as many times is needed to write all the required RTC bytes
5.    DS1307Emulator.setUserData();

This sequence is triggered automatically when a Master requires to write data.

### I2C bus interactions

If there is no I2C bus used, can be used for other tasks without affecting the timekeeping functionalities.

If the sketch requires to use the bus as a Master, the Wire can be re-initialized accordingly ([see Wire documentation](https://www.arduino.cc/en/Reference/Wire).

If the sketch requires to use the bus as a Master, the Wire can be re-initialized as a slave accordingly ([see Wire documentation](https://www.arduino.cc/en/Reference/Wire) and [HardWire](https://github.com/thexeno/HardWire-Arduino-Library)).
Remember that in the HardWire you have more handlers than the Wire. All of them shall be initialized accordingly when the library is used as a Slave outside the RTC emulator functionality.

When it is needed to assign the I2C bus to the RTC slave, can be used the initialization that will keep the time without re-initializing it:

-   **DS1307Emulator.softInit(pin)** -> attach the I2C/TWI bus to the DS1307Emulator library, without resetting the RTC data



### Facts
The reset state of the RTC Protocol is the same as stated in the DS1307 chip. As a conclusion, for details of when and what write the address and write/read data you can read the official page at [Maxim Integrated](https://www.maximintegrated.com/en/products/digital/real-time-clocks/DS1307.html). 
Be aware that the waveform generator is compliant only when outputting the 1Hz signal. Moreover, there are no battery management included in the library, so you must handle it on your own if you need to switch from power supply to battery and keep the timekeeping data.

Learn how to use the DS1307, and you can use the emulator!


