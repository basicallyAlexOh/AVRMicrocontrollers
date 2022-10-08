/*
 * Lab09-1.c
 *
 * Created: 4/7/2022 4:23:34 PM
 * Author : Alex
 */ 

#include <avr/io.h>


int main(void)
{
#define F_CPU 20000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

uint16_t THRESHOLD = 0x0300;
uint16_t TIMEOUT = 0x0600;
uint8_t prevSensors = 0x00;
uint16_t dischargeTime[5];
uint8_t hasBeenRead[5];
uint8_t state[5];
uint8_t timedout = 0;

ISR(PCINT1_vect){
	
}

ISR(OC1A){
	
}

void pinToggle(uint8_t pinNum, uint16_t count){
	dischargeTime[pinNum] = count;
	hasBeenRead[pinNum] = 1;
	if(count <= THRESHOLD){
		state[pinNum] = 1;
	}
}

void motorSetup(void){
	//Turn off motors
	DDRB = (1 << DDB3);
	DDRD = (1 << DDD3) | (1 << DDD5) | (1 << DDD6);
	PORTB = 0x00;
	PORTD = 0x00;
}


void interruptsOFF(void){
	//Turns off interrupts
	cli();
	PCICR &= ~(1 << PCIE1);
}

void interruptsON(void){
	//Turns on Interrupts
	sei();
	PCICR = (1 << PCIE1);
	PCMSK1 = (1 << PCINT8) | (1 << PCINT9) | (1 << PCINT10) | (1 << PCINT11) | (1 << PCINT12);
}

void LEDsOFF(void){
	DDRC |= (1 << DDC5);
	PORTC &= ~(1 << PORTC5);
}

void LEDsON(void){
	DDRC |= (1 << DDC5);
	PORTC |= (1 << PORTC5);
}

void charge(void){
	DDRC = (1 << DDC0) | (1 << DDC1) | (1 << DDC2) | (1 << DDC3) | (1 << DDC4);
	PORTC = (1 << PORTC0) | (1 << PORTC1) | (1 << PORTC2) | (1 << PORTC3) | (1 << PORTC4);
}

void discharge(void){
	DDRC &= ~((1 << DDC0) | (1 << DDC1) | (1 << DDC2) | (1 << DDC3) | (1 << DDC4));
}

void startTimer(void){
	OCR1A = TIMEOUT;
	TIMSK1 = (1 << OCIE1A);
	TCCR1A = (1 << COM1A0);
	TCCR1B = (1 << CS11) | (1 << CS10);
	
}

uint16_t findLight(uint8_t pinNum){
	TCCR1B = 0x00;
	TCCR1C = 0x00;
	TCCR1A = (1 << COM1A1) | (0 << COM1A0);
	TIMSK1 = 0x00;
	TCNT1 = 0;
	OCR1A = THRESHOLD;
	OCR1B = 0;
	TIFR1 = (0 << OCF1B) | (1 << OCF1A) | (0 << TOV1);
	
	
	DDRC |= (1 << pinNum);
	PORTC |= (1 << pinNum);
	_delay_us(10);
	DDRC |= (1 << PORTC5);
	PORTC |= (1 << PORTC5);
	
	TCCR1B = (0 << CS12) | (1 << CS11) | (0 << CS10);
	DDRC &= ~(1 << pinNum);
	PCIFR = (1 << PCIF1);
	PCMSK1 = (1 << pinNum);
	PCICR = (1 << PCIE1);
	sei();
	
	
	return 
}


int main(void)
{
	motorSetup();
	while(1){
		for(uint8_t i = 0; i < 5; ++i){
			dischargeTime[i] = findLight(i);
		}
	}
}


}

