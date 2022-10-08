/*
 * Lab08.c
 *
 * Created: 3/30/2022 3:56:22 PM
 * Author : Alex
 */ 
#define F_CPU 16000000
#include <avr/io.h>
#include <avr/delay.h>

void setup(void){
	DDRB = (1 << 3);
	DDRD = (1 << 3) | (1 << 5) | (1 << 6);
}

void PWMsetup(void){
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

void leftSpeed(uint8_t forwardSpeed, uint8_t backwardSpeed){
	OCR0A = forwardSpeed;
	OCR0B = backwardSpeed;
}
void rightSpeed(uint8_t forwardSpeed, uint8_t backwardSpeed){
	OCR2A = forwardSpeed;
	OCR2B = backwardSpeed;
}



void rightFullSpeedTest(void){
	rightSpeed(255, 0);
	leftSpeed(0,0);
	_delay_ms(2000);
	rightSpeed(0,255);
	leftSpeed(0,0);
	_delay_ms(2000);
}

void testBreak(void){
	rightSpeed(0,0);
	leftSpeed(0,0);
	_delay_ms(2000);
}

void leftFullSpeedTest(void){
	leftSpeed(255, 0);
	rightSpeed(0,0);
	_delay_ms(2000);
	leftSpeed(0,255);
	rightSpeed(0,0);
	_delay_ms(2000);
}

void forwardTest(uint8_t speed){
	leftSpeed(speed,0);
	rightSpeed(speed,0);
	_delay_ms(2000);

}

void backwardTest(uint8_t speed){
	leftSpeed(0,speed);
	rightSpeed(0,speed);
	_delay_ms(2000);
}

void testCW(uint8_t speed){
	leftSpeed(0, speed);
	rightSpeed(speed, 0);
	_delay_ms(2000);
}

void testCCW(uint8_t speed){
	leftSpeed(speed, 0);
	rightSpeed(0, speed);
	_delay_ms(2000);
}

void testRoutine(uint8_t speed){
	forwardTest(speed);
	testBreak();
	backwardTest(speed);
	testBreak();
	testCW(speed);
	testBreak();
	testCCW(speed);
	testBreak();
}


int main(void)
{
	setup();
	PWMsetup();
		
    leftFullSpeedTest();
	testBreak();
	rightFullSpeedTest();
	testBreak();
	testRoutine(255);
	testBreak();
	testRoutine(64);
	testBreak();
	
}



