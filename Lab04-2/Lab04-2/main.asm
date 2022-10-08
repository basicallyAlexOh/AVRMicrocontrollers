;
; Lab04-2.asm
;
; Created: 2/23/2022 4:06:31 PM
; Author : Alex
;


; Replace with your application code

.cseg
	ldi r16, 0b1111_1111
	out ddrd, r16
	ldi r16, 0b0000_0011
	out ddrc, r16
	ldi r16, 0b0000_0010
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


smalldelay:
	ldi r24, 0
	loop3:
		dec r24
		nop
		brne loop3
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


idle:
	clr r0
	
	idleloop1:
		rcall freqdelay
		inc r0
		cp r0, r19
		brne idleloop1
	ret

trapwave:
	ldi r20, 0xFF
	clr r21
	push r19
	trapforwardloop1:
		rcall copysettings
		inc r21
		;rcall freqdelay
		cp r20, r21
		brne trapforwardloop1

	rcall idle

	trapbackwardloop1:
		dec r21
		in r20, sreg
		push r20
		rcall copysettings
		;rcall freqdelay
		pop r20
		out sreg, r20
		brne trapbackwardloop1
	pop r19
	rcall idle

	ret


main:
	; expects total number of loops of staying on max/min in r19
	ldi r19, 5
	rcall trapwave
	rjmp main


