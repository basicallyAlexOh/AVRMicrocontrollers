/*
 * MazeSolver1.c
 *
 * Created: 4/18/2022 11:20:38 AM
 * Author : Alex
 */ 

#define F_CPU 20000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>


const uint16_t TIMEOUT = 0x0B00;
const uint16_t THRESHOLD = 0x0400;
const uint8_t Rbase = 40;
const uint8_t Lbase = 40;
const uint8_t a = 12;
const uint8_t b = 20;
const uint8_t c = 30;
const uint8_t d = 39;
const uint8_t turnDelay = 42;
const uint8_t inchDelay = 28;
const uint8_t uturnDelay = 82;

volatile uint16_t times[5];
volatile uint8_t hasBeenRead[5];
volatile uint8_t sensorArr[5];
volatile uint8_t state[5];
volatile uint16_t cumTimes[5];
volatile char buffer[3];

volatile uint8_t prevPinC;
volatile uint8_t currPinC;
volatile uint8_t xorPinC;
volatile uint16_t currCount;
volatile uint8_t counter;
volatile uint8_t solved = 0;

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
void setMotors(uint8_t code);
void setMotorsMemo(uint8_t code);
void followLine(void);
void firstSolveMaze(void);
void analyzePath(void);
void checkAgain(void);
void flush(void);
void secondSolveMze(void);
void inch(void);
void turnRight(void);
void turnLeft(void);
void uturn(void);
void stop(void);
void testSensors(void);
void solve(void);


//Stack Constructor
struct Stack{
	uint8_t myTop;
	char myStack[200];
};

//STACK IMPLEMENTATION
void push(struct Stack* s, char c);
char peek(struct Stack* s);
char pop(struct Stack* s);

//Declaring a Stack called stk, revStk for reverse
struct Stack stk;
struct Stack revStk;


//void push(const char c)
//Pre: stk is a not full stack. stk.myTop is the index of the
//     next available position.
//Post: Pushes c to s. Increments the value of s.
void push(struct Stack* s, char c){
	if(s->myTop < 200){
		s->myStack[(s->myTop)++] = c;
	}
}

//char peek(struct Stack* s)
//Pre: stk is a not empty stack. stk.myTop is the index of the
//     next available position.
//Post: returns the value of the top of the s.
char peek(struct Stack* s){
	if(s->myTop != 0){
		return s->myStack[(s->myTop)-1];
	}
	return '?';
}

//char pop(struct Stack* s)
//Pre: stk is a non-empty stack
//Post: Removes the first item in the stack and returns it.
char pop(struct Stack* s){
	if(s->myTop != 0){
		return s->myStack[--(s->myTop)];
	}
	return '?';
}

//uint8_t getSize(void)
//Pre: stk has been defined
//Post: Returns the size of stk.
uint8_t getSize(struct Stack* s){
	return s->myTop;
}


//void checkAgain(void)
//Pre: should only be called through analyzePath
//Post: Checks for boundary conditions

void checkAgain(void){
	if(getSize(&revStk) == 1){
		push(&stk, pop(&revStk));
	} else if(getSize(&revStk) == 2){
		push(&stk, pop(&revStk));
		push(&stk, pop(&revStk));
	} else if(getSize(&revStk) == 3){
		buffer[0] = pop(&revStk);
		buffer[1] = pop(&revStk);
		buffer[2] = pop(&revStk);
		if(buffer[0] == 'S' && buffer[1] == 'U' && buffer[2] == 'L'){
			push(&stk, 'R');
		} else if(buffer[0] == 'L' && buffer[1] == 'U' && buffer[2] == 'S'){
			push(&stk, 'R');
		} else if(buffer[0] == 'L' && buffer[1] == 'U' && buffer[2] == 'L'){
			push(&stk, 'S');
		} else{
			push(&stk, buffer[0]);
			push(&stk, buffer[1]);
			push(&stk, buffer[2]);
		}
	} else{
		buffer[0] = pop(&revStk);
		buffer[1] = pop(&revStk);
		buffer[2] = pop(&revStk);
	}
}

