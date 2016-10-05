//The MIT License (MIT)
//
//Copyright (c) 2016 Enrico Sanino
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.


#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "./includes/timers.h"

enum
{
	TMR_INT_CLK,
	TMR_EXT_XTAL,
	TMR_EXT_CLK,
	};

enum {INT_ON, INT_OFF};
	
/* Private variables and prototypes*/
static uint16_t timer0presc = 0; /* parti spento */
static uint16_t timer1presc = 0; /* parti spento */
static uint16_t timer2presc = 0; /* parti spento */

static uint8_t pwmPol0A = PWM_INVERT_A;
static uint8_t pwmPol0B = PWM_INVERT_B;
static uint8_t temp_pwmPol0A = PWM_INVERT_A;
static uint8_t temp_pwmPol0B = PWM_INVERT_B;
static uint8_t set_pwmPol0A = 0;
static uint8_t set_pwmPol0B = 0;
static uint8_t hasPinSet0A = 0;
static uint8_t hasPinSet0B = 0;

static uint8_t pwmPol1A = PWM_INVERT_A;
static uint8_t pwmPol1B = PWM_INVERT_B;

static uint8_t pwmPol2A = PWM_INVERT_A;
static uint8_t pwmPol2B = PWM_INVERT_B;
static uint8_t temp_pwmPol2A = PWM_INVERT_A;
static uint8_t temp_pwmPol2B = PWM_INVERT_B;
static uint8_t set_pwmPol2A = 0;
static uint8_t set_pwmPol2B = 0;
static uint8_t hasPinSet2A = 0;
static uint8_t hasPinSet2B = 0;


/* Public variables and prototypes */
void write16bit(uint16_t input_n, volatile uint8_t* out_h, volatile uint8_t* out_l);
void read16bit(volatile uint16_t *out_n, uint8_t in_h, uint8_t in_l);

#ifdef TMR_16BIT
void write16bit(uint16_t input_n, volatile uint8_t* out_h, volatile uint8_t* out_l)
{
	cli();
	*out_h = (uint8_t)(input_n >> 8);
	*out_l = (uint8_t)(input_n);
	sei();
}

void read16bit(volatile uint16_t *out_n, uint8_t in_h, uint8_t in_l)
{
	cli();
	*out_n = (uint16_t)in_l;
	*out_n |= ((uint16_t)(in_h))<<8;
	sei();
}

#endif


#ifdef _timer0


static void pwm0Pins(_t_pwm_pol pol);


void initTimer0(uint16_t prescaler, _t_edge edge,  _t_timer_mode mode , _t_timer_clock clock, uint8_t timer_int)
{
	//TCCR0A = 0;
	
	//TIMSK0 = 0;
	
	switch (mode)
		{
			case NORMAL:
			cbi(TCCR0A, WGM00);
			cbi(TCCR0A, WGM01);
			cbi(TCCR0B, WGM02);
			break;
			
			case PWM: /* TBD TOP = OCRA */
			sbi(TCCR0A, WGM00);
			cbi(TCCR0A, WGM01);
			cbi(TCCR0B, WGM02);
			break;
			
			case COMPARE_MATCH:
			cbi(TCCR0A, WGM00);
			sbi(TCCR0A, WGM01);
			cbi(TCCR0B, WGM02);
			break;
			
			case FAST_PWM:
			sbi(TCCR0A, WGM00);
			sbi(TCCR0A, WGM01);
			cbi(TCCR0B, WGM02);
			break;
			
			default:
			cbi(TCCR0A, WGM00);
			cbi(TCCR0A, WGM01);
			cbi(TCCR0B, WGM02);
			break;
		}

	
	
		if (timer0presc != 0)
		{
			TCCR0B &= 0xF8; /* Clear CTS0 1 e 2 */
			timer0presc = prescaler; /* e lo usi per riapplicare il prescaler quanto fai lo start */
			switch (prescaler)
			{

				case 1:
				TCCR0B |= 1;
				break;
		
				case 8:
				TCCR0B |= 2;
				break;
		
				case 64:
				TCCR0B |= 3;
				break;
		
				case 256:
				TCCR0B |= 4;
				break;
		
				case 1024:
				TCCR0B |= 5;
				break;
		
				default: /* spento se valore errato */
				TCCR0B |= 0;
				break;
			}
		}		
		else
		{
			TCCR0B &= 0xF8; /* Clear CTS0 1 e 2 */
			timer0presc = prescaler; /* e lo usi per riapplicare il prescaler quanto fai lo start */
		}	
	
	switch (clock)
	{
		case CLKPIN:
		/* Sovrascrivi prescaler */
		if (edge == TMR_CLK_RISE)
		{
			sbi(TCCR0B, CS02);
			sbi(TCCR0B, CS01);
			cbi(TCCR0B, CS00);
		}
		else /* Fall */
		{
			sbi(TCCR0B, CS02);
			sbi(TCCR0B, CS01);
			sbi(TCCR0B, CS00);
		}
		break;
		
		case CLKOSC:
		/* use prescaler value */
		break;
		
		default:
		/* use prescaler value */
		break;
	}
	
	switch (timer_int)
	{
		case INT_ON:
		if (mode == NORMAL)  /* Altrimenti setti interrupt nelle funzioni di compare init o pwm init */
		{
			sbi(TIMSK0, TOIE0);
		}
		break;
		case INT_OFF:
		cbi(TIMSK0, TOIE0);
		break;
		default:
		cbi(TIMSK0, TOIE0);
		break;
	}
	
}

