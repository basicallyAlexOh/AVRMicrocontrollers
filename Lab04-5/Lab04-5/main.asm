;
; Lab04-5.asm
;
; Created: 2/23/2022 6:30:24 PM
; Author : Alex
;


; Replace with your application code
.cseg
	ldi r16, 0b1111_1111
	out ddrd, r16
	ldi r16, 0b0000_0011
	out ddrc, r16
	ldi r16, 0b0000_0010
	out portc, r16

	ldi r16, high(RAMEND)
	out sph, r16
	ldi r16, low(RAMEND)
	out spl, r16
	.equ LEFTADD = 0x0200 ; left end of the start of sine table
	.equ interval = 4


init:
	.db 0
	.db 1
	.db 2
	.db 5
	.db 10
	.db 15
	.db 21
	.db 29
	.db 37
	.db 46
	.db 56
	.db 67
	.db 78
	.db 90
	.db 102
	.db 115
	.db 127
	.db 139
	.db 152
	.db 164
	.db 176
	.db 187
	.db 198
	.db 208
	.db 217
	.db 225
	.db 233
	.db 239
	.db 244
	.db 249
	.db 252
	.db 253
	.db 254
	rcall copybytes
	rjmp main



copybytes:
	ldi zh, high(init*2)
	ldi zl, low(init*2)
	ldi xh, high(LEFTADD)
	ldi xl, low(LEFTADD)
	clr r2;
	ldi r16, 33
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

delay1us:
	clr r0
	delay1usloop1:
		dec r0
		brne delay1usloop1
	ret


freqdelay:
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



copysettings:
	; expects the value in r21
	out portd, r21
	ldi r16, 0b0000_0000
	out portc, r16
	rcall delay1us
	ldi r16, 0b0000_0001
	out portc, r16
	ret

sinewave:
	clr r1 ; counter
	ldi r22, 33 ; max number of iterations
	

	sinewaveforwardloop1:
		ld r21, x+

		dec r25
		brne skipdisplay1
		
		ldi r25, interval 
		rcall copysettings

		skipdisplay1:
		inc r1
		;rcall freqdelay
		cp r22, r1
		brne sinewaveforwardloop1

	sinewavebackwardsloop1:
		ld r21, -x
		dec r1

		in r20, sreg
		push r20

		dec r25
		brne skipdisplay2

		ldi r25, interval 
		rcall copysettings
		;rcall freqdelay

		skipdisplay2:

		pop r20
		out sreg, r20
		brne sinewavebackwardsloop1

	ret


main:
	; expects total number of loops of staying on max/min in r19
	ldi r25, interval ; keep track of which to print out
	clr r21
	ldi xl, low(LEFTADD)
	ldi xh, high(LEFTADD)
	mainloop1:
	rcall sinewave
	rjmp mainloop1



