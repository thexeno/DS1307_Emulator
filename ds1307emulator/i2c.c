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
#include "system.h"
#include "stdio.h"




void i2c_fail(uint8_t bus)
{


}

void i2c_init(_t_i2c_mode mode, uint8_t bus){
	_t_priority prio = HIGH_PRI;
	i2cMode = mode;

	if (prio == HIGH_PRI) {
		IPR1bits.SSP1IP = 1;
	} else IPR1bits.SSP1IP = 0;

#ifdef _MIXED
	switch (bus){
	case 0:
		switch (mode){
		case I2C_MASTER:
			SSP1STAT = 0b11000000;
			SSP1CON1 = 0b00111000;
			SSP1CON2 = 0b00000000;
			// SSP1MSK = 0b00000000;
			// BRG su SSPxADD = 77h per 100kHz @48MHz
			SSP1ADD = 0x77;
			PIE1bits.SSP1IE = 1;
			break;
		case I2C_SLAVE:
			SSP1CON1 = 0b00110110; // I2C I2C_SLAVE mode, 7-bit address. SSP enabled. CLK released
			SSP1ADD = SLAVE_ADDR<<1; // address
			//SSP1MSK = 0x01;
			SSP1CON2 = 0b00000000; //NOT Enables interrupt when a general call address (0000h) is received, not mask, not stretch
			PIE1bits.SSP1IE = 1;
			break;

		default:
			SSP1STAT = 0xFF;
			SSP1CON1 = 0x00;
			SSP2CON2 = 0x00;
			PIE1bits.SSP1IE = 0;
			break;

		}

		PIR1bits.SSP1IF = 0;

		break;

	case 1:
		break;
	}
#endif

#ifdef _SLAVE
	SSP1CON1 = 0b00110110; // I2C I2C_SLAVE mode, 7-bit address. SSP enabled. CLK released
	SSP1ADD = SLAVE_ADDR<<1;
	//SSP1MSK = 0x01;
	PIR1bits.SSP1IF = 0; //interrupt after receive byte
	SSP1CON2 = 0b00000000; //NOT Enables interrupt when a general call address (0000h) is received, not mask, not stretch
	PIR1bits.SSP1IF = 0;
#endif

#ifdef _MASTER
	SSP1STAT = 0b11000000;
	SSP1CON1 = 0b00111000;
	SSP1CON2 = 0b00000000;
	// SSP1MSK = 0b00000000;
	// BRG su SSPxADD = 77h per 100kHz @48MHz
	SSP1ADD = 0x77;
#endif
}

#if defined (_MASTER) || defined(_MIXED)
void i2c_waitForIdle(uint8_t bus)
{
	if (bus == 0){ //bus0 of MCU
		while (( SSP1CON2 & 0x1F ) | SSP1STATbits.R_W ) {}; // wait until...
	}
}
#endif
/******************************************************************************************/

#if defined (_MASTER) || defined(_MIXED)
void i2c_start(uint8_t bus)
{
	if (bus == 0){ //bus0 of MCU
		i2c_waitForIdle(bus);
		SSP1CON2bits.SEN=1;
	}
}
#endif
/******************************************************************************************/

#if defined (_MASTER) || defined(_MIXED)
void i2c_repStart(uint8_t bus)
{
	if (bus == 0){ //bus0 of MCU
		i2c_waitForIdle(bus);
		SSP1CON2bits.RSEN=1;
	}
}
#endif
/******************************************************************************************/

#if defined (_MASTER) || defined(_MIXED)
void i2c_stop(uint8_t bus)
{
	if (bus == 0){ //bus0 of MCU
		SSP1CON2bits.PEN=1;
	}
}
#endif
/******************************************************************************************/

