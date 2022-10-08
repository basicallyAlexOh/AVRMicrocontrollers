
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


mult32bit:
	pop r25
	pop r24	

	;taking references off of the stack, x points to first number, y points to second
	pop xl
	pop xh
	pop yl
	pop yh
	ldi zl, low(SUMLOC)
	ldi zh, high(SUMLOC)

	;testing for sign of each number
	push xl
	push xh
	push yl
	push yh

	ldi r16, 5
	clz
	decloop:
		dec r16
		breq decloopend
		ld r0, -x
		ld r0, -y
		rjmp decloop
	decloopend:



	


	ldi r16, 0
	ldi r17, 0
	ld r0, x
	sbrc r0, 7
	ldi r16, 1
	ld r1, y
	sbrc r1, 7
	ldi r17, 1
	cp r16, r17 ; same MSB = 1, different MSB = 0
	ldi r18, 0
	brne sameMSB 
	ldi r18, 1
	sameMSB:

	pop yh
	pop yl
	pop xh
	pop xl



	push zl
	push zh

	ldi r19, 0
	cp r19, r16
	breq twocomp1end
		ldi r20, 5
		mov zl, xl
		mov zh, xh
		sec
		in r21, sreg
		push r21
		twocomp1loop:
			dec r20
			breq twocomp1end
			pop r21
			out sreg, r21
			ld r0, -z
			com r0
			ldi r22, 0
			adc r0, r22
			in r21, sreg
			push r21
			st z, r0
			rjmp twocomp1loop
	twocomp1end:
		sbrc r16, 0
		pop r21
	
	ldi r19, 0
	cp r19, r17
	breq twocomp2end
		ldi r20, 5
		mov zl, yl
		mov zh, yh
		sec
		in r21, sreg
		push r21
		twocomp2loop:
			dec r20
			breq twocomp2end
			ld r0, -z
			com r0
			ldi r22, 0
			pop r21
			out sreg, r21
			adc r0, r22
			in r21, sreg
			push r21
			st z, r0
			rjmp twocomp2loop

	twocomp2end:
		sbrc r17, 0
		pop r21


	pop zh
	pop zl
		



	


	
	

	;;initialize all values to 0.
	ldi r22, 9
	ldi r21, 0x00
	push zl
	push zh
	loop2:
		dec r22
		breq loop2end
		st z+, r21
		rjmp loop2
	loop2end:
	pop zh
	pop zl



	clc
	ldi r22, 5

	outerloop1:
		dec r22
		breq outerloop1end
		ldi r23, 5
		ld r3, -x ; hold byte in r3 and point to next byte

		push yl
		push yh ; saves the y register for use in each iteration
		push zl
		push zh
		push r11
		innerloop1:
		`	pop r11
			dec r23
			breq innerloop1end
			
			
			;restore status register for carry bit
			
			in r11, sreg

			ld r4, -y
			mul r3, r4 ; result goes in r1, r0
			mov r6, r1 ; place MSB in r6 and LSB in r5
			mov r5, r0 ;
			 
			ld r10, z ; fetch current value
			ldd r12, z+1

			adc r10, r5 ; adds the values
			adc r12, r6

			st z+, r10 ; stores it back in place
			st z, r12 ;


			;save the status register
			out sreg, r11
			push r11
			rjmp innerloop1
		innerloop1end:

		pop zh ; restore the z register
		pop zl
		
		inc zl ; increment to next slot (works since mem addresses starts at 0x0200


		pop yh
		pop yl ; restore the y register
		
		
		rjmp outerloop1

	outerloop1end:

	

	;Two's complement if necessary
	ldi r19, 1;
	cp r18, r19
	breq endOfTwoComp ; no need to change if same MSB -> positive answer
	ldi xl, low(SUMLOC)
	ldi xh, high(SUMLOC)


	ldi r17, 9
	sec 
	in r16, sreg
	push r16
	startOfTwoComp:
		dec r17
		breq endOfTwoComp
		ld r0, x
		com r0
		ldi r20, 0
		pop r16
		out sreg, r16
		adc r0, r20
		st x+, r0
		in r16, sreg
		push r16
		rjmp startOfTwoComp

	endOfTwoComp:


	; pushing to stack
	ldi xl, low(SUMLOC)
	ldi xh, high(SUMLOC)
	ldi r16, 9
	loop3:
		dec r16
		breq loop3end
		ld r17, x+
		push r17
		rjmp loop3
	loop3end:
	

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
	 
	ldi xl, 0x04
	ldi xh, 0x01
	ldi yl, 0x08
	ldi yh, 0x01
	push yh
	push yl
	push xh
	push xl

	rcall mult32bit

	
	ldi r16, 8 ; storing result in memory
	ldi xh, high(RESULTLOC+4)
	ldi xl, low(RESULTLOC+4)
	poploop2:
		pop r0
		st x+, r0
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
