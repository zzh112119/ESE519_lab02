#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000


int main(void)
{
	DDRD = 0x40;
	DDRB = 0x01;
	PORTB |= 0x01;
	PORTD |= 0X40;
	TCCR0A = (1 << COM0A0); //Toggle OC0A on compare match
	TCCR0B = (1 << CS02); // clk/256
	TIMSK0 = (1 << OCIE0A); // Timer/Counter0 Output Compare Match A Interrupt Enable
	TCNT0 = 0; //reset counter
	OCR0A += 71;
	sei();
	
	while (1)
	{
	}
}

ISR(TIMER0_COMPA_vect)
{
	OCR0A += 71;
}
