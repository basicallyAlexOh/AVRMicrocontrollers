/*
 * Lab07.c
 *
 * Created: 3/22/2022 10:06:44 PM
 * Author : Alex
 */ 


#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>



char namearr[4] = {'A', 'L', 'E', 'X'};
char *memPtr = 0x0200; //Place in memory
uint8_t count = 0; //Holds next character
	

void START(void)
{
	TWCR=(1<<TWINT) | (1<<TWSTA) | (1<<TWEN);  // Send a start condition
	_delay_us(5);
	while (!(TWCR & (1<< TWINT))); // wait for TWINT to be set to indicate a START condition has been set
}


void STOP(void){
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
	_delay_us(5);
}


void ADDRESS(char addr)
{
	TWDR=addr;
	TWCR = (1<<TWINT) | (1<<TWEN);
	_delay_us(5);
	while (!(TWCR &(1<<TWINT)));
}

void WRITE(char data)
{
	TWDR = data; // TWDR MUST be written when TWINT is high
	TWCR= _BV(TWINT)|_BV(TWEN);  // enable TWI and clear TWINT to begin transmission
	_delay_us(5);
	while(!(TWCR & _BV(TWINT)));  // wait for TWINT to be set to indicate data has been sent
}

void SET_WRITE(void){
	START();
	ADDRESS(0xA0); //looked online for this
	ADDRESS(0x00);
	ADDRESS(0x00);

	for(int i = 0; i < 4; ++i){
		WRITE(namearr[i]);
	}
	STOP();
}


void ONE_BYTE_WRITE(void)
{
	START();
	ADDRESS(0xA0); //looked online for this
	ADDRESS(0x00);
	ADDRESS(0x00);
	WRITE(namearr[count]);
	STOP();
}

void CONFIG(void){
	TWCR = (1 << TWIE) || (1 << TWEN); //enable 2 wire interface
	TWSR &= (~(1 << TWPS0 || 1 << TWPS1)); // clear the pre-scalar bits
	TWBR = 72; //  ((16000000 /100000) - 16) /2
}


void READ(void)
{
	//Helped from https://www.engineersgarage.com/how-to-use-i2c-twi-two-wire-interface-in-avr-atmega32-part-36-46/
	// Clear TWI interrupt flag,Get acknowlegement, Enable TWI
	TWCR= (1<<TWINT)|(1<<TWEA)|(1<<TWEN);
	_delay_us(5);
	while (!(TWCR & (1<<TWINT))); // Wait for TWINT flag
	char recv_data =TWDR; // Get value from TWDR
	*(memPtr++) = recv_data; // Store Data in SRAM
}

void SET_READ(void)
{
	START();
	ADDRESS(0xA0);
	ADDRESS(0x00);
	ADDRESS(0x00);
	START();
	ADDRESS(0xA1);


	for(int i = 0; i < 4; ++i){
		READ();
	}
	STOP();
}

void ONE_BYTE_READ(void)
{
	START();
	ADDRESS(0xA0);
	ADDRESS(0x00);
	ADDRESS(0x00);
	START();
	ADDRESS(0xA1);
	READ();
	STOP();
}

int main(void)
{
	CONFIG();	
	SET_WRITE();
	SET_READ();

}