void flush(void){
	if(buffer[0] == 'S' && buffer[1] == 'U' && buffer[2] == 'L'){
		push(&stk, 'R');
	} else if(buffer[0] == 'L' && buffer[1] == 'U' && buffer[2] == 'S'){
		push(&stk, 'R');
	} else if(buffer[0] == 'L' && buffer[1] == 'U' && buffer[2] == 'L'){
		push(&stk, 'S');
	} else{
		push(&stk, buffer[0]);
		push(&stk, buffer[1]);
		push(&stk, buffer[2]);
	}
}

//void analyzePath(void)
//Pre: stk has been filled with instructions and revStk is initialized
//Post: places the condensed instructions back in stk
void analyzePath(void){
	
	while(getSize(&stk) != 0){
		push(&revStk, pop(&stk));
	}
	
	if(getSize(&revStk) < 3){
		return;
	} 
	buffer[0] = pop(&revStk);
	buffer[1] = pop(&revStk);
	buffer[2] = pop(&revStk);
	
	if(getSize(&revStk) == 0){
		flush();
		while(getSize(&stk) != 0){
			push(&revStk, pop(&stk));
		}
	}
	
	

	
	
	while(getSize(&revStk) != 0){
		if(buffer[0] == 'S' && buffer[1] == 'U' && buffer[2] == 'L'){
			push(&revStk, 'R');
			checkAgain();
		} else if(buffer[0] == 'L' && buffer[1] == 'U' && buffer[2] == 'L'){
			push(&revStk, 'S');
			checkAgain();
		} else if(buffer[0] == 'L' && buffer[1] == 'U' && buffer[2] == 'S'){
			push(&revStk, 'R');
			checkAgain();
		} else{
			push(&stk, buffer[0]);
			buffer[0] = buffer[1];
			buffer[1] = buffer[2];
			buffer[2] = pop(&revStk);
			if(getSize(&revStk) == 0){
				flush();
			}
		}	
	}
	
	while(getSize(&stk) != 0){
		push(&revStk, pop(&stk));
	}
	
}





//Pin Change Interrupt 1 ISR
//Pre: Pin(s) have been changed on PCINT1(PORTC) bank
//Post: Finds the pins that have been changed. Changes the "hasBeenRead[]" array
//      Updates the timeout time in the "times[]" array
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

//Timer1 Output Compare A ISR
//Pre: Timer/Counter 1 has reached OCR1A
//Post: updates sensorArr[] for those that timed out.
ISR(TIMER1_COMPA_vect){
	for(counter = 0; counter < 5; ++counter){
		if(hasBeenRead[counter] == 0){
			hasBeenRead[counter] = 1;
			sensorArr[counter] = 1;
			times[counter] = TIMEOUT;
		}
	}
}


//void init(void)
//Post: initializes various registers needed for the program, Initializes stack
void init(void){
	DDRC |= (1 << DDC5); //LED data direction output
	PORTC = 0x00; //Clear Port C
	stk.myTop = 0;
	revStk.myTop = 0;
}

//void LEDOFF(void)
//Post: Turns the IR LEDs of
void LEDOFF(void){
	PORTC &= ~(1 << PORTC5);

}


//void LEDON(void)
//Post: Turns the IR LEDs on
void LEDON(void){
	PORTC |= (1 << PORTC5);
}


//void readSensors(void)
//Post: Reads the sensors and places the state in "sensorArr[]".
//      Places the times in the "times[]" array
//Note: charges capacitors and discharges, has a 10 us delay
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


//void motorSetup(void)
//Post: Initializes motor values to inverting control with 8x prescaling.
//      Also turns off all motors.
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


