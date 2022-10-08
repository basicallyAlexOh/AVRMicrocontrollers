;
; Lab04-1.asm
;
; Created: 2/23/2022 2:06:56 PM
; Author : Alex
;


; Replace with your application code

.cseg
	ldi r16, 0b1111_1111
	out ddrd, r16
	ldi r16, 0b0000_0011
	out ddrc, r16
	ldi r16, 0b0000_0001
	out portc, r16

	ldi r16, high(RAMEND)
	out sph, r16
	ldi r16, low(RAMEND)
	out spl, r16


	rjmp main

delay1us:
	clr r0
	delay1usloop1:
		dec r0
		brne delay1usloop1
	ret


freqdelay:
	loop2init:
		ldi r24, 1
	loop2:
		ldi r23, 0
		nop
	loop2inner:
		ldi r22, 0
	loop2innerinner:
		dec r22
		brne loop2innerinner
		dec r23
		brne loop2inner
		dec r24
		brne loop2
	ret



copysettings:
	; expects the value in r21
	out portd, r21
	ldi r16, 0b0000_0000
	out portc, r16
	rcall delay1us
	ldi r16, 0b0000_0001
	out portc, r16
	ret



triangularwave:
	ldi r20, 0xFF
	clr r21
	triforwardloop1:
		rcall copysettings
		inc r21
		;rcall freqdelay
		cp r20, r21
		brne triforwardloop1
	tribackwardloop1:
		dec r21
		in r20, sreg
		push r20
		rcall copysettings
		;rcall freqdelay
		pop r20
		out sreg, r20
		brne tribackwardloop1
	ret

rampwave:
	clr r21
	rampforwardloop1:
		rcall copysettings
		rcall freqdelay
		inc r21
		brne rampforwardloop1
	ret


squarewave:
	ldi r21, 0xFF
	rcall copysettings
	rcall freqdelay
	ldi r21, 0x00
	rcall copysettings
	rcall freqdelay
	ret

main:
	rcall squarewave
	rjmp main


