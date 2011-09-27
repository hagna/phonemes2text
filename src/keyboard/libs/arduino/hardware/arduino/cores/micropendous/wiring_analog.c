/*
  wiring_analog.c - analog input and output
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis

  Adapted for LUFA + Arduino by Opendous Inc. 2009-09
  For more information visit:  www.Micropendous.org/LUFAduino

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

  $Id: wiring.c 248 2007-02-03 15:36:30Z mellis $
*/

/*  Notes:	Timer1 cannot be used by the larger AVRs (ATmega32U4, AT90USB???6/7)
				as it is used by FreeRTOS.  Timer1 is enabled on the AT90USB162 as that
				IC cannot support FreeRTOS (not enough SRAM).
*/

#include "wiring_private.h"
#include "pins_micropendous.h"
#include <LUFA/Drivers/Peripheral/ADC.h>

uint8_t analog_reference = DEFAULT;

void analogReference(uint8_t mode)
{
	// can't actually set the register here because the default setting
	// will connect AVCC and the AREF pin, which would cause a short if
	// there's something connected to AREF.
	analog_reference = mode;
}

int analogRead(uint8_t pin)
{
	uint8_t muxMask = 0;
	
	// select the corresponding Channel - e.g., 3 = AI3 = ADC3 = CH3
	ADC_SetupChannel(pin);
	
	// set the analog reference (high two bits of ADMUX) and select the
	// Channel (low 4 bits).  this also sets ADLAR (left-adjust result)
	// to 0 (the default).
	muxMask = ((analog_reference << 6) | pin);

	//ADCSRB = 0;	// select Free-Running mode

	// complete a single reading and return the result - busy-loops
	return (int)ADC_GetChannelReading(muxMask);
}


// Right now, PWM output only works on the pins with
// hardware support.  These are defined in the appropriate
// pins_*.c file.  For the rest of the pins, we default
// to digital output.
// note timers are disabled in wiring_digital.c
void analogWrite(uint8_t pin, int val)
{
	// We need to make sure the PWM output is enabled for those pins
	// that support it, as we turn it off when digitally reading or
	// writing with them.  Also, make sure the pin is in output mode
	// for consistenty with Wiring, which doesn't require a pinMode
	// call for the analog output pins.
	pinMode(pin, OUTPUT);

	if (val == 0) {
		digitalWrite(pin, LOW);
		return;
	} else if (val == 255) {
		digitalWrite(pin, HIGH);
		return;
	} else {
		// proceed
	}

	// set up PWM accordingly for the chosen pin
			if (digitalPinToTimer(pin) == TIMER0B) {
		sbi(TCCR0A, COM0B1); // connect PWM to pin on Timer 0, Channel B
		OCR0B = val; // set PWM duty
	/* // Cannot use Timer1 as it is used by FreeRTOS
	} else if (digitalPinToTimer(pin) == TIMER1A) {
		sbi(TCCR1C, FOC1A); // force output compare as Timer1 is used by FreeRTOS
		sbi(TCCR1A, COM1A1); // connect PWM to pin on Timer 1, Channel A
		OCR1A = val; // set PWM duty
	} else if (digitalPinToTimer(pin) == TIMER1B) {
		sbi(TCCR1C, FOC1B);// force output compare as Timer1 is used by FreeRTOS
		sbi(TCCR1A, COM1B1); // connect PWM to pin on Timer 1, Channel B
		OCR1B = val; // set PWM duty
	} else if (digitalPinToTimer(pin) == TIMER1C) {
		sbi(TCCR1C, FOC1C);// force output compare as Timer1 is used by FreeRTOS
		sbi(TCCR1A, COM1C1); // connect PWM to pin on Timer 1, Channel B
		OCR1C = val; // set PWM duty
	*/

	#if (defined(__AVR_AT90USB162__)  || defined(__AVR_AT90USB82__))
	} else if (digitalPinToTimer(pin) == TIMER0A) {
		sbi(TCCR0A, COM0A1); // connect PWM to pin on Timer 0, Channel A
		OCR0A = val; // set PWM duty
	} else if (digitalPinToTimer(pin) == TIMER1A) {
		sbi(TCCR1A, COM1A1); // connect PWM to pin on Timer 1, Channel A
		OCR1A = val; // set PWM duty
	} else if (digitalPinToTimer(pin) == TIMER1B) {
		sbi(TCCR1A, COM1B1); // connect PWM to pin on Timer 1, Channel B
		OCR1B = val; // set PWM duty
	#endif


	#if (defined(__AVR_ATmega16U4__)  || defined(__AVR_ATmega32U4__))
	} else if (digitalPinToTimer(pin) == TIMER0A) {
		sbi(TCCR0A, COM0A1); // connect PWM to pin on Timer 0, Channel A
		OCR0A = val; // set PWM duty
	} else if (digitalPinToTimer(pin) == TIMER3A) {
		sbi(TCCR3A, COM3A1); // connect PWM to pin on Timer 3, Channel B
		OCR3A = val; // set PWM duty
	} else if (digitalPinToTimer(pin) == TIMER4A) {
		sbi(TCCR4A, COM4A1); // connect PWM to pin on Timer 4, Channel A
		OCR4A = val; // set PWM duty
	} else if (digitalPinToTimer(pin) == TIMER4B) {
		sbi(TCCR4A, COM4B1); // connect PWM to pin on Timer 4, Channel B
		OCR4B = val; // set PWM duty
	} else if (digitalPinToTimer(pin) == TIMER4D) {
		sbi(TCCR4C, COM4D0); // connect PWM11 and !PWM11 to pin on Timer 4, Channel D
		OCR4D = val; // set PWM duty
	#endif


	#if (defined(__AVR_AT90USB1287__) || defined(__AVR_AT90USB647__) ||  \
			defined(__AVR_AT90USB1286__) || defined(__AVR_AT90USB646__) ||  \
			defined(__AVR_ATmega32U6__))
	} else if (digitalPinToTimer(pin) == TIMER2A) {
		sbi(TCCR2A, COM2A1); // connect PWM to pin on Timer 2, Channel A
		OCR2A = val; // set PWM duty
	} else if (digitalPinToTimer(pin) == TIMER2B) {
		sbi(TCCR2A, COM2B1); // connect PWM to pin on Timer 2, Channel B
		OCR2B = val; // set PWM duty
	} else if (digitalPinToTimer(pin) == TIMER3A) {
		sbi(TCCR3A, COM3A1); // connect PWM to pin on Timer 3, Channel A
		OCR3A = val; // set PWM duty
	} else if (digitalPinToTimer(pin) == TIMER3B) {
		sbi(TCCR3A, COM3B1); // connect PWM to pin on Timer 3, Channel B
		OCR3B = val; // set PWM duty
	} else if (digitalPinToTimer(pin) == TIMER3C) {
		sbi(TCCR3A, COM3C1); // connect PWM to pin on Timer 3, Channel C
		OCR3C = val; // set PWM duty
	#endif

	} else {
		digitalWrite(pin, HIGH);
	}
}
