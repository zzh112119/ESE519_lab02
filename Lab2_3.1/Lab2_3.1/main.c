/*
 * Lab2 3.1.c
 *
 * Created: 9/17/2018 3:18:00 PM
 * Author : zzh11
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "uart.h"
#include <avr/interrupt.h>
float voltage = 0.0f;

int main(void)
{
	uart_init();
	
	DDRC = 0x0;	// init PORTC to be all inputs
	PORTC &= ~(1 << PORTC0); // set PC0 active high
	
	ADMUX |= (1 << REFS0); //AVcc refer
	ADCSRA |= (1 << ADEN); //turn on ADC
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //division factor = 128, ADC works in
	ADCSRA |= (1 << ADATE); //Signal source, in this case is the free-running
	ADCSRA |= (1 << ADSC); //Start conversation
	ADCSRB &= ~((1 << ADTS2)|(1 << ADTS1)|(1 << ADTS0));//ADC in free-running mode
	
	while (1)
	{
		float voltage = (float)ADC / 1024.0f * 5.0f;
		printf("%.2f V \n",voltage);
		printf("ADC: %d\n", ADC);
	}
}