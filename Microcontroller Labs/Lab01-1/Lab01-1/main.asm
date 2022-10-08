;
; Lab01-1.asm
;
; Created: 1/30/2022 3:28:50 PM
; Author : Alex
;


; Replace with your application code
.cseg
	jmp main;
main:
	clr r0; r0 holds count
	ldi r17, 0; holds 0
	ldi XH, high(data);
	ldi XL, low(data);

loop:
	ld r1, X+;
	cp r1, r17;
	breq fin; jump to equal if 0
	inc r0;
	jmp loop;
fin:
	rjmp fin;

.dseg
data: .byte 15;
sum: .byte 1;
	
	
	
