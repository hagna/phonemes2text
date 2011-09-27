/*
  pins_micropendous.c - pin definitions for Micropendous-based Arduino-like boards
  Part of LUFAduino / Arduino / Wiring Lite

  Copyright (c) 2005 David A. Mellis

  Adapted for LUFA + Arduino by Opendous Inc. 2010-12
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

  $Id$
*/

/*  Notes:	Timer1 cannot be used by the larger AVRs (ATmega32U4, AT90USB???6/7)
				as it is used by FreeRTOS.  Timer1 is enabled on the AT90USB162 as that
				IC cannot support FreeRTOS (not enough SRAM).
*/

#include <avr/io.h>
#include "wiring_private.h"
#include "pins_micropendous.h"

#define PA 1
#define PB 2
#define PC 3
#define PD 4
#define PE 5
#define PF 6

#define REPEAT8(x) x, x, x, x, x, x, x, x
#define BV0TO7 _BV(0), _BV(1), _BV(2), _BV(3), _BV(4), _BV(5), _BV(6), _BV(7)
#define BV7TO0 _BV(7), _BV(6), _BV(5), _BV(4), _BV(3), _BV(2), _BV(1), _BV(0)

#define HWB_PIN	16

/** Arduino Definitions for 64-pin USB AVRs */

#if (defined(__AVR_AT90USB1287__) || defined(__AVR_AT90USB647__) ||  \
		defined(__AVR_AT90USB1286__) || defined(__AVR_AT90USB646__) ||  \
		defined(__AVR_ATmega32U6__))

const uint16_t PROGMEM port_to_mode_PGM[] = {
	NOT_A_PORT,
	(uint16_t)&DDRA,
	(uint16_t)&DDRB,
	(uint16_t)&DDRC,
	(uint16_t)&DDRD,
	(uint16_t)&DDRE,
	(uint16_t)&DDRF,
};

const uint16_t PROGMEM port_to_output_PGM[] = {
	NOT_A_PORT,
	(uint16_t)&PORTA,
	(uint16_t)&PORTB,
	(uint16_t)&PORTC,
	(uint16_t)&PORTD,
	(uint16_t)&PORTE,
	(uint16_t)&PORTF,
};

const uint16_t PROGMEM port_to_input_PGM[] = {
	NOT_A_PIN,
	(uint16_t)&PINA,
	(uint16_t)&PINB,
	(uint16_t)&PINC,
	(uint16_t)&PIND,
	(uint16_t)&PINE,
	(uint16_t)&PINF,
};

const uint8_t PROGMEM digital_pin_to_port_PGM[] = {
	// PORTLIST		
	// -------------------------------------------		
	PD	, // PD 2 ** 0 ** UART_RX, INT2
	PD	, // PD 3 ** 1** UART_TX, INT3
	PD	, // PD 4 ** 2 ** 
	PD	, // PD 5 ** 3 ** 
	PD	, // PD 1 ** 4 ** PWM, I2C_SDA, INT1
	PD	, // PD 0 ** 5 ** PWM, I2C_SCL, INT0
	PD	, // PD 6 ** 6 ** 
	PD	, // PD 7 ** 7 ** 
	PB	, // PB 4 ** 8 ** PWM
	PB	, // PB 5 ** 9 ** PWM
	PB	, // PB 0 ** 10 ** SPI_SS
	PB	, // PB 2 ** 11 ** SPI_MOSI
	PB	, // PB 3 ** 12 ** SPI_MISO
	PB	, // PB 1 ** 13 ** SPI_CLK
	PB	, // PB 6 ** 14 ** 
	PB	, // PB 7 ** 15 ** 
	PE	, // PE 2 ** 16 ** HWB, SRAM
	PF	, // PF 0 ** 17 ** AI0
	PF	, // PF 1 ** 18 ** AI1
	PF	, // PF 2 ** 19 ** AI2
	PF	, // PF 3 ** 20 ** AI3
	PF	, // PF 4 ** 21 ** AI4
	PF	, // PF 5 ** 22 ** AI5
	PF	, // PF 6 ** 23 ** AI6
	PF	, // PF 7 ** 24 ** AI7
};

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[] = {
	// PIN TO PORT LIST		
	// -------------------------------------------		
	_BV( 2 )	, // PD 2 ** 0 ** UART_RX, INT2
	_BV( 3 )	, // PD 3 ** 1** UART_TX, INT3
	_BV( 4 )	, // PD 4 ** 2 ** 
	_BV( 5 )	, // PD 5 ** 3 ** 
	_BV( 1 )	, // PD 1 ** 4 ** PWM, I2C_SDA, INT1
	_BV( 0 )	, // PD 0 ** 5 ** PWM, I2C_SCL, INT0
	_BV( 6 )	, // PD 6 ** 6 ** 
	_BV( 7 )	, // PD 7 ** 7 ** 
	_BV( 4 )	, // PB 4 ** 8 ** PWM
	_BV( 5 )	, // PB 5 ** 9 ** PWM
	_BV( 0 )	, // PB 0 ** 10 ** SPI_SS
	_BV( 2 )	, // PB 2 ** 11 ** SPI_MOSI
	_BV( 3 )	, // PB 3 ** 12 ** SPI_MISO
	_BV( 1 )	, // PB 1 ** 13 ** SPI_CLK
	_BV( 6 )	, // PB 6 ** 14 ** 
	_BV( 7 )	, // PB 7 ** 15 ** 
	_BV( 2 )	, // PE 2 ** 16 ** HWB, SRAM
	_BV( 0 )	, // PF 0 ** 17 ** AI0
	_BV( 1 )	, // PF 1 ** 18 ** AI1
	_BV( 2 )	, // PF 2 ** 19 ** AI2
	_BV( 3 )	, // PF 3 ** 20 ** AI3
	_BV( 4 )	, // PF 4 ** 21 ** AI4
	_BV( 5 )	, // PF 5 ** 22 ** AI5
	_BV( 6 )	, // PF 6 ** 23 ** AI6
	_BV( 7 )	, // PF 7 ** 24 ** AI7
};                     

