/**
 * Copyright (c) 2018 Sean Stasiak. All rights reserved.
 * Developed by: Sean Stasiak <sstasiak@protonmail.com>
 * Refer to license terms in license.txt; In the absence of such a file,
 * contact me at the above email address and I can provide you with one.
 */

#include <pic.h>
#include <stdio.h>
#include <string.h>
#include "types.h"
#include "delay.h"
#include "serial.h"
#include "baff.h"
#include "bl.h"
#include "eeconst.h"

__CONFIG( HS & WDTDIS & PWRTEN & BOREN & LVPDIS & WP3 & DPROT & DEBUGDIS & PROTECT );
__IDLOC (0150); // do not prefix data w/0x, etc..

/**************************************************************************************
 *  DEVICE: 16F876A,  20mhz (no internal osc avail)
 *  PINOUT:
 *							        +----U----+
 *					       Vpp/MCLR | 1    28 |  RB7 - LedDriver15 -OR- ICSP d
 *				 TPSAnalogIn - RA0	| 2    27 |  RB6 - LedDriver14 -OR- ICSP clk
 *	      unused (10k to V+) - RA1	| 3    26 |  RB5 - LedDriver13
 *				  LedDriver6 - RA2	| 4    25 |  RB4 - LedDriver12
 *				  LedDriver5 - RA3	| 5    24 |  RB3 - unused LVP, (10k to GND)
 *	 unused (10k to V+) o/d! - RA4  | 6    23 |  RB2 - LedDriver11
 *				  LedDriver4 - RA5	| 7    22 |  RB1 - LedDriver10
 *						       GND	| 8    21 |  RB0 - LedDriver9
 *					           OSC	| 9    20 |  +V
 *					    	   OSC	| 10   19 |  GND
 *                LedDriver3 - RC0	| 11   18 |  RX
 *                LedDriver2 - RC1  | 12   17 |  TX
 *                LedDriver1 - RC2  | 13   16 |  RC5 - LedDriver8
 *	              LedDriver0 - RC3  | 14   15 |  RC4 - LedDriver7
 *			        				+---------+
 *
 * NOTE: RA1 is unusable unless RA2/RA3 are converted over to Vref's (impossible) so only resricted to one a/d channel
 * This device is intended to be programmed w/a HighV Vpp - no LVP allowed (LVPDIS) to keep RB3 open as GPIO
 **************************************************************************************/

// ----- DO NOT MODIFY, unless you're prepared to mod the hardcoded portions in the int handler -------
// SEQ_CNT must be a multiple of 2^n (see int handler mask note)
#define SEQ_CNT   32
#define LED_CNT	  16
// ----------------------------------------------------------------------------------------------------


