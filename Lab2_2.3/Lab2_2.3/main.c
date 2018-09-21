/*
 * Lab 2 2.3
 *
 * Created: 9/13/2018 5:13:58 PM
 * Author : zzh
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "uart.h"
#include <avr/interrupt.h>
#define F_CPU 16000000UL

volatile int flag = 0;
volatile unsigned int value1 = 0;
volatile unsigned int value2 = 0;
volatile int overflow = 0;
volatile unsigned int diff = 0;
volatile unsigned int pulse_width =0;
volatile int temp_pulse =0;
volatile int buzzer_count= 200;
void output_length();
void calculate_buzzer();


int main(void)
{
	uart_init();	// init UART
	
	DDRB = 0x00;	// init PORTB to be all inputs
	DDRC = 0xFF;	// init PORTC to be all outputs
	DDRD = 0xFF;	// init PORTD to be all outputs
	
	TCCR1A = 0x00;	// init High byte to zero
	TCCR1B = 0x00;  // init Mid byte to zero
	TCCR1C = 0x00;	// init Low byte to zero
	
	DDRB  |=  (1<<PORTB1);	// enable PB1 to be output channel
	DDRD  |=  (1 << PORTD6); 
	DDRB  &= ~(1<<PORTB2);	// enable PB2 to be input channel
	PORTB |=  (1<<PORTB1) | (1<<PORTB2) | (1<<PORTB5);// change PB1 PB2 to high

	TIMSK1 |= (1<<ICIE1) | (1<<TOIE1);  // set overflow interrupt 
	TCCR1B |= (1<<CS11 ) | (1<<ICES1);	// set clock prescale to 8 and enable rising edge capture
	
	TCCR0A |= (1 << COM0A0) | (1 << WGM01); //Toggle OC1A on compare match;CTC mode
	TCCR0B |= (1 << CS02); // prescaler 64
	
	
	TCNT0 = 0;
	TCNT1 = 0;	// reset Counter 1
	sei();		// enable interrupt
	
	while (1)
	{
		if (TCNT1>= 9){		// counts 9 times to obtain 100kHz
			PORTB ^= (1<<PORTB1);
			TCNT1 = 0;
			}
		output_length();
		TIMSK0 |= (1 << OCIE0A);
		/*	
		if (TCNT0> (buzzer_count/2)){
			PORTD ^= (1<<PORTD6);
			TCNT0 = 0;
		}
		*/
	}
}

void output_length(void){
	
	if (value2 < value1){overflow--;} // get rid of extra flow count if value2 < value1 
	
	diff = value2 - value1;
	
	pulse_width = (int)overflow * 65536u + (int)diff;
	
	//if (pulse_width!=0){
	//	printf("%d \n",-pulse_width); // print to serial
	//}
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
}

ISR(TIMER1_OVF_vect){
	overflow++;					//count overflow
}

ISR(TIMER0_COMPA_vect){
	temp_pulse = -pulse_width;
	if (temp_pulse<500){temp_pulse=500;} // set boundaries
	if (temp_pulse>5000){temp_pulse=5000;}
	buzzer_count = temp_pulse/5000.0f * 15 + 13; // convert pulse width to compare number

	OCR0A = buzzer_count; // set compare number and get frequency
	printf("%d\n",buzzer_count);
	TIMSK0 &= ~(1 << OCIE0A); // disable compare match interrupt
}