/**@brief Chiamabile più volte per inizializzare moduli A e B */
void initCompare0 (uint8_t comp_val, _t_compare_sel comp, uint16_t prescaler, _t_edge edge, _t_timer_clock clock, uint8_t timer_int)
{
	initTimer0(prescaler, edge, COMPARE_MATCH, clock, timer_int);
	switch (comp)
	{
		case COMP_A_TOGGLE:
		sbi(TCCR0A, COM0A0);
		cbi(TCCR0A, COM0A1);
		sbi(DDRD, DDD6);
		OCR0A = comp_val;
		break;
		
		case COMP_A_CLEAR:
		cbi(TCCR0A, COM0A0);
		sbi(TCCR0A, COM0A1);
		sbi(DDRD, DDD6);
		OCR0A = comp_val;
		break;
		
		case COMP_A_SET:
		sbi(TCCR0A, COM0A0);
		sbi(TCCR0A, COM0A1);
		sbi(DDRD, DDD6);
		OCR0A = comp_val;
		break;
		
		case COMP_B_TOGGLE:
		sbi(TCCR0A, COM0B0);
		cbi(TCCR0A, COM0B1);
		sbi(DDRD, DDD5);
		OCR0B = comp_val;
		break;
		
		case COMP_B_CLEAR:
		cbi(TCCR0A, COM0B0);
		sbi(TCCR0A, COM0B1);
		sbi(DDRD, DDD5);
		OCR0B = comp_val;
		break;
		
		case COMP_B_SET:
		sbi(TCCR0A, COM0B0);
		sbi(TCCR0A, COM0B1);
		sbi(DDRD, DDD5);
		OCR0B = comp_val;
		break;
		
		case COMP_A_DISCONNECT:
		cbi(TCCR0A, COM0A0);
		cbi(TCCR0A, COM0A1);
		OCR0A = comp_val;
		break;
		
		case COMP_B_DISCONNECT:
		cbi(TCCR0A, COM0B0);
		cbi(TCCR0A, COM0B1);
		OCR0B = comp_val;
		break;
		
		default:
		/* lascia OCnx sconnesso come lo è da initTimer0*/
		cbi(TCCR0A, COM0B0);
		cbi(TCCR0A, COM0B1);
		cbi(TCCR0A, COM0A0);
		cbi(TCCR0A, COM0A1);
		break;
	}
	
	switch (timer_int)
	{
		case INT_ON:
		/* Non devo chiamare questa funzione se uso il normale */
			if ((comp == COMP_A_CLEAR) || (comp == COMP_A_SET) ||(comp == COMP_A_TOGGLE)||(comp == COMP_A_DISCONNECT))
			{
				sbi(TIMSK0, OCIE0A);	
			}
			if ((comp == COMP_B_CLEAR) || (comp == COMP_B_SET) ||(comp == COMP_B_TOGGLE)||(comp == COMP_B_DISCONNECT))
			{
				sbi(TIMSK0, OCIE0B);	
			}
		break;
		
		case INT_OFF:
			if ((comp == COMP_A_CLEAR) || (comp == COMP_A_SET) ||(comp == COMP_A_TOGGLE)||(comp == COMP_A_DISCONNECT))
			{
				cbi(TIMSK0, OCIE0A);
			}
			if ((comp == COMP_B_CLEAR) || (comp == COMP_B_SET) ||(comp == COMP_B_TOGGLE)||(comp == COMP_B_DISCONNECT))
			{
				cbi(TIMSK0, OCIE0B);
			}
		break;
		
		default:
		cbi(TIMSK0, OCIE0A);
		cbi(TIMSK0, OCIE0B);
		break;
	}
		
}

void writeTimer0(uint8_t val)
{
	TCNT0 = val;
}

void startTimer0()
{
	TCCR0B &= 0xF8; /* Clear CTS0 1 e 2 */
	switch (timer0presc)
	{
		case 1:
		TCCR0B |= 1;
		break;
		
		case 8:
		TCCR0B |= 2;
		break;
		
		case 64:
		TCCR0B |= 3;
		break;
		
		case 256:
		TCCR0B |= 4;
		break;
		
		case 1024:
		TCCR0B |= 5;
		break;
		
		default: /* spento se valore errato */
		TCCR0B |= 0;
		break;
	}
}

void stopTimer0()
{
	TCCR0B &= 0xF8; /* Clear CTS0 1 e 2 */
}

void initSymPWM0(uint16_t prescaler, _t_pwm_pol pol)
{
	/* Override previous PWM init */
	initTimer0(prescaler, TMR_CLK_RISE, PWM, CLKOSC, INT_OFF);
	/* DC = 0%*/
	pwm0Pins(pol);
}