/*
const uint8_t seq[SEQ_CNT][LED_CNT] = {                 // this is displayed 'top down'
	{ 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0 },
	{ 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0, 0x00 },
	{ 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0, 0x00, 0x10 },
	{ 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0, 0x00, 0x10, 0x20 },
	{ 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0, 0x00, 0x10, 0x20, 0x30 },
	{ 0x50, 0x60, 0x70, 0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0, 0x00, 0x10, 0x20, 0x30, 0x40 },
	{ 0x60, 0x70, 0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0, 0x00, 0x10, 0x20, 0x30, 0x40, 0x50 },
	{ 0x70, 0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0, 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60 },
	{ 0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0, 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70 },
	{ 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0, 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80 },
	{ 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0, 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90 },
	{ 0xb0, 0xc0, 0xd0, 0xe0, 0xf0, 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xa0 },
	{ 0xc0, 0xd0, 0xe0, 0xf0, 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xa0, 0xb0 },
	{ 0xd0, 0xe0, 0xf0, 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xa0, 0xb0, 0xc0 },
	{ 0xe0, 0xf0, 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0 },
	{ 0xf0, 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0 }
};

const uint8_t seq[SEQ_CNT][LED_CNT] = {
	{ 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 100 },
	{ 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 100, 5 },
	{ 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 100, 5, 10 },
	{ 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 100, 5, 10, 15 },
	{ 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 100, 5, 10, 15, 20 },
	{ 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 100, 5, 10, 15, 20, 25 },
	{ 35, 40, 45, 50, 55, 60, 65, 70, 75, 100, 5, 10, 15, 20, 25, 30 },
	{ 40, 45, 50, 55, 60, 65, 70, 75, 100, 5, 10, 15, 20, 25, 30, 35 },
	{ 45, 50, 55, 60, 65, 70, 75, 100, 5, 10, 15, 20, 25, 30, 35, 40 },
	{ 50, 55, 60, 65, 70, 75, 100, 5, 10, 15, 20, 25, 30, 35, 40, 45 },
	{ 55, 60, 65, 70, 75, 100, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50 },
	{ 60, 65, 70, 75, 100, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55 },
	{ 65, 70, 75, 100, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60 },
	{ 70, 75, 100, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65 },
	{ 75, 100, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70 },
	{ 100, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75 }
};

const uint8_t seq[SEQ_CNT][LED_CNT] = {
	{ 100, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 100 },
	{ 100, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 100, 5 },
	{ 50, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 100, 5, 10 },
	{ 10, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 100, 5, 10, 15 },
	{ 8, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 100, 5, 10, 15, 20 },
	{ 6, 35, 40, 45, 50, 55, 60, 65, 70, 75, 100, 5, 10, 15, 20, 25 },
	{ 4, 40, 45, 50, 55, 60, 65, 70, 75, 100, 5, 10, 15, 20, 25, 30 },
	{ 2, 45, 50, 55, 60, 65, 70, 75, 100, 5, 10, 15, 20, 25, 30, 35 },
	{ 0, 50, 55, 60, 65, 70, 75, 100, 5, 10, 15, 20, 25, 30, 35, 40 },
	{ 2, 55, 60, 65, 70, 75, 100, 5, 10, 15, 20, 25, 30, 35, 40, 45 },
	{ 4, 60, 65, 70, 75, 100, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50 },
	{ 6, 65, 70, 75, 100, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55 },
	{ 8, 70, 75, 100, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60 },
	{ 10, 75, 100, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65 },
	{ 50, 100, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70 },
	{ 100, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75 }
};

const uint8_t seq[SEQ_CNT][LED_CNT] = {
	{ 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 },
	{ 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 },
	{ 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 },
	{ 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 },
	{ 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 },
	{ 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 },
	{ 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 },
	{ 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 },
	{ 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 },
	{ 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 },
	{ 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 },
	{ 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 },
	{ 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 },
	{ 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 },
	{ 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 },
	{ 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 }
};

const uint8_t seq[SEQ_CNT][LED_CNT] = {
	{  },
	{  },
	{  },
	{  },
	{  },
	{  },
	{  },
	{  },
	{  },
	{  },
	{  },
	{  },
	{  },
	{  },
	{  },
	{  }
};

*/

