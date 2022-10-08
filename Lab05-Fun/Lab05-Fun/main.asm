;
; Lab05-2.asm
;
; Created: 3/2/2022 9:06:15 PM
; Author : Alex
;


; Replace with your application code

.equ dutycycle = 1 ; 3 = 12.5%, 2 = 25%, 1 = 50%
.equ greaterthanhalf = 0 ; put 1 for greater 75% and 87.5%
.equ totalCount1 = 48539 
.equ totalCount2 = 54484
.equ totalCount3 = 61155
.equ totalCount4 = 48539
.equ totalCount5 = 48539
.equ totalCount6 = 48539
.equ totalCount7 = 54484
.equ totalCount8 = 54484
.equ totalCount9 = 54484
.equ totalCount10 = 48539
.equ totalCount11 = 40816
.equ totalCount12 = 40816

.equ countLoc = 0x0200
.equ numOfNotes = 12


.org 0
jmp reset
.org OC1Aaddr
jmp OC1match
.org INT_VECTORS_SIZE

init:
.db low(totalCount1) ; LSB 1
.db high(totalCount1) ; MSB 1
.db low(totalCount2) ; LSB 1
.db high(totalCount2) ; MSB 1
.db low(totalCount3) ; LSB 1
.db high(totalCount3) ; MSB 1
.db low(totalCount4) ; LSB 1
.db high(totalCount4) ; MSB 1
.db low(totalCount5) ; LSB 1
.db high(totalCount5) ; MSB 1
.db low(totalCount6) ; LSB 1
.db high(totalCount6) ; MSB 1
.db low(totalCount7) ; LSB 1
.db high(totalCount7) ; MSB 1
.db low(totalCount8) ; LSB 1
.db high(totalCount8) ; MSB 1
.db low(totalCount9) ; LSB 1
.db high(totalCount9) ; MSB 1
.db low(totalCount10) ; LSB 1
.db high(totalCount10) ; MSB 1
.db low(totalCount11) ; LSB 1
.db high(totalCount11) ; MSB 1
.db low(totalCount12) ; LSB 1
.db high(totalCount12) ; MSB 1



boundregisters:
	ld r0, x+
	ld r1, x+
	mov r2, r0
	mov r3, r1

	ldi r16, dutycycle+1 ; loop counter
	dec r16
	shiftingregisterloop1:
		breq shiftingregisterloop1end
		clc
		ror r3
		ror r2
		dec r16
		rjmp shiftingregisterloop1
	shiftingregisterloop1end:
	
	clc
	sub r0,r2
	sbc r1,r3
	; r1:r0 now holds larger time
	; r3:r2 now holds smaller time

	ldi r16, greaterthanhalf
	sbrs r16, 0
	rjmp endofbounding
		
	;swap r1:r0 and r3:r2
	mov r4,r2
	mov r5,r3
	mov r2, r0
	mov r3, r1
	mov r0, r4
	mov r1, r5

	endofbounding:
	;;; r1:r0 = timelow, r3:r2 = timehigh
	ret



reset:
	;initialize stack pointer
	ldi r16, high(RAMEND)
	out sph, r16
	ldi r16, low(RAMEND)
	out spl, r16

	rcall copybytes

	ldi xh, high(countLoc)
	ldi xl, low(countLoc)

	rcall boundregisters
	


	in r16, portb ; take input of port b
	sbrc r16, 1
	rcall loadhigh
	sbrs r16, 1
	rcall loadlow



	;output cmp
	ldi r17, 0b0000_1001 ; last 3 001 for no prescaling, 1 in bit 3 for CTC
	ldi r16, 0b0100_0000 ; toggle on cmp match
	sts TCCR1A, r16
	
	ldi r16, 0b0000_0010 ;set data direction on port B
	out ddrb, r16
	out portb, r16


	ldi r16, 0b0000_0010 ; enable interrupts in mask register
	sts TIMSK1, r16
	sts TCCR1B, r17
	sei ; enable global interrupts


main:
	clr r20

	mainloop1:
	inc r20
	rcall freqdelay

	in r16, portb
	push r16
	ldi r16, 0x00
	out portb, r16
	rcall boundregisters
	pop r16
	out portb, r16


	cpi r20, numOfNotes 
	brne mainloop1
	ldi xh, high(countLoc)
	ldi xl, low(countLoc)
	rjmp main

freqdelay:
	push r22
	push r23
	push r24
	loop2init:
		ldi r24, 50
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
	pop r24
	pop r23
	pop r22
	ret


smol:
	push r22
	push r23
	push r24
	loop3init:
		ldi r24, 20
	loop3:
		ldi r23, 0
		nop
	loop3inner:
		ldi r22, 0
	loop3innerinner:
		dec r22
		brne loop3innerinner
		dec r23
		brne loop3inner
		dec r24
		brne loop3
	pop r24
	pop r23
	pop r22
	ret

copybytes:
	push r16
	ldi zh, high(init*2)
	ldi zl, low(init*2)
	ldi xh, high(countLoc)
	ldi xl, low(countLoc)
	clr r2;
	ldi r16, 2*numOfNotes; total amount of numbers * 2
	
	loop1:
		cp r2, r16 ; r2 holds loop counter, r16 is number of iterations
		brsh return1;
		lpm r0, Z+;
		lpm r1, Z+; dummy load
		st X+, r0; data in r0 goes to address pointed to by X
		inc r2; increment counter
		rjmp loop1; go back to the top of the loop

	return1:
		pop r16
		ret; 


OC1match:
	push r16
	in r16, sreg
	push r16

	in r16, portb ; take input of port b
	sbrc r16, 1
	rcall loadhigh
	sbrs r16, 1
	rcall loadlow

	in r16, portb  ;;toggle the outputs 
	com r16
	out portb,r16

	pop r16
	out sreg, r16
	pop r16
	reti


loadlow:
	;don't touch r16
	push r16

	mov r16, r1
	sts ocr1ah, r16
	mov r16, r0
	sts ocr1al, r16

	pop r16
	ret

loadhigh:
	push r16
	mov r16, r3
	sts ocr1ah, r16
	mov r16, r2
	sts ocr1al, r16

	pop r16
	ret