#define  F_CPU  16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "inc/uart.h"
int edge1= 0;
int edge2 = 0;
int edge2_2 = 0;
int edge_count = 0;
char String[25];
volatile int timer1_counter = 0;
volatile int ovf = 0;
volatile int ovf2 = 0;

ISR(TIMER1_CAPT_vect)
{

	if ((PINB & (1<<PINB0)))    //If PB0 is HIGH
	{
		PORTB &= ~(1<<PORTB5);       //PB5 LOW -> LED off
		edge2 = 1;//edge1 count  = 1
		TIFR1 |= (1<<ICF1);//clear input capture flag
		TCCR1B &= ~(1 << ICES1);//falling edge
		TCCR1B &= ~(1<< CS12);//stop the timer1
		TCCR1B &= ~(1<< CS11);
		TCCR1B &= ~(1<< CS10);
		TCCR0B |= (1<< CS01);//start timer0
		TCNT1 = 0;//reset timer counter
		edge2_2 = 1;//wait for space
		ovf2 = 0;//reset counter for space



		}
	else 
	{
		PORTB |= (1<<PORTB5);     //PB5 HIGH -> LED ON
		edge1 =1;//edge2 count  = 1
		edge2_2 = 0;
		TIFR1 |= (1<ICF1);//clear input capture flag
		TCCR1B |=(1 << ICES1);    //rising edge trigger	
		TCCR0B &= ~(1<<CS00);//stop timer0
		TCCR0B &= ~(1<<CS01);	
		TCCR0B &= ~(1<<CS02);
		TCNT0 = 0;		
		TCCR1B |= (1<< CS10);//Start the timer1 with prescaler 0		
		
	}
}

int main(void)
{
	cli();
	UART_init(BAUD_PRESCALER); 
	DDRB |= (1<<DDB5);//PB5 as output
	DDRB |= (1<<DDB4);//PB4 as output
	DDRD |= (1<<DDD3);//PD3 as output
	PORTB |= (1 << PORTB0);//PB0 as input
	TIMSK1 = (1 << ICIE1);  //capture input enable
	TIFR1 |= (1 << ICF1);  //clear ICP
	sei();

	while(1)
	{
				edge_count = edge1+edge2;
				
				if (TCNT1 > 65000)
				{
					ovf = ovf+1;
				}
				


				if (edge_count == 2)
				{

					if(500<=ovf && ovf<= 1200){
					sprintf(String,".");
					UART_putstring(String);
					PORTD |= (1<<PORTD3);
					_delay_ms(50);
					PORTD &= ~(1<<PORTD3);
					edge1 = 0;
					edge2 = 0;
					ovf = 0;
					}
					else if(1201<=ovf && ovf<=2400){
						sprintf(String,"_");
						UART_putstring(String);
						PORTB |= (1<<PORTB4);
						_delay_ms(50);
						PORTB &= ~(1<<PORTB4);
						edge1 = 0;
						edge2 = 0;
						ovf = 0;
					}
					else{
						edge1 = 0;
						edge2 = 0;
						ovf = 0;
					}
				}
				if (TCNT0 > 240)//space
				{
					ovf2 = ovf2+1;
				}
				if(edge2_2 ==1)
				{
					if(ovf2>10){
						sprintf(String,"   ");
						UART_putstring(String);
						edge2_2 = 0;
					}
					else{
						_delay_ms(10);
					}
				}
				
	}
}