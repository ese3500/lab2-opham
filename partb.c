/*
 * CFile1.c
 *
 * Created: 2/3/2023 2:40:57 PM
 *  Author: Opham
 */ 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void Initialize()
{
	cli();
	
	DDRB &= ~(1<<DDB0);
	DDRB |= (1<<DDB5);
	
	//look for rising edge 
	TCCR1B |= (1<<ICES1);
	
	//clear interrupt 
	TIFR1 |= (1<<ICF1);
	
	//enable input capture interrupt
	TIMSK1 |= (1<<ICIE1);
	sei();
}

ISR(TIMER1_CAPT_vect)
{   if (PINB & (1<<PINB0)) {
		PORTB |= (1<<PORTB5);
		TCCR1B &= ~(1<<ICES1);
    } else {
		PORTB &= ~(1<<PORTB5);
		TCCR1B |= (1<<ICES1);
	}
	
}
int main(void)
{	
	Initialize();
	/* Replace with your application code */
	while(1){
	}

}
