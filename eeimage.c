
#asm

;-----------------------------------------
; expose anything you add into eeprom here

global		_EE_pwmMax
global		_EE_cycleTicks
global		_EE_flags

global		_EE_AN0UL
global		_EE_AN0LL
global		_EE_AN0dY
global		_EE_dX

;-----------------------------------------

uint8_t			macro	    data
				db		    data
				endm

uint16_t		macro	    data
				uint8_t	    low(data)
				uint8_t	    high(data)
				endm

uint32_t		macro	    data
				uint16_t	data ror 16
				uint16_t	highword(data)
				endm

psect eedata,delta=2,size=0x100,class=EEDATA
				org			0x00

; used as psuedo intensity control, consider bumping this up to a uint16_t
_EE_pwmMax		uint8_t		100
_EE_cycleTicks	uint16_t	50			; initial value before we begin sampling AN0
										; by making this low, you get an initial sample quickly

; used as values to interpolate the cycleTicks speed -------------------------------------
; *BUG* in hi-tech, these must be made global to prevent cromwell from crashing when generating
;       symbol data
global			upperLimit,lowerLimit,limitDiff

upperLimit		equ			350
lowerLimit		equ			50
limitDiff		equ			upperLimit-lowerLimit

_EE_AN0UL		uint32_t	upperLimit
_EE_AN0LL		uint32_t	lowerLimit
_EE_AN0dY		uint32_t	limitDiff
_EE_dX			uint32_t	1024						; a/d resolution
; ----------------------------------------------------------------------------------------


; flag definitions:
;
; bit 0 - invert led state, if set then leds are active low (0 == on)
; bit 1 - 
; bit 2 - 
; bit 3 - 

_EE_flags		uint8_t		00000000B





#endasm
