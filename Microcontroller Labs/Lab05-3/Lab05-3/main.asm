;
; Lab05-3.asm
;
; Created: 3/2/2022 9:54:09 PM
; Author : Alex
;


; Replace with your application code

.equ diffloc = 0x0200
.equ prevloc = 0x0300

.org 0
jmp reset

.org 0x0014
jmp edgedetect

.org 0x0016
jmp OC1match

.org INT_VECTORS_SIZE

reset:
	;initialize stack pointer
	ldi r16, high(RAMEND)
	out sph, r16
	ldi r16, low(RAMEND)
	out spl, r16


	ldi xh, high(diffloc)
	ldi xl, low(diffloc)
	ldi r16, 0xff
	ldi r17, 0xff
	st x+, r17
	st x+, r16

	ldi xh, high(prevloc)
	ldi xl, low(prevloc)
	ldi r16, 0x00
	ldi r17, 0x00
	st x+, r17
	st x+, r16




	;output cmp
	ldi r17, 0b0100_0001 ; last 3 001 for no prescaling, 1 in bit 6 for rising edge trigger
	ldi r16, 0b0100_0000 ; toggle on cmp match
	sts TCCR1A, r16
	
	ldi r16, 0b0000_0010 ;set data direction on port B
	out ddrb, r16
	out portb, r16

	clr r0 ; r0 will be used for keeping track of last time

	ldi r16, 0xFF ; default dummy value for testing purposes
	sts ocr1ah, r16
	sts ocr1al, r16
	 


	ldi r16, 0b0010_0010 ; enable interrupts from input capture and output cmp A
	sts TIMSK1, r16
	sts TCCR1B, r17
	sei ; enable global interrupts


main:
	rjmp main

OC1match:
	lds r5, tcnt1l ; high byte of load duration of clk cycle
	lds r6, tcnt1h ; low byte of duration of clk

	ldi xh, high(diffloc)
	ldi xl, low(diffloc)
	
	ld r3, x+
	ld r4, x+



	add r3, r5
	adc r4, r6
	; next compare register now in r4:r3
	sts ocr1ah, r4
	sts ocr1al, r3
	

	reti

edgedetect:

	;take input capture
	lds r6, icr1l
	lds r7, icr1h

	; load prev into r4:r3
	ldi xl, low(prevloc)
	ldi xh, high(prevloc)
	
	ld r3, x+
	ld r4, x+

	;update prev
	ldi xl, low(prevloc)
	ldi xh, high(prevloc)
	st x+, r6
	st x+, r7
	
	;sub will take care of case where timer overflows
	

	;find difference between capture and prev
	clc
	sub r6, r3
	sbc r7, r4


	ldi xl, low(diffloc)
	ldi xh, high(diffloc)
	st x+, r6
	st x+, r7
	
	reti