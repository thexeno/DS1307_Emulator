/*
 * i2c.c
 *
 * Created: 27/02/2016 12:32:50
 *  Author: TheXeno
 */ 

#include <string.h>
#include "system.h"



typedef struct  
{
	uint8_t bufferTx[I2C_BUFF_LEN];
	uint8_t bufferRx[I2C_BUFF_LEN];
	uint8_t buffLenTx;
	uint8_t buffLenRx;
	uint8_t stopReq;
	uint8_t reStartReq;
	uint8_t index;
	uint8_t status;
	uint8_t slaveAdx;
	uint8_t busy;
	uint8_t sendNack;
	uint16_t baud;
	uint8_t mode;
	uint8_t toRead;
	uint8_t toWrite;
} I2C_DATA;

//static void i2c_sendNack(void);
static void i2c_baudSelector(uint16_t speed);
static uint8_t i2c_read(void);
static void i2c_write(uint8_t data);
static void i2c_sendStart(void);
//static void i2c_sendReStartReq(void);
//static void i2c_sendStopReq(void);
static void i2c_clearInt(void);
//static uint8_t dataLogByte[288];
//static uint8_t cnt1 = 0;
//static uint8_t dataLogBytew[288];
//static uint8_t cnt2 = 0;
I2C_DATA i2cData;

/* --------------------------------------------- Private functions --------------------------------------- */

static void i2c_baudSelector(uint16_t speed)
{
	/** Speed is in kHz */
	uint8_t baudReg = 0;
	uint8_t presc = 4;
	
	baudReg = (uint8_t)((((uint16_t)(F_CPU/1000)/speed) - (uint16_t)16)/(uint16_t)(2*presc));
	
	/** Presc di 4 */
	TWSR |= (1<<TWPS0);
	TWSR &= ~(1<<TWPS1);
	TWBR = baudReg;
}


static uint8_t i2c_read(void)
{
	return (TWDR);
}

static void i2c_write(uint8_t data)
{
	TWDR = data;
}


static void i2c_sendStart(void)
{
	i2c_clearInt();
	TWCR = 0b11100101;
}



//static void i2c_dataTransferMode(void)
//{
	//TWCR &= ~(1<<TWSTA);
	//TWCR &= ~(1<<TWSTO);
//}

static void i2c_clearInt(void)
{
	if (i2cData.reStartReq == 1)
	{
		/* As start, but with no clear_int */
		/*en ack active again */
		TWCR = 0b10100101;
		i2cData.reStartReq = 0;
	}
	else if (i2cData.stopReq == 1)
	{
		/*en ack not active again */
		TWCR = 0b11010101;
		//TWCR &= ~(1<<TWSTA);
		//TWCR |= (1<<TWSTO);
		// il busy lo resett iquando leggi
		i2cData.busy = 0;
	}
	else
	{
		/* Normal */
		if (i2cData.sendNack)
		TWCR = 0b10000101;
		else
		TWCR = 0b11000101;
	//TWCR |= (1<<TWINT);
	//TWCR &= ~(1<<TWSTA);
	}
}

/* -------------------------------------------------- Public functions -------------------------------------------------*/

void i2c_addressRange(uint8_t adx)
{
	
}

void i2c_init(uint8_t speed_adx, uint8_t mode)
{
	/** Or speed, or address, depending if master or slave, respectively */
	if (mode == I2C_MASTER_INIT_MODE)
	{
		i2cData.baud = (uint16_t)speed_adx;
		TWCR = 0;
		i2c_baudSelector(i2cData.baud);
		TWCR |= (1<<TWIE); /* Int enabled */
		TWCR |= (1<<TWEN); /* enabled */
		/*By writing the TWEA bit to zero, the device can be virtually disconnected from the 2-wire Serial
		Bus temporarily. Address recognition can then be resumed by writing the TWEA bit to one
		again.
		 */	
		i2cData.mode = I2C_MASTER_INIT_MODE;
	}
	else if (mode == I2C_SLAVE_INIT_MODE)
	{
		TWCR = 0;
		TWCR |= (1<<TWIE); /* Int enabled */
		TWCR |= (1<<TWEA); /* ack if selected */
		TWAR =	(uint8_t)speed_adx;
		TWCR |= (1<<TWEN); /* enabled */
		i2cData.mode = I2C_SLAVE_MODE;
		i2cData.index = 0;
		i2cData.buffLenRx = I2C_BUFF_LEN;
		i2cData.buffLenTx = I2C_BUFF_LEN;
	}
	else
	{
		
	}

	i2cData.busy = 0;
}

