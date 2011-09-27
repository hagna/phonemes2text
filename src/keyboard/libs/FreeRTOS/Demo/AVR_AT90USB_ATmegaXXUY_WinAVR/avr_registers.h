/*	
	By Opendous Inc. - 2010-03-04
	For more info visit www.Micropendous.org/USBVirtualSerial_FreeRTOS

	Translates ATmega323 register/vector and other definitions to the
	AT90USB/ATmegaXXUY USB AVRs and possibly other modern AVRs
	with similar features (e.g., available UART must be UART1).

	This file is part of the AVR_AT90USB_ATmegaXXUY_WinAVR port
	of the FreeRTOS distribution.

	FreeRTOS is free software; you can redistribute it and/or modify it	under 
	the terms of the GNU General Public License (version 2) as published by the 
	Free Software Foundation and modified by the FreeRTOS exception.
	**NOTE** The exception to the GPL is included to allow you to distribute a
	combined work that includes FreeRTOS without being obliged to provide the 
	source code for proprietary components outside of the FreeRTOS kernel.  
	Alternative commercial license and support terms are also available upon 
	request.  See the licensing section of http://www.FreeRTOS.org for full 
	license details.

	FreeRTOS is distributed in the hope that it will be useful,	but WITHOUT
	ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
	FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
	more details.

	You should have received a copy of the GNU General Public License along
	with FreeRTOS; if not, write to the Free Software Foundation, Inc., 59
	Temple Place, Suite 330, Boston, MA  02111-1307  USA.
*/

#ifndef _ARCH_AVR_H_
#define _ARCH_AVR_H_

#define sig_INTERRUPT0 INT0_vect
#define sig_INTERRUPT1 INT1_vect
#define sig_INTERRUPT2 INT2_vect
#define sig_INTERRUPT3 INT3_vect
#define sig_INTERRUPT4 INT4_vect
#define sig_INTERRUPT5 INT5_vect
#define sig_INTERRUPT6 INT6_vect
#define sig_INTERRUPT7 INT7_vect
#define sig_OUTPUT_COMPARE2 TIMER2_COMPA_vect
#define sig_OVERFLOW2 TIMER2_OVF_vect
#define sig_INPUT_CAPTURE1 TIMER1_CAPT_vect
#define sig_OUTPUT_COMPARE1A TIMER1_COMPA_vect
#define sig_OUTPUT_COMPARE1B TIMER1_COMPB_vect
#define sig_OUTPUT_COMPARE1C TIMER1_COMPC_vect
#define sig_OVERFLOW1 TIMER1_OVF_vect
#define sig_OUTPUT_COMPARE0 TIMER0_COMPA_vect
#define sig_OVERFLOW0 TIMER0_OVF_vect
#define sig_SPI SPI_STC_vect
#define sig_UART0_TRANS INT5_vect // No UART0, so make it an INT to suppress compile errors
#define sig_UART0_DATA INT6_vect // No UART0, so make it an INT to suppress compile errors
#define sig_UART0_RECV INT7_vect // No UART0, so make it an INT to suppress compile errors
#define sig_UART1_TRANS USART1_TX_vect
#define sig_UART1_DATA USART1_UDRE_vect
#define sig_UART1_RECV USART1_RX_vect
#define sig_ADC ADC_vect
#define sig_EEPROM_READY EE_READY_vect
#define sig_2WIRE_SERIAL TWI_vect
#define sig_INPUT_CAPTURE3 TIMER3_CAPT_vect
#define sig_OUTPUT_COMPARE3A TIMER3_COMPA_vect
#define sig_OUTPUT_COMPARE3B TIMER3_COMPB_vect
#define sig_OUTPUT_COMPARE3C TIMER3_COMPC_vect
#define sig_OVERFLOW3 TIMER3_OVF_vect
#define sig_SPM_READY SPM_READY_vect

#define SIG_UART_RECV USART1_RX_vect
#define SIG_UART_DATA USART1_UDRE_vect


