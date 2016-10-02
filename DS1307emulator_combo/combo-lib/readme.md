##RTC Emulator V1.2 - DS1307 emulation

### This library is fully portable on more MCUs thanks to the user defined HAL files available. If needed, open an Issue.

Main changes from the V1.1:

- More efficient RAM usage
- Data handling memory improved of NVRAM
- I2C library upgrade

It is released under MIT licence, see LICENSE file.

This library is absolutely non-blocking with the eventually present user code, that can exist simultaneously. This means that interrupt capabilities must be supported on the used MCU. And all MCUs have this capability.

The host MCU shall contain/support at least:

-  Data Memory: 180 Bytes of RAM
-  Program Memory: 2kB of Flash/EEPROM
-  Oscillator: one external crystal
-  3 GPIO (one RTC output pin, two I2C pins)
-  I2C bus, physical or emulated on 2 GPIO pins
-  1 Timer (better if supports counting in sleep mode)
-  (Suggested) I2C physical
-  (Suggested) External crystal oscillator AND internal RC oscillator
-  (Suggested) 5V tolerant pins
-  (Suggested) More memory space for additional user code

The user firmware can:

-  Read/write data internally with DS1307 protocol with no bus or other components/masters required: this means that the emulation is also inside the MCU hosting the emulator itself.
-  Let another MCU (master) to read/write with DS1307 protocol using I2C bus: this is the real emulation, where the MCU hosting the emulator library is connected on the bus, and is detected as the DS1307 and behaves like it.

## Behavioral known discrepancies and fixing

At this moment the DS1307 emulator can do everything stated in RTC datasheet. Except that is improved, fixed the unpredicted behavior in case of illogical date and time written in DS1307 emulated register: in this library it mismatches the days and date, but nothing more strange.

But there is a discrepancy, it can output on a digital pin a restricted set of output waves at the moment:

-  1Hz tick indicator
-  Digital write high
-  Digital write low

Every other configurations like output 32768Hz, 8192Hz, 4096Hz are resulting in a digital write low. While the low power capability depends on the clock source and the supported functionalities of the selected timer.

## Firmware organization

The firmware was improved written in two main sections, the rtcProtocol and rtcHal files. The protocol handles the entire logical behavior of the emulated device, and is the core of the project. It can be used practically on any MCU. The Hal contains the calls to the device library, therefore the content of functions in this Hal library can vary, depending on the library available for the used MCU, the user preference and so on.

Here the basic blocks:

![alt tag](https://enricosanino.files.wordpress.com/2016/05/rtc_bd.png?w=700)

The *application* (in picture) is the code running on the MCU that could need the access of data. This library is non blocking and concurrent. This means that once you have set up the firmware, you could use it also internally and don't bother of performance reduction due to external requests from masters in the I2C bus. In other words, using this RTC emulator your MCU can do a lot of other things rather than being only an emulated RTC device. The infinte loop in the main is therefore not required to be fullfilled, since everything works in ISR.

The *hardware abstraction layer* is contained in the **rtcHal files**:
-  rtc_hal.c
-  rtc_hal.h

Contains all the calls to the hardware. This is the only part that needs to be modified when changing the MCU adopted, and is this part is confined in the User Defined Library space.

The *RTC application* is the core of the emulator and never changes when changing the MCU. Is contained in the **rtcProtocol files**:
-  rtc_protocol.c
-  rtc_protocol.h

If RTC data is needed from the application rather than from bus requests,  can be used be means of the API Data. Otherwise, serves the I2C master requests, as the DS1307 original chip does. Accessing through the API Data, if before ending a read/write cycle a bus request arise, will be returned no valid data because is temporary reserved to the application.

## API interfaces
Library inclusion by one header file: **rtc_protocol.h**

**rtcProtocol API**.  Are provided mainly 6 user functions:

-   **rtcProtocol_init()** -> this call initialize all the system variables and hardware. Must be called once before use any functionality.
-   **rtcProtocol_freezeUserData()** -> prepare the data safely avoiding rollover
-   **rtcProtocol_writeUserData(unsigned char byte)** -> write one byte in the RTC
-   **unsigned char data = rtcProtocol_readUserData()** -> return one byte to data
-   **rtcProtocol_setUserData()** -> need to be called in order to apply RTC the data adjustment
-   **rtcProtocol_tickIncrementISR()** -> Every HALF SECOND, this call will increment the internal RTC tick

Follows an example on using it.
Read RTC data safely by issuing APIs in this order:

1.    rtcProtocol_freezeUserData();
2.    rtcProtocol_writeUserData(address);
3.    rtcProtocol_setUserData();
4.    rtcProtocol_freezeUserData();
5.    var = rtcProtocol_readUserData();
6.    recall point 5 as many times is needed to read all the RTC bytes
7.    rtcProtocol_setUserData();

Write RTC data safely by issuing APIs in this order:

1.    rtcProtocol_freezeUserData();
2.    rtcProtocol_writeUserData(address);
3.    rtcProtocol_writeUserData(data);
4.    recall point 4 as many times is needed to write all the required RTC bytes
5.    rtcProtocol_setUserData();

While in an independent way the **rtcProtocol_tickIncrementISR()** shall be called every 500ms.

For details of when and what write the address and write/read data you can read the official page at [Maxim Integrated](https://www.maximintegrated.com/en/products/digital/real-time-clocks/DS1307.html). Remember that the DS1307 initial conditions are also those one in the emulator library. Learn how to use the DS1307, and you can use the emulator!
## HAL and User Defined Library

Are provided few APIs that can be fullfilled with user defined code, it is the User Defined Library (UDL) that implements the HAL.

These functions must be fullfilled with the required drivers in order to drive the GPIO, timer and bus. The bus will work on interrupt in order to not block the RTC tick. It is not needed to know exactly when and how their are called, but just know what functionality put in what API. And this is written below:

-    **rtcHal_setPinDigitalMode(void)** -> initialize the used defined pin as a digital output pin. Like the pinMode(OUTPUT) in Arduino. This is not a mandatory if a digital output is not needed.
-    **rtcHal_resetPinDigitalMode(void)** -> disable the pullup/pulldown capability of the pin and set it as input. Put something like pinMode(INPUT) in Arduino. This is not a mandatory if a digital output is not needed.
-    **rtcHal_setPinDigitalValue(uint8_t value)** -> write on a user defined pin the digital value. Like the digitalWrite() in Arduino. This is not a mandatory if a digital output is not needed. This can be used only if the previous ones are fullfilled.
-    **rtcHal_stopRtcTick(void)** -> here must be present code that stops the timer. This is mandatory.
-    **rtcHal_startRtcTick(void)** -> here must be present code that starts the timer. This is mandatory.

Remember that with the UDL, some other headers might be needed for the user code delcaration, if external functions are used.
