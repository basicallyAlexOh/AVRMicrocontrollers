/**
 * A PWM example for the ATmega328P using the 8-Bit Fast PWM mode.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <util/delay.h>

int main (void)
{   
    // reset both timer/counters
    TCCR0A = 0;
    TCCR0B = 0; 
    // set UNO pin 5/PD5 and pin 6/PD6 to output
    DDRD |= _BV(DDD5);
    DDRD |= _BV(DDD6);
 
    // TCCR0A [ COM0A1 COM0A0 COM0B1 COM0B0 0 0 WGM21 WGM20 ] = 0b10110011
    TCCR0A = _BV(COM0A1) | _BV(COM0B1) | _BV(COM0B0) | _BV(WGM21) | _BV(WGM20);
    // TCCR0B [ FOC2A FOC2B 0 0 WGM22 CS22 CS21 CS20 ] = 0b00000011
    TCCR0B = _BV(CS21) | _BV(CS20);
    OCR0A = 63;
    OCR0B = 127;
}