//void leftSpeed(uint8_t forwardSpeed, uint8_t backwardSpeed)
// Pre: forwardSpeed and backWardSpeed are unsigned 8 bit integers
// Post: sets the left motor's forward speed to "forwardSpeed"
//       sets the left motor's backwards speed to "backwardSpeed"
void leftSpeed(uint8_t forwardSpeed, uint8_t backwardSpeed){
	OCR0A = backwardSpeed;
	OCR0B = forwardSpeed;
}

//void rightSpeed(uint8_t forwardSpeed, uint8_t backwardSpeed)
// Pre: forwardSpeed and backWardSpeed are unsigned 8 bit integers
// Post: sets the right motor's forward speed to "forwardSpeed"
//       sets the right motor's backwards speed to "backwardSpeed"
void rightSpeed(uint8_t forwardSpeed, uint8_t backwardSpeed){
	OCR2A = backwardSpeed;
	OCR2B = forwardSpeed;
}


//void leftForward(uint8_t speed)
// Pre: speed is an unsigned 8 bit integer
// Post: sets the left motor's forward speed to "speed"
void leftForward(uint8_t speed){
	leftSpeed(speed, 0);
}

//void leftForward(uint8_t speed)
// Pre: speed is an unsigned 8 bit integer
// Post: sets the left motor's forward speed to "speed"
void rightForward(uint8_t speed){
	rightSpeed(speed, 0);
}


//void turnRight(void)
//Pre: Motors are configured
//Post: Turns the robot 90 degrees to the right
void turnRight(void){
	//TODO: IMPLEMENT HERE
	rightSpeed(0,Rbase);
	leftSpeed(Lbase,0);
	for(uint8_t i = 0; i < turnDelay; ++i){
		_delay_ms(10);
	}
	stop();
	for(uint8_t i = 0; i < turnDelay; ++i){
		_delay_ms(10);
	}
}


//void turnRight(void)
//Pre: Motors are configured
//Post: Turns the robot 90 degrees to the left
void turnLeft(void){
	//TODO: IMPLEMENT HERE
	rightSpeed(Rbase,0);
	leftSpeed(0,Lbase);
	for(uint8_t i = 0; i < turnDelay; ++i){
		_delay_ms(10);
	}
	stop();
	for(uint8_t i = 0; i < turnDelay; ++i){
		_delay_ms(10);
	}
}

//void uturn(void)
//Pre: motors are initialized
//Post: Turns the robot 180 degrees
void uturn(void){
	rightSpeed(0,Rbase);
	leftSpeed(Lbase,0);
	for(uint8_t i = 0; i < uturnDelay; ++i){
		_delay_ms(10);
	}
	stop();
	for(uint8_t i = 0; i < uturnDelay; ++i){
		_delay_ms(10);
	}
}

//void inch(void)
//Post: Moves the robot approximately an inch forward.
void inch(void){
	rightForward(Rbase);
	leftForward(Lbase);
	for(uint8_t i = 0; i < inchDelay; ++i){
		_delay_ms(10);
	}
	stop();
	for(uint8_t i = 0; i < 100; ++i){
		_delay_ms(10);
	}
}

//void stop(void)
//Post: Stops the robot by braking.
void stop(void){
	rightSpeed(0,0);
	leftSpeed(0,0);
}