const uint8_t PROGMEM digital_pin_to_timer_PGM[] = {
	// TIMERS		
	// -------------------------------------------		
	NOT_ON_TIMER	, // PD 2 ** 0 ** UART_RX, INT2
	NOT_ON_TIMER	, // PD 3 ** 1** UART_TX, INT3
	NOT_ON_TIMER	, // PD 4 ** 2 ** 
	NOT_ON_TIMER	, // PD 5 ** 3 ** 
	TIMER2B	, // PD 1 ** 4 ** PWM, I2C_SDA, INT1
	TIMER0B	, // PD 0 ** 5 ** PWM, I2C_SCL, INT0
	NOT_ON_TIMER	, // PD 6 ** 6 ** 
	NOT_ON_TIMER	, // PD 7 ** 7 ** 
	TIMER2A	, // PB 4 ** 8 ** PWM
	TIMER1A	, // PB 5 ** 9 ** PWM
	NOT_ON_TIMER	, // PB 0 ** 10 ** SPI_SS
	NOT_ON_TIMER	, // PB 2 ** 11 ** SPI_MOSI
	NOT_ON_TIMER	, // PB 3 ** 12 ** SPI_MISO
	NOT_ON_TIMER	, // PB 1 ** 13 ** SPI_CLK
	NOT_ON_TIMER	, // PB 6 ** 14 ** 
	NOT_ON_TIMER	, // PB 7 ** 15 ** 
	NOT_ON_TIMER	, // PE 2 ** 16 ** HWB, SRAM
	NOT_ON_TIMER	, // PF 0 ** 17 ** AI0
	NOT_ON_TIMER	, // PF 1 ** 18 ** AI1
	NOT_ON_TIMER	, // PF 2 ** 19 ** AI2
	NOT_ON_TIMER	, // PF 3 ** 20 ** AI3
	NOT_ON_TIMER	, // PF 4 ** 21 ** AI4
	NOT_ON_TIMER	, // PF 5 ** 22 ** AI5
	NOT_ON_TIMER	, // PF 6 ** 23 ** AI6
	NOT_ON_TIMER	, // PF 7 ** 24 ** AI7
};                                
#endif



/** Arduino Definitions for 44-pin USB AVRs */

#if (defined(__AVR_ATmega16U4__)  || defined(__AVR_ATmega32U4__))

const uint16_t PROGMEM port_to_mode_PGM[] = {
	NOT_A_PORT,
	NOT_A_PORT,
	(uint16_t)&DDRB,
	(uint16_t)&DDRC,
	(uint16_t)&DDRD,
	(uint16_t)&DDRE,
	(uint16_t)&DDRF,
};

const uint16_t PROGMEM port_to_output_PGM[] = {
	NOT_A_PORT,
	NOT_A_PORT,
	(uint16_t)&PORTB,
	(uint16_t)&PORTC,
	(uint16_t)&PORTD,
	(uint16_t)&PORTE,
	(uint16_t)&PORTF,
};

const uint16_t PROGMEM port_to_input_PGM[] = {
	NOT_A_PORT,
	NOT_A_PORT,
	(uint16_t)&PINB,
	(uint16_t)&PINC,
	(uint16_t)&PIND,
	(uint16_t)&PINE,
	(uint16_t)&PINF,
};

