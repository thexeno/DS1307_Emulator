#include <TimerOne.h>
#include "DS1307emulator.h"

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
#define MAX_RTC_TOTAL_ADDR 0x3F

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
	
} _DS1307_GLOBAL_DATA_T;

_DS1307_GLOBAL_DATA_T rtcData;


//HALCB
void (*DS1307emulator::HalCb_setDefaultPin)() = DS1307emulator::halcbDefaultUnused;
void (*DS1307emulator::HalCb_resetPinDigitalMode)() = DS1307emulator::halcbDefaultUnused;
void (*DS1307emulator::HalCb_setPinDigitalMode)() = DS1307emulator::halcbDefaultUnused;


//PRIVATE
void DS1307emulator::rtcProtocol_init(void)
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
	rtcData.dataSync = 0;
	rtcData.halfTick = 0;
	rtcData.notLeap = rtcProtocol_leapCalc(rtcData.year);
	rtcData.noSet = 0;
	rtcData.am_Pm = RTC_PROTOCOL_AM_MODE;
	rtcData.sqwOutPinFreq = RTC_PROTOCOL_SET_32768HZ_MODE;
	rtcData.sqwOutPinMode = RTC_PROTOCOL_OUT_DIGITAL_MODE;
	rtcData.sqwOutPinValue = RTC_PROTOCOL_OUTVAL_LOW;
	rtcData.dataByte = 0; /* first byte rx will be adx */
	rtcData.clockHalt = RTC_PROTOCOL_CH_MODE;
	rtcData.hourFormat = RTC_PROTOCOL_MILITARY_FORMAT_MODE;
	
}

static void DS1307emulator::rtcProtocol_freezeUserData(void)
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
	rtcData.dataSync = 1;
	rtcData.dataByte = 0;
}

