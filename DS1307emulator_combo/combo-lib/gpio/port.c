 // The MIT License (MIT)

// Copyright (c) 2016 Enrico Sanino

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/*
 * port.c
 *
 * Created: 27/02/2016 14:31:09
 *  Author: TheXeno
 */ 
#include "system.h"

uint8_t readPinMode(uint8_t port, uint8_t pin)
{
	uint8_t ret = 0;
	
	switch (port)
	{
		case PORT_C:
		if (!(DDRC & (1<<pin)))
		{
			ret = PIN_INPUT;
		}
		else
		{
			ret = PIN_OUTPUT;
		}						
		break;			
		
		case PORT_D:
		if (!(DDRD & (1<<pin)))
		{
			ret = PIN_INPUT;
		}
		else
		{
			ret = PIN_OUTPUT;
		}
		break;
		
		case PORT_B:
		if (!(DDRB & (1<<pin)))
		{
			ret = PIN_INPUT;
		}
		else
		{
			ret = PIN_OUTPUT;
		}
		break;

	}
	return ret;
}


uint8_t readPinValue(uint8_t port, uint8_t pin)
{
	uint8_t ret = 0;
	
	switch (port)
	{
		case PORT_C:
		if (PINC & (1 << pin))
		{
			ret = 1;
		}
		else
		{
			ret = 0;
		}
		break;
		
		case PORT_D:
		if (PIND & (1 << pin))
		{
			ret = 1;
		}
		else
		{
			ret = 0;
		}
		break;
		
		case PORT_B:
		if (PINB & (1 << pin))
		{
			ret = 1;
		}
		else
		{
			ret = 0;
		}
		break;
		
	}
	return ret;
}

void setPinValue(uint8_t port, uint8_t pin, uint8_t val)
{
	if (readPinMode(port, pin) == PIN_INPUT)
	{
		/* Do not modify pull values */
		return;
	}
	
	switch (port)
	{
		case PORT_C:
		if (val != 0)
		{
			PORTC |= (1<<pin);
		}
		else
		{
			PORTC &= ~(1<<pin);
		}	
		break;		
		
		case PORT_D:
		if (val != 0)
		{
			PORTD |= (1<<pin);
		}
		else
		{
			PORTD &= ~(1<<pin);
		}
		break;
		
		case PORT_B:
		if (val != 0)
		{
			PORTB |= (1<<pin);
		}
		else
		{
			PORTB &= ~(1<<pin);
		}
		break;
		
	}
}



void setPinMode(uint8_t port, uint8_t pin, uint8_t inOut, uint8_t pull)
{
	if (pull == PULLUP_ENABLED)
	{
		/* Enable pullup on all ports */
		MCUCR &= ~(1 << PUD);
	}
	else
	{
		/* If only one port require pullup, the other calls must keep that enabled */
	}
	
	switch (port)
	{
		case PORT_C:
		if (inOut == PIN_INPUT)
		{
			DDRC &= ~(1 << pin);
			if (pull == PULLUP_ENABLED)
			{
				PORTC |= (1 << pin);
			}
			else
			{
				PORTC &= ~(1 << pin);
			}
		}
		else
		{
			DDRC |= (1 << pin);
		}	
		break;
		
		case PORT_D:
		if (inOut == PIN_INPUT)
		{
			DDRD &= ~(1 << pin);
			if (pull == PULLUP_ENABLED)
			{
				PORTD |= (1 << pin);
			}
			else
			{
				PORTD &= ~(1 << pin);
			}
		}
		else
		{
			DDRD |= (1 << pin);
		}
		break;
		
		case PORT_B:
		if (inOut == PIN_INPUT)
		{
			DDRB &= ~(1 << pin);
			if (pull == PULLUP_ENABLED)
			{
				PORTB |= (1 << pin);
			}
			else
			{
				PORTB &= ~(1 << pin);
			}
		}
		else
		{
			DDRB |= (1 << pin);
		}
		break;
		
	}
}

