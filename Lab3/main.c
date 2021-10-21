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
int mode = 1;
int duty_cycle;


ISR(TIMER2_OVF_vect){	
	
	Timer2_ovf++;//overflow every 0.000016s-->16us
	if (Timer2_ovf == 1 ){
	TCCR2B &= ~(1<<CS21);//stop timer2
	TCNT2 = 0;
	Timer2_ovf = 0;
	PORTD &= ~(1<<PORTD5);//set PD5 to LOW
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
		//sprintf(String,"distance %d cm\n",distance);
		//UART_putstring(String);		
	}
}
ISR(PCINT2_vect){
	mode = !mode;
	sprintf(String,"ADC %d\n",ADC);
	UART_putstring(String);
}

ISR(ADC_vect){
	
}


int main(void)
{
   
	cli();
	DDRD |= (1 << DDD5);//PD5 as output
	DDRD |= (1 << DDD3);//PD3 as output
	DDRB |= (1 << DDB1);//PD9 as output
	DDRB &= ~(1<< DDB0);//PB0 as input
	DDRD &= ~(1<< DDD4);//PD4 as input
	TIFR0 |= (1<<OCF0A);//clear flag timer0 com match
	TIMSK1 = (1 << ICIE1);  //capture input enable
	TIFR1 |= (1 << ICF1);  //clear ICP
	TCCR1B |= (1<<CS10);//start timer1 with prescaler = 1
	TIMSK2 |= (1<<TOIE2);//Enable timer2 ovf
	TIMSK1 |= (1<<TOIE1);//Enable timer1 ovf
	TCCR1B |=(1 << ICES1);    //rising edge trigger
	PCICR |= (1<<PCIE2);//enable PD4 to Pin change interrupt
	PCMSK2 |= (1<<PCINT20);
	PORTD |= (1<<PORTD4);//enable pull up resistor
	PORTD |= (1 << PORTD6);//PD6 as output
	DDRD |= (1<<DDD6);
	TCCR0B |= (1<<CS02);//start timer0 with prescaler = 256
	TCCR0A |= (1<<WGM00);
	TCCR0A &= ~(1<<WGM01);
	TCCR0B |= (1<<WGM02);//PWM correct mode
	TCCR0A |= (1<<COM0A0);//PD6/OC0A toggle on compare match
	
	//ADC setup
	 PRR &= ~(1<<PRADC);//writing 0 to disable power reduction for ADC
	 ADMUX |= (1<<REFS0);//Vref = Vcc
	 ADMUX &= ~(1<<REFS1);
	 ADCSRA |= (1<<ADPS0);//ADC clock/128 16MHz/128 = 125kHz
	 ADCSRA |= (1<<ADPS1);
	 ADCSRA |= (1<<ADPS2);
	 ADMUX &= ~(1<<MUX0);//select channel 0
	 ADMUX &= ~(1<<MUX1);
	 ADMUX &= ~(1<<MUX2);
	 ADMUX &= ~(1<<MUX3);
	 ADCSRA |= (1<<ADATE);//Set to auto trigger
	 ADCSRB &= ~(1<<ADTS0);//set to free running
	 ADCSRB &= ~(1<<ADTS1);
	 ADCSRB &= ~(1<<ADTS2);
	 DIDR0 |= (1<<ADC0D);//disable digital input buffer on ADC pin
	 ADCSRA |= (1<<ADEN);//enable ADC
	 // ADCSRA |= (1<<ADIE);//enable ADC interrupt
	 ADCSRA |= (1<<ADSC);//start conversion
	
	
	sei();
	
	UART_init(BAUD_PRESCALER);

	
    while (1) 
    {	

			
			if(mode==1){
				//continuous
				PORTD &= ~(1<<PORTD3);       //PD3 LOW -> LED off
				PORTB |= (1<<PORTB1);     //PD9 HIGH -> LED ON
				if(Timer1_ovf == 40){//timer1 overflows 40 times-->164ms --> measure cycle
					PORTD |= (1<<PORTD5);//set PD5 to HIGH
					TCCR2B |= (1<<CS21);//start timer2 with prescaler = 256
					//sprintf(String,"Mode mode %d \n",mode);
					//UART_putstring(String);
					Timer1_ovf = 0;
					}//reset timer1 ovf
				if(distance > 200){
					OCR0A = 0;
				}
				else{
					OCR0A = -0.3*distance +30;
				}
			}
			if(mode==0){
				//discrete
				PORTB &= ~(1<<PORTB1);       //PD9 LOW -> LED off
				PORTD |= (1<<PORTD3);     //PD3 HIGH -> LED ON
				if(distance > 200){
					OCR0A = 0;
				}
				if(Timer1_ovf == 40){//timer1 overflows 40 times-->164ms --> measure cycle
					PORTD |= (1<<PORTD5);//set PD5 to HIGH
					TCCR2B |= (1<<CS21);//start timer2 with prescaler = 256
					//sprintf(String,"Mode mode %d \n",mode);
					//UART_putstring(String);
					Timer1_ovf = 0;
				}//reset timer1 ovf
				if (2<distance && distance< 10){
					OCR0A = 29;
				}
				if (9<distance && distance< 18){
					OCR0A = 26;
				}
				if (17<distance && distance< 26){
					OCR0A = 23;
				}
				if (25<distance && distance< 34){
					OCR0A = 22;
				}
				if (33<distance && distance< 42){
					OCR0A = 19;
				}
				if (41<distance && distance< 50){
					OCR0A = 17;
				}
				if (49<distance && distance< 58){
					OCR0A = 15;
				}
				if (57<distance && distance< 66){
					OCR0A = 14;}
			}
			 if((10<ADC) && (ADC < 110)) {
					OCR0B = 0.05*OCR0A;
				 }
			 if((110<ADC) && (ADC < 210)) {
					 OCR0B = 0.1*OCR0A;
				 }
			 if((210<ADC) && (ADC < 310)) {
					 OCR0B = 0.15*OCR0A;
				 }
			 if((310<ADC) && (ADC < 410)) {
					 OCR0B = 0.2*OCR0A;
				 }
			 if((410<ADC) && (ADC < 510)) {
					 OCR0B = 0.25*OCR0A;
				 }
			 if((510<ADC) && (ADC < 610)) {
					 OCR0B = 0.3*OCR0A;
				 }
			 if((610<ADC) && (ADC < 710)) {
					 OCR0B = 0.35*OCR0A;
				 }
			 if((710<ADC) && (ADC < 810)) {
					 OCR0B = 0.4*OCR0A;
				 }
			 if((810<ADC) && (ADC < 910)) {
					 OCR0B = 0.45*OCR0A;
				 }
			 if((910<ADC) && (ADC < 1100)) {
					 OCR0B = 0.5*OCR0A;
				 }			 
			 }
		  }
		  




