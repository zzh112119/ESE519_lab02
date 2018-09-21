/*
 * Lab_3.2.c
 *
 * Created: 2018/9/16 22:13:33
 * Author : Evan Zhang
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "uart.h"
#include <avr/interrupt.h>
volatile int flag = 0;
volatile unsigned int value1 = 0;
volatile unsigned int value2 = 0;
volatile int overflow = 0;
volatile unsigned int diff = 0;
volatile unsigned int pulse_width =0;
volatile int temp_pulse =0;
volatile int buzzer_count= 200;
void output_length();

// set 8 stages for frequency range
#define C7 15;
#define B7 16;
#define A7 18;
#define G6 20;
#define F6 22;
#define E6 24;
#define D6 26;
#define C6 30;

int main(void)
{
	uart_init();	// init UART
	
	DDRB = 0x00;	// init PORTB to be all inputs
	DDRC = 0xFF;	// init PORTC to be all outputs
	DDRD = 0x40;	// enable PD6 to be output
	
	TCCR1A = 0x00;	// init High byte to zero
	TCCR1B = 0x00;  // init Mid byte to zero
	TCCR1C = 0x00;	// init Low byte to zero
	
	DDRB  |=  (1<<PORTB1) |(1<<PORTB2) | (1<<PORTB3) |(1<<PORTB4) |(1<<PORTB5);	// enable PB1 to be output channel
	DDRB  &= ~(1<<PORTB0);	// enable PB0 to be input channel
	PORTD |=  (1 << PORTD6); //enable PD6 to be output channel 
	PORTB |=  (1<<PORTB1) | (1<<PORTB2) | (1<<PORTB3) | (1<<PORTB4);// change PB1 to high

	TIMSK1 |= (1<<ICIE1) | (1<<TOIE1);  // set overflow interrupt 
	TCCR1B |= (1<<CS11 ) | (1<<ICES1);	// set clock prescale to 8 and enable rising edge capture
	
	TCNT1 = 0;	// reset Counter 1
	
	TCCR0A |= (1 << COM0A0) | (1 << WGM01); //Toggle OC1A on compare match;CTC mode
	TCCR0B |= (1 << CS01) | (1 << CS00); // prescaler 64

	sei();		// enable interrupt
	
	while (1)
	{
		TIMSK1 |= (1<<ICIE1) | (1<<TOIE1);
		if (TCNT1>= 9){		// counts 9 times to obtain 100kHz
			PORTB ^= (1<<PORTB1);
			TCNT1 = 0;
			}
		output_length();
		TIMSK0 |= (1 << OCIE0A);	
	}
}

void output_length(void){
	
	if (value2 < value1){overflow--;} // get rid of extra flow count if value2 < value1 
	
	diff = value2 - value1;
	
	pulse_width = (int)overflow * 65536u + (int)diff;
	
	/*
	if (pulse_width!=0){
		printf("%d \n",-pulse_width); // print to serial
	}
	*/
	
}

ISR(TIMER1_CAPT_vect){
	if (flag){
		TCCR1B |= (1<<ICES1);	// detect rising edge
		TIFR1 |= 0x20;			// clears flag
		value1 = ICR1;			// stores time 1
		flag = 0;
	}
	else{
		TCCR1B &= ~(1<<ICES1);	// detect falling edge
		TIFR1 |= 0x20;			// clears flag
		value2 = ICR1;			// stores time 2
		flag = 1;
	}
	TIMSK1 &= ~(1<<ICIE1) | (1<<TOIE1);
}

ISR(TIMER1_OVF_vect){
	overflow++;					//count overflow
}

ISR(TIMER0_COMPA_vect)
{	
	// change the output voltage of DAC according to the pulse width
	if (pulse_width!=0){
		printf("%d \n",-pulse_width); // print to serial
		PORTB ^= (1 << PORTB5);
	}
	//000
	if(pulse_width > -1187){
		PORTB &= ~(1 << PORTB2);
		PORTB &= ~(1 << PORTB3);
		PORTB &= ~(1 << PORTB4);

	}
	//001
	else if(pulse_width > -1874){
		PORTB |= (1 << PORTB2);
		PORTB &= ~(1 << PORTB3);
		PORTB &= ~(1 << PORTB4);
	}
	//010
	else if(pulse_width > -2561){
		PORTB &= ~(1 << PORTB2);
		PORTB |= (1 << PORTB3);
		PORTB &= ~(1 << PORTB4);
	}
	//011
	else if(pulse_width > -3248){
		PORTB |= (1 << PORTB2);
		PORTB |= (1 << PORTB3);
		PORTB &= ~(1 << PORTB4);
	}
	//100
	else if(pulse_width > -3935){
		PORTB &= ~(1 << PORTB2);
		PORTB &= ~(1 << PORTB3);
		PORTB |= (1 << PORTB4);
	}
	//101
	else if(pulse_width > -4622){
		PORTB |= (1 << PORTB2);
		PORTB &= ~(1 << PORTB3);
		PORTB |= (1 << PORTB4);
	}
	//110
	else if(pulse_width > -5309){
		PORTB &= ~(1 << PORTB2);
		PORTB |= (1 << PORTB3);
		PORTB |= (1 << PORTB4);
	}
	//111
	else{
		PORTB |= (1 << PORTB2);
		PORTB |= (1 << PORTB3);
		PORTB |= (1 << PORTB4);
	}
	TIMSK0 &= ~(1 << OCIE0A);
	
	
}