static uint8_t DS1307emulator::rtcProtocol_writeToRTC(unsigned char data){
	
	uint8_t noWrite = 1;
	
	if (!rtcData.dataSync)
	{
		return 1;
	}

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

static void DS1307emulator::rtcProtocol_setUserData(void)
{
	if (rtcData.noSet == 1)
	{
		return;
	}
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
	
	rtcData.dataSync = 0;
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

static uint8_t DS1307emulator::rtcProtocol_readUserData(void)
{
	uint8_t data = 0;
	rtcProtocol_getFromRTC(&data);
	return (data);
}

static uint8_t DS1307emulator::rtcProtocol_leapCalc(uint8_t year)
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

static void DS1307emulator::rtcProtocol_putYear(uint8_t data)
{
	rtcDataBuffered.year = (data & RTC_PROTOCOL_YEAR_MASK);
}

static void DS1307emulator::rtcProtocol_putMonth(uint8_t data)
{
	rtcDataBuffered.month = (data & RTC_PROTOCOL_MONTH_MASK);
}
static void DS1307emulator::rtcProtocol_putMday(uint8_t data)
{
	rtcDataBuffered.Mday = (data & RTC_PROTOCOL_MDAY_MASK);
}
static void DS1307emulator::rtcProtocol_putWday(uint8_t data)
{
	rtcDataBuffered.Wday = (data & RTC_PROTOCOL_WDAY_MASK);
}
static void DS1307emulator::rtcProtocol_putHour(uint8_t data)
{
	rtcDataBuffered.hourFormat = (data & RTC_PROTOCOL_HOUR_FORMAT_MASK);
	if (rtcDataBuffered.hourFormat == RTC_PROTOCOL_MILITARY_FORMAT_MODE)
	{
		rtcDataBuffered.hour = (data & RTC_PROTOCOL_HOUR_MIL_MASK);	
	}
	else
	{
		rtcDataBuffered.hour = (data & RTC_PROTOCOL_HOUR_MASK);	
	}
	rtcDataBuffered.am_Pm = (data & RTC_PROTOCOL_AM_PM_MASK);
}
static void DS1307emulator::rtcProtocol_putMinute(uint8_t data)
{
	rtcDataBuffered.minute = (data & RTC_PROTOCOL_MINUTE_MASK);
}
static void DS1307emulator::rtcProtocol_putSecond(uint8_t data)
{
	rtcDataBuffered.second = (data & RTC_PROTOCOL_SECOND_MASK);
	rtcData.clockHalt = data & RTC_PROTOCOL_CH_MASK;
}
static void DS1307emulator::rtcProtocol_putNvRam(uint8_t data, uint8_t w_addr)
{
	/* Not buffered */
	rtcData.NvRam[w_addr] = data;
}
static void DS1307emulator::rtcProtocol_putControlHandler(uint8_t data)
{
	// Control Register from datasheet
	rtcData.sqwOutPinMode = data & RTC_PROTOCOL_SQWE_MASK;//RTC_PROTOCOL_OUT_DIGITAL_MODE, RTC_PROTOCOL_OUT_SQUAREW_MODE
	rtcData.sqwOutPinValue = data & RTC_PROTOCOL_OUT_MASK;//
	rtcData.sqwOutPinFreq = data & RTC_PROTOCOL_RS_MASK; //RTC_PROTOCOL_SET_1HZ_MODE
}

/* Read RTC data from structure */

static void DS1307emulator::rtcProtocol_getControlHandler(uint8_t* val)
{
	uint8_t data = 0;
	data = (rtcData.sqwOutPinMode | rtcData.sqwOutPinFreq | rtcData.sqwOutPinValue);
	*val = data;
}

static void DS1307emulator::rtcProtocol_getYear(uint8_t* data)
{
	*data = (rtcDataBuffered.year & RTC_PROTOCOL_YEAR_MASK);
}

static void DS1307emulator::rtcProtocol_getMonth(uint8_t *data)
{
	*data = (rtcDataBuffered.month & RTC_PROTOCOL_MONTH_MASK);
}
static void DS1307emulator::rtcProtocol_getMday(uint8_t *data)
{
	*data = (rtcDataBuffered.Mday & RTC_PROTOCOL_MDAY_MASK);
}
static void DS1307emulator::rtcProtocol_getWday(uint8_t *data)
{
	*data = (rtcDataBuffered.Wday & RTC_PROTOCOL_WDAY_MASK);
}
static void DS1307emulator::rtcProtocol_getHour(uint8_t *data)
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
static void DS1307emulator::rtcProtocol_getMinute(uint8_t *data)
{
	*data = (rtcDataBuffered.minute & RTC_PROTOCOL_MINUTE_MASK);
}
static void DS1307emulator::rtcProtocol_getSecond(uint8_t *data)
{
	*data = (rtcDataBuffered.second & RTC_PROTOCOL_SECOND_MASK);
    *data |=(rtcData.clockHalt);
}

static void DS1307emulator::rtcProtocol_getNvRam(uint8_t *data, uint8_t w_addr)
{
	*data = rtcData.NvRam[w_addr];
}


uint8_t DS1307emulator::rtcProtocol_getFromRTC(unsigned char *data)
{
	
	if (!rtcData.dataSync)
	{
		return 1;
	}
	
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

	if (rtcData.ptr >= MAX_RTC_TOTAL_ADDR)
	{
		rtcData.ptr = 0;
	}
	else
	{
		rtcData.ptr++;
	}
	return 0;
}

/* API Chiamata da ISR timer */
static void DS1307emulator::rtcProtocol_tickIncrementISR(void)
{
	uint8_t tickIncrement_hourIncr = 0;
	/* Counting made 0-24h, retieved converted upon approrpiate setting (mil or american format) */
	/* Also the BCD format will be converted only upon request in rtcProtocol API*/
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
}


//HAL
static void DS1307emulator::rtcHal_init(void)
{
	/* For better accuracy, use a timer that allow to use a 32768Hz crystal.
	   If not available, you should use the higher resolution timer with the lower prescaler value */
	/* With Arduno board, 16MHz and prescaler 256 with 16bit Timer1, introduces error of 16ppm over the 16MHz osc ppm error */
	
        pinMode(13,OUTPUT);
	digitalWrite(13, HIGH);
	
	rtcHal_setRtcTick(); // metti quello che c'è sopra qui dentro
	rtcHal_setDefaultPin();
	
}

static void DS1307emulator::rtcHal_setRtcTick(void)
{
        Timer1.initialize(500000);
        Timer1.attachInterrupt(rtcHal_RtcTick); 
}

static void DS1307emulator::rtcHal_RtcTick(void)
{
	rtcProtocol_tickIncrementISR();
	digitalWrite(13, digitalRead(13) ^ 1);
}

static void DS1307emulator::rtcHal_setDefaultPin(void)
{
	//setPinMode(PORT_C, 1, PIN_OUTPUT, PULLUP_DISABLED);
	//setPinValue(PORT_C, 1, 0);
}

static void DS1307emulator::rtcHal_resetRtcTick(void)
{
	Timer1.restart();
}

static void DS1307emulator::rtcHal_startRtcTick(void)
{
	//Timer1.start();
	Timer1.resume();
}

static void DS1307emulator::rtcHal_stopRtcTick(void)
{
	Timer1.stop();  
}

static void DS1307emulator::rtcHal_resetPinDigitalMode(void)
{
	//setPinMode(PORT_C, 1, PIN_INPUT, PULLUP_DISABLED);
}

static void DS1307emulator::rtcHal_setPinDigitalMode(void)
{
	//setPinMode(PORT_C, 1, PIN_OUTPUT, PULLUP_DISABLED);
}

static void DS1307emulator::rtcHal_setPinDigitalValue(uint8_t value)
{
	//setPinValue(PORT_C, 1, value);
}

//utility
static unsigned char DS1307emulator::bcdToDec(unsigned char data)
{
	unsigned char temp = 0;
	temp = data >> 4;
	temp = temp*10;
	return (temp + (data & 0x0F));
}

static unsigned char DS1307emulator::decToBcd(unsigned char data)
{
	char temp = 0;
	temp = data/10;
	temp = temp << 4;
	return (temp + (data - bcdToDec(temp)));
}

void DS1307emulator::halcbDefaultUnused()
{
}

///interface
DS1307emulator::DS1307emulator(){

}

void DS1307emulator::start(){
   rtcProtocol_init(); 
}

void DS1307emulator::freezeUserData(){
  rtcProtocol_freezeUserData(); 
}

void DS1307emulator::writeToRTC(uint8_t data){
  rtcProtocol_writeToRTC(data); 
}

void DS1307emulator::setUserData(){
  rtcProtocol_setUserData(); 
}

uint8_t DS1307emulator::readUserData(){
  return rtcProtocol_readUserData(); 
}
