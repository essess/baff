/**
 * Copyright (c) 2018 Sean Stasiak. All rights reserved.
 * Developed by: Sean Stasiak <sstasiak@protonmail.com>
 * Refer to license terms in license.txt; In the absence of such a file,
 * contact me at the above email address and I can provide you with one.
 */

#ifndef EECONST_H
#define EECONST_H

// ----  refer to eeImage.c if you're interested in the actual locations --------------------
// ----  when passing their addresses into the get/set make sure you use '&' !!
extern uint8_t * const EE_pwmMax;
extern uint16_t * const EE_cycleTicks;
extern uint8_t * const EE_flags;
extern uint32_t * const EE_AN0UL;
extern uint32_t * const EE_AN0LL;
extern uint32_t * const EE_AN0dY;
extern uint32_t * const EE_dX;
//extern uint32_t * const EE_longTest;
// ------------------------------------------------------------------------------------------

typedef struct {
	uint8_t	 pwmMax;
	uint16_t stepMax;
	u8bitv_t flags;
	uint32_t AN0UL;
	uint32_t AN0LL;
	uint32_t AN0dY;
	uint32_t dX;
} eeConst_t;


void getEEProm8b(uint8_t const * const, uint8_t * const);
void getEEProm16b(uint16_t const * const, uint16_t * const);
void getEEProm32b(uint32_t const * const, uint32_t * const);
void dumpEEPromConstants(eeConst_t const * const);
void loadEEPromConstants(eeConst_t * const);
#endif