//void config(void)
//Pre: Motors are setup up and initialized and reflectance sensors
//     been read.
//Post: Sets the motors to a given speed based on the light sensor arrangement.
void config(void){
	if(state[0] == 0x01 && state[1] == 0x01 && state[2] == 0x00 && state[3] == 0x00 && state[4] == 0x00){
		setMotors((uint8_t) 2);
	} else if(state[0] == 0x00 && state[1] == 0x01 && state[2] == 0x01 && state[3] == 0x00 && state[4] == 0x00){
		setMotors((uint8_t) 4);
	} else if(state[0] == 0x00 && state[1] == 0x00 && state[2] == 0x01 && state[3] == 0x01 && state[4] == 0x00){
		setMotors((uint8_t) 6);
	} else if(state[0] == 0x00 && state[1] == 0x00 && state[2] == 0x00 && state[3] == 0x01 && state[4] == 0x01){
		setMotors((uint8_t) 8);
	} else if(state[0] == 0x01 && state[1] == 0x00 && state[2] == 0x00 && state[3] == 0x00 && state[4] == 0x00){
		setMotors((uint8_t) 1);
	} else if(state[0] == 0x00 && state[1] == 0x01 && state[2] == 0x00 && state[3] == 0x00 && state[4] == 0x00){
		setMotors((uint8_t) 3);
	} else if(state[0] == 0x00 && state[1] == 0x00 && state[2] == 0x01 && state[3] == 0x00 && state[4] == 0x00){
		setMotors((uint8_t) 5);
	} else if(state[0] == 0x00 && state[1] == 0x00 && state[2] == 0x00 && state[3] == 0x01 && state[4] == 0x00){
		setMotors((uint8_t) 7);
	} else if(state[0] == 0x00 && state[1] == 0x00 && state[2] == 0x00 && state[3] == 0x00 && state[4] == 0x01){
		setMotors((uint8_t) 9);
	} else if(state[0] == 0x00 && state[1] == 0x00 && state[2] == 0x01 && state[3] == 0x01 && state[4] == 0x01){
		setMotors((uint8_t) 10);
	} else if(state[0] == 0x01 && state[1] == 0x01 && state[2] == 0x01 && state[3] == 0x00 && state[4] == 0x00){
		setMotors((uint8_t) 11);
	} else if(state[0] == 0x01 && state[1] == 0x01 && state[2] == 0x01 && state[3] == 0x01 && state[4] == 0x01){
		setMotors((uint8_t) 12);
	} else if(state[0] == 0x00 && state[1] == 0x00 && state[2] == 0x00 && state[3] == 0x00 && state[4] == 0x00){
		setMotors((uint8_t) 13);
	} else if(state[0] == 0x00 && state[1] == 0x01 && state[2] == 0x01 && state[3] == 0x01 && state[4] == 0x00){
		setMotors((uint8_t) 12);
	} else if(state[0] == 0x01 && state[1] == 0x01 && state[2] == 0x01 && state[3] == 0x01 && state[4] == 0x00){
		setMotors((uint8_t) 16);
	} else if(state[0] == 0x00 && state[1] == 0x01 && state[2] == 0x01 && state[3] == 0x01 && state[4] == 0x01){
		setMotors((uint8_t) 15);
	} else {
		setMotors((uint8_t) 99);
	}
}