void initPWM0(uint16_t prescaler, _t_pwm_pol pol)
 {
	/* Override previous PWM init */
	initTimer0(prescaler, TMR_CLK_RISE, FAST_PWM, CLKOSC, INT_OFF);
	/* DC = 0%*/
	if ((pol == PWM_NORMAL_A)||(pol == PWM_INVERT_A))
	{
		set_pwmPol0A = 0;	
		sbi(DDRD, DDD6);
		cbi(PORTD, PORTD6); /* out low */
	}
	if ((pol == PWM_NORMAL_B)||(pol == PWM_INVERT_B))
	{
		set_pwmPol0B = 0;
		sbi(DDRD, DDD5);
		cbi(PORTD, PORTD5); /* out low */
	}		
	pwm0Pins(pol);
	if (pol == PWM_NORMAL_A)
	{
		setDutyPWM0(0, PWM_A);
	}
	if (pol == PWM_NORMAL_B)
	{
		setDutyPWM0(0, PWM_B);
	}
	if (pol == PWM_INVERT_A)
	{
		setDutyPWM0(0xff, PWM_A);
	}
	if (pol == PWM_INVERT_B)
	{
		setDutyPWM0(0xff, PWM_B);
	}
	startTimer0();
}

static void pwm0Pins(_t_pwm_pol pol)
{
		if (pol==PWM_NORMAL_A)
		{
			
			cbi(TCCR0A, COM0A0);
			sbi(TCCR0A, COM0A1);
			OCR0A = 0;
			//sbi(DDRD, DDD6);
			//cbi(PORTD, PORTD6); /* out low */
			pwmPol0A = pol;
		}
		else if (pol==PWM_INVERT_A)
		{
			sbi(TCCR0A, COM0A0);
			sbi(TCCR0A, COM0A1);
			OCR0A = 0xff; //prima assegna: no glitch
			//sbi(DDRD, DDD6);
			//cbi(PORTD, PORTD6); /* out low */
			pwmPol0A = pol;
		}
		if (pol==PWM_NORMAL_B)
		{
			cbi(TCCR0A, COM0B0);
			sbi(TCCR0A, COM0B1);
			OCR0B = 0;

			//sbi(DDRD, DDD5);
			//cbi(PORTD, PORTD5); /* out low */
			pwmPol0B = pol;
		}
		else if (pol==PWM_INVERT_B)
		{
			sbi(TCCR0A, COM0B0);
			sbi(TCCR0A, COM0B1);
			OCR0B = 0xff;

			//sbi(DDRD, DDD5);
			//cbi(PORTD, PORTD5); /* out low */
			pwmPol0B = pol;
		}
		else
		{
			/* Usi futuri: funzionalità di toggle di OC0A*/
		}
}

void setDutyPWM0(uint8_t duty, uint8_t pwmType)
{
	if (pwmType == PWM_A)
	{
		if (duty == 0)
		{
			if (set_pwmPol0A==0)
			{
				/* Fix per glitch quando a 0% */
				temp_pwmPol0A = pwmPol0A;
				set_pwmPol0A = 1;
					
				if (pwmPol0A == PWM_NORMAL_A)
				{	
					pwm0Pins(PWM_INVERT_A);		
				}
				else if (pwmPol0A == PWM_INVERT_A)
				{
					pwm0Pins(PWM_NORMAL_A);		
				}
				else
				{
					
				}
			}
			hasPinSet0A = 0;
			OCR0A = 0xff;			
		}
		else
		{
			if (hasPinSet0A == 0)
			{
				if (set_pwmPol0A == 1) // Se aveva settato un duty = 0 prima
				{
					set_pwmPol0A = 0;
					pwm0Pins(temp_pwmPol0A);
				}
				else
				{
					pwm0Pins(pwmPol0A);
				}
				hasPinSet0A = 1;	
			}
			
			OCR0A = duty;	
		}
		
	}
	else if (pwmType == PWM_B)
	{
		if (duty == 0)
		{
			if (set_pwmPol0B==0)
			{
				/* Fix per glitch quando a 0% */
				temp_pwmPol0B = pwmPol0B;
				set_pwmPol0B = 1;
				
				if (pwmPol0B == PWM_NORMAL_B)
				{
					pwm0Pins(PWM_INVERT_B);
				}
				else if (pwmPol0B == PWM_INVERT_B)
				{
					pwm0Pins(PWM_NORMAL_B);
				}
				else
				{
					
				}
			}
			hasPinSet0B = 0;
			OCR0B = 0xff;
			
		}
		else
		{
			if (hasPinSet0B == 0)
			{
				if (set_pwmPol0B == 1) // Se aveva settato un duty = 0 prima
				{
					set_pwmPol0B = 0;
					pwm0Pins(temp_pwmPol0B);
				}
				else
				{
					pwm0Pins(pwmPol0B);
				}
				hasPinSet0B = 1;	
			}	
						
			OCR0B = duty;
		}		
	}
	else
	{
		
	}
}
#endif

#ifdef _timer1

static void pwm1Pins(_t_pwm_pol pol);