/* supporta reStart */
uint8_t i2c_byteHandler_ISR(void)
{

	uint8_t state;
	i2cData.status = TWSR;
	state = (i2cData.status & I2C_STATUS_MASK);
	
	/* Bad sync with the Linux RTC driver. Issues bad start/stop conditions, filtered with this delay. */

	switch (state)
	{
		case I2C_MASTER_START_STATE:
#if I2C_SLAVE_ONLY
		i2cData.stopReq = 1;
#else
		/** Write/read mode */	
		i2c_write(i2cData.slaveAdx);
#endif
		break;
		
		case I2C_MASTER_RESTART_STATE:
#if I2C_SLAVE_ONLY
		i2cData.stopReq = 1;
#else
		if (i2cData.mode == I2C_MASTER_RECEIVE_SEND_MODE)
		{
			/* Now we are in send */
			i2cData.index = 0;
			/* Put mode send */
			i2cData.mode = I2C_MASTER_SEND_MODE;
			i2cData.sendNack = 0;
			i2c_write(i2cData.slaveAdx&0xFE); /* Send bit write*/
		}
		else if (i2cData.mode == I2C_MASTER_SEND_RECEIVE_MODE)
		{
			/* Now we are in send */
			i2cData.index = 0;
			/* Put mode send */
			i2cData.mode = I2C_MASTER_RECEIVE_MODE;
			i2cData.sendNack = 0;
			i2c_write(i2cData.slaveAdx|I2C_READ_BIT); /* Send bit read*/
		}
		else
		{
			/*Errore */
			i2cData.stopReq = 1;
		}
#endif
		break;
		
		case I2C_SLAVE_RX_ADX_STATE:
		i2cData.busy = 1;
#if I2C_SLAVE_LEN_LIMITED
		if ((i2cData.index+1) >= i2cData.buffLenRx)
		{
			/* Send NACK after next byte */
			i2cData.sendNack = 1;
		}
#endif
		rtcProtocol_freezeUserData();
		break;
		
		case I2C_SLAVE_RX_DATA_ACK_TX_STATE:
		/* Normal condition: always here during this state */
		rtcProtocol_writeToRTC(i2c_read());
#if I2C_SLAVE_LEN_LIMITED
		if ((i2cData.index+1) >= i2cData.buffLenRx)
		{
			/* Send NACK after next byte */
			i2cData.sendNack = 1;
		}
#else
		if ((i2cData.index+1) >= I2C_BUFF_LEN)
		{
			i2cData.index = 0;
		}
#endif
		break;
		
		case I2C_SLAVE_RX_DATA_NACK_TX_STATE:
		/* next: I2C_SLAVE_STOP_RESTART_STATE */
		rtcProtocol_setUserData();
		break;
		
		case I2C_SLAVE_STOP_RESTART_STATE:
		i2cData.busy = 0;
		i2cData.buffLenRx = i2cData.index;
		i2cData.index = 0;
		i2cData.sendNack = 0;
		rtcProtocol_setUserData();
		break;
		
		case I2C_SLAVE_TX_ADX_STATE:
		i2cData.busy = 1;
		rtcProtocol_freezeUserData();
		i2c_write(rtcProtocol_readUserData());

#if I2C_SLAVE_LEN_LIMITED
		if ((i2cData.index+1) >= i2cData.buffLenTx)
		{
			/* Send NACK after next byte, means the last byte inthis state is transmitted */
			i2cData.sendNack = 1;
		}
#else
		if ((i2cData.index+1) >= I2C_BUFF_LEN)
		{
			i2cData.index = 0;
		}
#endif
		break;
		
		case I2C_SLAVE_TX_DATA_ACK_RX_STATE:
		i2c_write(rtcProtocol_readUserData());
#if I2C_SLAVE_LEN_LIMITED
		if ((i2cData.index+1) >= i2cData.buffLenTx)
		{
			/* Send NACK after next byte, means the last byte in this state is transmitted */
			i2cData.sendNack = 1;
		}
#else
		if ((i2cData.index+1) >= I2C_BUFF_LEN)
		{
			i2cData.index = 0;
		}
#endif
		break;
		
		case I2C_SLAVE_END_TX_DATA_ACK_RX_STATE:
		/* No more transfers. reactivate adx recognition */
		/* If transmitted data is not available, must implemented a protocol to handle it */
		/* this case covers the slave intention of not transmit enymore */
		i2cData.sendNack = 0;
		i2cData.busy = 0;
		i2cData.index = 0;
		break;
		
		case I2C_SLAVE_TX_DATA_NACK_RX_STATE:
		/* No more transfers. reactivate adx recognition */
		i2cData.sendNack = 0;
		i2cData.busy = 0;
		i2cData.index = 0;
		rtcProtocol_setUserData();
		break;
		
		case I2C_MASTER_TX_ADX_ACK_NOT_RECEIVED_STATE:
		i2cData.stopReq = 1;
		break;
		
		case I2C_MASTER_TX_ADX_ACK_RECEIVED_STATE:
#if I2C_SLAVE_ONLY
		i2cData.stopReq = 1;	
#else 
		if (i2cData.index >= i2cData.buffLenTx)
		{
			if (i2cData.mode == I2C_MASTER_SEND_RECEIVE_MODE)
			{
				i2cData.reStartReq = 1;
			}
			else
			{
				i2cData.stopReq = 1;
			}
		}
		else
		{
			/** Index should be 0 here */
			i2c_write(i2cData.bufferTx[i2cData.index++]);
		}
#endif
		break;
		
		case I2C_MASTER_RX_ADX_ACK_RECEIVED_STATE:
#if I2C_SLAVE_ONLY
		i2cData.stopReq = 1;
#else
		/* wait for next data */
		if ((i2cData.index+1) >= i2cData.buffLenRx)
		{
			/*Last byte: Send NACK after next byte */
			i2cData.sendNack = 1;
		}
#endif
		break;
		
		case I2C_MASTER_RX_ADX_ACK_NOT_RECEIVED_STATE:
		i2cData.stopReq = 1;
		break;
		
		case I2C_MASTER_END_RX_DATA_STATE:
#if I2C_SLAVE_ONLY
		i2cData.stopReq = 1;
#else
		/* last byte */
		i2cData.bufferRx[i2cData.index] = i2c_read();
		/* NACK was sent, now send stop or restart */
		if (i2cData.mode == I2C_MASTER_RECEIVE_SEND_MODE)
		{
			i2cData.reStartReq = 1;
		}
		else
		{
			i2cData.stopReq = 1;
		}
#endif
		break;
		
		case I2C_MASTER_RX_DATA_STATE:
#if I2C_SLAVE_ONLY
		i2cData.stopReq = 1;
#else
		if (i2cData.index >= i2cData.buffLenRx)
		{
			/* Error condition: we must never be in this condition because the last byte will be sent in I2C_END_RX_DATA */
			if (i2cData.index == i2cData.buffLenRx)
			{
				i2cData.bufferRx[i2cData.index++] = i2c_read();	
			}
			i2cData.stopReq = 1;	
		}
		else
		{
			/* Normal condition: always here during this state */
			i2cData.bufferRx[i2cData.index++] = i2c_read();
			if ((i2cData.index+1) >= i2cData.buffLenRx)
			{
				/* Send NACK after next byte */
				i2cData.sendNack = 1;
			}
			
		}
#endif
		break;
		
		case I2C_MASTER_TX_ACK_RECEIVED_STATE:
#if I2C_SLAVE_ONLY
		i2cData.stopReq = 1;
#else
		if (i2cData.index >= i2cData.buffLenTx)
		{
			if (i2cData.mode == I2C_MASTER_SEND_RECEIVE_MODE)
			{
				i2cData.reStartReq = 1;
			}
			else
			{
				i2cData.stopReq = 1;
			}
		}
		else
		{
			i2c_write(i2cData.bufferTx[i2cData.index++]);	
		}
#endif
		break;
		
		case I2C_MASTER_TX_ACK_NOT_RECEIVED_STATE:
		i2cData.stopReq = 1;
		break;
		
		default:
		i2cData.stopReq = 1;
		setPinValue(PORT_B, 5, 1);
		break;
	}
	//dataLogByte[cnt1++] = state;
	i2c_clearInt();

	return state;
}

