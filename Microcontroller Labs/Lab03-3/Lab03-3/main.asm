;
; Lab03-3.asm
;
; Created: 2/15/2022 8:42:52 PM
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
	out PORTD, r16
	rcall delay
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

shiftleft:
	sbrc r16, 7
	rjmp flashall1
	rol r16
	rcall copysettings
	rjmp leftend
	flashall1:
		ldi r16, 0b1111_1100
		rcall copysettings
		ldi r16, 0b0000_0100
		rcall copysettings
	leftend:
	ret

shiftright:
	sbrc r16, 2
	rjmp flashall2
	clc
	ror r16
	rcall copysettings
	rjmp rightend
	flashall2:
		ldi r16, 0b1111_1100
		rcall copysettings
		ldi r16, 0b1000_0000
		rcall copysettings
	rightend:
	ret
	 

main:
	ldi r16, 0b0000_0100
	rcall copysettings
	loop1:
		in r17, PINC
		sbrc r17, 0
		rjmp switchon
		rcall shiftright
		rjmp condend
		switchon:
		rcall shiftleft
		condend:
		rjmp loop1
	
	jmp main