void initTimer1(uint16_t prescaler, _t_edge edge,  _t_timer_mode mode , _t_timer_clock clock, uint8_t timer_int)
{
	//TCCR0A = 0;
	
	//TIMSK0 = 0;
	
	switch (mode)
	{
		case NORMAL: 
		cbi(TCCR1A, WGM10);
		cbi(TCCR1A, WGM11);
		cbi(TCCR1B, WGM12);
		cbi(TCCR1B, WGM13);
		break;
		
		case PWM: /* TBD TOP = OCRA e TOP = ICR */
		/* TOP = 0xFF */
		sbi(TCCR1A, WGM10);
		cbi(TCCR1A, WGM11);
		cbi(TCCR1B, WGM12);
		cbi(TCCR1B, WGM13);
		
		break;
		
		case COMPARE_MATCH:
		cbi(TCCR1A, WGM10);
		cbi(TCCR1A, WGM11);
		sbi(TCCR1B, WGM12);
		cbi(TCCR1B, WGM13);		
		break;
		
		case FAST_PWM:
		/* TOP = 0xff */
		sbi(TCCR1A, WGM10);
		cbi(TCCR1A, WGM11);
		sbi(TCCR1B, WGM12);
		cbi(TCCR1B, WGM13);
		break;
		
		default:
		cbi(TCCR1A, WGM10);
		cbi(TCCR1A, WGM11);
		cbi(TCCR1B, WGM12);
		cbi(TCCR1B, WGM13);
		break;
	}

	
	
	if (timer1presc != 0)
	{
		TCCR1B &= 0xF8; /* Clear CTS0 1 e 2 */
		timer1presc = prescaler; /* e lo usi per riapplicare il prescaler quanto fai lo start */
		switch (prescaler)
		{

			case 1:
			TCCR1B |= 1;
			break;
			
			case 8:
			TCCR1B |= 2;
			break;
			
			case 64:
			TCCR1B |= 3;
			break;
			
			case 256:
			TCCR1B |= 4;
			break;
			
			case 1024:
			TCCR1B |= 5;
			break;
			
			default: /* spento se valore errato */
			TCCR1B |= 0;
			break;
		}
	}
	else
	{
		TCCR1B &= 0xF8; /* Clear CTS0 1 e 2 */
		timer1presc = prescaler; /* e lo usi per riapplicare il prescaler quanto fai lo start */
	}
	
	switch (clock)
	{
		case CLKPIN:
		/* Sovrascrivi prescaler */
		if (edge == TMR_CLK_RISE)
		{
			sbi(TCCR1B, CS02);
			sbi(TCCR1B, CS01);
			cbi(TCCR1B, CS00);
		}
		else /* Fall */
		{
			sbi(TCCR1B, CS02);
			sbi(TCCR1B, CS01);
			sbi(TCCR1B, CS00);
		}
		break;
		
		case CLKOSC:
		/* use prescaler value */
		break;
		
		default:
		/* use prescaler value */
		break;
	}
	
	switch (timer_int)
	{
		case INT_ON:
		if (mode == NORMAL)  /* Altrimenti setti interrupt nelle funzioni di compare init o pwm init */
		{
			sbi(TIMSK1, TOIE1);
		}
		break;
		case INT_OFF:
		cbi(TIMSK1, TOIE1);
		break;
		default:
		cbi(TIMSK1, TOIE1);
		break;
	}
	
}

/**@brief Chiamabile più volte per inizializzare moduli A e B */
void initCompare1 (uint16_t comp_val, _t_compare_sel comp, uint16_t prescaler, _t_edge edge, _t_timer_clock clock, uint8_t timer_int)
{
	initTimer1(prescaler, edge, COMPARE_MATCH, clock, timer_int);
	switch (comp)
	{
		case COMP_A_TOGGLE:
		sbi(TCCR1A, COM1A0);
		cbi(TCCR1A, COM1A1);
		sbi(DDRB, DDB1);
		write16bit(comp_val, &OCR1AH, &OCR1AL);
		OCR1A = comp_val;
		break;
		
		case COMP_A_CLEAR:
		cbi(TCCR1A, COM1A0);
		sbi(TCCR1A, COM1A1);
		sbi(DDRB, DDB1);
		write16bit(comp_val, &OCR1AH, &OCR1AL);
		break;
		
		case COMP_A_SET:
		sbi(TCCR1A, COM1A0);
		sbi(TCCR1A, COM1A1);
		sbi(DDRB, DDB1);
		write16bit(comp_val, &OCR1AH, &OCR1AL);
		break;
		
		case COMP_B_TOGGLE:
		sbi(TCCR1A, COM1B0);
		cbi(TCCR1A, COM1B1);
		sbi(DDRB, DDB2);
		write16bit(comp_val, &OCR1BH, &OCR1BL);
		break;
		
		case COMP_B_CLEAR:
		cbi(TCCR1A, COM1B0);
		sbi(TCCR1A, COM1B1);
		sbi(DDRB, DDB2);
		write16bit(comp_val, &OCR1BH, &OCR1BL);
		break;
		
		case COMP_B_SET:
		sbi(TCCR1A, COM1B0);
		sbi(TCCR1A, COM1B1);
		sbi(DDRB, DDB2);
		write16bit(comp_val, &OCR1BH, &OCR1BL);
		break;
		
		case COMP_A_DISCONNECT:
		cbi(TCCR1A, COM1A0);
		cbi(TCCR1A, COM1A1);
		write16bit(comp_val, &OCR1AH, &OCR1AL);
		break;
		
		case COMP_B_DISCONNECT:
		cbi(TCCR1A, COM1B0);
		cbi(TCCR1A, COM1B1);
		write16bit(comp_val, &OCR1BH, &OCR1BL);
		break;
		
		default:
		/* lascia OCnx sconnesso come lo è da initTimer0*/
		cbi(TCCR1A, COM1B0);
		cbi(TCCR1A, COM1B1);
		cbi(TCCR1A, COM1A0);
		cbi(TCCR1A, COM1A1);
		break;
	}
	
	switch (timer_int)
	{
		case INT_ON:
		/* Non devo chiamare questa funzione se uso il normale */
		if ((comp == COMP_A_CLEAR) || (comp == COMP_A_SET) ||(comp == COMP_A_TOGGLE)||(comp == COMP_A_DISCONNECT))
		{
			sbi(TIMSK1, OCIE1A);
		}
		if ((comp == COMP_B_CLEAR) || (comp == COMP_B_SET) ||(comp == COMP_B_TOGGLE)||(comp == COMP_B_DISCONNECT))
		{
			sbi(TIMSK1, OCIE1B);
		}
		break;
		
		case INT_OFF:
		if ((comp == COMP_A_CLEAR) || (comp == COMP_A_SET) ||(comp == COMP_A_TOGGLE)||(comp == COMP_A_DISCONNECT))
		{
			cbi(TIMSK1, OCIE1A);
		}
		if ((comp == COMP_B_CLEAR) || (comp == COMP_B_SET) ||(comp == COMP_B_TOGGLE)||(comp == COMP_B_DISCONNECT))
		{
			cbi(TIMSK1, OCIE1B);
		}
		break;
		
		default:
		cbi(TIMSK1, OCIE1A);
		cbi(TIMSK1, OCIE1B);
		break;
	}
	
}

