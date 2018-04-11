/**
 * Copyright (c) 2018 Sean Stasiak. All rights reserved.
 * Developed by: Sean Stasiak <sstasiak@protonmail.com>
 * Refer to license terms in license.txt; In the absence of such a file,
 * contact me at the above email address and I can provide you with one.
 */

#ifndef TYPES_H
#define TYPES_H

typedef unsigned char uint8_t;
typedef unsigned int  uint16_t;
typedef unsigned long uint32_t;

#define SET				0x1
#define CLEAR			0x0

#define testbit(var, bit)   ((var) & (1 <<(bit)))
#define setbit(var, bit)    ((var) |= (1 << (bit)))
#define clrbit(var, bit)    ((var) &= ~(1 << (bit)))

#define _paste(a,b)			a##b
#define bitof(var,num)		((var).bits._paste(b,num))

typedef struct {
	unsigned b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1;
} u8BitStruct_t;

typedef struct {
	unsigned b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1, b8:1, b9:1, b10:1, b11:1, b12:1, b13:1, b14:1, b15:1;
} u16BitStruct_t;

typedef union {
	uint8_t		  u8;
	u8BitStruct_t bits;
} u8bitv_t;

typedef union {
	uint16_t		u16;
	u16BitStruct_t	bits;
} u16bitv_t;

// NOTE: when assigning an error, make sure you assign a number - it is used in error dumping routines
typedef enum { // TODO: check to make sure this is an uint8_t
	E_OK				= 0x00,
	E_UNKNOWN_INT		= 0x01,
	E_TIMEOUT			= 0x02,
	E_GENERIC_ERROR		= 0xFF
} enumEC_t;

#endif