//void config(void)
//Pre: The maze has already been solved
//Post: Sets the motors to a given speed based on the light sensor arrangement for memorized route.
void configMemo(void){
	if(state[0] == 0x01 && state[1] == 0x01 && state[2] == 0x00 && state[3] == 0x00 && state[4] == 0x00){
		setMotorsMemo((uint8_t) 2);
	} else if(state[0] == 0x00 && state[1] == 0x01 && state[2] == 0x01 && state[3] == 0x00 && state[4] == 0x00){
		setMotorsMemo((uint8_t) 4);
	} else if(state[0] == 0x00 && state[1] == 0x00 && state[2] == 0x01 && state[3] == 0x01 && state[4] == 0x00){
		setMotorsMemo((uint8_t) 6);
	} else if(state[0] == 0x00 && state[1] == 0x00 && state[2] == 0x00 && state[3] == 0x01 && state[4] == 0x01){
		setMotorsMemo((uint8_t) 8);
	} else if(state[0] == 0x01 && state[1] == 0x00 && state[2] == 0x00 && state[3] == 0x00 && state[4] == 0x00){
		setMotorsMemo((uint8_t) 1);
	} else if(state[0] == 0x00 && state[1] == 0x01 && state[2] == 0x00 && state[3] == 0x00 && state[4] == 0x00){
		setMotorsMemo((uint8_t) 3);
	} else if(state[0] == 0x00 && state[1] == 0x00 && state[2] == 0x01 && state[3] == 0x00 && state[4] == 0x00){
		setMotorsMemo((uint8_t) 5);
	} else if(state[0] == 0x00 && state[1] == 0x00 && state[2] == 0x00 && state[3] == 0x01 && state[4] == 0x00){
		setMotorsMemo((uint8_t) 7);
	} else if(state[0] == 0x00 && state[1] == 0x00 && state[2] == 0x00 && state[3] == 0x00 && state[4] == 0x01){
		setMotorsMemo((uint8_t) 9);
	} else if(state[0] == 0x00 && state[1] == 0x00 && state[2] == 0x01 && state[3] == 0x01 && state[4] == 0x01){
		setMotorsMemo((uint8_t) 10);
	} else if(state[0] == 0x01 && state[1] == 0x01 && state[2] == 0x01 && state[3] == 0x00 && state[4] == 0x00){
		setMotorsMemo((uint8_t) 11);
	} else if(state[0] == 0x01 && state[1] == 0x01 && state[2] == 0x01 && state[3] == 0x01 && state[4] == 0x01){
		setMotorsMemo((uint8_t) 12);
	} else if(state[0] == 0x00 && state[1] == 0x00 && state[2] == 0x00 && state[3] == 0x00 && state[4] == 0x00){
		setMotorsMemo((uint8_t) 13);
	}  else if(state[0] == 0x00 && state[1] == 0x01 && state[2] == 0x01 && state[3] == 0x01 && state[4] == 0x00){
		setMotorsMemo((uint8_t) 12);
	} else if(state[0] == 0x01 && state[1] == 0x01 && state[2] == 0x01 && state[3] == 0x01 && state[4] == 0x00){
		setMotorsMemo((uint8_t) 16);
	} else if(state[0] == 0x00 && state[1] == 0x01 && state[2] == 0x01 && state[3] == 0x01 && state[4] == 0x01){
		setMotorsMemo((uint8_t) 15);
	} else {
		setMotorsMemo((uint8_t) 99);
	}
}

//void setMotors(uint8_t code)
//Pre: Should only be called through the config() function
//Post: Sets the motors to the corresponding values depending on the current state of the robot.
void setMotors(uint8_t code){
	switch(code){
		case 1:
			//10000
			leftForward(Lbase - d);
			rightForward(Rbase + d);
			break;
		case 2:
			//11000
			leftForward(Lbase - c);
			rightForward(Rbase + c);
			break;
		case 3:
			//01000
			leftForward(Lbase - b);
			rightForward(Rbase + b);
			break;
		case 4:
			//01100
			leftForward(Lbase - a);
			rightForward(Rbase + a);
			break;
		case 5:
			//00100
			leftForward(Lbase);
			rightForward(Rbase);
			break;
		case 6:
			//00110
			leftForward(Lbase + a);
			rightForward(Rbase - a);
			break;
		case 7:
			//00010
			leftForward(Lbase + b);
			rightForward(Rbase - b);
			break;
		case 8:
			//00011
			leftForward(Lbase + c);
			rightForward(Rbase - c);
			break;
		case 9:
			//00001
			leftForward(Lbase + d);
			rightForward(Rbase - d);
			break;
		case 10:
			//right or straight + right (00111)
			inch();
			avgSensors();
			
			if(state[0] == 0 && state[1] == 0 && state[2] == 0 && state[3] == 0 && state[4] == 0){
				//if hit a right only
				turnRight();
			} else if(state[2] == 1 || state[1] == 1 || state[3] == 1){
				//if straight or right
				//keep going straight
				//Push to stack
				//TODO: Fill in here
				push(&stk, 'S');
			}
			break;
		case 11:
			//left only or straight + left (11100)
			inch();
			avgSensors();
			if(state[0] == 0 && state[1] == 0 && state[2] == 0 && state[3] == 0 && state[4] == 0){
				//if hit a left only
				turnLeft();
			} else if(state[2] == 1 || state[1] == 1 || state[3] == 1){
				//if straight or left
				turnLeft();
				//Push to stack
				//TODO: Fill in here
				push(&stk, 'L');
			}
			break;	
		case 12:
			//All sensors (11111) or 11110 or 01111 or 01110
			inch();
			avgSensors();
			if(state[2] == 0){
				//T intersection
				turnLeft();
				/*
				for(uint8_t i = 0; i < 50; ++i){
					_delay_ms(100);
				}
				*/
				//Push to stack
				push(&stk, 'L');
				
			} else if(state[0] == 1 && state[1] == 1 && state[2] == 1 && state[3] == 1 && state[4] == 1){
				//Finished maze
				solved = 1;
			} else{
				//4 way intersection
				turnLeft();
				//Push to stack
				push(&stk, 'L');
				
			}
			break;
		case 13:
			// Dead end (00000)
			stop();
			for(uint8_t i = 0; i < 10; ++i){
				_delay_ms(100);
			}
			uturn();
			//Push to U stack
			push(&stk, 'U');
			
			break;
			
		case 14:
			//01110
			inch();
			break;
		case 15:
			//01111
			rightSpeed(20,0);
			leftSpeed(0,20);
			break;
		case 16:
			//11110
			rightSpeed(0,20);
			leftSpeed(20,0);
			break;
			
		
		default:
			leftSpeed(0,0);
			rightSpeed(0,0);
			break;
	}
}


