/*
 * i2c.h
 *
 * Created: 27/02/2016 12:33:01
 *  Author: TheXeno
 */ 


#ifndef I2C_H_
#define I2C_H_

#define I2C_BUFF_LEN 16
#define I2C_SLAVE_LEN_LIMITED 0
#define I2C_WRITE_BIT 0x00
#define I2C_READ_BIT 0x01

/* Identify the I2C status from the MCU */
#define I2C_STATUS_MASK 0xF8

/* Current bus status defines */
#define I2C_MASTER_START_STATE 0x08
#define I2C_MASTER_RESTART_STATE 0x10
#define I2C_MASTER_TX_ADX_ACK_NOT_RECEIVED_STATE 0x20
#define I2C_MASTER_TX_ADX_ACK_RECEIVED_STATE 0x18
#define I2C_MASTER_RX_ADX_ACK_RECEIVED_STATE 0x40
#define I2C_MASTER_RX_ADX_ACK_NOT_RECEIVED_STATE 0x48
#define I2C_MASTER_END_RX_DATA_STATE 0x58
#define I2C_MASTER_RX_DATA_STATE 0x50
#define I2C_MASTER_TX_ACK_RECEIVED_STATE 0x28
#define I2C_MASTER_TX_ACK_NOT_RECEIVED_STATE 0x30
#define I2C_SLAVE_RX_ADX_STATE 0x60
#define I2C_SLAVE_RX_DATA_ACK_TX_STATE 0x80
#define I2C_SLAVE_RX_DATA_NACK_TX_STATE 0x88
#define I2C_SLAVE_TX_ADX_STATE 0xA8
#define I2C_SLAVE_TX_DATA_ACK_RX_STATE 0xB8
#define I2C_SLAVE_TX_DATA_NACK_RX_STATE 0xC0
#define I2C_SLAVE_END_TX_DATA_ACK_RX_STATE 0xC8
#define I2C_SLAVE_STOP_RESTART_STATE 0xA0

/* Bus roles enumeration */
enum {
	I2C_MASTER_INIT_MODE,
	I2C_MASTER_RECEIVE_MODE,
	I2C_MASTER_SEND_MODE,
	I2C_MASTER_RECEIVE_SEND_MODE,
	I2C_MASTER_SEND_RECEIVE_MODE,
	I2C_SLAVE_INIT_MODE,
	I2C_SLAVE_MODE,
	I2C_SLAVE_RECEIVE_MODE,
	I2C_SLAVE_SEND_MODE
	};


void i2c_init(uint8_t speed, uint8_t mode);
uint8_t i2c_byteHandler_ISR(void);
uint8_t i2c_masterReceiveData(uint8_t adx, uint8_t len);
uint8_t i2c_readData(uint8_t* buff);
uint8_t i2c_busy(void);
uint8_t i2c_isRead(void);
uint8_t i2c_masterSendData(uint8_t* buff, uint8_t len, uint8_t adx);
void i2c_masterSendReceiveData(uint8_t adx, uint8_t lentx, uint8_t* buffTx, uint8_t lenrx, uint8_t* buffRx);
void i2c_masterReceiveSendData(uint8_t adx, uint8_t lenrx, uint8_t* buffRx, uint8_t lentx, uint8_t* buffTx);
uint8_t i2c_slaveWriteData(uint8_t* buff, uint8_t len);
#endif /* I2C_H_ */