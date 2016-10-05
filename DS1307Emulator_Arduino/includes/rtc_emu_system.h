#ifndef RTC_EMU_SYSTEM_H_
#define RTC_EMU_SYSTEM_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "rtc_protocol.h"
#include "i2c.h"
#include "timers.h"

#ifdef	__cplusplus
extern "C" {
#endif

#define F_CPU 16000000UL
#define EMU_DS1307_DBG_LED 13
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))





#ifdef	__cplusplus
}
#endif



#endif /* SYSTEM_H_ */