;
; Lab01.asm
;
; Created: 1/29/2022 4:49:53 PM
; Author : Alex
;

.cseg
	jmp main;
main:
	clr r0;
	ldi r16, 4;
	ldi XH, high(data);
	ldi XL, low(data);
loop:
	ld r1, X+;
	add r0, r1;
	dec r16;
	brne loop;
	st X+, r0;
fin:
	rjmp fin;

.dseg
data: .byte 5;
sum: .byte 1;