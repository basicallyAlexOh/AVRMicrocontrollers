/*
 * LineSensors.c
 *
 * Created: 4/15/2022 11:15:08 AM
 * Author : Alex
 */ 

#define F_CPU 20000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>


const uint16_t TIMEOUT = 0x0B00;
const uint16_t THRESHOLD = 0x0600;

volatile uint16_t times[5];
volatile uint8_t hasBeenRead[5];
volatile uint8_t sensorArr[5];

volatile uint8_t prevPinC;
volatile uint8_t currPinC;
volatile uint8_t xorPinC;
volatile uint16_t currCount;
volatile uint8_t counter;

void init(void);
void readSensors(void);
void motorSetup(void);
void LEDOFF(void);
void LEDON(void);



ISR(PCINT1_vect){
	currCount = TCNT1;
	currPinC = PINC;
	xorPinC = prevPinC ^ currPinC;
	prevPinC = currPinC;
	for(counter = 0; counter < 5; ++counter){
		if(xorPinC % 2 == 1 && hasBeenRead[counter] != 1){
			times[counter] = currCount;
			if(currCount > THRESHOLD){
				sensorArr[counter] = 1;
			} else {
				sensorArr[counter] = 0;
			}
			hasBeenRead[counter] = 1;
		}
		xorPinC = xorPinC >> 1;
	}
}



ISR(TIMER1_COMPA_vect){
	for(counter = 0; counter < 5; ++counter){
		if(hasBeenRead[counter] == 0){
			hasBeenRead[counter] = 1;
			sensorArr[counter] = 1;
			times[counter] = TIMEOUT;
		}
	}
}


void init(void){
	DDRC |= (1 << DDC5); //LED data direction output
	PORTC = 0x00; //Clear Port C	
	
}


void LEDOFF(void){
	PORTC &= ~(1 << PORTC5);

}

void LEDON(void){
	PORTC |= (1 << PORTC5);
}

void readSensors(void){
	cli();
	LEDOFF();
	
	
	prevPinC = (1 << 4) | (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0);
	for(uint8_t i = 0; i < 5; ++i){
		hasBeenRead[i] = 0;
		times[i] = 0;
	}
	PORTC = 0x00;
	TCNT1 = 0;
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1C = 0;
	
	TCCR1A = (1 << COM1A1) | (0 << COM1A0);
	OCR1A = TIMEOUT;
	TIMSK1 = (1 << OCIE1A);
	
	//charge capacitors
	DDRC |= (1 << DDC0) | (1 << DDC1) | (1 << DDC2) | (1 << DDC3) | (1 << DDC4);
	PORTC |= (1 << PORTC0) | (1 << PORTC1) | (1 << PORTC2) | (1 << PORTC3) | (1 << PORTC4);
	_delay_us(10);
	
	//LEDs ON
	DDRC |= (1 << DDC5);
	PORTC |= (1 << PORTC5);
	
	TCCR1B = (1 << CS11);
	DDRC &= ~((1 << DDC0) | (1 << DDC1) | (1 << DDC2) | (1 << DDC3) | (1 << DDC4));
	
	//Interrupts on
	PCICR |= (1 << PCIE1);
	PCMSK1 |= 0x1F;
	
	
	sei();
	while(1)
	{
		if(hasBeenRead[0] == 1 && hasBeenRead[1] == 1 && hasBeenRead[2] == 1 && hasBeenRead[3] == 1 && hasBeenRead[4] == 1){
			break;
		}
	}
	
	TCCR1B = 0;
		

}

void motorSetup(void){
	//Turn off motors
	DDRB = (1 << DDB3);
	DDRD = (1 << DDD3) | (1 << DDD5) | (1 << DDD6);
	PORTB = 0x00;
	PORTD = 0x00;
	
	
}





int main(void)
{
	
	motorSetup();
	init();
    while (1) 
    {
		readSensors();
		
    }
}
