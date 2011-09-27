/* Copyright (c) 2009  By Eric B. Weddington
   All rights reserved.

   Defines missing from currently used versions of avr-libc.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __MISSING_DEFINES__
#define __MISSING_DEFINES__

#include <avr/version.h>


#ifndef PINHIGH
	#define PINHIGH(PORT, PIN)		PORT |= (1 << PIN);
#endif

#ifndef PINLOW
	#define PINLOW(PORT, PIN)		PORT &= ~(1 << PIN);
#endif


/* Missing WinAVR include defines */
/* WinAVR does not define these for the ATmega??u4*/
#if (defined(__AVR_ATmega16U4__)  || defined(__AVR_ATmega32U4__))
	#ifndef PB7
		#define PB7		7
	#endif
	#ifndef PB6
		#define PB6		6
	#endif
	#ifndef PB5
		#define PB5		5
	#endif
	#ifndef PB4
		#define PB4		4
	#endif
	#ifndef PB3
		#define PB3		3
	#endif
	#ifndef PB2
		#define PB2		2
	#endif
	#ifndef PB1
		#define PB1		1
	#endif
	#ifndef PB0
		#define PB0		0
	#endif
	#ifndef PC7
		#define PC7		7
	#endif
	#ifndef PC6
		#define PC6		6
	#endif
	#ifndef PD7
		#define PD7		7
	#endif
	#ifndef PD6
		#define PD6		6
	#endif
	#ifndef PD5
		#define PD5		5
	#endif
	#ifndef PD4
		#define PD4		4
	#endif
	#ifndef PD3
		#define PD3		3
	#endif
	#ifndef PD2
		#define PD2		2
	#endif
	#ifndef PD1
		#define PD1		1
	#endif
	#ifndef PD0
		#define PD0		0
	#endif
	#ifndef PE2
		#define PE2		2
	#endif
	#ifndef PE6
		#define PE6		6
	#endif
	#ifndef PF7
		#define PF7		7
	#endif
	#ifndef PF6
		#define PF6		6
	#endif
	#ifndef PF5
		#define PF5		5
	#endif
	#ifndef PF4
		#define PF4		4
	#endif
	#ifndef PF1
		#define PF1		1
	#endif
	#ifndef PF0
		#define PF0		0
	#endif
#endif

// missing avr-libc Linux defines
//#ifndef clock_prescale_set
//#if (__AVR_LIBC_VERSION__ < 10606UL)

	#ifndef clock_div_1
		#define clock_div_1		0
	#endif
	#ifndef clock_div_2
		#define clock_div_2		1
	#endif
	#ifndef clock_div_4
		#define clock_div_4		2
	#endif
	#ifndef clock_div_8
		#define clock_div_8		3
	#endif
	#ifndef clock_div_16
		#define clock_div_16		4
	#endif
	#ifndef clock_div_32
		#define clock_div_32		5
	#endif
	#ifndef clock_div_64
		#define clock_div_64		6
	#endif
	#ifndef clock_div_128
		#define clock_div_128	7
	#endif
	#ifndef clock_div_256
		#define clock_div_256	8
	#endif

	#ifndef clock_prescale_set
		#define clock_prescale_set(x) \
		{ \
		uint8_t tmp = _BV(CLKPCE); \
		__asm__ __volatile__ ( \
		        "in __tmp_reg__,__SREG__" "\n\t" \
		        "cli" "\n\t" \
		        "sts %1, %0" "\n\t" \
		        "sts %1, %2" "\n\t" \
		        "out __SREG__, __tmp_reg__" \
		        : /* no outputs */ \
		        : "d" (tmp), \
		          "M" (_SFR_MEM_ADDR(CLKPR)), \
		          "d" (x) \
		        : "r0"); \
		}
	#endif

//#endif // (__AVR_LIBC_VERSION__ < 10606UL)

#endif // MissingDefines.h
