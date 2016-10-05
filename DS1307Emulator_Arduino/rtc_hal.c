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



/* Requires a timer library and i2c library */

#include "EmuDS1307.h"

enum {RTC_INT_ON, RTC_INT_OFF};

ISR(TIMER1_COMPA_vect)
{

	rtcProtocol_tickIncrementISR();

}

ISR(TWI_vect)
{

	//dataLog[cnt++] =
	i2c_byteHandler_ISR();
	
}


void rtcHal_init(void)
{
	/* For better accuracy, use a timer that allow to use a 32768Hz crystal.
	   If not available, you should use the higher resolution timer with the lower prescaler value */
	/* With Arduno board, 16MHz and prescaler 256 with 16bit Timer1, introduces error of 16ppm over the 16MHz osc ppm error */
	
	rtcHal_setRtcTick();
	rtcHal_setDefaultPin();
}

void rtcHal_setRtcTick(void)
{
	/* source at 32768Hz */
	//initTimer2(64,  NORMAL,  INT_ON, TMR_EXT_CRY);
	//uint16_t comp = ((F_CPU)/256)-1;
	initCompare1(31249, COMP_A_DISCONNECT, 256, TMR_CLK_RISE, CLKOSC, RTC_INT_ON);
}

void rtcHal_setDefaultPin(void)
{
	pinMode(EMU_DS1307_PIN, OUTPUT);
	digitalWrite(EMU_DS1307_PIN, LOW);
}

/* La user chiama questa */
/* Ricorda le API per l'uso interno al programma.. mettile in RTC PROTOCOL */


void rtcHal_resetRtcTick(void)
{
	writeTimer1(0);
}

void rtcHal_startRtcTick(void)
{
//	rtcHal_resetRtcTick(); // riparto allineato
	startTimer1();
}

void rtcHal_stopRtcTick(void)
{
	stopTimer1();
}



void rtcHal_resetPinDigitalMode(void)
{
	pinMode(EMU_DS1307_PIN, INPUT);
}

void rtcHal_setPinDigitalMode(void)
{
	pinMode(EMU_DS1307_PIN, OUTPUT);
}

void rtcHal_setPinDigitalValue(uint8_t value)
{
	if (value)
	{
		digitalWrite(EMU_DS1307_PIN, HIGH);
	}
	else
	{
		digitalWrite(EMU_DS1307_PIN, LOW);
	}
}
