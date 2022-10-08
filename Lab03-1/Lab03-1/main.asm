;
; Lab03-1.asm
;
; Created: 2/15/2022 6:16:03 PM
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
	

main:
	rcall copysettings
	rjmp main
