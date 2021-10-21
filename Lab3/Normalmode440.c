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

ISR(TIMER0_COMPA_vect){
	PORTD ^= (1<<DDD6);
	TCNT0 = 0;
}

int main(void)
{
    cli();
	DDRD &= ~(1<<DDD6);//PD6 setup as output pin
	OCR0A = 70;
	TCCR0B |= (1<<CS02);//prescaler 256
	TIMSK0 |= (1<<OCIE0A);//PD6/OC0A toggle on compare match
	sei();
    while (1) 
    {
    }
}

