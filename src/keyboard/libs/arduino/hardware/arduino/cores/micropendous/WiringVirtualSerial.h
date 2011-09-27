/*
             LUFA Library
     Copyright (C) Dean Camera, 2008.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2008  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, and distribute this software
  and its documentation for any purpose and without fee is hereby
  granted, provided that the above copyright notice appear in all
  copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

#ifndef __WIRINGSERIAL_H__
#define __WIRINGSERIAL_H__

	#include <avr/io.h>
	#include <inttypes.h>
	#include "Print.h"

	extern "C" {
		#include "USBVirtualSerial.h"
	}

	/* C++ Classes: */
	class USBVirtualSerial
		{
			public:
				void begin(uint32_t baudrate) { USB_Serial_Begin(); };
				int  enumerated(void)         { return USB_Serial_Enumerated(); };
				int  available(void)          { return USB_Serial_Available(); };
				int  read(void)               { return USB_Serial_Read(); };
				void write(char byte)         { USB_Serial_Write(byte); };
				void print(char* str)         { USB_Serial_Print(str); };
				void print(unsigned char chr)         { USB_Serial_Write(chr); };
				void print(int n, int base)         { n = n + base; };
				void print(char byte, int n)         { n = n + byte; };
				void printLine(char* str)     { USB_Serial_PrintLine(str); };
		};

	extern USBVirtualSerial Serial;

#endif
