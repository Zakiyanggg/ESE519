/*
 * GccApplication2.c
 *
 * Created: 10/6/2021 2:51:53 PM
 * Author : Zaki
 */ 
#define  F_CPU  16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "inc/uart.h"


int main(void)
{
    cli();
	PORTD |= (1 << PORTD6);//PD as output
	DDRD |= (1<<DDD6);
	
	TCCR0B |= (1<<CS02);//start timer0 with prescaler = 256

	TCCR0A |=(1<<WGM00);
	TCCR0A &=~(1<<WGM01);
	TCCR0B |= (1<<WGM02);//PWM correct mode
	
	TCCR0A |= (1<<COM0A0);//PD6/OC0A toggle on compare match
	
	OCR0A = 35;

	sei();
    while (1) 
    {
    }
}

