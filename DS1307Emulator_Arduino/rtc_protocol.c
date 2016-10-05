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



#include "EmuDS1307.h"


/*DS RTC defines*/
#define SEC 0x00
#define MIN 0x01
#define HOUR 0x02
#define WDAY 0x03
#define MDAY 0x04
#define MONTH 0x05
#define YEAR 0x06
#define CONTROL_ADDRESS 0x07
#define NVRAM_BEGIN_ADDR 0x08

#define RTC_PROTOCOL_BUFFER_SIZE 7

#define MAX_RTC_ONLY_ADDRESS CONTROL_ADDRESS
#define MAX_RTC_TOTAL_ADDR 0x40

/* Digital/swq selection */
#define RTC_PROTOCOL_SQWE_MASK 0x10
#define RTC_PROTOCOL_OUT_DIGITAL_MODE 0x00
#define RTC_PROTOCOL_OUT_SQUAREW_MODE 0x10

/* Prescaler selection */
#define RTC_PROTOCOL_RS_MASK 0x03
#define RTC_PROTOCOL_SET_1HZ_MODE 0x00
#define RTC_PROTOCOL_SET_32768HZ_MODE 0x03

/* digital pin value */
#define RTC_PROTOCOL_OUT_MASK 0x80
#define RTC_PROTOCOL_OUTVAL_LOW 0
#define RTC_PROTOCOL_OUTVAL_HIGH 1

/* CH */
#define RTC_PROTOCOL_CH_MASK 0x80
#define RTC_PROTOCOL_CH_MODE 0x80
#define RTC_PROTOCOL_NCH_MODE 0x00

/* hour */
#define RTC_PROTOCOL_HOUR_FORMAT_MASK 0x40
#define RTC_PROTOCOL_MILITARY_FORMAT_MODE 0x00
#define RTC_PROTOCOL_NO_MILITARY_FORMAT_MODE 0x40

#define RTC_PROTOCOL_HOUR_MASK 0x1F
#define RTC_PROTOCOL_HOUR_MIL_MASK 0x3F

#define RTC_PROTOCOL_AM_PM_MASK 0x20
#define RTC_PROTOCOL_AM_MODE 0x00
#define RTC_PROTOCOL_PM_MODE 0x20

/* second */
#define RTC_PROTOCOL_SECOND_MASK 0x7F

/* minute */
#define RTC_PROTOCOL_MINUTE_MASK 0x7F

/* wday */
#define RTC_PROTOCOL_WDAY_MASK 0x07

/* mday */
#define RTC_PROTOCOL_MDAY_MASK 0x3F

/* month */
#define RTC_PROTOCOL_MONTH_MASK 0x1F

/* year */
#define RTC_PROTOCOL_YEAR_MASK 0xFF



/*Private */
struct userRtcBuffer {
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	uint8_t Wday;
	uint8_t Mday;
	uint8_t month;
	uint8_t year;
	uint8_t am_Pm;
	uint8_t hourFormat;
} rtcDataBuffered;

_DS1307_GLOBAL_DATA_T rtcData;

static const uint8_t nvRamReset[] = {
	0x09, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x04
};

static uint8_t rtcProtocol_getFromRTC(unsigned char *data);

	
static unsigned char decToBcd(unsigned char data);
static unsigned char bcdToDec(unsigned char data);
static uint8_t rtcProtocol_leapCalc(uint8_t year);

static void rtcProtocol_putYear(uint8_t data);
static void rtcProtocol_putMonth(uint8_t data);
static void rtcProtocol_putMday(uint8_t data);
static void rtcProtocol_putWday(uint8_t data);
static void rtcProtocol_putHour(uint8_t data);
static void rtcProtocol_putMinute(uint8_t data);
static void rtcProtocol_putSecond(uint8_t data);
static void rtcProtocol_putNvRam(uint8_t data, uint8_t w_addr);
static void rtcProtocol_putControlHandler(uint8_t data);

static void rtcProtocol_getControlHandler(uint8_t* val);
static void rtcProtocol_getYear(uint8_t* data);
static void rtcProtocol_getMonth(uint8_t *data);
static void rtcProtocol_getMday(uint8_t *data);
static void rtcProtocol_getWday(uint8_t *data);
static void rtcProtocol_getHour(uint8_t *data);
static void rtcProtocol_getMinute(uint8_t *data);
static void rtcProtocol_getSecond(uint8_t *data);
static void rtcProtocol_getNvRam(uint8_t *data, uint8_t w_addr);

/*Private functions */

