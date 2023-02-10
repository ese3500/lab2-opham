/*
 * GccApplication4.c
 *
 * Created: 2/8/2023 4:32:12 PM
 * Author : Opham
 */ 
/*--------------------Libraries---------------------------*/
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include "inc/uart.h"
int code[4] = {0,0,0,0};
char String[25];
volatile int spaceMode = 0;
volatile int timer = 0;
volatile int currIndex = 0;
volatile int overcount = 1;
volatile int prevtime = 0;
volatile int currtime = 0;
volatile int interval = 0;


// 0 = space, 1 = dot, 2 = dash

void Initialize()
{
	cli();
	
	DDRB &= ~(1<<DDB0);
	DDRB |= (1<<DDB2);
	DDRB |= (1<<DDB3);

	
	//16MHz/1024 prescaling = 15625 Hz, about one overflow every 4 secs****
	TCCR1B |= (1<<CS12); 
	TCCR1B &= ~(1<<CS11); 
	TCCR1B |= (1<<CS10); 
	
	TCCR1A &= ~(1<<WGM10);
	TCCR1A &= ~(1<<WGM11);
	TCCR1A &= ~(1<<WGM12);
	TCCR1A &= ~(1<<WGM13);
		
	//input capture noise canceler
	TCCR1B |= (1<<ICNC1);
	
	//look for rising edge
	TCCR1B |= (1<<ICES1);
	
	//clear interrupt
	TIFR1 |= (1<<ICF1);
	
	//enable input capture interrupt
	TIMSK1 |= (1<<ICIE1);
	sei();
}

void Morse(int interval) {
	//30ms = 469 ticks
	//200ms = 3125 ticks
	//600ms = 9375 ticks
	if ((interval > 9375) || (currIndex > 3)) {
		sprintf(String,"_");
		UART_putstring(String);
		translate(code);
		for (int i = 0; i < 4; i++) {
			code[i] = 0;
		}
		currIndex = 0;
	} else if (interval > 469 && interval <= 3125) {
		sprintf(String,".");
		UART_putstring(String);
		code[currIndex] = 1;
		currIndex++;
		PORTB |= (1<<PORTB2);
		_delay_ms(50);
		PORTB &= ~(1<<PORTB2);
	} else if (interval > 3125 && interval <= 9375) {
		sprintf(String,"-");
		UART_putstring(String);
		code[currIndex] = 2;
		currIndex++;
		PORTB |= (1<<PORTB3);
		_delay_ms(50);
		PORTB &= ~(1<<PORTB3);
	}
}

//decodes the Morse using decoder diagram
void translate(int code[]) {
	if (code[0] == 1) {
		if (code[1] == 0) {
			sprintf(String,"E");
			UART_putstring(String);
		} else if (code[1] == 1) {
			if (code[2] == 0) {
				sprintf(String,"I");
				UART_putstring(String);
				
			} else if (code[2] == 1) {
				if (code[3] == 0) {
					sprintf(String,"S");
					UART_putstring(String);
				} else if (code[3] == 1) {
					sprintf(String,"H");
					UART_putstring(String);
				} else if (code[3] == 2) {
				sprintf(String,"V");
				UART_putstring(String);
				} 
				
			} else if (code[2] == 2) {
				if (code[3] == 0) {
					sprintf(String,"U");
					UART_putstring(String);
				} else if (code[3] ==  1) {
					sprintf(String,"F");
					UART_putstring(String);
				} 
			}
		} else if (code[1] == 2) {
			if (code[2] == 0) {
				sprintf(String,"A");
				UART_putstring(String);
			} else if (code[2] == 1) {
				if (code[3] == 0) {
					sprintf(String,"R");
					UART_putstring(String);
				} else if (code[3] == 1) {
					sprintf(String,"L");
					UART_putstring(String);
				}
			} else if (code[2] == 2) {
				if (code[3] == 0) {
					sprintf(String,"W");
					UART_putstring(String);
				} else if (code[3] == 1) {
					sprintf(String,"P");
					UART_putstring(String);
				} else if (code[3] == 2) {
					sprintf(String,"J");
					UART_putstring(String);
				}
			}
		}
	} else if (code[0] == 2) {
		if (code[1] == 0) {
			sprintf(String,"T");
			UART_putstring(String);
		} else if (code[1] == 2) {
			if (code[2] == 0) {
				sprintf(String,"M");
				UART_putstring(String);
			} else if (code[2] == 2) {
				sprintf(String,"O");
				UART_putstring(String);
			} else if (code[2] == 1) {
				if (code[3] == 0) {
					sprintf(String,"G");
					UART_putstring(String);
				} else if (code[3] == 1) {
					sprintf(String,"Z");
					UART_putstring(String);
				} else if (code[3] == 2) {
					sprintf(String,"Q");
					UART_putstring(String);
				}
			}
		} else if (code[1] == 1) {
			if (code[2] == 0) {
				sprintf(String,"N");
				UART_putstring(String);
			} else if (code[2] == 1) {
				if (code[3] == 0) {
					sprintf(String,"D");
					UART_putstring(String);
				} else if (code[3] == 1) {
					sprintf(String,"B");
					UART_putstring(String);
				} else if (code[3] == 2) {
					sprintf(String,"X");
					UART_putstring(String);
				}
		} else if (code[2] == 2) {
			if (code[3] == 0) {
				sprintf(String,"K");
				UART_putstring(String);
			} else if (code[3] == 1) {
				sprintf(String,"Y");
				UART_putstring(String);
			} else if (code[3] == 2) {
				sprintf(String,"C");
				UART_putstring(String);
			}
			}
		}
	} else {
		sprintf(String,"?");
		UART_putstring(String);
	}
}

ISR(TIMER1_OVF_vect)
{
	overcount++;
}

ISR(TIMER1_CAPT_vect)
{   
	currtime = TCNT1;
	//subtract the difference of times to find the time between presses
	if (prevtime > currtime) {
		interval = currtime + 63536 - prevtime; //accounts for overflowing
	} else {
		interval = currtime - prevtime; //prev normally lower than interval
	}
	if (interval > 9375 && (code[0] != 0)) { //won't try to translate when no morse in array
		Morse(interval);
	} else if (PINB & (1<<PINB0)) { //morse code converter only called when button released
		TCCR1B &= ~(1<<ICES1); // switches to detect falling edge
	} else {
		Morse(interval);
		TCCR1B |= (1<<ICES1); // switches to rising edge
	}
	prevtime = TCNT1; 
}

int main(void)
{
	UART_init(BAUD_PRESCALER);
	Initialize();
	while(1)
	{
	}
}