void writeTimer1(uint16_t val)
{
	write16bit(val, &TCNT1H, &TCNT1L);
}

void startTimer1()
{
	TCCR1B &= 0xF8; /* Clear CTS0 1 e 2 */
	switch (timer1presc)
	{
		case 1:
		TCCR1B |= 1;
		break;
		
		case 8:
		TCCR1B |= 2;
		break;
		
		case 64:
		TCCR1B |= 3;
		break;
		
		case 256:
		TCCR1B |= 4;
		break;
		
		case 1024:
		TCCR1B |= 5;
		break;
		
		default: /* spento se valore errato */
		TCCR1B |= 0;
		break;
	}
}

void stopTimer1()
{
	TCCR1B &= 0xF8; /* Clear CTS0 1 e 2 */
}



void initSymPWM1(uint16_t prescaler, _t_pwm_pol pol)
{
	/* Override previous PWM init */
	initTimer1(prescaler, TMR_CLK_RISE, PWM, CLKOSC, INT_OFF);
	/* DC = 0%*/
	pwm1Pins(pol);
}

void initPWM1(uint16_t prescaler, _t_pwm_pol pol)
{
	/* Override previous PWM init */
	initTimer1(prescaler, TMR_CLK_RISE, FAST_PWM, CLKOSC, INT_OFF);
	/* DC = 0%*/
	pwm1Pins(pol);

}

static void pwm1Pins(_t_pwm_pol pol)
{
	if (pol==PWM_NORMAL_A)
	{
		cbi(TCCR1A, COM1A0);
		sbi(TCCR1A, COM1A1);
		write16bit(0, &OCR1AH, &OCR1AL);
		sbi(DDRB, DDB1);
		cbi(PORTB, PORTB1); /* out low */
		pwmPol1A = pol;
	}
	else if (pol==PWM_INVERT_A)
	{
		sbi(TCCR1A, COM1A0);
		sbi(TCCR1A, COM1A1);
		write16bit(0, &OCR1AH, &OCR1AL);
		sbi(DDRB, DDB1);
		cbi(PORTB, PORTB1); /* out low */
		pwmPol1A = pol;
	}
	if (pol==PWM_NORMAL_B)
	{
		cbi(TCCR1A, COM1B0);
		sbi(TCCR1A, COM1B1);
		write16bit(0, &OCR1BH, &OCR1BL);
		sbi(DDRB, DDB2);
		cbi(PORTB, PORTB2); /* out low */
		pwmPol1A = pol;
	}
	else if (pol==PWM_INVERT_B)
	{
		sbi(TCCR1A, COM1B0);
		sbi(TCCR1A, COM1B1);
		write16bit(0, &OCR1BH, &OCR1BL);
		sbi(DDRB, DDB2);
		cbi(PORTB, PORTB2); /* out low */
		pwmPol1B = pol;
	}
	else
	{
		/* Usi futuri: funzionalità di toggle di OC0A*/
	}
}

void setDutyPWM1(uint16_t duty, uint8_t pwmType) 
{
	if (pwmType == PWM_A)
	{
		if (duty == 0)
		{
			/* Fix per glitch quando a 0% */
			pwm1Pins(PWM_INVERT_A);
			write16bit(0xffff, &OCR1AH, &OCR1AL);
		}
		else
		{
			pwm1Pins(pwmPol1A);
			write16bit(duty, &OCR1AH, &OCR1AL);
		}
		write16bit(duty, &OCR1AH, &OCR1AL);
	}
	else if (pwmType == PWM_B)
	{
		if (duty == 0)
		{
			/* Fix per glitch quando a 0% */
			pwm1Pins(PWM_INVERT_B);
			write16bit(0xffff, &OCR1BH, &OCR1BL);
		}
		else
		{
			pwm1Pins(pwmPol1B);
			write16bit(duty, &OCR1BH, &OCR1BL);
		}
	}
	else
	{
		
	}
}

