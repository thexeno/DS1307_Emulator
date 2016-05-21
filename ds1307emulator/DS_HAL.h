/* 
* File:   DS_HAL.h
* Author: TheXeno
*
* Created on 21 febbraio 2015, 21.27
*/

#ifndef DS_HAL_H
#define	DS_HAL_H

#ifdef	__cplusplus
extern "C" {
#endif


#define BIT12_24_MASK 0x40
#define BITPM_AM_MASK 0x20
#define HOUR_FIELD_MASK 0x1F
#define HOURMIL_FIELD_MASK 0x3F
#define EN_BIT_POS 0x7
#define RATE_CONTROL_MASK 0x03
#define NOON 0x12
#define PINOUT_1HZ 0
#define PINOUT_32768HZ 0x03
#define SEC_ADX 0x00
#define MIN_ADX 0x00
#define HOUR_ADX 0x01
#define WDAY_ADX 0x01
#define MDAY_ADX 0x02
#define MONTH_ADX 0x02
#define YEAR_ADX 0x03

	void rtc_ptr(uint8_t p);
	void rtc_init();
	void getRateControlRTC(unsigned char *temp);
	void getOutControlRTC(unsigned char *temp);
	void getRTCHigh(unsigned char *data);
	void getRTCLow(unsigned char *data);
	void getYear(unsigned char *data);
	void getMonth(unsigned char *data);
	void getMday(unsigned char * data);
	void getWday(unsigned char *data);
	void getHour(unsigned char *data);
	void getMin(unsigned char *data);
	void getSec(unsigned char * data);
	void putRateControlRTC(unsigned char data);
	void putOutControlRTC(unsigned char data);
	void putYear(unsigned char data);
	void putMonth(unsigned char data);
	void putMday(unsigned char data);
	void putWday(unsigned char data);
	void putHours(unsigned char data);
	void putMinutes(unsigned char data);
	void putSeconds(unsigned char data);
	void rtc_init();


#ifdef	__cplusplus
}
#endif

#endif	/* DS_HAL_H */

