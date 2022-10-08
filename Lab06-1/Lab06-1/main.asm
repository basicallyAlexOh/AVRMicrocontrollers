;
; Lab06-1.asm
;
; Created: 3/16/2022 1:35:33 AM
; Author : Alex
;


; Replace with your application code	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Lab 6: Interrupts and (bouncy) switches
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.DEF ZERO=R0
.dseg ; SRAM variables
LEDON: .byte 1 ; LED status ~0 ==> ON, 0 ==> OFF
PCLH: .byte 1 ; Port C pin 0 status 1 ==> High, 0 ==> Low for PCINT0
COUNT: .byte 4 ; number of times the interrupt routine was executed
; interrupt table
; this program requires RESET, INT0, and PCINT1
.cseg
 jmp RESET ; Reset Handler
 jmp EXT_INT0 ; IRQ0 Handler (external hardware interrupt)
;.org PCI1addr ; This is used in part 4, the timed lights, comment it out here
; jmp PC_INT1 ; PCINT1 Handler (pin change interrupt on Port B) for part 4
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

 ; LED status flag
 sts LEDON,ZERO ; 0 ==> LED is not on
 sts LEDON+1,ZERO ; clear this byte to make the next 4 easier to see
 ; number of times the ISR has been executed
 sts COUNT,ZERO ; clear ISR count high byte
 sts COUNT+1,ZERO ; clear ISR count upper middle byte
 sts COUNT+2,ZERO ; clear ISR count lower middle byte
 sts COUNT+3,ZERO ; clear ISR count low byte
; set up port B for output to LED
 sbi DDRB,0 ; bit 0 in Port B DDR <- 1, data direction out
 cbi PORTB,0 ; bit 0 in Port B <- 0, turn off the LED
; set up hardware external interrupt INT0 to trigger when the Arduino board digital
; pin 2 (aliased to both INT0 and Port D pin 2) is pulled low
 ; set up Port D pin 2 for input with pull-up to logic level high
 cbi DDRD,2 ; (a) set it to input
 sbi PORTD,2 ; (b) engage the 5v pull-up
 ; Now set up for a hardware interrupt on INT0 (momentary switch)
; INT0 is on the same pin (Port D pin 2)
sts EICRA,r0 ; clear bits 0,1 so that IRQ is generated when pin is pulled low
sbi EIMSK,0 ; enable hardware interrupt INT0; bit 0 <- 1;
sei ; enable interrupts globally
; wait here for interrupt (Hit break, ||, in debug mode to pause execution here)
SPIN:
 rjmp SPIN
; ISR for external interrupt 0 (INT0)
EXT_INT0:
; load count of number of times ISR has been executed and increment it
 lds r27,COUNT ; load the current ISR count, high byte
 lds r26,COUNT+1 ; load the current ISR count, upper middle byte
 lds r25,COUNT+2 ; load the current ISR count, lower middle byte
 lds r24,COUNT+3 ; load the current ISR count, low byte
 adiw r25:r24,1 ; increment the count
 brcc NOUHINC ; if C==0, nothing to add to the upper half
 adiw r27:r26,1 ; else add one to upper half
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
; save the ISR execution count
 sts COUNT,r27 ; update the ISR count, high byte
 sts COUNT+1,r26 ; update the ISR count, upper middle byte
 sts COUNT+2,r25 ; update the ISR count, lower middle byte
 sts COUNT+3,r24 ; update the ISR count, low byte
 reti