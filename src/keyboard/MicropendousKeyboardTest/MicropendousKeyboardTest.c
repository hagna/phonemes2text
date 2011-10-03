/*
             LUFA Library
     Copyright (C) Dean Camera, 2010.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2010  Dean Camera (dean [at] fourwalledcubicle [dot] com)
  Modified for Micropendous Boards by Opendous Inc. (www.Micropendous.org)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
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

/** \file
 *
 *  Main source file for the Keyboard demo. This file contains the main tasks of
 *  the demo and is responsible for the initial application hardware configuration.
 */

#include "MicropendousKeyboardTest.h"

/* Global 16-bit integer to store the current value of Timer1 */
volatile uint16_t Timer1Val = 0;
volatile uint16_t Timer1OverflowCount = 0;

/** Buffer to hold the previously generated Keyboard HID report, for comparison purposes inside the HID class driver. */
uint8_t PrevKeyboardHIDReportBuffer[sizeof(USB_KeyboardReport_Data_t)];

/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_HID_Device_t Keyboard_HID_Interface =
 	{
		.Config =
			{
				.InterfaceNumber              = 0,

				.ReportINEndpointNumber       = KEYBOARD_EPNUM,
				.ReportINEndpointSize         = KEYBOARD_EPSIZE,
				.ReportINEndpointDoubleBank   = false,

				.PrevReportINBuffer           = PrevKeyboardHIDReportBuffer,
				.PrevReportINBufferSize       = sizeof(PrevKeyboardHIDReportBuffer),
			},
    };

#define NSTENO 3
#define TSTAMP ((uint32_t)Timer1OverflowCount << 16) + (uint32_t)TCNT1 
#define KEYUP_INTERVAL 3000


RingBuff_t debug_buffer;
RingBuff_t steno_buffer;