static unsigned char bcdToDec(unsigned char data)
{
	unsigned char temp = 0;
	temp = data >> 4;
	temp = temp*10;
	return (temp + (data & 0x0F));
}

static unsigned char decToBcd(unsigned char data)
{
	char temp = 0;
	temp = data/10;
	temp = temp << 4;
	return (temp + (data - bcdToDec(temp)));
}

static uint8_t rtcProtocol_leapCalc(uint8_t year)
{
	uint8_t ret = 1;
	if ((year % 100) == 0)
	{
		if ((year % 400) == 0)
		{
			ret = 0;
		}
	}
	else if ((year % 4) == 0)
	{
		ret = 0;
	}
	return ret;
}

static void rtcProtocol_putYear(uint8_t data)
{
	uint8_t temp = 0;
	temp = bcdToDec(data);
	if (temp > 99)
	{
		rtcDataBuffered.year = (0x99 & RTC_PROTOCOL_YEAR_MASK);
	}
	else
	{
		rtcDataBuffered.year = (data & RTC_PROTOCOL_YEAR_MASK);	
	}
	
}

static void rtcProtocol_putMonth(uint8_t data)
{
	uint8_t temp = 0;
	temp = bcdToDec(data);
	if (temp > 12 || temp == 0)
	{
		rtcDataBuffered.month = (0x01 & RTC_PROTOCOL_MONTH_MASK);
	}
	else
	{
		rtcDataBuffered.month = (data & RTC_PROTOCOL_MONTH_MASK);	
	}
}

static void rtcProtocol_putMday(uint8_t data)
{
	uint8_t temp = 0;
	temp = bcdToDec(data);
	if (temp > 31 || temp == 0)
	{
		rtcDataBuffered.Mday = (0x01 & RTC_PROTOCOL_MDAY_MASK);
	}
	else
	{
		rtcDataBuffered.Mday = (data & RTC_PROTOCOL_MDAY_MASK);
	}
}

static void rtcProtocol_putWday(uint8_t data)
{
	uint8_t temp = 0;
	temp = bcdToDec(data);
	if (temp > 7 || temp == 0)
	{
		rtcDataBuffered.Wday = (0x01 & RTC_PROTOCOL_WDAY_MASK);
	}
	else
	{
		rtcDataBuffered.Wday = (data & RTC_PROTOCOL_WDAY_MASK);
	}
}

static void rtcProtocol_putHour(uint8_t data)
{
	uint8_t temp = 0;
	
	rtcDataBuffered.hourFormat = (data & RTC_PROTOCOL_HOUR_FORMAT_MASK);
	if (rtcDataBuffered.hourFormat == RTC_PROTOCOL_MILITARY_FORMAT_MODE)
	{
		rtcDataBuffered.hour = (data & RTC_PROTOCOL_HOUR_MIL_MASK);	
		temp = bcdToDec(rtcDataBuffered.hour);
		if (temp > 23)
		{
			rtcDataBuffered.hour = (/*0x00*/ 0x01 & RTC_PROTOCOL_HOUR_MIL_MASK);	
		}
	}
	else
	{	
		rtcDataBuffered.hour = (data & RTC_PROTOCOL_HOUR_MASK);	
		temp = bcdToDec(rtcDataBuffered.hour);
		if (temp > 12 || temp == 0)
		{
			rtcDataBuffered.hour = (0x01 & RTC_PROTOCOL_HOUR_MASK);	
		}
	}
	rtcDataBuffered.am_Pm = (data & RTC_PROTOCOL_AM_PM_MASK);
}

static void rtcProtocol_putMinute(uint8_t data)
{
	uint8_t temp = 0;
	temp = bcdToDec(data);
	if (temp > 59)
	{
		rtcDataBuffered.minute = (0x00 & RTC_PROTOCOL_MINUTE_MASK);
	}
	else
	{
		rtcDataBuffered.minute = (data & RTC_PROTOCOL_MINUTE_MASK);
	}
}

