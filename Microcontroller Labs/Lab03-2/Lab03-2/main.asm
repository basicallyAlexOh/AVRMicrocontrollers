;
; Lab03-2.asm
;
; Created: 2/15/2022 7:16:01 PM
; Author : Alex
;


; Replace with your application code

.cseg
	ldi r16, high(RAMEND)
	out sph, r16
	ldi r16, low(RAMEND)
	out spl, r16
	ldi r16, 0b1111_1100
	out DDRD, r16
	ldi r16, 0b0000_0000
	out DDRC, r16
	ldi r16, 0b1111_1111
	out PORTC, r16
	jmp main


copysettings:
	in r16, PINC
	lsl r16
	lsl r16
	out PORTD, r16
	ret
	

delay:
	loop2init:
		ldi r24, 10
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

main:
	
	clr r19
	ldi r18, 0b0000_0100; holds current state
	
	loop1:
		clc
		out PORTD, r18
		rol r18
		rcall delay
		cp r18, r19
		breq flashall
		rjmp loop1
	flashall:
		ldi r18, 0b1111_1100
		out PORTD, r18
		rcall delay
		ldi r18, 0b0000_0100
		rjmp loop1

	rjmp main