STENO steno[NSTENO] = { {.cur = 0, .prev = 0, .maxtime = 0, .buf = 0},
                        {.cur = 0, .prev = 0, .maxtime = 0, .buf = 0},
                        {.cur = 0, .prev = 0, .maxtime = 0, .buf = 0},

};
/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
int main(void)
{
	SetupHardware();
 
    for (int i=0; i<NSTENO; i++) {
        steno[i].maxtime = TSTAMP;
    }
    Buffer_Initialize(&debug_buffer);
    Buffer_Initialize(&steno_buffer);

	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
	sei();

	for (;;)
	{
		HID_Device_USBTask(&Keyboard_HID_Interface);
		USB_USBTask();
	}
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware()
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	/* disable JTAG to allow corresponding pins to be used - PF4, PF5, PF6, PF7 */
	/* TODO - remove this if you want to use your JTAG debugger to debug this firmware */
	#if (defined(__AVR_AT90USB1287__) || defined(__AVR_AT90USB647__) ||  \
			defined(__AVR_AT90USB1286__) || defined(__AVR_AT90USB646__) ||  \
			defined(__AVR_ATmega16U4__)  || defined(__AVR_ATmega32U4__) ||  \
			defined(__AVR_ATmega32U6__))
		// note the JTD bit must be written twice within 4 clock cycles to disable JTAG
		// you must also set the IVSEL bit at the same time, which requires IVCE to be set first
		// port pull-up resistors are enabled - PUD(Pull Up Disable) = 0
		MCUCR = (1 << JTD) | (1 << IVCE) | (0 << PUD);
		MCUCR = (1 << JTD) | (0 << IVSEL) | (0 << IVCE) | (0 << PUD);
	#endif

	/* Hardware Initialization */
	/* enable Ports based on which IC is being used */
	/* For more information look over the corresponding AVR's datasheet in the
		'I/O Ports' Chapter under subheading 'Ports as General Digital I/O' */
	#if (defined(__AVR_AT90USB162__) || defined(__AVR_AT90USB82__) || \
			defined(__AVR_ATmega16U2__) || defined(__AVR_ATmega32U2__))
		DDRD = 0;
		PORTD = 0xFF;
		DDRB = 0;
		PORTB = 0xFF;
		DDRC = 0;
		PORTC |= (1 << PC2) | (1 << PC4) | (1 << PC5) | (1 << PC6) | (1 << PC7); //only PC2,4,5,6,7 are pins
		// be careful using PortC as PC0 is used for the Crystal and PC1 is nRESET
	#endif

	#if (defined(__AVR_ATmega16U4__)  || defined(__AVR_ATmega32U4__))
		DDRD = 0;
		PORTD = 0xFF;
		DDRB = 0;
		PORTB = 0xFF;
		DDRC = 0;
		PORTC = (1 << PC6) | (1 << PC7); //only PC6,7 are pins
		DDRE = 0;
		PORTE = (1 << PE2) | (1 << PE6); //only PE2,6 are pins
		DDRF = 0;
		PORTF = (1 << PF0) | (1 << PF1) | (1 << PF4) | (1 << PF5) | (1 << PF6) | (1 << PF7); // only PF0,1,4,5,6,7 are pins
	#endif

	#if (defined(__AVR_AT90USB1287__) || defined(__AVR_AT90USB647__) ||  \
			defined(__AVR_AT90USB1286__) || defined(__AVR_AT90USB646__) ||  \
			defined(__AVR_ATmega32U6__))
		DDRA = 0;
		PORTA = 0xFF;
		DDRB = 0;
		PORTB = 0xFF;
		DDRC = 0;
		PORTC = 0xFF;
		DDRD = 0;
		PORTD = 0xFF;
		DDRE = 0;
		PORTE = 0xFF;
		DDRF = 0;
		PORTF = 0xFF;
		#if (BOARD == BOARD_MICROPENDOUS)
		// set PortB pin 1 to an output as it connects to an LED on the Micropendous
		DDRB |= (1 << PB1);
		// Set PE4=1 to disable external SRAM, PE6=0 to disable TXB0108, PE7=1 to select USB-B connector
		DDRE |= ((1 << PE4) | (1 << PE6) | (1 << PE7));
		PORTE |= ((1 << PE4) | (1 << PE7));
		PORTE &= ~(1 << PE6);
		#else // other boards such as the Micropendous3 or Micropendous4
		// Set PE6=1 to disable external SRAM
		DDRE |= (1 << PE6);
		PORTE |= (1 << PE6);
		#endif
	#endif

	/* Hardware Initialization */
	Joystick_Init();
	LEDs_Init();
	Buttons_Init();
	SELECT_USB_B;
	USB_Init();

        /* Timer Initialization */
        // 16-bit Timer1 Initialization
	TCCR1A = 0; //start the timer
	//TCCR1B = (1 << CS10); // no Timer1 prescaling, use CLK
	TCCR1B = (1 << CS12); // prescale Timer1 by CLK/256
	// 8000000 / 256 = 31250 ticks per second
	// 16-bit = 2^16 = 65536 maximum ticks for Timer1
	// 65536 / 31250 =~ 2.1
	// so Timer1 will overflow back to 0 about every 2 seconds
	Timer1Val = TCNT1; // get current Timer1 value
	Timer1Val = ((uint32_t)Timer1OverflowCount << 16) + (uint32_t)TCNT1; // get current Timer1 value
	TIMSK1 = (1 << TOIE1); // enable Timer1 overflow interrupt

}


/* Timer1 overflow interrupt */
ISR(TIMER1_OVF_vect) {    
        Timer1OverflowCount++;

}


/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= HID_Device_ConfigureEndpoints(&Keyboard_HID_Interface);

	USB_Device_EnableSOFEvents();

	LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	HID_Device_ProcessControlRequest(&Keyboard_HID_Interface);
}

/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame(void)
{
	HID_Device_MillisecondElapsed(&Keyboard_HID_Interface);
}


int check_interval(uint32_t maxtime) {
    // returns False if maxtime is outside of the interval
    // otherwise True
    uint32_t t = TSTAMP;
    if (t < maxtime) { // overflow
        return ((maxtime - t) > KEYUP_INTERVAL);
    } 
    return ((t - maxtime) > KEYUP_INTERVAL);
}