#endif

#ifdef _timer2


#define TCR2A_BUSY_BIT 0x01
#define TCR2B_BUSY_BIT 0x00
#define OCR2A_BUSY_BIT 0x08
#define OCR2B_BUSY_BIT 0x04
#define TCNT2_BUSY_BIT 0x10

static uint8_t pwm2Pins(_t_pwm_pol pol);
//static uint8_t clk_source;
static uint16_t timeout;

uint8_t initTimer2(uint16_t prescaler,  _t_timer_mode mode ,  uint8_t timer_int, uint8_t clkSrc)
{
	//TCCR0A = 0;

	//TIMSK0 = 0;
	
	//clk_source = clkSrc;
	switch (clkSrc)
	{
		case TMR_EXT_XTAL:
		ASSR &= ~(1<<EXCLK);
		ASSR |= (1<AS2);
		break;

		case TMR_EXT_CLK:
		ASSR |= (1<<EXCLK);
		ASSR |= (1<AS2);
		break;

		case TMR_INT_CLK:
		ASSR &= ~(1<<EXCLK);
		ASSR &= ~(1<AS2);
		break;
	}

	timeout = 1000;
	while ((ASSR & TCR2A_BUSY_BIT) || (ASSR & TCR2B_BUSY_BIT))
	{
		timeout--;
		if (timeout == 0)
		{
			return 8;
		}
	}

	switch (mode)
	{
		case NORMAL:
		cbi(TCCR2A, WGM20);
		cbi(TCCR2A, WGM21);
		cbi(TCCR2B, WGM22);
		break;
		
		case PWM: /* TBD TOP = OCRA e TOP = ICR */
		/* TOP = 0xFF */
		sbi(TCCR2A, WGM20);
		cbi(TCCR2A, WGM21);
		cbi(TCCR2B, WGM22);

		
		break;
		
		case COMPARE_MATCH:
		cbi(TCCR2A, WGM20);
		sbi(TCCR2A, WGM21);
		cbi(TCCR2B, WGM22);

		break;
		
		case FAST_PWM:
		/* TOP = 0xff */
		sbi(TCCR2A, WGM20);
		sbi(TCCR2A, WGM21);
		cbi(TCCR2B, WGM22);
		break;
		
		default:
		cbi(TCCR2A, WGM20);
		cbi(TCCR2A, WGM21);
		cbi(TCCR2B, WGM22);	
		
		break;
	}

	timeout = 1000;
	while ((ASSR & TCR2A_BUSY_BIT) || (ASSR & TCR2B_BUSY_BIT))
	{
		timeout--;
		if (timeout == 0)
		{
			return 8;
		}
	}

	
	if (timer2presc != 0)
	{
		TCCR2B &= 0xF8; /* Clear CTS0 1 e 2 */
		timer2presc = prescaler; /* e lo usi per riapplicare il prescaler quanto fai lo start */
		switch (prescaler)
		{

			case 1:
			TCCR2B |= 1;
			break;
			
			case 8:
			TCCR2B |= 2;
			break;
			
            case 32:
            TCCR2B |= 3;
            break;

			case 64:
			TCCR2B |= 4;
			break;
			
			case 128:
			TCCR2B |= 5;
			break;
			
			case 256:
			TCCR2B |= 6;
			break;

            case 1024:
            TCCR2B |= 7;
            break;
			
			default: /* spento se valore errato */
			TCCR2B |= 0;
			break;
		}
	}
	else
	{
		TCCR2B &= 0xF8; /* Clear CTS0 1 e 2 */
		timer2presc = prescaler; /* e lo usi per riapplicare il prescaler quanto fai lo start */
	}
	
	
    switch (timer_int)
	{
		case INT_ON:
		if (mode == NORMAL)  /* Altrimenti setti interrupt nelle funzioni di compare init o pwm init */
		{
			sbi(TIMSK2, TOIE2);
		}
		break;
		case INT_OFF:
		cbi(TIMSK2, TOIE2);
		break;
		default:
		cbi(TIMSK2, TOIE2);
		break;
	}
	return 0;
}

