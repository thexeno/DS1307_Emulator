/* The MIT License (MIT)
 * Copyright (c) 2015 Enrico Sanino
 * File:   timers.h
 * Author: Enrico Sanino
 *
 * Created on 28 febbraio 2015, 18.42
 */

#ifndef TIMERS_H
#define	TIMERS_H

//#include "EmuDS1307.h"
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))


#ifdef	__cplusplus
extern "C" {
#endif

/* Public type declarations */
#define TMR_16BIT
//#define _timer0 
#define _timer1 
//#define _timer2

typedef enum {CLKPIN, CLKOSC, CLKINST} _t_timer_clock;

typedef enum {TMR_CLK_RISE, TMR_CLK_FALL} _t_edge;
typedef enum
{
	COMP_A_TOGGLE,
	COMP_A_CLEAR,
	COMP_A_SET,
	COMP_B_TOGGLE,
	COMP_B_CLEAR,
	COMP_B_SET,
	COMP_A_DISCONNECT,
	COMP_B_DISCONNECT
}	_t_compare_sel;
	
typedef enum
{
	NORMAL,
	PWM,
	COMPARE_MATCH,
	FAST_PWM
	} _t_timer_mode;


enum
{
	PWM_A,
	PWM_B
};	
	
typedef enum
{
	PWM_NORMAL_A,
	PWM_NORMAL_B,
	PWM_INVERT_A,
	PWM_INVERT_B
	} _t_pwm_pol;



/* Public and private (static) prototypes */

#ifdef _timer0


void initTimer0(uint16_t prescaler, _t_edge edge,  _t_timer_mode mode , _t_timer_clock clock, uint8_t timer_int );
void startTimer0();
void stopTimer0();
/*uint8_t readTimer0();*/
void initCompare0 (uint8_t comp_val, _t_compare_sel comp, uint16_t prescaler, _t_edge edge,   _t_timer_clock clock, uint8_t timer_int);
void writeTimer0(uint8_t value);
void initPWM0(uint16_t prescaler, _t_pwm_pol pol);
void setDutyPWM0(uint8_t duty, uint8_t type);
#endif


#ifdef _timer1


void initTimer1(uint16_t prescaler, _t_edge edge,  _t_timer_mode mode , _t_timer_clock clock, uint8_t timer_int );
void startTimer1();
void stopTimer1();
/*uint8_t readTimer1();*/
void initCompare1 (uint16_t comp_val, _t_compare_sel comp, uint16_t prescaler, _t_edge edge,   _t_timer_clock clock, uint8_t timer_int);
void writeTimer1(uint16_t value);
void initPWM1(uint16_t prescaler, _t_pwm_pol pol);
void setDutyPWM1(uint16_t duty, uint8_t type);
#endif

#ifdef _timer2


uint8_t initTimer2(uint16_t prescaler,  _t_timer_mode mode ,  uint8_t timer_int, uint8_t clkSrc);
uint8_t startTimer2(void);
uint8_t stopTimer2(void);
/*uint8_t readTimer1();*/
uint8_t initCompare2 (uint16_t comp_val, _t_compare_sel comp, uint16_t prescaler, uint8_t timer_int,  uint8_t clkSrc);
uint8_t writeTimer2(uint8_t val);
void initPWM2(uint16_t prescaler, _t_pwm_pol pol, uint8_t source);
uint8_t setDutyPWM2(uint8_t duty, uint8_t pwmType);
void initSymPWM2(uint16_t prescaler, _t_pwm_pol pol, uint8_t clkSrc);

#endif

#ifdef	__cplusplus
}
#endif


#endif	/* TIMERS_H */

