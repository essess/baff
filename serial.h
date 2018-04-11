/**
 * Copyright (c) 2018 Sean Stasiak. All rights reserved.
 * Developed by: Sean Stasiak <sstasiak@protonmail.com>
 * Refer to license terms in license.txt; In the absence of such a file,
 * contact me at the above email address and I can provide you with one.
 */

#ifndef SERIAL_H
#define SERIAL_H

void initSerialPort(void);
void flushRcvBuff(void);
enumEC_t getchTO(uint8_t const, uint8_t * const);		// timeout after (0-255ms),  0 == infinite
enumEC_t getcheTO(uint8_t const, uint8_t * const);		// echo version

#endif SERIAL_H
