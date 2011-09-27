/*
  wiring.c - Partial implementation of the Wiring API for the ATmega8.
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis
  Modified for AT90USB by Opendous Inc. - 2009-08-30

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  $Id: wiring.c 585 2009-05-12 10:55:26Z dmellis $
*/

#include "wiring_private.h"

/* let FreeRTOS handle delays properly */
#define delayMicroseconds(us)		vTaskDelay(us);


// the prescaler is set so that timer0 ticks every 64 clock cycles, and the
// the overflow handler is called every 256 ticks.
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))

// the whole number of milliseconds per timer0 overflow
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)

// the fractional number of milliseconds per timer0 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

volatile unsigned long timer0_overflow_count = 0;
volatile unsigned long timer0_millis = 0;
static unsigned char timer0_fract = 0;

ISR(TIMER0_OVF_vect)
{
	// copy these to local variables so they can be stored in registers
	// (volatile variables must be read from memory on every access)
	unsigned long m = timer0_millis;
	unsigned char f = timer0_fract;

	m += MILLIS_INC;
	f += FRACT_INC;
	if (f >= FRACT_MAX) {
		f -= FRACT_MAX;
		m += 1;
	}

	timer0_fract = f;
	timer0_millis = m;
	timer0_overflow_count++;
}

unsigned long millis()
{
	unsigned long m;
	uint8_t oldSREG = SREG;

	// disable interrupts while we read timer0_millis or we might get an
	// inconsistent value (e.g. in the middle of a write to timer0_millis)
	cli();
	m = timer0_millis;
	SREG = oldSREG;

	return m;
}

unsigned long micros() {
	unsigned long m, t;
	uint8_t oldSREG = SREG;

	cli();	
	t = TCNT0;

#ifdef TIFR0
	if ((TIFR0 & _BV(TOV0)) && (t == 0))
		t = 256;
#else
	if ((TIFR & _BV(TOV0)) && (t == 0))
		t = 256;
#endif

	m = timer0_overflow_count;
	SREG = oldSREG;
	
	return ((m << 8) + t) * (64 / clockCyclesPerMicrosecond());
}

//void delay(unsigned long ms)
//{
//	unsigned long start = millis();
//	while (millis() - start <= ms)
//		;
//}


void init()
{
	// this needs to be called before setup() or some functions won't
	// work there
	sei();

	// timer 0 is also used for fast hardware pwm
	TCCR0A = ((1 << WGM01) | (1 << WGM00));

	// set timer 0 prescale factor to 64
	TCCR0B = ((1 << CS01) | (1 << CS00));

	// enable timer 0 overflow interrupt
	TIMSK0 = (1 << TOIE0);


	// timer 1 is used by FreeRTOS in CTC
	// timer 3 is used for phase-correct hardware pwm
	// this is better for motors as it ensures an even waveform
	// note, however, that fast pwm mode can achieve a frequency of up
	// 8 MHz (with a 16 MHz clock) at 50% duty cycle

	// set timer 3 prescale factor to 64
	TCCR3B = ((1 << CS31) | (1 << CS30));

	// put timer 3 in 8-bit phase correct pwm mode
	TCCR3A = (1 << WGM30);

	// if there is a Timer2, prescale by 64 and use for phase correct pwm (8-bit)
	#if defined(TCCR2A)
		TCCR2B = (1 << CS22);
		TCCR2A = (1 << WGM20);
	#endif

	// if there is a Timer4, enable fast pwm with CLKpck/128 prescaling
	#if defined(TCCR4A)
		TCCR4A = ((1 << PWM4A) | (1 << PWM4B));
		TCCR4B = ((1 << PWM4X) | (1 << CS43));
		TCCR4C = (1 << PWM4D);
		TCCR4D = (1 << WGM40); // Phase and Frequency Correct PWM
	#endif

	// set A2D prescale factor
	// 16 MHz / 128 = 125 kHz, inside the desired 50-200 KHz range.
	// 8 MHz / 64 = 125 kHz, inside the desired 50-200 KHz range.
	#if (F_CLOCK == 16000000) // 16MHz
		ADCSRA = ((1 << ADEN) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2));
	#else // 8Mhz
		ADCSRA = ((1 << ADEN) | (1 << ADPS1) | (1 << ADPS2));
	#endif

}