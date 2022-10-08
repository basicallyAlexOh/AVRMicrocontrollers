
.cseg
	ldi zh, high(const*2)
	ldi zl, low(const*2)
	.equ a32v1 = 0x0130 ; address location to place inputs
	.equ SUMLOC = 0x0160 ; address location of sum
	.equ RESULTLOC = 0x0200 ; address for final result

const:
	;Number 1: 0x13154955 , Number 2: 0xA3B6C198
	.db 0x13 ; MSB 1
	.db 0x15 
	.db 0x49
	.db 0x55 ; LSB 1
	.db 0xA3 ; MSB 2
	.db 0xB6
	.db 0xC1
	.db 0x98 ; LSB 2

	rjmp main



add32bit:
	pop r19 ; LSB of return address
	pop r18 ; MSG of return address

	; get numbers off of the stack -> local memory
	ldi xh, high(a32v1 + 8)
	ldi xl, low(a32v1 + 8)
	ldi r16, 8
	poploop:
		pop r0
		st -x, r0
		dec r16
		brne poploop
	

	ldi r16, 4 ; counter for number of times through the loop
	clc ; clear the carry bit
	clz ; clear the zero flag
	ldi xh, high(a32v1 + 8)
	ldi xl, low(a32v1 + 8)
	ldi yh, high(a32v1 + 4)
	ldi yl, low(a32v1 + 4)
	ldi zh, high(SUMLOC+4)
	ldi zl, low(SUMLOC+4)
	addloop:
		ld r0, -x
		ld r1, -y
		breq endofloop
		adc r0, r1
		st -z, r0
		dec r16 ; does not alter the carry flag
		rjmp addloop 

	endofloop:

	in r4, sreg
	ldi xh, high(SUMLOC)
	ldi xl, low(SUMLOC)
	ldi r16, 4
	pushloop2:
		ld r0, x+
		push r0
		dec r16
		brne pushloop2
	out sreg, r4

	brcc nocarryout
	ldi r16, 1
	sts SUMLOC+4, r16 ; places a 1 at SUMLOC + 4 if there is a carry out
	nocarryout:

	push r18
	push r19
	ret




sub32bit:
	pop r25
	pop r24

	;pop all values off of the stack that are being subtracted
	pop r0 ; LSB
	pop r1
	pop r2
	pop r3 ; MSB

	ldi r20, 0xFF
	ldi r21, 0x01
	ldi r22, 0x00
	;invert all bits
	eor r0, r20
	eor r1, r20
	eor r2, r20
	eor r3, r20
	add r0, r21
	adc r1, r22
	adc r2, r22
	adc r3, r22
	
	;r3-r0 now hold the two complement
	push r3
	push r2
	push r1
	push r0

	rcall add32bit


	push r24
	push r25
	ret
	


main:
	ldi r16, low(RAMEND)
	out spl, r16
	ldi r16, high(RAMEND)
	out sph, r16

	ldi r16, 8;
	ldi xl, 0x00
	ldi xh, 0x01

	rcall copybytes ; gets numbers from program memory
	
	;push the numbers
	ldi xl, 0x00
	ldi xh, 0x01
	ldi r16, 8
	pushloop:
		ld r0, x+
		push r0
		dec r16
		brne pushloop



	;;UNCOMMENT WHICHEVER WILL BE USED
	;rcall add32bit
	rcall sub32bit

	
	ldi r16, 4 ; storing result in memory
	ldi xh, high(RESULTLOC+4)
	ldi xl, low(RESULTLOC+4)
	poploop2:
		pop r0
		st -x, r0
		dec r16
		brne poploop2

	
	


	rjmp end;



copybytes:
	clr r2;
	loop1:
		cp r2, r16 ; r2 holds loop counter, r16 is number of iterations
		brsh return1;
		lpm r0, Z+;
		lpm r1, Z+; dummy load
		st X+, r0; data in r0 goes to address pointed to by X
		inc r2; increment counter
		rjmp loop1; go back to the top of the loop

	return1:
		ret; 




end:
	rjmp end;

