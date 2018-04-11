/**
 * Copyright (c) 2018 Sean Stasiak. All rights reserved.
 * Developed by: Sean Stasiak <sstasiak@protonmail.com>
 * Refer to license terms in license.txt; In the absence of such a file,
 * contact me at the above email address and I can provide you with one.
 */

#include <pic.h>
#include <stdio.h>
#include "types.h"
#include "eeconst.h"


void getEEProm8b(uint8_t const * const address, uint8_t * const buf) {
	*buf = eeprom_read((uint8_t)address);
}

void getEEProm16b(uint16_t const * const address, uint16_t * const buf) {
	*buf  = (uint16_t)eeprom_read((uint8_t)address);
	*buf |= ((uint16_t)eeprom_read(((uint8_t)address)+1) << 8);
}

void getEEProm32b(uint32_t const * const address, uint32_t * const buf) {
	*buf  = (uint32_t)eeprom_read((uint8_t)address);
	*buf |= ((uint32_t)eeprom_read(((uint8_t)address)+1) << 8);
	*buf |= ((uint32_t)eeprom_read(((uint8_t)address)+2) << 16);
	*buf |= ((uint32_t)eeprom_read(((uint8_t)address)+3) << 24);
}

void dumpEEPromConstants(eeConst_t const * const c) {
	printf("\r\n");
	printf("--- eeprom dump ------------------------------------\r\n");
	printf("EE_pwmMax: %d,[0x%x]\r\n",c->pwmMax,c->pwmMax);
	printf("EE_cycleTicks: %d,[0x%x]\r\n",c->stepMax,c->stepMax);
	printf("EE_flags: 0x%x\r\n",c->flags.u8);
	printf("EE_AN0UL: %ld,[0x%lx]\r\n",c->AN0UL,c->AN0UL);
	printf("EE_AN0LL: %ld[0x%lx]\r\n",c->AN0LL,c->AN0LL);
	printf("EE_AN0dY: %ld,[0x%lx]\r\n",c->AN0dY,c->AN0dY);
	printf("EE_dX: %ld,[0x%lx]\r\n",c->dX,c->dX);
	printf("----------------------------------------------------\r\n");
}

void loadEEPromConstants(eeConst_t * const c) {
	getEEProm8b(&EE_flags,&(c->flags.u8));
	getEEProm8b(&EE_pwmMax,&(c->pwmMax));
	getEEProm16b(&EE_cycleTicks,&(c->stepMax));
	getEEProm16b(&EE_AN0UL,&(c->AN0UL));
	getEEProm16b(&EE_AN0LL,&(c->AN0LL));
	getEEProm16b(&EE_AN0dY,&(c->AN0dY));
	getEEProm16b(&EE_dX,&(c->dX));
}
