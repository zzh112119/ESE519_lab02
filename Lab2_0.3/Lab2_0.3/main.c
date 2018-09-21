/*
 * GccApplication2.c
 *
 * Created: 9/6/2018 6:55:00 PM
 * Author : zzh11
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000
int extra_time = 0;

int main(void)
{
	//initialize ports
	DDRD = 0x40;
	DDRB = 0x01;
	PORTB |= 0x01;
	PORTD |= 0X40;
	
	TCCR0A = (1 << WGM01);//enable CTC mode
	OCR0A = 150;//set frequency
	
	TCCR0B = (1<<CS02) | (1<<CS00);

    while (1) 
    {
		extra_time++;
		// when reach compare number
			if(OCIE0A){
				PORTD ^= (1 << PORTD6); // 
				TIMSK0 = (1 << OCIE0A); // Timer/Counter0 Output Compare Match A Interrupt Enable
		}
    }
}
