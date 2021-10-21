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
#include <float.h>
#include <string.h>
#include "inc\uart.h"
int Timer0_ovf;
int Timer2_ovf;
int Timer1_ovf;
unsigned long edge1;
unsigned long edge2;
int distance;
char String[25];

ISR(TIMER2_OVF_vect){	
	
	Timer2_ovf++;//overflow every 0.000016s-->16us
	if (Timer2_ovf == 2 ){
	TCCR2B &= ~(1<<CS22);//stop timer0
	TCNT2 = 0;
	Timer2_ovf = 0;
	PORTD &= ~(1<<PORTD6);//set PD6 to LOW
	//sprintf(String,"pb6 low \n");
	//UART_putstring(String);}
		}
}

ISR(TIMER1_OVF_vect){
	Timer1_ovf++;//overflow every 0.004100s-->4100us
}

ISR(TIMER1_CAPT_vect){
	if(PINB & (1<<PINB0)){//if PB0 is HIGH
	TCNT1 = 0;
	//edge1 = TCNT1;
	edge1 = TCNT1+Timer1_ovf*65536;
	TIFR1 |= (1<<ICF1);//clear input capture flag
	TCCR1B &= ~(1 << ICES1);//falling edge
	//sprintf(String,"Edge1 %d \n",edge1);
	//UART_putstring(String);
	}
	else{
		//edge2 = ICR1;
		edge2 = ICR1+Timer1_ovf*65536;
		TIFR1 |= (1<ICF1);//clear input capture flag
		TCCR1B |=(1 << ICES1);    //rising edge trigger
		//sprintf(String,"Edge2 %d \n",edge2);
		//UART_putstring(String);
		distance = (edge2-edge1)/16/58;
		sprintf(String,"distance %d cm\n",distance);
		UART_putstring(String);		
	}
}
int main(void)
{
    cli();
	DDRD |= (1 << DDD6);//PD6 as output
	DDRB &= ~(1<< DDB0);//PB0 as input
	TIMSK1 = (1 << ICIE1);  //capture input enable
	TIFR1 |= (1 << ICF1);  //clear ICP
	TCCR1B |= (1<<CS10);//start timer1 with prescaler = 1		
	TIMSK2 |= (1<<TOIE2);//Enable timer2 ovf 
	TIMSK1 |= (1<<TOIE1);//Enable timer1 ovf 
	TCCR1B |=(1 << ICES1);    //rising edge trigger
	UART_init(BAUD_PRESCALER);

	sei();
    while (1) 
    {	
		if(Timer1_ovf == 40){//timer1 overflows 40 times-->164ms --> measure cycle
			PORTD |= (1<<PORTD6);//set PD6 to HIGH
			//sprintf(String,"pb6 high \n");
			//UART_putstring(String);
			TCCR2B |= (1<<CS22);//start timer2 with prescaler = 256			
			Timer1_ovf = 0;
			}//reset timer1 ovf 
		
	}
}

