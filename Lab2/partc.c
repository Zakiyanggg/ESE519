/*
 * GccApplication1.c
 *
 * Created: 9/17/2021 12:09:03 PM
 * Author : Zaki
 */ 

#define  F_CPU  16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

ISR(TIMER1_CAPT_vect)
{

	if ((PINB & (1<<PINB0)))    //If PB0 is HIGH
	{
		PORTB &= ~(1<<PORTB5);       //PB5 LOW -> LED off
		TCCR1B &= ~(1 << ICES1);//falling edge
		
	}
	else
	{
		    PORTB |= (1<<PORTB5);     //PB5 HIGH -> LED ON
		    TCCR1B |=(1 << ICES1);    //rising edge trigger
	}
}

int main(void)
{
	DDRB |= (1<<DDB5);//PB5 as output
	PORTB |= (1 << PORTB0);//PB0 as input
	TIMSK1 = (1 << ICIE1);  //capture input enable
	TIFR1 |= (1 << ICF1);  //clear ICP	
	sei();

	while(1)
	{
		_delay_ms(1);
	}
}




