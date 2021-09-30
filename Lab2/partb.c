/*
 * GccApplication1.c
 *
 * Created: 9/17/2021 12:09:03 PM
 * Author : Zaki
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>


int main(void)
{
	/* Replace with your application code */
	DDRB &= ~(1<<DDB0);//PB8 setup as input pin
	PORTB |= (1<<PORTB0);//enable internal pull up resistor
	DDRB &= ~(1<<DDB5);//PB5 setup as output pin
	while (1)
	{
		if(PINB & (1<<PINB0)){
			PORTB &= ~(1<<PORTB5);
		}
		else{
			PORTB |= (1<<PORTB5);
		}
	}
}