//void setMotorsMemo(uint8_t code)
//Pre: Should only be called through the configMemo() function. revStk should be filled with instructions.
//Post: Sets the motors to the corresponding values depending on the current state of the robot.
void setMotorsMemo(uint8_t code){
	switch(code){
		case 1:
			//10000
			leftForward(Lbase - d);
			rightForward(Rbase + d);
			break;
		case 2:
			//11000
			leftForward(Lbase - c);
			rightForward(Rbase + c);
			break;
		case 3:
			//01000
			leftForward(Lbase - b);
			rightForward(Rbase + b);
			break;
		case 4:
			//01100
			leftForward(Lbase - a);
			rightForward(Rbase + a);
			break;
		case 5:
			//00100
			leftForward(Lbase);
			rightForward(Rbase);
			break;
		case 6:
			//00110
			leftForward(Lbase + a);
			rightForward(Rbase - a);
			break;
		case 7:
			//00010
			leftForward(Lbase + b);
			rightForward(Rbase - b);
			break;
		case 8:
			//00011
			leftForward(Lbase + c);
			rightForward(Rbase - c);
			break;
		case 9:
			//00001
			leftForward(Lbase + d);
			rightForward(Rbase - d);
			break;
		case 10:
			//right or straight + right (00111)
			inch();
			avgSensors();
			
			if(state[2] == 0 && state[3] == 0 && state[1] == 0){
				//if hit a right only
				turnRight();
			} else if(state[2] == 1 || state[1] == 1 || state[3] == 1){
				//if straight or right
				if(peek(&revStk) == 'R'){
					turnRight();
				} else {
					 inch();
				}
				pop(&revStk);
				_delay_ms(100);

			}
			break;
		case 11:
			//left only or straight + left (11100)
			inch();
			avgSensors();
			if(state[1] == 0 && state[2] == 0 && state[3] == 0){
				//if hit a left only
				turnLeft();
			} else if(state[2] == 1 || state[1] == 1 || state[3] == 1){
				//if straight or left
				if(peek(&revStk) == 'L'){
					turnLeft();
				} else{
					inch();
				}
				pop(&revStk);
				_delay_ms(100);

			}
			break;
		case 12:
			//All sensors (11111)
			inch();
			avgSensors();
			if(state[2] == 0){
				//T intersection
				if(peek(&revStk) == 'L'){
					turnLeft();
				} else if(peek(&revStk) == 'R'){
					turnRight();
				}
				pop(&revStk);
				_delay_ms(100);

			} else if(state[0] == 1 && state[1] == 1 && state[2] == 1 && state[3] == 1 && state[4] == 1){
				solved = 1;
				//Finished maze
				
			} else{
				if(peek(&revStk) == 'L'){
					turnLeft();
				} else if(peek(&revStk) == 'R'){
					turnRight();
				} else{
					inch();
				}
				pop(&revStk);
				_delay_ms(100);

			}
			break;
		case 13:
			// Dead end (00000) - SHOULD NOT HAPPEN!
			//Stop the robot.
			leftSpeed(40,0);
			rightSpeed(0,40);
			
			break;
			
		case 14:
			//01110
			inch();
			break;
		case 15:
			//01111
			rightSpeed(20,0);
			leftSpeed(0,20);
			break;
		case 16:
			//11110
			rightSpeed(0,20);
			leftSpeed(20,0);
			break;
		default:
			leftSpeed(0,0);
			rightSpeed(0,0);
			break;
	}
}



