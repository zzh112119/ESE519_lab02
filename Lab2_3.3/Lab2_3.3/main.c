/*
 * Lab 2 2.2.2.c
 *
 * Created: 9/13/2018 5:13:58 PM
 * Author : zzh
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include "uart.h"
#include <avr/interrupt.h>
volatile int flag = 0;
volatile unsigned int value1 = 0;
volatile unsigned int value2 = 0;
volatile int overflow = 0;
volatile unsigned int diff = 0;
volatile unsigned int pulse_width =0;
void output_length();
void ADC_DAC();

#define C7 60;
#define B7 64;
#define A7 71;
#define G6 80;
#define F6 90;
#define E6 95;
#define D6 107;
#define C6 120;

#define ADC_max = 600;
#define ADC_min = 300;
float voltage = 0.0f;

int main(void)
{
	uart_init();	// init UART
	
	DDRB = 0x00;	// init PORTB to be all inputs
	DDRC = 0xFF;	// init PORTC to be all outputs
	DDRD = 0x40;	// enable PD6 to be output
	
	TCCR1A = 0x00;	// init High byte to zero
	TCCR1B = 0x00;  // init Mid byte to zero
	TCCR1C = 0x00;	// init Low byte to zero
	
	DDRB  |=  (1<<PORTB1) | (1<<PORTB2) | (1<<PORTB3)| (1<<PORTB4);	// enable PB1 to be output channel
	PORTD |=  (1 << PORTD6); //enable PD6 to be output channel 
	PORTB |=  (1<<PORTB1) | (1<<PORTB2);// change PB1 PB2 to high

	TIMSK1 |= (1<<ICIE1) | (1<<TOIE1);  // set overflow interrupt 
	TCCR1B |= (1<<CS11 ) | (1<<ICES1);	// set clock prescale to 8 and enable rising edge capture
	
	TCNT1 = 0;	// reset Counter 1
	
	TCCR0A |= (1 << COM0A0) | (1 << WGM01); //Toggle OC1A on compare match;CTC mode
	TCCR0B |= (1 << CS01) | (1 << CS00); // prescaler 64

	DDRC = 0x0;	// init PORTC to be all inputs
	PORTC &= ~(1 << PORTC0); // set PC0 active high
		
	ADMUX |= (1 << REFS0); //AVcc refer
	ADCSRA |= (1 << ADEN); //turn on ADC
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //division factor = 128, ADC works in
	ADCSRA |= (1 << ADATE); //Signal source, in this case is the free-running
	ADCSRA |= (1 << ADSC); //Start conversation
	ADCSRB &= ~((1 << ADTS2)|(1 << ADTS1)|(1 << ADTS0));//ADC in free-running mode

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
		
		//volume control
		ADC_DAC();
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
	if(pulse_width > - 1187){
		OCR0A = C7;
		printf("C7 ");
	}
	else if(pulse_width > -1874){
		OCR0A = B7;
		printf("B7 ");
	}
	else if(pulse_width > -2561){
		OCR0A = A7;
		printf("A7 ");
	}
	else if(pulse_width > -3248){
		OCR0A = G6;
		printf("G6 ");
	}
	else if(pulse_width > -3935){
		OCR0A = F6;
		printf("F6 ");
	}
	else if(pulse_width > -4622){
		OCR0A = E6;
		printf("E6 ");
	}
	else if(pulse_width > -5309){
		OCR0A = D6;
		printf("D6 ");
	}
	else{
		OCR0A = C6;
		printf("C6 ");
	}
	
	TIMSK0 &= ~(1 << OCIE0A);
	
	printf("pulse_width: %d, OCR0A: %d\n",-pulse_width,OCR0A); // print to serial
}


void ADC_DAC(void){
	printf("ADC: %d\n",ADC);

	if((ADC > 350) && (ADC < 380)){
		PORTB &= ~(1 << PORTB2);
		PORTB &= ~(1 << PORTB3);
		PORTB &= ~(1 << PORTB4);

	}
	else if((ADC > 380) && (ADC < 420)){
		PORTB |= (1 << PORTB2);
		PORTB &= ~(1 << PORTB3);
		PORTB &= ~(1 << PORTB4);
	}
	else if((ADC >= 420) && (ADC < 450)){
		PORTB &= ~(1 << PORTB2);
		PORTB |= (1 << PORTB3);
		PORTB &= ~(1 << PORTB4);
	}
	else if((ADC >= 450) && (ADC < 480)){
		PORTB |= (1 << PORTB2);
		PORTB |= (1 << PORTB3);
		PORTB &= ~(1 << PORTB4);
	}
	else if((ADC >= 480) && (ADC < 510)){
		PORTB &= ~(1 << PORTB2);
		PORTB &= ~(1 << PORTB3);
		PORTB |= (1 << PORTB4);
	}
	else if((ADC >= 510) && (ADC < 540)){
		PORTB |= (1 << PORTB2);
		PORTB &= ~(1 << PORTB3);
		PORTB |= (1 << PORTB4);
	}
	else if((ADC >= 540) && (ADC < 570)){
		PORTB &= ~(1 << PORTB2);
		PORTB |= (1 << PORTB3);
		PORTB |= (1 << PORTB4);
	}
	else if((ADC >= 570) && (ADC < 600)){
		PORTB |= (1 << PORTB2);
		PORTB |= (1 << PORTB3);
		PORTB |= (1 << PORTB4);
	}
	else{
		PORTB &= ~(1 << PORTB2);
		PORTB &= ~(1 << PORTB3);
		PORTB &= ~(1 << PORTB4);			
	}
}
