;
; Lab03-4.asm
;
; Created: 2/16/2022 12:35:09 AM
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

	ldi r16, 0b0000_0001
	out PORTD, r16

	ldi r16, 0b0000_0000
	out DDRC, r16
	ldi r16, 0b0011_1111
	out PORTC, r16
	
	jmp main


copysettings:
	inc r16
	out PORTD, r16
	dec r16
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
	clc
	rol r16
	brcs carryleft
	rjmp endleft
	carryleft:
	sbr r16, 4 ; bitmask at 2
	endleft:
	ret


shiftright:
	clc
	sbrc r16, 2
	sec
	ror r16
	ret

main:
	in r16, PINC
	clc
	rol r16
	clc
	rol r16
	rcall copysettings
	ldi r20, 7
	in r21, PIND
	sbrc r21, 0
	rjmp loop1
	rjmp loop23

	loop1:
		dec r20
		breq endloop1
		rcall shiftleft
		rcall copysettings
		rjmp loop1
	endloop1:
		rjmp end

	loop23:
		dec r20
		breq endloop2
		rcall shiftright
		rcall copysettings
		rjmp loop23
	endloop2:
		rjmp end
	end:
	rjmp main
		