const uint8_t PROGMEM digital_pin_to_port_PGM[] = {
	PD, // PD 2 ** 0 ** UART_RX, INT2
	PD, // PD 3 ** 1 ** UART_TX, INT3
	PD, // PD 4 ** 2 ** AI8
	PD, // PD 5 ** 3 ** 
	PD, // PD 1 ** 4 ** I2C_SDA, INT1
	PD, // PD 0 ** 5 ** PWM, I2C_SCL, INT0
	PD, // PD 6 ** 6 ** !PWMXD, AI9
	PD, // PD 7 ** 7 ** PWMXD, AI10
	PB, // PB 4 ** 8 ** AI11
	PB, // PB 5 ** 9 ** AI12
	PB, // PB 0 ** 10 ** SPI_SS
	PB, // PB 2 ** 11 ** SPI_MOSI
	PB, // PB 3 ** 12 ** SPI_MISO
	PB, // PB 1 ** 13 ** SPI_CLK
	PB, // PB 6 ** 14 ** PWM, AI13
	PB, // PB 7 ** 15 ** PWM
	PE, // PE 2 ** 16 ** HWB
	PF, // PF 0 ** 17 ** AI0
	PF, // PF 1 ** 18 ** AI1
	PC, // PC 6 ** 19 ** PWM
	PC, // PC 7 ** 20 ** PWM
	PF, // PF 4 ** 21 ** AI4
	PF, // PF 5 ** 22 ** AI5
	PF, // PF 6 ** 23 ** AI6
	PF, // PF 7 ** 24 ** AI7
	PE, // PE 6 ** 25 ** INT6
};

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[] = {
	_BV( 2 )	, // PD 2 ** 0 ** UART_RX, INT2
	_BV( 3 )	, // PD 3 ** 1 ** UART_TX, INT3
	_BV( 4 )	, // PD 4 ** 2 ** AI8
	_BV( 5 )	, // PD 5 ** 3 ** 
	_BV( 1 )	, // PD 1 ** 4 ** I2C_SDA, INT1
	_BV( 0 )	, // PD 0 ** 5 ** PWM, I2C_SCL, INT0
	_BV( 6 )	, // PD 6 ** 6 ** !PWMXD, AI9
	_BV( 7 )	, // PD 7 ** 7 ** PWMXD, AI10
	_BV( 4 )	, // PB 4 ** 8 ** AI11
	_BV( 5 )	, // PB 5 ** 9 ** AI12
	_BV( 0 )	, // PB 0 ** 10 ** SPI_SS
	_BV( 2 )	, // PB 2 ** 11 ** SPI_MOSI
	_BV( 3 )	, // PB 3 ** 12 ** SPI_MISO
	_BV( 1 )	, // PB 1 ** 13 ** SPI_CLK
	_BV( 6 )	, // PB 6 ** 14 ** PWM, AI13
	_BV( 7 )	, // PB 7 ** 15 ** PWM
	_BV( 2 )	, // PE 2 ** 16 ** HWB
	_BV( 0 )	, // PF 0 ** 17 ** AI0
	_BV( 1 )	, // PF 1 ** 18 ** AI1
	_BV( 6 )	, // PC 6 ** 19 ** PWM
	_BV( 7 )	, // PC 7 ** 20 ** PWM
	_BV( 4 )	, // PF 4 ** 21 ** AI4
	_BV( 5 )	, // PF 5 ** 22 ** AI5
	_BV( 6 )	, // PF 6 ** 23 ** AI6
	_BV( 7 )	, // PF 7 ** 24 ** AI7
	_BV( 6 )	, // PE 6 ** 25 ** INT6
};                    

const uint8_t PROGMEM digital_pin_to_timer_PGM[] = {
	// TIMERS		
	// -------------------------------------------
	NOT_ON_TIMER, // PD 2 ** 0 ** UART_RX, INT2
	NOT_ON_TIMER, // PD 3 ** 1 ** UART_TX, INT3
	NOT_ON_TIMER, // PD 4 ** 2 ** AI8
	NOT_ON_TIMER, // PD 5 ** 3 ** 
	NOT_ON_TIMER, // PD 1 ** 4 ** I2C_SDA, INT1
	TIMER0B, // PD 0 ** 5 ** PWM, I2C_SCL, INT0
	TIMER4D, // PD 6 ** 6 ** !PWMXD, AI9
	TIMER4D, // PD 7 ** 7 ** PWMXD, AI10
	NOT_ON_TIMER, // PB 4 ** 8 ** AI11
	NOT_ON_TIMER, // PB 5 ** 9 ** AI12
	NOT_ON_TIMER, // PB 0 ** 10 ** SPI_SS
	NOT_ON_TIMER, // PB 2 ** 11 ** SPI_MOSI
	NOT_ON_TIMER, // PB 3 ** 12 ** SPI_MISO
	NOT_ON_TIMER, // PB 1 ** 13 ** SPI_CLK
	TIMER4B, // PB 6 ** 14 ** PWM, AI13
	TIMER0A, // PB 7 ** 15 ** PWM
	NOT_ON_TIMER, // PE 2 ** 16 ** HWB
	NOT_ON_TIMER, // PF 0 ** 17 ** AI0
	NOT_ON_TIMER, // PF 1 ** 18 ** AI1
	TIMER3A, // PC 6 ** 19 ** PWM
	TIMER4A, // PC 7 ** 20 ** PWM
	NOT_ON_TIMER, // PF 4 ** 21 ** AI4
	NOT_ON_TIMER, // PF 5 ** 22 ** AI5
	NOT_ON_TIMER, // PF 6 ** 23 ** AI6
	NOT_ON_TIMER, // PF 7 ** 24 ** AI7
	NOT_ON_TIMER, // PE 6 ** 25 ** INT6
};                            
#endif