//void avgSensors(void)
//Pre: cumTimes and state are initialized.
//Post: takes an average of 4 readings and compares it to set the state of the sensors in the state[] array.
void avgSensors(void){
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
}


//void followLine(void)
//Pre: Motors and sensors are initialized.
//Post: takes a reading, then sets the motors to the correct value.
void followLine(void){
	avgSensors();
	config();
}




//void firstSolveMaze(void)
//Pre: all methods and variables are predefined globally
//Post: Solves the maze the first time, and stores the raw data on a global stack.
void firstSolveMaze(void){
	while(solved != 1){
		_delay_ms(10);
		avgSensors();
		config();
	}
}

//void firstSolveMaze(void)
//Pre: all methods and variables are predefined globally
//Post: Solves the maze the second time, using the data on stk.
void secondSolveMaze(void){
	solved = 0;
	while(solved != 1){
		_delay_ms(10);
		avgSensors();
		configMemo();
	}
}


//void testStack(void)
//Pre: stk and revStk are intialized
//Post: tests the path analysis algorithm.
void testStack(void){
	
	push(&stk, 'L');
	push(&stk, 'U');
	push(&stk, 'L');
	push(&stk, 'L');
	push(&stk, 'S');
	push(&stk, 'L');
	push(&stk, 'U');
	push(&stk, 'S');
	push(&stk, 'L');
	push(&stk, 'L');
	push(&stk, 'U');
	push(&stk, 'L');
	push(&stk, 'S');
	push(&stk, 'U');
	push(&stk, 'L');
	push(&stk, 'L');
	push(&stk, 'U');
	push(&stk, 'L');
	
	
	
	/*
	push(&stk, 'L');
	push(&stk, 'U');
	push(&stk, 'S');
	*/
	analyzePath();
	
}

void testSensors(void){
	motorSetup();
	init();
	while(1){
		readSensors();
		for(uint8_t i = 0; i < 100; ++i){
			_delay_ms(10);
		}
	}
}

void solve(void){
	firstSolveMaze();
	for(uint8_t i = 0; i < 50; ++i){
		_delay_ms(100);
	}
	analyzePath();
	secondSolveMaze();
}

int main(void)
{
	
	motorSetup();
	init();
	for(uint8_t i = 0; i < 10; ++i){
		_delay_ms(100);
	}
	solve();
	//testStack();
	_delay_ms(100);
	
	
	/*
	firstSolveMaze();
	for(uint8_t i = 0; i < 50; ++i){
		_delay_ms(100);
	}
	analyzePath();
	
	
	
	while(getSize(&revStk) != 0){
		char c = pop(&revStk);
		if(c == 'L'){
			turnLeft();
		} else if(c == 'R'){
			turnRight();
		} else if(c == 'U'){
			uturn();
		} else{
			inch();
		}
	}
	
	*/
	
	
	
	
	
}