/**@brief Chiamabile più volte per inizializzare moduli A e B */
uint8_t initCompare2 (uint16_t comp_val, _t_compare_sel comp, uint16_t prescaler, uint8_t timer_int,  uint8_t clkSrc)
{
	initTimer2(prescaler, COMPARE_MATCH, timer_int, clkSrc);
	timeout = 1000;
	while ((ASSR & TCR2A_BUSY_BIT) || (ASSR & TCR2B_BUSY_BIT) || (ASSR & OCR2A_BUSY_BIT) || (ASSR & OCR2B_BUSY_BIT))
	{
		timeout--;
		if (timeout == 0)
		{
			return 8;
		}
	}

	switch (comp)
	{
		case COMP_A_TOGGLE:
		sbi(TCCR2A, COM2A0);
		cbi(TCCR2A, COM2A1);
		sbi(DDRB, DDB3);
		OCR2A = comp_val;
		break;
		
		case COMP_A_CLEAR:
		cbi(TCCR2A, COM2A0);
		sbi(TCCR2A, COM2A1);
		sbi(DDRB, DDB3);
		OCR2A = comp_val;
		break;
		
		case COMP_A_SET:
		sbi(TCCR2A, COM2A0);
		sbi(TCCR2A, COM2A1);
		sbi(DDRB, DDB3);
		OCR2A = comp_val;
		break;
		
		case COMP_B_TOGGLE:
		sbi(TCCR2A, COM2B0);
		cbi(TCCR2A, COM2B1);
		sbi(DDRD, DDD3);
		OCR2B = comp_val;
		break;
		
		case COMP_B_CLEAR:
		cbi(TCCR2A, COM2B0);
		sbi(TCCR2A, COM2B1);
		sbi(DDRD, DDD3);
		OCR2B = comp_val;
		break;
		
		case COMP_B_SET:
		sbi(TCCR2A, COM2B0);
		sbi(TCCR2A, COM2B1);
		sbi(DDRD, DDD3);
		OCR2B = comp_val;
		break;
		
		case COMP_A_DISCONNECT:
		cbi(TCCR2A, COM2B0);
		cbi(TCCR2A, COM2B1);
		OCR2A = comp_val;
		break;
		
		case COMP_B_DISCONNECT:
		cbi(TCCR2A, COM2B0);
		cbi(TCCR2A, COM2B1);
		OCR2B = comp_val;
		break;
		
		default:
		/* lascia OCnx sconnesso come lo è da initTimer0*/
		cbi(TCCR2A, COM2B0);
		cbi(TCCR2A, COM2B1);
		cbi(TCCR2A, COM2A0);
		cbi(TCCR2A, COM2A1);
		break;
	}
	
	switch (timer_int)
	{
		case INT_ON:
		/* Non devo chiamare questa funzione se uso il normale */
		if ((comp == COMP_A_CLEAR) || (comp == COMP_A_SET) ||(comp == COMP_A_TOGGLE)||(comp == COMP_A_DISCONNECT))
		{
			sbi(TIMSK2, OCIE2A);
		}
		if ((comp == COMP_B_CLEAR) || (comp == COMP_B_SET) ||(comp == COMP_B_TOGGLE)||(comp == COMP_B_DISCONNECT))
		{
			sbi(TIMSK2, OCIE2B);
		}
		break;
		
		case INT_OFF:
		if ((comp == COMP_A_CLEAR) || (comp == COMP_A_SET) ||(comp == COMP_A_TOGGLE)||(comp == COMP_A_DISCONNECT))
		{
			cbi(TIMSK2, OCIE2A);
		}
		if ((comp == COMP_B_CLEAR) || (comp == COMP_B_SET) ||(comp == COMP_B_TOGGLE)||(comp == COMP_B_DISCONNECT))
		{
			cbi(TIMSK2, OCIE2B);
		}
		break;
		
		default:
		cbi(TIMSK2, OCIE2A);
		cbi(TIMSK2, OCIE2B);
		break;
	}
	return 0;
}

uint8_t writeTimer2(uint8_t val)
{
	timeout = 1000;
	while (ASSR & TCNT2_BUSY_BIT)
	{
		timeout--;
		if (timeout == 0)
		{
			return 8;
		}
	}
	val = TCNT2;
	return 0;
}

uint8_t startTimer2()
{
	timeout = 1000;
	while (ASSR & TCR2B_BUSY_BIT)
	{
		timeout--;
		if (timeout == 0)
		{
			return 8;
		}
	}

	TCCR2B &= 0xF8; /* Clear CTS0 1 e 2 */
	switch (timer2presc)
	{
		case 1:
        TCCR2B |= 1;
        break;
        
        case 8:
        TCCR2B |= 2;
        break;
        
        case 32:
        TCCR2B |= 3;
        break;

        case 64:
        TCCR2B |= 4;
        break;
        
        case 128:
        TCCR2B |= 5;
        break;
        
        case 256:
        TCCR2B |= 6;
        break;

        case 1024:
        TCCR2B |= 7;
        break;
        
        default: /* spento se valore errato */
        TCCR2B |= 0;
        break;
	}
	return 0;
}

uint8_t stopTimer2()
{
	timeout = 1000;
	while (ASSR & TCR2B_BUSY_BIT)
	{
		timeout--;
		if (timeout == 0)
		{
			return 8;
		}
	}
	TCCR2B &= 0xF8; /* Clear CTS0 1 e 2 */
	return 0;
}

void initSymPWM2(uint16_t prescaler, _t_pwm_pol pol, uint8_t clkSrc)
{
	/* Override previous PWM init */
	initTimer2(prescaler, PWM, INT_OFF, clkSrc);
	/* DC = 0%*/
	pwm2Pins(pol);
}

