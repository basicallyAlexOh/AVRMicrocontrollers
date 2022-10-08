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

uint16_t THRESHOLD = 0x0F00;
uint16_t TIMEOUT = 0x2000;
uint8_t prevSensors = 0x00;
uint16_t dischargeTime[5];
uint8_t hasBeenRead = 0;
uint8_t state = 0;
uint8_t timedout = 0;
uint8_t currPins = 0;
uint8_t prevPins;
uint16_t count;
uint8_t i;
uint8_t leftMotor = 0;
uint8_t rightMotor = 0;



uint8_t Rbase = 80;
uint8_t Lbase = 50;
uint8_t a = 4;
uint8_t b = 8;
uint8_t c = 12;
uint8_t d = 16;

void motorSetup(void);
void leftSpeed(uint8_t forwardSpeed, uint8_t backwardSpeed);
void rightSpeed(uint8_t forwardSpeed, uint8_t backwardSpeed);
void leftForward(uint8_t speed);
void rightForward(uint8_t speed);
void pinToggle(uint8_t pinNum, uint16_t count);
void interruptsOFF(void);
void interruptsON(void);
void LEDsOFF(void);
void LEDsON(void);
void charge(void);
void discharge(void);
void startTimer(void);
void clearAll(void);
void stateToMem(void);
void readToMem(void);
void timeToMem(void);
void readSensors(void);
void config(void);
void setMotors(uint8_t state);

ISR(PCINT1_vect){
	currPins = PINC;
	prevPins = prevSensors;
	prevSensors = PINC;
	count = TCNT1;
	for(i = 0; i < 5; ++i){
		if(hasBeenRead[i] != 1){
			if(currPins % 2 != prevPins % 2){
				pinToggle(i);
			}
			currPins = currPins >> 1;
			prevPins = prevPins >> 1;
		}
	}	
}

ISR(OC1A){
	timedout = 1;
	for(i = 0; i < 5; ++i){
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
	TCCR2A = _BV(COM2A1) | _BV(COM2A0) | _BV(COM2B1) | _BV(COM2B0) | _BV(WGM01) | _BV(WGM00);
	TCCR2B = _BV(CS21) | _BV(CS20);


	OCR0A = 0; //Left Forward Direction = 0
	OCR0B = 0; //Left Backward Direction = 0
	OCR2A = 0; //Right Forward Direction = 0
	OCR2B = 0; //Right Forward Direction = 0
}

void leftSpeed(uint8_t forwardSpeed, uint8_t backwardSpeed){
	OCR0B = forwardSpeed;
	OCR0A = backwardSpeed;
}
void rightSpeed(uint8_t forwardSpeed, uint8_t backwardSpeed){
	OCR2B = forwardSpeed;
	OCR2A = backwardSpeed;
}

void leftForward(uint8_t speed){
	leftSpeed(speed, 0);
}
void rightForward(uint8_t speed){
	rightSpeed(speed, 0);
}

void pinToggle(uint8_t pinNum, uint16_t count){
	dischargeTime[pinNum] = count;
	hasBeenRead[pinNum] = 1;
	if(count <= THRESHOLD){
		state[pinNum] = 1;
	}
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
	for(i = 0; i < 5; ++i){
		state[i] = 1;
		hasBeenRead[i] = 0;
	}
}


void stateToMem(void){
	uint8_t* p1 = 0x0400;
	for(i = 0; i < 5; ++i){
		*(p1+i) = state[i];
	}
}

void readToMem(void){
	uint8_t* p1 = 0x03F4;
	for(i = 0; i < 5; ++i){
		*(p1+i) = hasBeenRead[i];
	}
}
void timeToMem(void){
	uint16_t* p1 = 0x40C;
	for(i = 0; i < 5; ++i){
		*(p1+i) = dischargeTime[i];
	}
}

void readSensors(void){
	interruptsOFF();
	uint8_t a = PINC;
	for(i = 0; i < 5; ++i){
		state[i] = a%2;
		a = a >> 1;
	}
	//config();
	stateToMem();
	LEDsOFF();
	clearAll();
	charge();
	_delay_us(10);
	LEDsON();
	startTimer();
	discharge();
	interruptsON();
	while(PINC % 32 != 0)
	{}
}

void config(void){
	if(state[0] == 0x01 && state[1] == 0x01){
		setMotors(2);
	} else if(state[1] == 0x01 && state[2] == 0x01){
		setMotors(4);
	} else if(state[2] == 0x01 && state[3] == 0x01){
		setMotors(6);
	} else if(state[3] == 0x01 && state[4] == 0x01){
		setMotors(8);
	} else if(state[0] == 0x01){
		setMotors(1);
	} else if(state[1] == 0x01){
		setMotors(3);
	} else if(state[2] == 0x01){
		setMotors(5);
	} else if(state[3] == 0x01){
		setMotors(7);
	} else if(state[4] == 0x01){
		setMotors(9);
	} else{
		setMotors(10);
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
			leftSpeed(0,100);
			rightSpeed(100,0);			
	}
}



int main(void)
{
	motorSetup();
	while (1) {
		readSensors();
    }
}