static void rtcProtocol_putSecond(uint8_t data)
{
	int8_t temp = 0;
	rtcDataBuffered.second = (data & RTC_PROTOCOL_SECOND_MASK);
	rtcData.clockHalt = data & RTC_PROTOCOL_CH_MASK;
	temp = bcdToDec(rtcDataBuffered.second);
	if (temp > 59)
	{
		rtcDataBuffered.second = (0x00 & RTC_PROTOCOL_SECOND_MASK);
	}
	
}
static void rtcProtocol_putNvRam(uint8_t data, uint8_t w_addr)
{
	/* Not buffered */
	rtcData.NvRam[w_addr-8] = data;
}
static void rtcProtocol_putControlHandler(uint8_t data)
{
	// Control Register from datasheet
	rtcData.sqwOutPinMode = data & RTC_PROTOCOL_SQWE_MASK;//RTC_PROTOCOL_OUT_DIGITAL_MODE, RTC_PROTOCOL_OUT_SQUAREW_MODE
	rtcData.sqwOutPinValue = data & RTC_PROTOCOL_OUT_MASK;//
	rtcData.sqwOutPinFreq = data & RTC_PROTOCOL_RS_MASK; //RTC_PROTOCOL_SET_1HZ_MODE
}


/* Read RTC data from structure */

static void rtcProtocol_getControlHandler(uint8_t* val)
{
	uint8_t data = 0;
	data = (rtcData.sqwOutPinMode | rtcData.sqwOutPinFreq | rtcData.sqwOutPinValue);
	*val = data;
}

static void rtcProtocol_getYear(uint8_t* data)
{
	*data = (rtcDataBuffered.year & RTC_PROTOCOL_YEAR_MASK);
}

static void rtcProtocol_getMonth(uint8_t *data)
{
	*data = (rtcDataBuffered.month & RTC_PROTOCOL_MONTH_MASK);
}
static void rtcProtocol_getMday(uint8_t *data)
{
	*data = (rtcDataBuffered.Mday & RTC_PROTOCOL_MDAY_MASK);
}
static void rtcProtocol_getWday(uint8_t *data)
{
	*data = (rtcDataBuffered.Wday & RTC_PROTOCOL_WDAY_MASK);
}
static void rtcProtocol_getHour(uint8_t *data)
{
	if (rtcDataBuffered.hourFormat == RTC_PROTOCOL_MILITARY_FORMAT_MODE)
	{
		*data = (rtcDataBuffered.hour & RTC_PROTOCOL_HOUR_MIL_MASK);
	}
	else
	{
		*data = (rtcDataBuffered.hour & RTC_PROTOCOL_HOUR_MASK);	
		*data |=  rtcDataBuffered.am_Pm;
	}
	*data |=  rtcDataBuffered.hourFormat;
}
static void rtcProtocol_getMinute(uint8_t *data)
{
	*data = (rtcDataBuffered.minute & RTC_PROTOCOL_MINUTE_MASK);
}
static void rtcProtocol_getSecond(uint8_t *data)
{
	*data = (rtcDataBuffered.second & RTC_PROTOCOL_SECOND_MASK);
    *data |=(rtcData.clockHalt);
}

static void rtcProtocol_getNvRam(uint8_t *data, uint8_t w_addr)
{
	*data = rtcData.NvRam[w_addr-8];
}


/* Public functions */

void rtcProtocol_setUserData(void)
{
			
	if (rtcData.noSet == 1)
	{
		/* do nothing */
	}
	else
	{
		/* always work in decimal */
		rtcHal_stopRtcTick();

		rtcData.second = bcdToDec(rtcDataBuffered.second);
		rtcData.minute = bcdToDec(rtcDataBuffered.minute);
		rtcData.hour = 	 bcdToDec(rtcDataBuffered.hour);
		rtcData.Wday = 	 bcdToDec(rtcDataBuffered.Wday);
		rtcData.Mday = 	 bcdToDec(rtcDataBuffered.Mday);
		rtcData.month =	 bcdToDec(rtcDataBuffered.month);
		rtcData.year = 	 bcdToDec(rtcDataBuffered.year);

		rtcData.am_Pm = rtcDataBuffered.am_Pm;
	
		rtcData.notLeap = rtcProtocol_leapCalc(rtcData.year);
		/* just to be in order, since there are no mask values for leap */
		if (rtcData.notLeap && rtcData.month == 2 && rtcData.Mday == 29)
		{
			rtcData.Mday = 28;
		}
	
		//rtcData.dataSync = 0;
		rtcData.halfTick = 0;
		/* Set other settings */
		/* Update out pin RTC */
		if (rtcData.sqwOutPinMode == RTC_PROTOCOL_OUT_DIGITAL_MODE)
		{
			rtcHal_setPinDigitalMode();
			rtcHal_setPinDigitalValue(!!rtcData.sqwOutPinValue);
		}
		else if (rtcData.sqwOutPinMode == RTC_PROTOCOL_OUT_SQUAREW_MODE)
		{
			/* Ignores the digital data */
			if (rtcData.sqwOutPinFreq == RTC_PROTOCOL_SET_1HZ_MODE)	
			{
				rtcHal_setPinDigitalMode();
				/* Digital changed in ISR */
			}
			else
			{
				rtcHal_resetPinDigitalMode();
				/* And use fast waveform */	
			}
		}
		else
		{
			rtcHal_setPinDigitalMode();	
			rtcHal_setPinDigitalValue(0);
		}

		if (rtcData.clockHalt == RTC_PROTOCOL_CH_MODE)
		{
			rtcHal_stopRtcTick(); 
		}
		else if (rtcData.clockHalt == RTC_PROTOCOL_NCH_MODE)
		{
			rtcHal_startRtcTick(); 	
		}
	}

}

