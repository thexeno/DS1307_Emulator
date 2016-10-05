#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "rtc_protocol.h"
#include "i2c.h"
#include "port.h"
#include "timers.h"

#ifdef	__cplusplus
extern "C" {
	#endif

	#define F_CPU 16000000UL

	#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
	#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

	enum {INT_ON, INT_OFF};



	#ifdef	__cplusplus
}
#endif



#endif /* SYSTEM_H_ */