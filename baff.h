/**
 * Copyright (c) 2018 Sean Stasiak. All rights reserved.
 * Developed by: Sean Stasiak <sstasiak@protonmail.com>
 * Refer to license terms in license.txt; In the absence of such a file,
 * contact me at the above email address and I can provide you with one.
 */

#ifndef BAFF_H
#define BAFF_H

#define PWM_MAX			100			// soft PWM cycle reload, set @ 100 to correlate to a 0->100% intensity
#define STEP_RATE_MAX  	167			// reload tick value for shift task  167 == 30Hz

#define INPUT			1			// bit settings for TRISx regs, I hate referring back to the ds all the time
#define OUTPUT			0

void initLEDpins(void);
void initT2(void);
void initAD(void);
void initGlobals(void);
uint16_t interpolate(uint16_t x);

#endif // BAFF_H
