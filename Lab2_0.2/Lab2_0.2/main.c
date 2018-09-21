/*
 * GccApplication1.c
 *
 * Created: 2018/9/6 17:52:14
 * Author : Evan Zhang
 */ 

#define F_CPU 20000000L

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	//initialization
	DDRB = 0x20;
	PORTB = 0x01;
	
	while (1)
	{
		//if PB0 is low 
		if(!(PINB & 0x01))
		{
			PORTB |= 0x20;
		}
		//if PB0 is high
		else
		{
			PORTB &= 0xDF;
		}
	}
}