void initPWM2(uint16_t prescaler, _t_pwm_pol pol, uint8_t clkSrc)
{
	/* Override previous PWM init */
	initTimer2(prescaler, FAST_PWM, INT_OFF, clkSrc);
	/* DC = 0%*/
	if ((pol == PWM_NORMAL_A)||(pol == PWM_INVERT_A))
	{
		set_pwmPol2A = 0;
		sbi(DDRB, DDB3);
		cbi(PORTB, PORTB3); /* out low */
	}
	if ((pol == PWM_NORMAL_B)||(pol == PWM_INVERT_B))
	{
		set_pwmPol2B = 0;
		sbi(DDRD, DDD3);
		cbi(PORTD, PORTD3); /* out low */
	}		
	pwm2Pins(pol);
	if (pol == PWM_NORMAL_A)
	{
		setDutyPWM2(0, PWM_A);
	}
	if (pol == PWM_NORMAL_B)
	{
		setDutyPWM2(0, PWM_B);
	}
	if (pol == PWM_INVERT_A)
	{
		setDutyPWM2(0xff, PWM_A);
	}
	if (pol == PWM_INVERT_B)
	{
		setDutyPWM2(0xff, PWM_B);
	}
	startTimer2();

}

static uint8_t pwm2Pins(_t_pwm_pol pol)
{
	timeout = 1000;
	while ((ASSR & TCR2B_BUSY_BIT) || (ASSR & TCR2A_BUSY_BIT))
	{
		timeout--;
		if (timeout == 0)
		{
			return 8;
		}
	}
	if (pol==PWM_NORMAL_A)
	{
		cbi(TCCR2A, COM2A0);
		sbi(TCCR2A, COM2A1);
		OCR2A = 0;
		
		//sbi(DDRB, DDB3);
		//cbi(PORTB, PORTB3); /* out low */
		pwmPol2A = pol;
	}
	else if (pol==PWM_INVERT_A)
	{
		sbi(TCCR2A, COM2A0);
		sbi(TCCR2A, COM2A1);
		OCR2A = 0xff;
		
		//sbi(DDRB, DDB3);
		//cbi(PORTB, PORTB3); /* out low */
		pwmPol2A = pol;
	}
	if (pol==PWM_NORMAL_B)
	{
		
		cbi(TCCR2A, COM2B0);
		sbi(TCCR2A, COM2B1);
		OCR2B = 0;
		//sbi(DDRD, DDD3);
		//cbi(PORTD, PORTD3); /* out low */
		pwmPol2B = pol;
	}
	else if (pol==PWM_INVERT_B)
	{
		
		sbi(TCCR2A, COM2B0);
		sbi(TCCR2A, COM2B1);
		OCR2B = 0xff;
		//sbi(DDRD, DDD3);
		//cbi(PORTD, PORTD3); /* out low */
		pwmPol2B = pol;
	}
	else
	{
		/* Usi futuri: funzionalità di toggle di OC0A*/
	}
	return 0;
}

uint8_t setDutyPWM2(uint8_t duty, uint8_t pwmType)
{
	timeout = 1000;
	while ((ASSR & TCR2B_BUSY_BIT) || (ASSR & TCR2A_BUSY_BIT))
	{
		timeout--;
		if (timeout == 0)
		{
			return 8;
		}
	}
	
	if (pwmType == PWM_A)
	{
		if (duty == 0)
		{
			if (set_pwmPol2A==0)
			{
				/* Fix per glitch quando a 0% */
				temp_pwmPol2A = pwmPol2A;
				set_pwmPol2A = 1;
					
				if (pwmPol2A == PWM_NORMAL_A)
				{	
					pwm2Pins(PWM_INVERT_A);		
				}
				else if (pwmPol2A == PWM_INVERT_A)
				{
					pwm2Pins(PWM_NORMAL_A);		
				}
				else
				{
					
				}
			}
			hasPinSet2A = 0;
			OCR2A = 0xff;
		}
		else
		{
			if (hasPinSet2A == 0)
			{
				if (set_pwmPol2A == 1) // Se aveva settato un duty = 0 prima
				{
					set_pwmPol2A = 0;
					pwm2Pins(temp_pwmPol2A);
				}
				else
				{
					pwm2Pins(pwmPol2A);
				}
				hasPinSet2A = 1;	
			}	
			OCR2A = duty;
		}			
	}
	else if (pwmType == PWM_B)
	{
		if (duty == 0)
		{
			if (set_pwmPol2B==0)
			{
				/* Fix per glitch quando a 0% */
				temp_pwmPol2B = pwmPol2B;
				set_pwmPol2B = 1;
					
				if (pwmPol2B == PWM_NORMAL_B)
				{	
					pwm2Pins(PWM_INVERT_B);		
				}
				else if (pwmPol2B == PWM_INVERT_B)
				{
					pwm2Pins(PWM_NORMAL_B);		
				}
				else
				{
					
				}
			}
			hasPinSet2B = 0;
			OCR2B = 0xff;
		}
		else
		{
			if (hasPinSet2B == 0)
			{
				if (set_pwmPol2B == 1) // Se aveva settato un duty = 0 prima
				{
					set_pwmPol2B = 0;
					pwm2Pins(temp_pwmPol2B);
				}
				else
				{
					pwm2Pins(pwmPol2B);
				}
				hasPinSet2B = 1;	
			}	
			OCR2B = duty;
		}
	}		
	else
	{
		
	}
	return 0;
}

#endif