uint8_t rtcProtocol_readUserData(void)
{
	uint8_t data = 0;
	rtcProtocol_getFromRTC(&data);
	return (data);
}

void rtcProtocol_init(void)
{
	rtcHal_init();
	rtcData.second = 0;
	rtcData.minute = 0;
	rtcData.hour = 0;
	rtcData.Wday = 1;
	rtcData.Mday = 1;
	rtcData.month = 1;
	rtcData.year = 0;
	rtcData.ptr = 0;
	rtcData.halfTick = 0;
	memcpy(rtcData.NvRam, nvRamReset, sizeof(rtcData.NvRam));
	rtcData.notLeap = rtcProtocol_leapCalc(rtcData.year);
	rtcData.noSet = 1;
	rtcData.am_Pm = RTC_PROTOCOL_AM_MODE;
	rtcData.sqwOutPinFreq = RTC_PROTOCOL_SET_1HZ_MODE;
	rtcData.sqwOutPinMode = RTC_PROTOCOL_OUT_DIGITAL_MODE;
	rtcData.sqwOutPinValue = RTC_PROTOCOL_OUTVAL_LOW;
	rtcData.dataByte = 0; /* first byte rx will be adx */
	rtcData.clockHalt = RTC_PROTOCOL_CH_MODE;
	rtcData.hourFormat = RTC_PROTOCOL_MILITARY_FORMAT_MODE;
	rtcData.isrSync = 1;
	rtcProtocol_freezeUserData();
	rtcProtocol_setUserData();

}

void rtcProtocol_i2cInit(void)
{
	i2c_init(RTC_PROTOCOL_DEVICE_ADDRESS, I2C_SLAVE_INIT_MODE);
}

/* API Chiamata da ISR timer */
void rtcProtocol_tickIncrementISR(void)
{
	uint8_t tickIncrement_hourIncr = 0;
	/* Counting made 0-24h, retieved converted upon approrpiate setting (mil or american format) */
	/* Also the BCD format will be converted only upon request in rtcProtocol API*/
	
	
	//sei();
	rtcData.isrSync = 0;
	
	if (rtcData.halfTick)
	{
		rtcData.halfTick = 0;	
	}
	else
	{
		rtcData.halfTick = 1;
	}

	if (!rtcData.halfTick)
	{
		/* Primary tick -> execute handler */
		if ((++rtcData.second)==60)
		{
			rtcProtocol_freezeUserData();
			rtcData.second = 0;
			if ((++rtcData.minute) == 60)
			{
				rtcData.minute = 0;
				if (rtcData.hourFormat == RTC_PROTOCOL_MILITARY_FORMAT_MODE)
				{
					/* 0-24 */
					if ((++rtcData.hour) == 24)
					{
						rtcData.hour = 0;
						tickIncrement_hourIncr = 1;
					}
					else
					{
						tickIncrement_hourIncr = 0;
					}
				}
				else
				{
					if ((++rtcData.hour) > 12)	
					{
						rtcData.hour = rtcData.hour - 12; /* back to 1 */
						if (rtcData.am_Pm == RTC_PROTOCOL_AM_MODE)
						{
							rtcData.am_Pm = RTC_PROTOCOL_PM_MODE;		
							tickIncrement_hourIncr = 0;
						}
						else
						{
							rtcData.am_Pm = RTC_PROTOCOL_AM_MODE;
							tickIncrement_hourIncr = 1;			
						}
					}
				}
				if (tickIncrement_hourIncr == 1)
				{
					rtcData.Mday++;
					if (++rtcData.Wday == 8)
					{
						rtcData.Wday = 1;
					}
					
					if ((rtcData.Mday == 32) ||
					   ((rtcData.Mday == 31) && 
						((rtcData.month == 4) || (rtcData.month == 6) ||
						 (rtcData.month == 9) || (rtcData.month == 11))) ||
					   	 ((rtcData.month == 2) && (rtcData.Mday == 29) && rtcData.notLeap) ||
					   	 ((rtcData.month == 2) && (rtcData.Mday == 30)))
					{
							rtcData.Mday = 1;
							if ((++rtcData.month)==13)
							{
								rtcData.month = 1;
								rtcData.year++;
								rtcData.notLeap = rtcProtocol_leapCalc(rtcData.year);
							}
					}
				}
			}
		}
	}
	if (rtcData.sqwOutPinMode == RTC_PROTOCOL_OUT_SQUAREW_MODE &&
		rtcData.sqwOutPinFreq == RTC_PROTOCOL_SET_1HZ_MODE)
	{
		if (rtcData.halfTick)
		{
			rtcHal_setPinDigitalValue(1);
		}
		else
		{
			rtcHal_setPinDigitalValue(0);
		}
	}
	rtcData.isrSync = 1;
}


