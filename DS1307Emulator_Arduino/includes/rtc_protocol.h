//The MIT License (MIT)
//
//Copyright (c) 2016 Enrico Sanino
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.




#ifndef RTC_PROTOCOL_H_
#define RTC_PROTOCOL_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "rtc_hal.h"



#define RTC_PROTOCOL_DEVICE_ADDRESS 0xD0 /* of DS1397 protocol */


/* Public for application access */
typedef struct {
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	uint8_t Wday;
	uint8_t Mday;
	uint8_t month;
	uint8_t year;
	uint8_t NvRam[56];
	uint8_t ptr;

	uint8_t clockHaltReq;

	uint8_t dataSync;
	
	uint8_t busState;

	uint8_t halfTick;
	uint8_t notLeap;
	
	uint8_t am_Pm;
	uint8_t sqwOutPinFreq;
	uint8_t sqwOutPinMode;
	uint8_t sqwOutPinValue;
	uint8_t dataByte;
	uint8_t clockHalt;
	uint8_t hourFormat;
	
	uint8_t noSet;
	
	uint8_t isrSync;
	
} _DS1307_GLOBAL_DATA_T;



void rtcProtocol_tickIncrementISR(void);
uint8_t rtcProtocol_readUserData(void);
uint8_t rtcProtocol_writeToRTC(unsigned char data);
void rtcProtocol_freezeUserData(void);
void rtcProtocol_setUserData(void);
void rtcProtocol_init(void);
void rtcProtocol_i2cInit(void);

#ifdef	__cplusplus
}
#endif

#endif /* RTC_PROTOCOL_H_ */
