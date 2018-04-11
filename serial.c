/**
 * Copyright (c) 2018 Sean Stasiak. All rights reserved.
 * Developed by: Sean Stasiak <sstasiak@protonmail.com>
 * Refer to license terms in license.txt; In the absence of such a file,
 * contact me at the above email address and I can provide you with one.
 */

#include <pic.h>
#include <stdio.h>
#include "types.h"
#include "delay.h"
#include "serial.h"

uint8_t bank1 msecTemp;		// short on mem and auto vars are only allowed in bank0
uint8_t bank1 chTemp;
enumEC_t bank1 errTemp;

char getche(void) {
	putch(chTemp = getch());
	return chTemp;
}

void putch(char c) {
	TXEN  = SET;			// enable xmtr
	TXREG = c;				// xfer data
    while(TRMT == CLEAR);	// block until xmit is complete
	TXEN  = CLEAR;
}

char getch(void) {
	CREN = SET;			// enable continuous reception and clear any FERR/OERR's
	while(RCIF == 0);	// block until byte is received
	CREN = CLEAR;
	return(RCREG);		// return received char and clear RXIF
}
void initSerialPort(void) {

	// TODO: check to see if I'm xmit'ing (wait for TRMT == 1), if so - wait for xmit to finish before init'ing port ?

	// setup RCSTA
	SPEN = SET;			// enable serial port
	RX9  = CLEAR;		// 8b reception
	CREN = CLEAR;		// enable reception only in getch()

	// setup TXSTA
	TX9  = CLEAR;		// 8b mode
	TXEN = CLEAR;		// disable transmitter, the xmitr will only be enabled when I put something in TXREG
						// when cleared, also has the side effect of aborting transmission and clearing the TSR
	SYNC = CLEAR;		// set to async mode
	BRGH = SET;			// set high speed, reduces baud rate error significantly

	// set rate to 56k
	// TODO: make this configurable
	SPBRG = 20;			// decimal - straight from manual
	TXIE  = CLEAR;		// do not generate ints yet
	RCIE  = CLEAR;		//

}

void flushRcvBuff(void) {
	CREN   = CLEAR;		// disable reception of chars
	chTemp = RCREG;
	chTemp = RCREG;		// double read flushes buffer and resets RCIF flag

	CREN = SET;
	CREN = CLEAR;		// toggling CREN flushes OERR and FERR
}

enumEC_t getcheTO(uint8_t const msec, uint8_t * const ch) {

	errTemp = getchTO(msec, ch);

	if( errTemp != E_OK)
		return(errTemp);

	putch(*ch);
	return(E_OK);
}

enumEC_t getchTO(uint8_t const msec, uint8_t * const ch) {

	if(msec==0) {			// infinite timeout
		*ch = getch();
		return(E_OK);
	}

	msecTemp = msec;
	while(msecTemp) {
		CREN=SET;
		if(RCIF) {
			*ch = RCREG;
			CREN=CLEAR;
			return(E_OK);
		}
		DelayMs(1);
		msecTemp--;
	}
	return(E_TIMEOUT);
}