#define SIG_INTERRUPT0 sig_INTERRUPT0
#define SIG_INTERRUPT1 sig_INTERRUPT1
#define SIG_INTERRUPT2 sig_INTERRUPT2
#define SIG_INTERRUPT3 sig_INTERRUPT3
#define SIG_INTERRUPT4 sig_INTERRUPT4
#define SIG_INTERRUPT5 sig_INTERRUPT5
#define SIG_INTERRUPT6 sig_INTERRUPT6
#define SIG_INTERRUPT7 sig_INTERRUPT7
#define SIG_OUTPUT_COMPARE2 sig_OUTPUT_COMPARE2
#define SIG_OVERFLOW2 sig_OVERFLOW2
#define SIG_INPUT_CAPTURE1 sig_INPUT_CAPTURE1
#define SIG_OUTPUT_COMPARE1A sig_OUTPUT_COMPARE1A
#define SIG_OUTPUT_COMPARE1B sig_OUTPUT_COMPARE1B
#define SIG_OUTPUT_COMPARE1C sig_OUTPUT_COMPARE1C
#define SIG_OVERFLOW1 sig_OVERFLOW1
#define SIG_OUTPUT_COMPARE0 sig_OUTPUT_COMPARE0
#define SIG_OVERFLOW0 sig_OVERFLOW0
#define SIG_SPI sig_SPI
#define SIG_UART0_TRANS sig_UART0_TRANS
#define SIG_UART0_DATA sig_UART0_DATA
#define SIG_UART0_RECV sig_UART0_RECV
#define SIG_UART1_TRANS sig_UART1_TRANS
#define SIG_UART1_DATA sig_UART1_DATA
#define SIG_UART1_RECV sig_UART1_RECV
#define SIG_ADC sig_ADC
#define SIG_EEPROM_READY sig_EEPROM_READY
#define SIG_2WIRE_SERIAL sig_2WIRE_SERIAL
#define SIG_INPUT_CAPTURE3 sig_INPUT_CAPTURE3
#define SIG_OUTPUT_COMPARE3A sig_OUTPUT_COMPARE3A
#define SIG_OUTPUT_COMPARE3B sig_OUTPUT_COMPARE3B
#define SIG_OUTPUT_COMPARE3C sig_OUTPUT_COMPARE3C
#define SIG_OVERFLOW3 sig_OVERFLOW3
#define SIG_SPM_READY sig_SPM_READY




#ifndef UDR
#define UDR     UDR1
#endif
#ifndef USR
#define USR     UCSR1A
#endif
#ifndef UCR
#define UCR     UCSR1B
#endif
#ifndef EICR
#define EICR    EICRB
#endif
#ifndef RXC
#define RXC     RXC1
#endif
#ifndef UDRE
#define UDRE    UDRE1
#endif
#ifndef FE
#define FE      FE1
#endif
#ifndef DOR
#define DOR     DOR1
#endif
#ifndef RXCIE
#define RXCIE   RXCIE1
#endif
#ifndef TXCIE
#define TXCIE   TXCIE1
#endif
#ifndef UDRIE
#define UDRIE   UDRIE1
#endif
#ifndef RXEN
#define RXEN    RXEN1
#endif
#ifndef TXEN
#define TXEN    TXEN1
#endif

#ifndef ADCW
#define ADCW    ADC
#endif
#ifndef ADCSR
#define ADCSR   ADCSRA
#endif
#ifndef ADFR
#define ADFR    ADATE
#endif
#ifndef OCIE0
#define OCIE0   OCIE0A
#endif
#ifndef TCCR0
#define TCCR0   TCCR0A
#endif
#ifndef TCCR2
#define TCCR2   TCCR2A
#endif
#ifndef OCR0
#define OCR0    OCR0A
#endif
#ifndef TIMSK
#define TIMSK   TIMSK1
#endif
#ifndef TIFR
#define TIFR   TIFR1
#endif

#ifndef MCUCSR
#define MCUCSR   MCUCR
#endif

#ifndef SRW
#define SRW   MCUCR
#endif


#ifndef RAMPZ
#define RAMPZ   _SFR_IO8(0x3B)
#endif


#ifndef AS0
#define AS0 AS2
#endif

#ifndef TCN0UB
#define TCN0UB  TCN2UB
#endif

