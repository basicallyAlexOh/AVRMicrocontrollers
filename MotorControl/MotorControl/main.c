/*
 * MotorControl.c
 *
 * Created: 4/17/2022 4:46:13 PM
 * Author : Alex
 */ 
#define F_CPU 20000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>


const uint16_t TIMEOUT = 0x0B00;
const uint16_t THRESHOLD = 0x0400;
const uint8_t Rbase = 50;
const uint8_t Lbase = 50;
const uint8_t a = 25;
const uint8_t b = 30;
const uint8_t c = 35;
const uint8_t d = 40;

volatile uint16_t times[5];
volatile uint8_t hasBeenRead[5];
volatile uint8_t sensorArr[5];
volatile uint8_t state[5];
volatile uint16_t cumTimes[5];

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
void leftSpeed(uint8_t forwardSpeed, uint8_t backwardSpeed);
void rightSpeed(uint8_t forwardSpeed, uint8_t backwardSpeed);
void leftForward(uint8_t speed);
void rightForward(uint8_t speed);
void config(void);
void setMotors(uint8_t state);


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
	
		
	TCCR0A = 0;
	TCCR0B = 0;
	TCCR0A = _BV(COM0A1) | _BV(COM0A0) | _BV(COM0B1) | _BV(COM0B0) |_BV(WGM01) | _BV(WGM00);
	TCCR0B = _BV(CS01) | _BV(CS00);
		
	TCCR2A = 0;
	TCCR2B = 0;
	TCCR2A = _BV(COM2A1) | _BV(COM2A0) | _BV(COM2B1) | _BV(COM2B0) | _BV(WGM01) | _BV(WGM00);
	TCCR2B = _BV(CS21) | _BV(CS20);

	OCR0A = 0; //Left Forward Direction = 0
	OCR0B = 0; //Left Backward Direction = 0
	OCR2A = 0; //Right Forward Direction = 0
	OCR2B = 0; //Right Forward Direction = 0
}

void leftSpeed(uint8_t forwardSpeed, uint8_t backwardSpeed){
	OCR0A = backwardSpeed;
	OCR0B = forwardSpeed;
}
void rightSpeed(uint8_t forwardSpeed, uint8_t backwardSpeed){
	OCR2A = backwardSpeed;
	OCR2B = forwardSpeed;
}

void leftForward(uint8_t speed){
	leftSpeed(speed, 0);
}

void rightForward(uint8_t speed){
	rightSpeed(speed, 0);
}


void config(void){
	if(state[0] == 0x01 && state[1] == 0x01){
		setMotors((uint8_t) 2);
	} else if(state[1] == 0x01 && state[2] == 0x01){
		setMotors((uint8_t) 4);
	} else if(state[2] == 0x01 && state[3] == 0x01){
		setMotors((uint8_t) 6);
	} else if(state[3] == 0x01 && state[4] == 0x01){
		setMotors((uint8_t) 8);
	} else if(state[0] == 0x01){
		setMotors((uint8_t) 1);
	} else if(state[1] == 0x01){
		setMotors((uint8_t) 3);
	} else if(state[2] == 0x01){
		setMotors((uint8_t) 5);
	} else if(state[3] == 0x01){
		setMotors((uint8_t) 7);
	} else if(state[4] == 0x01){
		setMotors((uint8_t) 9);
	} else{
		setMotors((uint8_t) 10);
	}
}

void setMotors(uint8_t state){
	switch(state){
		case 1:
			leftForward(Lbase - d);
			rightForward(Rbase + d);
			break;
		case 2:
			leftForward(Lbase - c);
			rightForward(Rbase + c);
			break;
		case 3:
			leftForward(Lbase - b);
			rightForward(Rbase + b);
			break;
		case 4:
			leftForward(Lbase - a);
			rightForward(Rbase + a);
			break;
		case 5:
			leftForward(Lbase);
			rightForward(Rbase);
			break;
		case 6:
			leftForward(Lbase + a);
			rightForward(Rbase - a);
			break;
		case 7:
			leftForward(Lbase + b);
			rightForward(Rbase - b);
			break;
		case 8:
			leftForward(Lbase + c);
			rightForward(Rbase - c);
			break;
		case 9:
			leftForward(Lbase + d);
			rightForward(Rbase - d);
			break;
		default:
			leftSpeed(0,0);
			rightSpeed(0,0);
	}
}



int main(void)
{
	
	motorSetup();
	init();
	while (1)
	{
		
		
		for(uint8_t i = 0; i < 5; ++i){
			cumTimes[i] = 0;
			state[i] = 0;
		}
		for(uint8_t i = 0; i < 4; ++i){
			readSensors();
			for(uint8_t j = 0; j < 5; ++j){
				cumTimes[j] += times[j];
			}
		}
		for(uint8_t i = 0; i < 5; ++i){
			if((cumTimes[i] >> 2) < THRESHOLD){
				state[i] = 0;
			} else{
				state[i] = 1;
			}
		}		
		config();		
	}
}