uint8_t rtcProtocol_getFromRTC(unsigned char *data)
{
	
	
	switch (rtcData.ptr){
		case SEC: //seconds
		rtcProtocol_getSecond(data);
		break;

		case MIN:
		rtcProtocol_getMinute(data);
		break;
		case HOUR: // hours
		rtcProtocol_getHour(data);
		break;

		case WDAY:  // day of week
		rtcProtocol_getWday(data);
		break;

		case MDAY: // day of month
		rtcProtocol_getMday(data);
		break;
		case MONTH : // month
		rtcProtocol_getMonth(data);
		break;
		case YEAR: // year yy
		rtcProtocol_getYear(data);
		break;
		case CONTROL_ADDRESS:
		rtcProtocol_getControlHandler(data);
		break;
		
		default:
		if (rtcData.ptr >= NVRAM_BEGIN_ADDR && rtcData.ptr < MAX_RTC_TOTAL_ADDR)
		{
			rtcProtocol_getNvRam(data, rtcData.ptr);
		}
		break;
	}

	if (rtcData.ptr >= MAX_RTC_TOTAL_ADDR-1)
	{
		rtcData.ptr = 0;
	}
	else
	{
		rtcData.ptr++;
	}
	return 0;
}	

void rtcProtocol_freezeUserData(void)
{
	/* Save all time dependant data, as stated in DS1307 */
	rtcDataBuffered.second = decToBcd(rtcData.second);
	rtcDataBuffered.minute = decToBcd(rtcData.minute);
	rtcDataBuffered.hour = decToBcd(rtcData.hour);
	rtcDataBuffered.Wday = decToBcd(rtcData.Wday);
	rtcDataBuffered.Mday = decToBcd(rtcData.Mday);
	rtcDataBuffered.month = decToBcd(rtcData.month);
	rtcDataBuffered.year = decToBcd(rtcData.year);
	rtcDataBuffered.am_Pm = rtcData.am_Pm;
	rtcData.dataByte = 0;
}

uint8_t rtcProtocol_writeToRTC(unsigned char data){
	
	uint8_t noWrite = 1;
	
	if (rtcData.dataByte == 0)
	{
		rtcData.ptr = data;
	}
	else
	{
		switch (rtcData.ptr){
		case SEC: //seconds
			rtcProtocol_putSecond(data);
			break;

		case MIN:
			rtcProtocol_putMinute(data);
			break;
		case HOUR: // hours
			rtcProtocol_putHour(data);
			break;

		case WDAY:  // day of week
			rtcProtocol_putWday(data);
			break;

		case MDAY: // day of month
			rtcProtocol_putMday(data);
			break;
		case MONTH : // month
			rtcProtocol_putMonth(data);
			break;
		case YEAR: // year yy
			rtcProtocol_putYear(data);
			break;
		case CONTROL_ADDRESS:
			rtcProtocol_putControlHandler(data);
			break;
		
		default:
			if (rtcData.ptr >= NVRAM_BEGIN_ADDR && rtcData.ptr < MAX_RTC_TOTAL_ADDR)
			{
				rtcProtocol_putNvRam(data, rtcData.ptr);
			}
			break;
		}
		noWrite = 0;
	}

	if (rtcData.ptr >= MAX_RTC_TOTAL_ADDR)
	{
		rtcData.ptr = 0;
	}
	else
	{
		if (rtcData.dataByte == 1)
			rtcData.ptr++;
		else
			rtcData.dataByte = 1;
	}

	if (noWrite == 1)
	{
		rtcData.noSet = 1;
	}
	else
	{
		rtcData.noSet = 0;
	}

	return 0;
}


