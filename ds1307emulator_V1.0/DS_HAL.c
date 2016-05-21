#if defined(__XC)
#include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>        /* HiTech General Include File */
#elif defined(__18CXX)
#include <p18cxxx.h>    /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include "DS_HAL.h"
#endif

const uint8_t rateControlMatrix[4][2] = {
	{1, 0},
	{0, 1},
	{0, 1},
	{0, 1},
};
uint8_t __12_not24 = 0, Pm_notAm = 0; //ch not used..!

void rtc_init(){
	/*Inits RTC preparing the output pin with 2^16Hz and enable the RTC*/

	TRISCbits.TRISC0 = 1;
	TRISCbits.TRISC1 = 1;
	TRISBbits.TRISB1 = 0;

	EECON2 = 0x55;
	EECON2 = 0xAA;
	RTCCFGbits.RTCWREN = 1;

	RTCCFGbits.RTCOE = 1;
	RTCCFGbits.RTCEN = 1;
	PADCFG1 = 0b00000101; // 2^16 rtcc pin
	// and Timer1 crystal driver is off, used the RTC external
}


/*These functions named as putXXX are translating the DS specification to the actual MCU implementation, writing to DS mode*/
void putSeconds(unsigned char data){

	rtc_ptr(SEC_ADX);
	T1CONbits.T1OSCEN = ~(data >> EN_BIT_POS);
	RTCVALL = data;

}

void putMinutes(unsigned char data){
	rtc_ptr(MIN_ADX);

	RTCVALH = data;
}

void putHours(unsigned char data){
	rtc_ptr(HOUR_ADX);
	/*The PIC18 has only military format, need a translation eventually*/
	__12_not24 = (uint8_t) (data & BIT12_24_MASK);
	if (__12_not24 == BIT12_24_MASK) { // 12 hours, use am/pm
		RTCVALL = data & HOUR_FIELD_MASK;
		Pm_notAm = (data & BITPM_AM_MASK);
	}
	else { //24 h
		RTCVALL = data & HOURMIL_FIELD_MASK;  //24 hours
	}
}

void putWday(unsigned char data){ //day of week
	rtc_ptr(WDAY_ADX);
	RTCVALH = data;
}

void putMday(unsigned char data){
	rtc_ptr(MDAY_ADX);
	RTCVALL = data; // day of month
}

void putMonth(unsigned char data){
	rtc_ptr(MONTH_ADX);
	RTCVALH = data; // month
}

void putYear(unsigned char data){
	rtc_ptr(YEAR_ADX);
	RTCVALL = data; // year
}



/*Translation related to the control register of the DS*/
void putOutControlRTC(unsigned char data){
	PORTBbits.RB1 = data >> 7;
	RTCCFGbits.RTCOE = data >> 4;
}

void putRateControlRTC(unsigned char data){
	/* "data" index the matrix which translates to the bits needed to the MCU. Useful
		when MCU changes to another similar or the DS changes*/
	PADCFG1bits.RTSECSEL1 = rateControlMatrix[data & RATE_CONTROL_MASK][1];
	PADCFG1bits.RTSECSEL0 = rateControlMatrix[data & RATE_CONTROL_MASK][0];
}



/*These functions named as getXXX are translating the DS specification to the actual MCU implementation, reading from DS mode*/


void getSec(unsigned char * data){
	rtc_ptr(SEC_ADX);
	getRTCLow(data);
	*data &= ~((unsigned char)(T1CONbits.T1OSCEN << EN_BIT_POS)); //active low
}

void getMin(unsigned char *data){
	rtc_ptr(MIN_ADX);
	getRTCHigh(data);
}

void getHour(unsigned char *data){
	rtc_ptr(HOUR_ADX);
	while(RTCCFGbits.RTCSYNC!=0);
	/*The PIC18 has only military format, need a translation eventually by checking the last settings*/
	*data = RTCVALL & HOURMIL_FIELD_MASK;
	if (__12_not24 == BIT12_24_MASK){
		if ((*data & HOUR_FIELD_MASK) >= NOON) {
			Pm_notAm = BITPM_AM_MASK; //am active low
			*data = *data - NOON;
			*data |= (__12_not24 | Pm_notAm);
		}
		else {
			Pm_notAm = 0; //am active low
			*data |= (__12_not24);
			*data &= ~(BITPM_AM_MASK);
		}
	}
}

void getWday(unsigned char *data){
	rtc_ptr(WDAY_ADX);
	getRTCHigh(data);
}
void getMday(unsigned char * data){
	rtc_ptr(MDAY_ADX);
	getRTCLow(data);

}
void getMonth(unsigned char *data){
	rtc_ptr(MONTH_ADX);
	getRTCHigh(data);

}
void getYear(unsigned char *data){
	rtc_ptr(YEAR_ADX);
	getRTCLow(data);
}

/*Functions used to get the value of some high or low register, can be handy add this hiearchy, reduces complexity*/
void getRTCLow(unsigned char *data){
	while(RTCCFGbits.RTCSYNC!=0);
	*data = RTCVALL;
}
void getRTCHigh(unsigned char *data){
	while(RTCCFGbits.RTCSYNC!=0);
	*data = RTCVALH;
}

/*Translation related to the control register of the DS*/

void getOutControlRTC(unsigned char *temp){
	*temp |= (unsigned char)(PORTBbits.RB1 << 7);
	*temp |= (unsigned char)(RTCCFGbits.RTCOE << 4);
}

void getRateControlRTC(unsigned char *temp){
	if (PADCFG1bits.RTSECSEL1 == 0 && PADCFG1bits.RTSECSEL0 == 1)
	*temp |= PINOUT_1HZ;
	else if (PADCFG1bits.RTSECSEL1 == 1 && PADCFG1bits.RTSECSEL0 == 0)
	*temp |= PINOUT_32768HZ;
}


/*Pointer of the RTC, very mcu dependent*/
void rtc_ptr(uint8_t p){
	RTCCFG &= (0b11111100); // ptr 00
	RTCCFG |= p;
}