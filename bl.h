/**
 * Copyright (c) 2018 Sean Stasiak. All rights reserved.
 * Developed by: Sean Stasiak <sstasiak@protonmail.com>
 * Refer to license terms in license.txt; In the absence of such a file,
 * contact me at the above email address and I can provide you with one.
 */

#ifndef BL_H
#define BL_H


// TODO: add these as eeprom items

#define IDLE_CNT_MAX	300		// wait a max of 'n' IDLE_POLL_DELAY's (300*.01 = 3s) (uint16_t)
#define IDLE_POLL_DELAY 10		// poll for keypress every 10ms (uint8_t)

void bl(void);
void showMenu(void);
void showHelp(void);
void monitorAD(void);
void interpolateTest(void);

#endif
