;
; Lab06-4.asm
;
; Created: 3/16/2022 1:44:26 PM
; Author : Alex
;


; Replace with your application code
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Lab 6: Interrupts and (bouncy) switches
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.equ nms = 2000 ; total number of milliseconds of delay
.DEF ZERO=R0
.dseg ; SRAM variables
LEDON: .byte 1 ; LED status ~0 ==> ON, 0 ==> OFF
PCLH: .byte 1 ; Port C pin 0 status 1 ==> High, 0 ==> Low for PCINT0
COUNT: .byte 4 ; number of times the interrupt routine was executed
	; interrupt table
	; this program requires RESET, INT0, and PCINT1
.cseg
	 jmp RESET ; Reset Handler
	 ;jmp EXT_INT0 ; IRQ0 Handler (external hardware interrupt)
.org PCI1addr ; This is used in part 4, the timed lights, comment it out here
	jmp PC_INT1 ; PCINT1 Handler (pin change interrupt on Port B) for part 4
RESET:
main:
	; initialize stack pointer
	 ldi r16,high(RAMEND)
	 out sph,r16
	 ldi r16,low(RAMEND)
	 out spl,r16
	; (R0)<-0x00 aliased to ZERO
	clr ZERO
	; go to program
	 rcall L06P01
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Lab 6 program 1: Toggle LED with momentary push-button switch
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
L06P01:
	; initialize variables

;	 ; LED status flag
;	 sts LEDON,ZERO ; 0 ==> LED is not on
;	 sts LEDON+1,ZERO ; clear this byte to make the next 4 easier to see
;	 ; number of times the ISR has been executed
;	 sts COUNT,ZERO ; clear ISR count high byte
;	 sts COUNT+1,ZERO ; clear ISR count upper middle byte
;	 sts COUNT+2,ZERO ; clear ISR count lower middle byte
;	 sts COUNT+3,ZERO ; clear ISR count low byte

	; set up port B for output to LED
	 sbi DDRB,0 ; bit 0 in Port B DDR <- 1, data direction out
	 cbi PORTB,0 ; bit 0 in Port B <- 0, turn off the LED
	; set up hardware external interrupt INT0 to trigger when the Arduino board digital
	; pin 2 (aliased to both INT0 and Port D pin 2) is pulled low
	 ; set up Port D pin 2 for input with pull-up to logic level high
	 sbi DDRC,0 ; (a) set it to output
	 sbi PORTC,0 ; (b) set it to on

	 ldi r16, 0b0000_0010 ; setup pcint1
	 sts pcmsk0, r16
	 ldi r16, 0b0000_0001 ; setup pcint8
	 sts pcmsk1, r16

	 ldi r16, 0b0000_0010 
	 sts pcicr, r16 

	 ; Now set up for a hardware interrupt on INT0 (momentary switch)
	; INT0 is on the same pin (Port D pin 2)

	ldi r16, 0b0000_0000;
	out portc, r16
	ldi r16, 0b0000_0001;
	out portb, r16

	sts EICRA,r0 ; clear bits 0,1 so that IRQ is generated when pin is pulled low
	sbi EIMSK,0 ; enable hardware interrupt INT0; bit 0 <- 1;
	sei ; enable interrupts globally
	; wait here for interrupt (Hit break, ||, in debug mode to pause execution here)
SPIN:

	in r16, portc
	tst r16
	breq offstate
	ldi r16, 0b0000_0000;
	out portc, r16
	rjmp endofchange
	offstate:
	ldi r16, 0b0000_0001
	out portc, r16
	endofchange:
	rcall waitnmsec

	rjmp SPIN

; ISR for external interrupt 0 (INT0)
PC_INT1:
	push r16
	in r16, sreg
	push r16
	
NOUHINC: ;
	; toggle the LED state
	 lds r17,LEDON ; get the LED status in r17
	 tst r17 ; is it zero?
	 brne ITSON ; no? then the LED is on; go turn it off
	 sbi PORTB,0 ; yes? then it's off; turn it on
	 sbr r17,1 ; set the flag in r17
	 rjmp DUNIT
ITSON:
	 cbi PORTB,0 ; turn it off
	 clr r17 ; clear the flag in r17
DUNIT:
	 sts LEDON,r17 ; save r17 to the LED status flag

	pop r16
	out sreg, r16
	pop r16
	reti









; 1 msec timer subroutine (exact; time includes call and return)
; wait1ms expends 16000 clock cycles.
; rcall wait1ms uses 3 clock cycles
; Therefore from the rcall in the calling program through the
; ret in the subroutine wait1ms expends
; 3+1+1+1+(3996*4)[in loop]+3[end of loop]+1+1+1+4 = 16000cc
; which at 16MHz (cc/s) equals 1ms.
wait1ms:
 ldi zh,0x0F ; 1cc
 ldi zl,0x9D ; 1cc
 clz ; 1cc
waitloop: ; countdown 0x0F9D = 3997dec
 sbiw z,1 ; 2cc
 brne waitloop ; 2cc true, 1cc false
 nop ; 1cc
 nop ; 1cc
 nop ; 1cc
 ret ; 4cc




 waitnmsec:
	push r0
	push r16
	push r17
	push r18
	in r16, sreg
	push r16
	ldi r18, high(nms)
	ldi r17, low(nms)
	
	waitnmsecloop1outer:
		rcall wait1ms
		dec r17
		brne waitnmsecloop1outer
		tst r18
		breq endofnmsloop
		dec r18
		brne waitnmsecloop1outer
	
	endofnmsloop:


	pop r16
	out sreg, r16
	pop r18
	pop r17
	pop r16
	pop r0
	ret