void i2c_masterReceiveSendData(uint8_t adx, uint8_t lenrx, uint8_t* buffRx, uint8_t lentx, uint8_t* buffTx)
{
	i2cData.buffLenTx = lentx;
	i2cData.buffLenRx = lenrx;
	i2cData.index = 0;
	i2cData.mode = I2C_MASTER_RECEIVE_SEND_MODE;
	i2cData.busy = 1;
	i2cData.stopReq = 0;
	i2cData.reStartReq = 0;
	i2cData.sendNack = 0;
	memcpy(i2cData.bufferTx, buffTx, lentx);
	i2cData.slaveAdx = ((adx << 1) | I2C_READ_BIT); // The send code will be put in ISR
	i2c_sendStart();
}

void i2c_masterSendReceiveData(uint8_t adx, uint8_t lentx, uint8_t* buffTx, uint8_t lenrx, uint8_t* buffRx)
{
	i2cData.buffLenTx = lentx;
	i2cData.buffLenRx = lenrx;
	i2cData.index = 0;
	i2cData.mode = I2C_MASTER_SEND_RECEIVE_MODE;
	i2cData.busy = 1;
	i2cData.stopReq = 0;
	i2cData.reStartReq = 0;
	i2cData.sendNack = 0;
	memcpy(i2cData.bufferTx, buffTx, lentx);
	i2cData.slaveAdx = ((adx << 1) | I2C_WRITE_BIT);
	i2c_sendStart();
}