const uint8_t seq[SEQ_CNT][LED_CNT] = {
	{ 0, 0, 100, 30, 6, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 100, 30, 6, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 100, 30, 6, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 50, 6, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 30, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 6, 30, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 2, 6, 30, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 2, 6, 30, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 2, 6, 30, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 2, 6, 30, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 2, 6, 30, 100, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 2, 6, 30, 100, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 2, 6, 30, 100, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 2, 6, 30, 100, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 2, 6, 30, 100, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 6, 30, 100, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 6, 30, 100, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 6, 30, 100, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 6, 30, 100 }, //
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 6, 50 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 30 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 30, 6 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 30, 6, 2 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 30, 6, 2, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 30, 6, 2, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 30, 6, 2, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 100, 30, 6, 2, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 100, 30, 6, 2, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 100, 30, 6, 2, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 100, 30, 6, 2, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 100, 30, 6, 2, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 100, 30, 6, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

#pragma inline(initLEDpins)
#pragma inline(initT2)
#pragma inline(initAD)

// ------------------------- bank0 vars ------------------------------------
uint8_t  pwmCnt;			// holds the # of T2 overflow counts we should iterate through before we reload the countdown regs
                            // trying for 100 since it's easier to 'speak' in terms of 0->100% intensity (up counter)
uint8_t  seqIdx;            // holds the current line index into the seq[][] array
uint8_t  bank1 ia[LED_CNT];       // holds current intensity array pointed to by &seq[seqIdx][0], used by soft PWM loop
uint16_t stepCnt;			// holds # of pwnCnt periods we should pass through before doing a line shift (up counter)
uint16_t stepMax;			// this is a bank0 var cached from eeData.stepMax
eeConst_t bank1 eeData;		//

// ------------------------- bank1 vars ------------------------------------
uint16_t bank1 tpsAnalogIn;		// holds a/d sample for TPS input (AN0)
enumEC_t bank1 errorReg;		// read this when you see an error indicated on RA4, errors defined in types.h


void main() {
	uint8_t delayCnt = 0;

	loadEEPromConstants(&eeData);

	// ------ INIT ------------------------------------------------------------------
	initLEDpins();
    initT2();
	initAD();
	initGlobals();
	initSerialPort();					// hardcoded for 56k
	// ------------------------------------------------------------------------------

//	dumpEEPromConstants(&eeData);

	printf("ready, press any key to poke around.\r\n");
	TMR2IF = SET;			// trigger an initial update of intensity array
	ei();					// let it run.

	while(1) {

//		CREN = SET;
		DelayMs(10), delayCnt++;

		// ------ Every 10ms update stepMax based on adjusted TPS value -----------------
		stepMax = eeData.AN0UL - ((eeData.AN0dY * tpsAnalogIn)/eeData.dX);


		// ------ Check for keypress and jump to bl every 1000ms ------------------------
//		if(delayCnt >= 100) {
//			putch('.');				// put out 'heartbeat' every second
//			if(RCIF) {				// if a char is waiting, then we need to drop into the bl()
//				flushRcvBuff();
//				bl();
//				flushRcvBuff();
//			}
//			delayCnt = 0x0;
//		}
	}
}

void interrupt intHandler(void) {
	u8bitv_t ledTmp;										// avoid r-m-w probems by using a tempvar
															// TODO: think about making this a bank1 var and do bank switches
															// only once (do everyting from one bank) see generated code for
															// the answer to this.

    if(TMR2IF) {

		if(stepCnt >= stepMax) {							// update our current pointer into the seq[][] array
			ADGO = SET;									    // trigger a/d at end of every step cycle
			seqIdx++, seqIdx &= 0b00011111;					// bump index and reset pointer on overflow
									     						// NOTE: currently hardcoded for 32 shifts!
			stepCnt = 0;									// reset step counter for next shift period
			//pwmCnt  = eeData.pwmMax;						// force an update of the intensity array
		}

        if(pwmCnt >= eeData.pwmMax) {							// reload intensity array & reset soft PWM cycle
			memcpy(&ia[0], &seq[seqIdx][0], (size_t)LED_CNT);	// EXPENSIVE!  ~160us to run  TODO: look into a more efficient method tailored closer to my specific requirements
            pwmCnt = 0;											// reset pwm cycle
		}

		ledTmp.u8 = CLEAR;
        ia[6]  ? ia[6]--,  ledTmp.bits.b2 = 1: ledTmp.bits.b2 = 0;		// hard coded for speed, iterating through an array is too slow (miss int's)
        ia[5]  ? ia[5]--,  ledTmp.bits.b3 = 1: ledTmp.bits.b3 = 0;		// takes ~30us
        ia[4]  ? ia[4]--,  ledTmp.bits.b5 = 1: ledTmp.bits.b5 = 0;
		PORTA = ledTmp.u8;

		ledTmp.u8 = CLEAR;
        ia[3]  ? ia[3]--,  ledTmp.bits.b0 = 1: ledTmp.bits.b0 = 0;
        ia[2]  ? ia[2]--,  ledTmp.bits.b1 = 1: ledTmp.bits.b1 = 0;
        ia[1]  ? ia[1]--,  ledTmp.bits.b2 = 1: ledTmp.bits.b2 = 0;
        ia[0]  ? ia[0]--,  ledTmp.bits.b3 = 1: ledTmp.bits.b3 = 0;
		ia[7]  ? ia[7]--,  ledTmp.bits.b4 = 1: ledTmp.bits.b4 = 0;
		ia[8]  ? ia[8]--,  ledTmp.bits.b5 = 1: ledTmp.bits.b5 = 0;
		PORTC = ledTmp.u8;

		ledTmp.u8 = CLEAR;
        ia[9]  ? ia[9]--,  ledTmp.bits.b0 = 1: ledTmp.bits.b0 = 0;
        ia[10] ? ia[10]--, ledTmp.bits.b1 = 1: ledTmp.bits.b1 = 0;
        ia[11] ? ia[11]--, ledTmp.bits.b2 = 1: ledTmp.bits.b2 = 0;
        ia[12] ? ia[12]--, ledTmp.bits.b4 = 1: ledTmp.bits.b4 = 0;
		ia[13] ? ia[13]--, ledTmp.bits.b5 = 1: ledTmp.bits.b5 = 0;
		ia[14] ? ia[14]--, ledTmp.bits.b6 = 1: ledTmp.bits.b6 = 0;
		ia[15] ? ia[15]--, ledTmp.bits.b7 = 1: ledTmp.bits.b7 = 0;
		PORTB = ledTmp.u8;

		stepCnt++;
		pwmCnt++;
        TMR2IF=CLEAR;
	} else
	if(ADIF) {
		tpsAnalogIn  = ADRESL;	// 10b conversion - right justified
		tpsAnalogIn |= (ADRESH << 8);
		ADIF = CLEAR;
	} else {
		RA4 = 0;				// set error output if spurious INT occurs
		errorReg = E_UNKNOWN_INT;
	}
}
void initT2(void) {
        T2CON = 0x0;			// reset everything first
        TMR2  = 0x0;
        PR2   = 125;			// 200us roll rate based on scale values below
        TMR2IE= SET;			// allow t2 to generate ints
        PEIE  = SET;

								// TODO: add code to use value passed in from eeprom RAM structure
								// might want to do this ONLY when using incadesent lamps, the LED's
								// get really choppy since they turn on/off very quickly
        T2CON = 0b00001101;		// 1:2 postscale, T2 on, 1:4 prescale = 200us period @ 125 ticks
}

void initGlobals(void) {
	pwmCnt       = eeData.pwmMax;	// force a reload of ia[] on powerup
	stepMax		 = eeData.stepMax;	//
	seqIdx       = 0x0;				// start at the beginning of seq[][] on powerup
	stepCnt      = 0x0;				// clear step cntr in prep for full first cycle
	errorReg     = E_OK;
}

void initLEDpins(void) {
	// TODO: I suspect r-m-w problems w/my logic - check code generation for setting pin states to be sure.

	// TRISA is 0xFF on reset (all inputs), clear drivers to set them as outputs
	TRISA1 = TRISA2 = TRISA3 = TRISA4 = TRISA5 = OUTPUT;
	RA4 = 1;							// RA4 is used a general purpose error indicator, set it high for now to reset the error (external p/u exists)
	RA1 = RA2 = RA3 = RA5 = 0x0;		// TODO: for now, we just set everything low - in the future this logic will have to change
										// once I implement the polarity invert option in EEPROM

	// TRISB is 0xFF - weak p/u's are disabled on reset
	TRISB0 = TRISB1 = TRISB2 = TRISB4 = TRISB5 = TRISB6 = TRISB7 = OUTPUT;
	RB0 = RB1 = RB2 = RB4 = RB5 = RB6 = RB7 = 0x0;
	INTE = 0;							// disable INT function of RB0 so its not generating spurious ints
	RBIE = 0;							// disable any INT generation on RB port changes

	// TRISC is 0xFF on reset
	TRISC0 = TRISC1 = TRISC2 = TRISC3 = TRISC4 = TRISC5 = OUTPUT;
	RC0 = RC1 = RC2 = RC3 = RC4 = RC5 = 0x0;

}

void initAD(void) {
	// TODO: this func should NOT assume that AN0 has been preconfigured!!  change this

	// setup ADCON1 - set to 0x00 at reset
	ADFM  = 1;			// change this if you want to go to 8b conversions
	ADCS2 = 1;			// this is split over ADCON0

	PCFG3 = 1;			// restricted to ONE a/d channel if we must use the others as DIO
	PCFG2 = 1;			// see note at top
	PCFG1 = 1;
	PCFG0 = 0;

	// setup ADCON0 - set to 0x00 at reset
	ADCS0 = 1;			// set conversion clock to internal r/c
	ADCS1 = 1;
	ADON  = 1;			// turn on a/d


	ADIF = 0;			// clear a/d int flag
	ADIE = 1;			// turn on int's, in the handler we'll handle the scanning of multiple channels
						// and xfer the values into ram
}
