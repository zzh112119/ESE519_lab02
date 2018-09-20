/*
 * Lab2_1.1.c
 *
 * Created: 2018/9/7 16:59:43
 * Author : Evan Zhang
 */ 

#include <avr/io.h>

#include <avr/interrupt.h>

int led_on ;

ISR(TIMER1_CAPT_vect){
	led_on = ~led_on;
}


int main(void)
{
	DDRB = 0x20;
	PORTB = 0x01;
	TIMSK1 = (1 << ICIE1); // external interrupt	
	TCCR1B = (1<<CS12) | (1<<CS10) ; // refer clock fre = clk/1024; catch falling edge
	TCCR1B &= ~(1 << ICES1);
	TIFR1 |= (1 << ICF1);
	led_on = 0;
	sei(); // global interrupt
	
	while(1){
		if (led_on)
		{
			PORTB |= 0X20;	
			TCCR1B |= (1 << ICES1);		
		}
		else{
			PORTB &= 0XDF;
			TCCR1B &= ~(1 << ICES1);				
		}
	}
}

