;
; Lab01-2.asm
;
; Created: 1/30/2022 4:07:29 PM
; Author : Alex
;


; Replace with your application code

; Note: code was made for unsigned integers.
.cseg
	jmp main;
main:
	clr r0; r0 holds count
	clr r3; holds max
	ldi r17, 0; holds 0
	ldi XH, high(data);
	ldi XL, low(data);

loop:
	ld r1, X+;
	cp r1, r17;
	breq fin; jump to equal if 0
	inc r0;
	cp r3, r1;
	brsh lessthan;
	mov r3, r1;
lessthan:
	jmp loop;
fin:
	rjmp fin;
	; max value held in r3

.dseg
data: .byte 15;
sum: .byte 1;
	
	