#ifndef OCR0UB
#define OCR0UB  OCR2AUB
#endif

#ifndef TCR0UB
#define TCR0UB  TCR2AUB
#endif

#ifndef OCF0
#define OCF0    OCF1A
#endif


#ifndef TXC
#define TXC    TXC1
#endif





#ifndef UDR0
#define UDR0     UDR1
#endif
#ifndef U2X
#define U2X     U2X1
#endif
#ifndef TXB8
#define TXB8     TXB81
#endif
#ifndef USBS
#define USBS     USBS1
#endif
#ifndef UCPOL
#define UCPOL     UCPOL1
#endif
#ifndef UMSEL
#define UMSEL     UMSEL1
#endif
#ifndef UPE
#define UPE     UPE1
#endif
#ifndef MPCM
#define MPCM     MPCM1
#endif
#ifndef UBRR0
#define UBRR0    UBRR1
#endif
#ifndef UBRR0L
#define UBRR0L    UBRR1L
#endif
#ifndef UBRR0H
#define UBRR0H    UBRR1H
#endif
#ifndef UCSR0A
#define UCSR0A     UCSR1A
#endif
#ifndef UCSR0B
#define UCSR0B     UCSR1B
#endif
#ifndef UCSR0C
#define UCSR0C     UCSR1C
#endif
#ifndef RXC0
#define RXC0     RXC1
#endif
#ifndef UDRE0
#define UDRE0   UDRE1
#endif
#ifndef FE0
#define FE0      FE1
#endif
#ifndef DOR0
#define DOR0     DOR1
#endif
#ifndef RXCIE0
#define RXCIE0   RXCIE1
#endif
#ifndef TXCIE0
#define TXCIE0   TXCIE1
#endif
#ifndef UDRIE0
#define UDRIE0   UDRIE1
#endif
#ifndef RXEN0
#define RXEN0    RXEN1
#endif
#ifndef TXEN0
#define TXEN0    TXEN1
#endif
#ifndef UPM0
#define UPM0    UPM10
#endif
#ifndef UPM1
#define UPM1    UPM11
#endif
#ifndef U2X0
#define U2X0    U2X1
#endif
#ifndef UCSZ0
#define UCSZ0    UCSZ10
#endif
#ifndef UCSZ1
#define UCSZ1    UCSZ11
#endif
#ifndef UCSZ2
#define UCSZ2    UCSZ12
#endif
#ifndef UMSEL0
#define UMSEL0    UMSEL10
#endif
#ifndef UMSEL1
#define UMSEL1    UMSEL11
#endif

#ifndef WGM0
#define WGM0    WGM01
#endif
#ifndef WGM1
#define WGM1    WGM11
#endif
#ifndef WGM2
#define WGM2    WGM21
#endif
#ifndef WGM3
#define WGM3    WGM31
#endif
#ifndef OCIE2
#define OCIE2    OCIE2A
#endif
#ifndef OCR2
#define OCR2    OCR2A
#endif

#ifndef TICIE3
#define TICIE3    ICIE3
#endif
#ifndef ETIMSK
#define ETIMSK    TIMSK3
#endif



#ifndef ADCW
#define ADCW    ADC
#endif
#ifndef ADCSR
#define ADCSR   ADCSRA
#endif
#ifndef ADFR
#define ADFR    ADATE
#endif
#ifndef OCIE0
#define OCIE0   OCIE0A
#endif
#ifndef TCCR0
#define TCCR0   TCCR0A
#endif
#ifndef TCCR2
#define TCCR2   TCCR2A
#endif
#ifndef OCR0
#define OCR0    OCR0A
#endif
#ifndef TIMSK
#define TIMSK   TIMSK0
#endif
#ifndef TIFR
#define TIFR   TIFR0
#endif

#ifndef UBRR
#define UBRR   UBRR1
#endif

#ifndef UBRRL
#define UBRRL   UBRR1L
#endif

#ifndef UBRRH
#define UBRRH   UBRR1H
#endif

#ifndef UCSRB
#define UCSRB   UCSR1B
#endif

#ifndef UCSRC
#define UCSRC   UCSR1C
#endif


#endif /* _ARCH_AVR_H_ */
