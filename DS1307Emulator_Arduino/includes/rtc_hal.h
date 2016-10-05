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

#include <avr/io.h>

#ifndef DS_APPLICATION_H
#define	DS_APPLICATION_H

#ifdef	__cplusplus
extern "C" {
#endif

void rtcHal_setUserData(void);
uint8_t rtcHal_readUserData(void);
void rtcHal_writeUserData(uint8_t data);
void rtcHal_freezeUserData(void);
void rtcHal_setPinDigitalValue(uint8_t value);
void rtcHal_setPinDigitalMode(void);
void rtcHal_resetPinDigitalMode(void);
void rtcHal_stopRtcTick(void);
void rtcHal_startRtcTick(void);
void rtcHal_resetRtcTick(void);
void rtcHal_setDefaultPin(void);
void rtcHal_setRtcTick(void);
void rtcHal_init(void);

#define EMU_DS1307_PIN 13

#ifdef	__cplusplus
}
#endif

#endif	/* DS_APPLICATION_H */

