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
#include "bl.h"
#include <ctype.h>

uint8_t  bank1 ch;		// auto variables are only allowed in bank0 and were getting tight on mem
uint16_t bank1 idleCnt;
enumEC_t bank1 errCode;
extern uint16_t bank1 tpsAnalogIn;
uint32_t bank1 y;
uint32_t bank1 j;

void bl(void) {
	idleCnt = IDLE_CNT_MAX;

	printf("\r\n--- bl() entered. ---\r\n");
	showMenu();

	while(idleCnt--) {
		errCode = getcheTO(IDLE_POLL_DELAY, &ch);			// timeout every 100ms
		if((errCode == E_OK)&&(isalpha(ch))) {
			switch(tolower(ch)) {							// got a valid keypress
				case 'a':
					monitorAD();
					break;
				case 'i':
					interpolateTest();
					break;
				case 'x':
					goto exitBL;
				default:
					showHelp();
			}
			idleCnt = IDLE_CNT_MAX;							// reset timeout
			showMenu();
		}
	}

exitBL:
	printf("\r\n--- bl() exiting. ---\r\n");
}

void showMenu(void) {
	char const * const menu = \
		"\r\n" \
		"[a] monitor anAlog input\r\n" \
		"[i] Interpolate A/D\r\n" \
		"[ ] empty\r\n" \
		"[x] eXit\r\n"
		" ? >";
	printf(menu);
}

void showHelp(void) {
	char const * const help = \
		"\r\n" \
		"Your keypress is unrecognized or not implemented, make another selection:\r\n";
	printf(help);
}

void monitorAD(void) {
	// add use of ANSI codes to control the text output, for now just dump it
	printf("\r\n");
	while(1) {
		errCode = getcheTO(50, &ch);
		if(errCode == E_OK)
			return;
		printf("%4d\r\n",tpsAnalogIn);
	}
}

void interpolateTest(void) {

/*
	printf("\r\n");
	while(1) {
		errCode = getcheTO(100, &ch);
		if(errCode == E_OK)
			return;

		//y = (uint32_t)(500) - (uint32_t)((uint32_t)(450*tpsAnalogIn)/(uint32_t)1024);
		//y = (450*tpsAnalogIn)/1024;
		//y = tpsAnalogIn * 450;

		j=500;

		y= j - (((uint32_t)450 * tpsAnalogIn)/1024);

		printf("%lx\r\n",y);
	}*/
}