uint8_t i2c_masterReceiveData(uint8_t adx, uint8_t len)
{
		i2cData.buffLenRx = len;
		i2cData.index = 0;
		i2cData.mode = I2C_MASTER_RECEIVE_MODE;
		i2cData.busy = 1;
		i2cData.sendNack = 0;
		i2cData.slaveAdx = ((adx << 1) | I2C_READ_BIT);
		i2c_sendStart();		
	return 0;
}

uint8_t i2c_readData(uint8_t* buff)
{
	memcpy(buff, i2cData.bufferRx, i2cData.buffLenRx);
	i2cData.toRead = 0;
	/*returns the copied characters */
	return (i2cData.buffLenRx);
}

uint8_t i2c_slaveWriteData(uint8_t* buff, uint8_t len)
{
	i2cData.buffLenTx = len;
	memcpy(i2cData.bufferTx, buff, i2cData.buffLenTx);
	i2cData.toWrite = 1;
	/*returns the copied characters */
	return (i2cData.buffLenTx);
}

uint8_t i2c_masterSendData(uint8_t* buff, uint8_t len, uint8_t adx)
{
	if (len > I2C_BUFF_LEN)
	{
		return 2;
	}
	i2cData.buffLenTx = len;
	i2cData.mode = I2C_MASTER_SEND_MODE;
	i2cData.index = 0;
	i2cData.busy = 1;
	i2cData.sendNack = 0;
	i2cData.slaveAdx = ((adx << 1) | I2C_WRITE_BIT);
	memcpy(i2cData.bufferTx, buff, len);
	i2c_sendStart();
	return 0;
}

/* Used to check if master has received data */
uint8_t i2c_isBusy(void)
{
	return (i2cData.busy);
}

uint8_t i2c_isRead(void)
{
	return !(i2cData.toRead);
}

uint8_t i2c_readMode(void)
{
	return (i2cData.mode);
}

uint8_t i2c_readCalledAdx(void)
{
	return (i2cData.slaveAdx);
}

// Se indirizzato da slave, ogni indirizzo chiama un wrapper dedicato 


