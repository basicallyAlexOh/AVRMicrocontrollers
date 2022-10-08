;
; Lab01-3.asm
;
; Created: 1/30/2022 4:15:37 PM
; Author : Alex
;


; Replace with your application code
; Note: code was made for unsigned integers.
.cseg
	jmp main;
main:
	clr r0; r0 holds count
	clr r3; holds max
	clr r19; holds 01 if max > 2*min, else holds 00;
	ldi r18, 0xFF; //holds min (initialize to max value)
	ldi r17, 0; holds 0
	ldi XH, high(data);
	ldi XL, low(data);

loop:
	ld r1, X+;
	cp r1, r17;
	breq compute; jump to equal if 0
	inc r0;
	cp r3, r1;
	brsh lessthan;
	mov r3, r1;
lessthan:
	cp r1, r18;
	brsh lessthan2;
	mov r18, r1;
lessthan2:
	jmp loop;
compute:
	add r18, r18;
	cp r18, r3;
	brsh fin;
	ldi r19, 1;
fin:
	rjmp fin;
	; max value held in r19

.dseg
data: .byte 15;
sum: .byte 1;
	