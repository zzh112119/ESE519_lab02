/*
 * Lab2_2.1.c
 *
 * Created: 2018/9/10 13:47:23
 * Author : Evan Zhang
 */ 

#include <avr/io.h>

#define F_CPU 16000000




int main(void)
{
	// initializing ports
	// when reach compare num, PD6(OC0A) will change automatically
	DDRD |= 0x40;
	TCCR0A = (1 << WGM01) | (1 << COM0A0); // fast PWM mode 
	TCCR0B = (1 << CS02); //prescaler = 8
	OCR0A = 50; // set frequency

	
	/* Replace with your application code */
	while (1)
	{
	}
}

