/*
 * interrupts.c
 *
 * Created: 11/10/2015 17:07:47
 *  Author: TheXeno
 */ 

#include "system.h"

//static uint8_t dataLog[288];
//static uint8_t cnt = 0;

ISR(TIMER1_COMPA_vect)
{
	setPinValue(PORT_B, 5, 1);
	rtcProtocol_tickIncrementISR();
	setPinValue(PORT_B, 5, 0);
}

ISR(TWI_vect)
{

//dataLog[cnt++] = 
i2c_byteHandler_ISR();
	
}