/** HID class driver callback function for the creation of HID reports to the host.
 *
 *  \param[in]     HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in,out] ReportID    Report ID requested by the host if non-zero, otherwise callback should set to the generated report ID
 *  \param[in]     ReportType  Type of the report to create, either HID_REPORT_ITEM_In or HID_REPORT_ITEM_Feature
 *  \param[out]    ReportData  Pointer to a buffer where the created report should be stored
 *  \param[out]    ReportSize  Number of bytes written in the report (or zero if no report is to be sent
 *
 *  \return Boolean true to force the sending of the report, false to let the library determine if it needs to be sent
 */
bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo, uint8_t* const ReportID,
                                         const uint8_t ReportType, void* ReportData, uint16_t* const ReportSize)
{
	USB_KeyboardReport_Data_t* KeyboardReport = (USB_KeyboardReport_Data_t*)ReportData;

	if (debug_buffer.Elements) {
		uint16_t t = Buffer_GetElement(&debug_buffer);
		KeyboardReport->Modifier = (t & 0xff00) >> 8;
		KeyboardReport->KeyCode[0] = t & 0x00ff;
	} 
	if (steno_buffer.Elements) {
		uint16_t t = Buffer_GetElement(&steno_buffer);
		KeyboardReport->Modifier = (t & 0xff00) >> 8;
		KeyboardReport->KeyCode[0] = t & 0x00ff;
	}
    for (int i=0; i<2; i++) {
        STENO *hand;
        hand = &steno[i];
        if (i == 0) {
            hand->cur = ~PINB;  // 0 == keyup 
        } else {
            hand->cur = ~PIND;
        }
        if ((hand->cur == 0) && (hand->buf != 0)) {
            //flush buffer all keys are up
            Buffer_StoreElement(&debug_buffer, HID_KEYBOARD_SC_F);
            Buffer_StoreElement(&steno_buffer, HID_KEYBOARD_SC_A + hand->buf);
            hand->buf = 0;
        } else {
            uint8_t d = hand->cur ^ hand->prev;
            hand->prev = hand->cur;
            for (int j=0; j<8; j++) {
                if (d & (1 << j)) { // d(j) == 1
                    // detected a change 
                    Buffer_StoreElement(&debug_buffer, HID_KEYBOARD_SC_H);
                    if (hand->cur & (1 << j)) { // cur(j) == 0
                        //keyup
                        Buffer_StoreElement(&debug_buffer, HID_KEYBOARD_SC_U);
                        //if (check_interval(hand->maxtime)) {
                        if (0) {
                            //clear buffer
                            hand->buf = 0;
                            Buffer_StoreElement(&debug_buffer, HID_KEYBOARD_SC_R);
                        }
                        // add to buffer
                        Buffer_StoreElement(&debug_buffer, HID_KEYBOARD_SC_D);
                        hand->maxtime = TSTAMP;
                        hand->buf |= 1 << j;
              
                    } // end if keyup
                } // end if changed 
            } // end looping through changes
        } 
    } // end looping through pads
	//uint8_t JoyStatus_LCL    = Joystick_GetStatus();
	//uint8_t ButtonStatus_LCL = Buttons_GetStatus();

	uint8_t UsedKeyCodes = 0;

	// 32-pin USB AVRs
	#if (defined(__AVR_AT90USB162__) || defined(__AVR_AT90USB82__) || \
			defined(__AVR_ATmega16U2__) || defined(__AVR_ATmega32U2__))
				  if (~PINB & (1 << PINB0)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_B;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_0_AND_CLOSING_PARENTHESIS;
		} else if (~PINB & (1 << PINB1)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_B;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_1_AND_EXCLAMATION;
		 else if (~PINB & (1 << PINB2)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_B;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_2_AND_AT;
		} else if (~PINB & (1 << PINB3)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_B;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_3_AND_HASHMARK;
		} else if (~PINB & (1 << PINB4)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_C;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_4_AND_DOLLAR;
		} else if (~PINB & (1 << PINB5)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_B;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_5_AND_PERCENTAGE;
		} else if (~PINB & (1 << PINB6)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_B;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_6_AND_CARET;
		} else if (~PINB & (1 << PINB7)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_B;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_7_AND_AND_AMPERSAND;

		} else if (~PINC & (1 << PINC2)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_C;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_2_AND_AT;
		} else if (~PINC & (1 << PINC4)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_C;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_4_AND_DOLLAR;
		} else if (~PINC & (1 << PINC5)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_C;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_5_AND_PERCENTAGE;
		} else if (~PINC & (1 << PINC6)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_C;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_6_AND_CARET;
		} else if (~PINC & (1 << PINC7)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_C;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_7_AND_AND_AMPERSAND;

		} else if (~PIND & (1 << PIND0)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_D;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_0_AND_CLOSING_PARENTHESIS;
		} else if (~PIND & (1 << PIND1)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_D;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_1_AND_EXCLAMATION;
		} else if (~PIND & (1 << PIND2)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_D;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_2_AND_AT;
		} else if (~PIND & (1 << PIND3)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_D;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_3_AND_HASHMARK;
		} else if (~PIND & (1 << PIND4)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_D;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_4_AND_DOLLAR;
		} else if (~PIND & (1 << PIND5)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_D;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_5_AND_PERCENTAGE;
		} else if (~PIND & (1 << PIND6)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_D;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_6_AND_CARET;
		} else if (~PIND & (1 << PIND7)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_D;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_7_AND_AND_AMPERSAND;
		}
	#endif // 32-pin USB AVRs


	// 44-pin USB AVRs
	#if (defined(__AVR_ATmega16U4__)  || defined(__AVR_ATmega32U4__))
	#endif // 44-pin USB AVRs


	// 64-pin USB AVRs
	#if (defined(__AVR_AT90USB1287__) || defined(__AVR_AT90USB647__) ||  \
			defined(__AVR_AT90USB1286__) || defined(__AVR_AT90USB646__) ||  \
			defined(__AVR_ATmega32U6__))

				  if (~PINA & (1 << PINA0)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_A;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_0_AND_CLOSING_PARENTHESIS;
		} else if (~PINA & (1 << PINA1)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_A;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_1_AND_EXCLAMATION;
		} else if (~PINA & (1 << PINA2)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_A;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_2_AND_AT;
		} else if (~PINA & (1 << PINA3)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_A;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_3_AND_HASHMARK;
		} else if (~PINA & (1 << PINA4)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_A;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_4_AND_DOLLAR;
		} else if (~PINA & (1 << PINA5)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_A;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_5_AND_PERCENTAGE;
		} else if (~PINA & (1 << PINA6)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_A;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_6_AND_CARET;
		} else if (~PINA & (1 << PINA7)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_A;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_7_AND_AND_AMPERSAND;

		} else if (~PINB & (1 << PINB0)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_B;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_0_AND_CLOSING_PARENTHESIS;
		} else if (~PINB & (1 << PINB1)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_B;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_1_AND_EXCLAMATION;
		} else if (~PINB & (1 << PINB2)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_B;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_2_AND_AT;
		} else if (~PINB & (1 << PINB3)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_B;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_3_AND_HASHMARK;
		} else if (~PINB & (1 << PINB4)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_B;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_4_AND_DOLLAR;
		} else if (~PINB & (1 << PINB5)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_B;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_5_AND_PERCENTAGE;
		} else if (~PINB & (1 << PINB6)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_B;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_6_AND_CARET;
		} else if (~PINB & (1 << PINB7)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_B;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_7_AND_AND_AMPERSAND;

		} else if (~PINC & (1 << PINC0)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_C;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_0_AND_CLOSING_PARENTHESIS;
		} else if (~PINC & (1 << PINC1)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_C;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_1_AND_EXCLAMATION;
		} else if (~PINC & (1 << PINC2)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_C;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_2_AND_AT;
		} else if (~PINC & (1 << PINC3)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_C;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_3_AND_HASHMARK;
		} else if (~PINC & (1 << PINC4)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_C;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_4_AND_DOLLAR;
		} else if (~PINC & (1 << PINC5)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_C;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_5_AND_PERCENTAGE;
		} else if (~PINC & (1 << PINC6)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_C;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_6_AND_CARET;
		} else if (~PINC & (1 << PINC7)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_C;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_7_AND_AND_AMPERSAND;

		} else if (~PIND & (1 << PIND0)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_D;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_0_AND_CLOSING_PARENTHESIS;
		} else if (~PIND & (1 << PIND1)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_D;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_1_AND_EXCLAMATION;
		} else if (~PIND & (1 << PIND2)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_D;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_2_AND_AT;
		} else if (~PIND & (1 << PIND3)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_D;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_3_AND_HASHMARK;
		} else if (~PIND & (1 << PIND4)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_D;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_4_AND_DOLLAR;
		} else if (~PIND & (1 << PIND5)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_D;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_5_AND_PERCENTAGE;
		} else if (~PIND & (1 << PIND6)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_D;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_6_AND_CARET;
		} else if (~PIND & (1 << PIND7)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_D;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_7_AND_AND_AMPERSAND;

		} else if (~PINE & (1 << PINE0)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_E;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_0_AND_CLOSING_PARENTHESIS;
		} else if (~PINE & (1 << PINE1)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_E;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_1_AND_EXCLAMATION;
		} else if (~PINE & (1 << PINE2)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_E;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_2_AND_AT;
		} else if (~PINE & (1 << PINE3)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_E;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_3_AND_HASHMARK;
		} else if (~PINE & (1 << PINE4)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_E;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_4_AND_DOLLAR;
		} else if (~PINE & (1 << PINE5)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_E;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_5_AND_PERCENTAGE;

		// On Micropendous boards pins E6, E7 are off-limits as they control the voltage translator and USB switch
		#if (BOARD != BOARD_MICROPENDOUS)
		} else if (~PINE & (1 << PINE6)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_E;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_6_AND_CARET;
		} else if (~PINE & (1 << PINE7)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_E;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_7_AND_AND_AMPERSAND;
		#endif // BOARD != MICROPENDOUS

		} else if (~PINF & (1 << PINF0)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_F;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_0_AND_CLOSING_PARENTHESIS;
		} else if (~PINF & (1 << PINF1)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_F;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_1_AND_EXCLAMATION;
		} else if (~PINF & (1 << PINF2)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_F;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_2_AND_AT;
		} else if (~PINF & (1 << PINF3)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_F;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_3_AND_HASHMARK;
		} else if (~PINF & (1 << PINF4)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_F;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_4_AND_DOLLAR;
		} else if (~PINF & (1 << PINF5)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_F;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_5_AND_PERCENTAGE;
		} else if (~PINF & (1 << PINF6)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_F;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_6_AND_CARET;
		} else if (~PINF & (1 << PINF7)) {
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_F;
			KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_7_AND_AND_AMPERSAND;
		}
	#endif // 64-pin USB AVRs

	//if (UsedKeyCodes) // cannot use modifier as numbers will not show
	//	KeyboardReport->Modifier = HID_KEYBOARD_MODIFER_LEFTSHIFT;

	*ReportSize = sizeof(USB_KeyboardReport_Data_t);
	return false;
}

/** HID class driver callback function for the processing of HID reports from the host.
 *
 *  \param[in] HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in] ReportID    Report ID of the received report from the host
 *  \param[in] ReportType  The type of report that the host has sent, either HID_REPORT_ITEM_Out or HID_REPORT_ITEM_Feature
 *  \param[in] ReportData  Pointer to a buffer where the created report has been stored
 *  \param[in] ReportSize  Size in bytes of the received HID report
 */
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                          const uint8_t ReportID,
                                          const uint8_t ReportType,
                                          const void* ReportData,
                                          const uint16_t ReportSize)
{
	uint8_t  LEDMask   = LEDS_NO_LEDS;
	uint8_t* LEDReport = (uint8_t*)ReportData;

	if (*LEDReport & HID_KEYBOARD_LED_NUMLOCK)
		LEDMask |= LEDS_LED1;

	if (*LEDReport & HID_KEYBOARD_LED_CAPSLOCK)
		LEDMask |= LEDS_LED3;

	if (*LEDReport & HID_KEYBOARD_LED_SCROLLLOCK)
		LEDMask |= LEDS_LED4;

	LEDs_SetAllLEDs(LEDMask);
}
