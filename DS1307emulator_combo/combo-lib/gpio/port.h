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
 * port.h
 *
 * Created: 27/02/2016 14:31:17
 *  Author: TheXeno
 */ 


#ifndef PORT_H_
#define PORT_H_

enum 
{
	PORT_C,
	PORT_D,
	PORT_B
	};
	
enum
{
	PULLUP_ENABLED,
	PULLUP_DISABLED
	};

enum
{
	PIN_INPUT,
	PIN_OUTPUT
	};

void setPinValue(uint8_t port, uint8_t pin, uint8_t val);
uint8_t readPinValue(uint8_t port, uint8_t pin);
uint8_t readPinMode(uint8_t port, uint8_t pin);
void setPinMode(uint8_t port, uint8_t pin, uint8_t inOut, uint8_t pull);


#endif /* PORT_H_ */