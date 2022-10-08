;
; InterruptExample1.asm
;
; Created: 2/15/2018 12:31:26 PM
; Author : sternbal
;


;; input will be two switches, on B0 and D7.  B0 should trigger PCINT0,
;; D7 should trigger PCINT2
;; D5 is an output for the LED
.equ LED=5
.cseg
.org 0
	jmp RESET
.org INT0addr
    jmp PCI0_ISR
.org PCI0addr
	jmp PCI0_ISR
.org PCI2addr
	jmp PCI2_ISR
.org PCI1addr
	jmp PCI1_ISR
.org INT_VECTORS_SIZE
RESET:
	;; configure stack pointers
	ldi r16,high(RAMEND)
	out SPH,r16
	ldi r16,low(RAMEND)
	out SPL,r16

	;; configure port directions
	cbi DDRB,0  ;; make B0 an input, not really needed
	cbi DDRD,7  ;; make D7 an input, not really needed
	sbi DDRD,5 ;; make D5 and ouput -- needed

	;; another way
	ldi r16,0b00100000
	out DDRD,r16
	;; another way
	ldi r16,0x20
	out DDRD,r16
	;; another way
	ldi r16,1<<DDD4
	out DDRD,r16

	;; enable pullup resistors for switches
	sbi PORTB,0 ;; pullup on B0
	sbi PORTD,7 ;; pullup on D7

	;; turn on my LED
	sbi PORTD,LED

	;; enable pin-change interrupts
	ldi r16,0x07
	//out PCICR,r16 ;; doc8271, p73
	sts PCICR,r16 ;; doc8271, p73

	;; enable interrupts on D7
	ldi r16,0x80
	sts PCMSK2,r16

	;; enable interrupts on B0
	ldi r16,0x03
	sts PCMSK0,r16

	;; Ext Int 0
	ldi r16, 0x01
	out EIMSK, r16


	;; enable global interrupts
	sei

	clr r0 ;; use it as a counter
	ldi r16,0x43  ;; this is an important number!

MAIN:
	ldi r19,0x01
	ldi r20,0x04
	clr r17
	ldi r21,0x11
	ldi r17,0x2E
	clr r19
	ldi r22,0x4A
	clr r21
	clr r22
	breq MAIN
	rjmp END

PCI0_ISR:
	push r16
	in r16,SREG
	push r16
	ldi r17,1
	inc r0
    sbi PORTD,LED
	pop r16
	out SREG,r16
	pop r16
	reti

PCI1_ISR:
	push r16
	in r16,SREG
	push r16

	clr r0

	pop r16
	out SREG,r16
	pop r16
	reti

PCI2_ISR:
	push r16
	in r16,SREG
	push r16

	dec r0
	cbi PORTD,LED

	pop r16
	out SREG,r16
	pop r16
	reti

END:
	rjmp END