unsigned char i2c_read( unsigned char ack, uint8_t bus )
{
	if (bus == 0){ //bus0 of MCU
		if (i2cMode == I2C_MASTER){
			unsigned char i2cReadData;

			PIR1bits.SSP1IF = 0;
			SSP1CON2bits.RCEN=1;

			i2cReadData = SSP1BUF;
			PIR1bits.SSP1IF = 0;

			if ( ack )
			{
				SSP1CON2bits.ACKDT=0;
			}
			else
			{
				SSP1CON2bits.ACKDT=1;
			}
			SSP1CON2bits.ACKEN=1;               // send acknowledge sequence
			return( i2cReadData );
		}

		else if (i2cMode == I2C_SLAVE)
		return( SSP1BUF );
	}
	return 0;
}

/******************************************************************************************/

unsigned char i2c_write( unsigned char i2cWriteData, uint8_t bus )
{
	if (bus == 0){ //bus0 of MCU
		if (i2cMode == I2C_MASTER){
			PIR1bits.SSP1IF = 0;
			SSP1BUF = i2cWriteData;
			return (0);
		}
		else if (i2cMode == I2C_SLAVE){
			SSP1BUF = i2cWriteData;
			return 0;
		}
	}
	return 0;
}
#if defined (_SLAVE) || defined(_MIXED)
uint8_t getI2Cstatus(uint8_t bus){
	if (bus == 0){ //bus0 of MCU
		return (SSP1STAT & SLAVE_STATUS);
	} else if (bus == 1){

	}

	return 0;
}
#endif

uint8_t isAckI2C(uint8_t bus){
	if (bus == 0){ //bus0 of MCU
		return (SSP1CON2bits.ACKSTAT);
	} else if (bus == 1){
		return (0);
	}
	return 0;
}

#if defined (_SLAVE) || defined(_MIXED)
void i2cSlaveHandler(uint8_t bus)
{

	temp = getI2Cstatus(bus);

	if (temp == ADX_W){
		temp = _writeAdx;
		i2c_read(0, bus);  //dummy read, empty the buff
	}
	else if (temp == DATA_W) { // write to DS
		temp = _writeData; //write to slave, i.e. receive
	}
	else if ((temp & MASK_BF_BIT) == ADX_R) {
		temp = _readAdx;
		i2c_read(0, bus); // empty the buff
	}
	else if (temp == DATA_R) { //read from DS, not needed to update the RTC timetable
		temp = _readData; // read from slave, i.e. transmit
		i2c_read(0, bus); // empty the buff
	}
	else if (temp == ERR) {

	}
	else {

	}
}

#endif


//
//#if defined(__XC)
//    #include <xc.h>         /* XC8 General Include File */
//#elif defined(HI_TECH_C)
//    #include <htc.h>        /* HiTech General Include File */
//#elif defined(__18CXX)
//    #include <p18cxxx.h>    /* C18 General Include File */
//#endif
//
//#if defined(__XC) || defined(HI_TECH_C)
//
//#include <stdint.h>         /* For uint8_t definition */
//#include <stdbool.h>        /* For true/false definition */
//
//#endif
//
//#include "i2c.h"
//
//
//
//void i2c_init(){
//    SSP1CON1 = 0b00110110; // I2C I2C_SLAVE mode, 7-bit address. SSP enabled. CLK released
//    SSP1ADD = SLAVE_ADDR<<1; // Indirizzo del DS1307
//    //SSP1MSK = 0x01;
//    SSP1CON2 = 0b00000000; //NOT Enables interrupt when a general call address (0000h) is received, not mask, not stretch
//    PIR1bits.SSP1IF = 0; //interrupt after receive byte
//    PIE1bits.SSP1IE = 1;
//}
//
//
//unsigned char i2c_read()
//{
// return( SSP1BUF );
//}
//
///******************************************************************************************/
//
//void i2c_write( unsigned char i2cWriteData )
//{
//
// //i2c_waitForIdle();
// SSP1BUF = i2cWriteData;
//}
//
//uint8_t getI2Cstatus(){
//    return (SSP1STAT & SLAVE_STATUS);
//}
//
