/*
 * Lab10.c
 *
 * Created: 4/9/2022 4:36:21 PM
 * Author : Alex
 */ 


#define F_CPU 20000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

const uint16_t THRESHOLD = 0x0200;
const uint16_t TIMEOUT = 0x0600;
volatile uint8_t prevSensors = 0x00;
volatile uint16_t dischargeTime[5];
volatile uint8_t hasBeenRead[5];
volatile uint8_t state[5];
volatile uint8_t timedout = 0;
volatile uint8_t currPins;
volatile uint8_t prevPins;
volatile uint16_t count;

uint8_t Rbase = 80;
uint8_t Lbase = 80;
uint8_t a = 4;
uint8_t b = 8;
uint8_t c = 12;
uint8_t d = 16;


ISR(PCINT1_vect){
	currPins = PINC;
	prevPins = prevSensors;
	prevSensors = PINC;
	count = TCNT1;
	for(uint8_t i = 0; i < 5; ++i){
		if(hasBeenRead[i] != 1){
			if(currPins % 2 != prevPins % 2){
				dischargeTime[i] = count;
				hasBeenRead[i] = 1;
				if(count <= THRESHOLD){
					state[i] = 1;
				}
			}
			currPins = currPins >> 1;
			prevPins = prevPins >> 1;
		}
	}
}

ISR(TIMER1_COMPA_vect){
	timedout = 1;
	for(uint8_t i = 0; i < 5; ++i){
		if(hasBeenRead[i] != 1){
			state[i] = 0;
			dischargeTime[i] = 0xFFFF;
		}
	}
}


void motorSetup(void){
	//Turn off motors
	DDRB = (1 << DDB3);
	DDRD = (1 << DDD3) | (1 << DDD5) | (1 << DDD6);
	PORTB = 0x00;
	PORTD = 0x00;
	TCCR0A = 0;
	TCCR0B = 0;
	TCCR0A = _BV(COM0A1) | _BV(COM0A0) | _BV(COM0B1) | _BV(COM0B0) |_BV(WGM01) | _BV(WGM00);
	TCCR0B = _BV(CS01) | _BV(CS00);

	TCCR2A = 0;
	TCCR2B = 0;
	TCCR2A = _BV(COM2A1) | _BV(COM2A0) | _BV(COM2B1) | _BV(COM2B1) | _BV(WGM01) | _BV(WGM00);
	TCCR2B = _BV(CS21) | _BV(CS20);



	OCR0A = 0; //Left Forward Direction = 0
	OCR0B = 0; //Left Backward Direction = 0
	OCR2A = 0; //Right Forward Direction = 0
	OCR2B = 0; //Right Forward Direction = 0
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
	DDRC |= (1 << DDC0) | (1 << DDC1) | (1 << DDC2) | (1 << DDC3) | (1 << DDC4);
	PORTC |= (1 << PORTC0) | (1 << PORTC1) | (1 << PORTC2) | (1 << PORTC3) | (1 << PORTC4);
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

void clearAll(void){
	TCCR1A = 0;
	TCCR1B = 0;
	OCR1A = 0;
	TIMSK1 = 0;
	PORTC = 0x00;
	timedout = 0;
	for(uint8_t i = 0; i < 5; ++i){
		state[i] = 1;
		hasBeenRead[i] = 0;
	}
}


void readSensors(void){
	interruptsOFF();
	/*
	uint8_t a = PINC;
	for(i = 0; i < 5; ++i){
		state[i] = a%2;
		a = a >> 1;
	}
	*/
	LEDsOFF();
	clearAll();
	charge();
	_delay_us(10);
	LEDsON();
	startTimer();
	discharge();
	interruptsON();
	while(timedout != 1)
	{}
	
	
}

int main(void)
{
	motorSetup();
	while (1) {
		readSensors();		
    }
}

