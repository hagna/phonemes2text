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

#include "USBVirtualSerial.h"
#include "LUFA_Arduino_USBVirtualSerial_FreeRTOS.h"

volatile bool Enumerated = false;


void USB_Serial_Begin(void)
{
	Enumerated = true;
}

int USB_Serial_Enumerated(void)
{
	return (Enumerated == true) ? 1 : -1;
}

int USB_Serial_Available(void)
{
	return haveData();
}

int USB_Serial_Read(void)
{
	if (haveData()) {
		return getData(stdin);
	} else {
		return -1;
	}
}

void USB_Serial_Write(char Byte)
{
	if (Enumerated)
	{
		sendData(Byte, stdout);
	}
}

void USB_Serial_Print(char* String)
{

	if (Enumerated)
	{
		while (*String != 0x00)
		{
			sendData(*(String++), stdout);
		}
	}
}

void USB_Serial_PrintLine(char* String)
{
	USB_Serial_Print(String);
	USB_Serial_Write('\r');
	USB_Serial_Write('\n');
}
