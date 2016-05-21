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

#endif

#include "i2c.h"
#include "DS_adapter.h"
#include "DS_HAL.h"

unsigned char timetable[] = {0, 0, 0, 0, 0, 0, 0, 0};
uint8_t rxCount;
uint8_t buff_ovf = 0, bus = 0;
uint16_t buff_len = 0;
uint8_t RxBuffer;
uint8_t temp;
uint8_t rx_begin = 0;
uint8_t rx_complete = 0;
uint8_t w_addr = 0;
uint8_t dsNvRam[] = { 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0 };  // 56x8






void rtcPut(unsigned char data, unsigned char w_addr){
	switch (w_addr){
	case SEC: //seconds
		putSeconds(data);
		break;

	case MIN:
		putMinutes(data);
		break;
	case HOUR: // hours
		putHours(data);
		break;

	case WDAY:  // day of week
		putWday(data);
		break;

	case MDAY: // day of month
		putMday(data);
		break;
	case MONTH : // month
		putMonth(data);
		break;
	case YEAR: // year yy
		putYear(data);
		break;
	}
}


void rtcGet(){ //updates the timetable


	getSec(&timetable[0]);
	getMin(&timetable[1]);
	getHour(&timetable[2]);
	getWday(&timetable[3]);
	getMday(&timetable[4]);
	getMonth(&timetable[5]);
	getYear(&timetable[6]);

}



void DS_auxPut(unsigned char data, unsigned char w_addr) { // function specific DS1307
	if (w_addr == CONTROL_ADDRESS){ // control register, p. 9 DS1307
		putOutControlRTC(data);
		putRateControlRTC(data);
	}
	else {
		dsNvRam[w_addr - 8] = data;
	}
}

unsigned char DS_auxGet() {
	unsigned char temp = 0;
	if (w_addr == CONTROL_ADDRESS){ // control register, p. 9 DS1307
		getOutControlRTC(&temp);
		getRateControlRTC(&temp);
		return temp;
	}
	else
	return (dsNvRam[w_addr - NVRAM_BEGIN_ADDR]);
}

void readReg(){
	if (w_addr > MAX_ADDR){
		w_addr = 0;
	}
	if (w_addr <= MAX_DATA_ADDRESS) {
		i2c_write(timetable[w_addr++], bus); // the first is not w_addr
	}
	else {
		i2c_write(DS_auxGet(), bus);
		w_addr++;
	}

}

void updReg(){
	if (w_addr > MAX_ADDR){
		w_addr = 0;
	}
	if (w_addr <= MAX_DATA_ADDRESS) {// is written data and not only address
		rtcPut(RxBuffer, w_addr); //put RxBuffer at waddr
	} else {
		DS_auxPut(RxBuffer, w_addr);
	}
	w_addr++;

}

void dsEmulator_handler()
{
	//When reading the time and date registers, the user buffers are synchronized to the
	//internal registers on any I2C START.

	if (temp == _writeAdx){
		//rx_begin = 1;
		rxCount = 0;
	}
	else if (temp == _writeData) { // write to DS

		if (rxCount == 0){
			w_addr = i2c_read(0,bus); //fill the buffer //the first is the address of the DS
		} else {
			RxBuffer = i2c_read(0,bus); //fill the buffer
			updReg(); // as specified by DS1307 datasheet, addr is incremented everytime in updReg()
		}
		rxCount++;
	}
	else if (temp == _readAdx) {
		//first start condition, update the registers
		rtcGet(); //last updated, buffered
		//get the data back...
		readReg(); //and increment the addr too

	}
	else if (temp == _readData) { //read from DS, not needed to update the RTC timetable

		readReg();//increment the addr too
	}

	else {
		//  PORTDbits.RD6 &= ~(PORTDbits.RD6);
		// PORTDbits.RD7 = PORTDbits.RD6;
	}
}
