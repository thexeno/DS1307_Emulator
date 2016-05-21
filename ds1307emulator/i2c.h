/*
* File:   i2c.h
* Author: Enrico Sanino
*
* Created on 23 luglio 2014, 18.28
*/

#ifndef I2C_MASTER_H
#define	I2C_MASTER_H

//#include "system.h"
#if !(defined _MASTER || defined _SLAVE || defined _MIXED)
#define _MIXED
#endif

#ifdef	__cplusplus
extern "C" {
#endif
	typedef enum i2cMode {I2C_MASTER,I2C_SLAVE,I2C_OFF} _t_i2c_mode;
	typedef enum {_writeAdx, _writeData, _readAdx, _readData} _t_i2c_status;
#if defined (_SLAVE) || defined(_MIXED)

	_t_i2c_mode i2cMode;
	uint8_t temp;

	/*I2C interaction defines, needed by functions that interact with the i2c*/
#define SLAVE_ADDR 0x68
#define SLAVE_STATUS 0b00101101
#define ADX_W 0b00001001
#define DATA_W 0b00101001
#define ADX_R 0b00001100
#define DATA_R 0b00101100
#define ERR 0b00101100
#define MASK_BF_BIT 0xFE
#define UNMASK_S_BIT 0x08

	uint8_t getI2Cstatus(uint8_t bus);
#endif


#if defined (_MASTER) || defined(_MIXED)
	void i2c_start(uint8_t bus);
	void i2c_waitForIdle(uint8_t bus);
	void i2c_repStart(uint8_t bus);
	void i2c_stop(uint8_t bus);
	void i2c_fail(uint8_t bus);
#endif


	unsigned char i2c_read(unsigned  char ack , uint8_t bus);
	unsigned char i2c_write( unsigned char i2cWriteData, uint8_t bus) ;


	void i2c_init( _t_i2c_mode ,uint8_t bus );
	void i2cSlaveHandler(uint8_t bus);



#ifdef	__cplusplus
}
#endif

#endif	/* I2C_H */














///*
// * File:   i2c.h
// * Author: thexeno
// *
// * Created on 23 luglio 2014, 18.28
// */
//
//#ifndef I2C_H
//#define	I2C_H
//
//#ifdef	__cplusplus
//extern "C" {
//#endif
//
///*I2C interaction defines, needed by functions that interact with the i2c*/
//#define SLAVE_ADDR 0x68
//#define SLAVE_STATUS 0b00101101
//#define ADX_W 0b00001001
//#define DATA_W 0b00101001
//#define ADX_R 0b00001100
//#define DATA_R 0b00101100
//#define ERR 0b00101100
//#define MASK_BF_BIT 0xFE
//#define UNMASK_S_BIT 0x08
//
//void i2c_init();
//unsigned char i2c_read();
//void i2c_write( unsigned char i2cWriteData );
//uint8_t getI2Cstatus();
////void i2c_start();
////void i2c_restart();
////void i2c_wait();
////void i2c_send(unsigned char );
////uint8_t i2c_read();
////void i2c_ack();
////void i2c_fail();
////void i2c_nack();
////void i2c_stop();
////void i2c_stop();
////uint8_t i2c_ack_rx();
//
//#ifdef	__cplusplus
//}
//#endif
//
//#endif	/* I2C_H */
//
