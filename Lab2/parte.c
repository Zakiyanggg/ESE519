#define  F_CPU  16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "inc/uart.h"
int edge1= 0;
int edge2 = 0;
int edge2_2 = 0;
int edge_count = 0;
char String[25];
volatile int timer1_counter = 0;
volatile int ovf = 0;
volatile int ovf2 = 0;
char output[7];
char arr[6];
char A[6] = "._";
char B[6] = "_...";
char C[6] = "_._.";
char D[6] = "_..";
char E[6] = ".";
char F[6] = ".._.";
char G[6] = "__..";
char H[6] = "....";
char I[6] = "..";
char J[6] = ".___";
char K[6] = "_._";
char L[6] = "._..";
char M[6] = "__";
char N[6] = "_.";
char O[6] = "___";
char P[6] = ".__.";
char Q[6] = "__._";
char R[6] = "._.";
char S[6] = "...";
char T[6] = "_";
char U[6] = ".._";
char V[6] = "..._";
char W[6] = ".__";
char X[6] = "_.._";
char Y[6] = "_.__";
char Z[6] = "__..";


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
				//sprintf(String,".");
				//UART_putstring(String);
				PORTD |= (1<<PORTD3);//PD3 HIGH to lit led for 50ms
				_delay_ms(50);
				PORTD &= ~(1<<PORTD3);
				strcat(arr,".");
				edge1 = 0;
				edge2 = 0;
				ovf = 0;
			}
			else if(1201<=ovf && ovf<=2400){
				//sprintf(String,"_");
				//UART_putstring(String);
				PORTB |= (1<<PORTB4);//PB4 HIGH to lit led for 50ms
				_delay_ms(50);
				PORTB &= ~(1<<PORTB4);
				strcat(arr,"_");
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
				sprintf(String,"morse code:");
				UART_putstring(String);
				UART_putstring(arr);
				
				if (strcmp(arr,A)==0)
				{
					sprintf(output,"a\n");
				}
				else if(strcmp(arr,B)==0)
				{
					sprintf(output,"b\n");
				}
				else if(strcmp(arr,C)==0)
				{
					sprintf(output,"c\n");
				}
				else if(strcmp(arr,D)==0)
				{
					sprintf(output,"d\n");
				}
				else if(strcmp(arr,E)==0)
				{
					sprintf(output,"e\n");
				}
				else if(strcmp(arr,F)==0)
				{
					sprintf(output,"f\n");
				}
				else if(strcmp(arr,G)==0)
				{
					sprintf(output,"g\n");
				}
				else if(strcmp(arr,H)==0)
				{
					sprintf(output,"h\n");
				}
				else if(strcmp(arr,I)==0)
				{
					sprintf(output,"i\n");
				}
				else if(strcmp(arr,J)==0)
				{
					sprintf(output,"j\n");
				}
				else if(strcmp(arr,K)==0)
				{
					sprintf(output,"k\n");
				}
				else if(strcmp(arr,L)==0)
				{
					sprintf(output,"l\n");
				}
				else if(strcmp(arr,M)==0)
				{
					sprintf(output,"m\n");
				}
				else if(strcmp(arr,N)==0)
				{
					sprintf(output,"n\n");
				}
				else if(strcmp(arr,O)==0)
				{
					sprintf(output,"o\n");
				}
				else if(strcmp(arr,P)==0)
				{
					sprintf(output,"p\n");
				}
				else if(strcmp(arr,Q)==0)
				{
					sprintf(output,"q\n");
				}
				else if(strcmp(arr,R)==0)
				{
					sprintf(output,"r\n");
				}
				else if(strcmp(arr,S)==0)
				{
					sprintf(output,"s\n");
				}
				else if(strcmp(arr,T)==0)
				{
					sprintf(output,"t\n");
				}
				else if(strcmp(arr,U)==0)
				{
					sprintf(output,"u\n");
				}
				else if(strcmp(arr,V)==0)
				{
					sprintf(output,"v\n");
				}
				else if(strcmp(arr,W)==0)
				{
					sprintf(output,"w\n");
				}
				else if(strcmp(arr,X)==0)
				{
					sprintf(output,"x\n");
				}
				else if(strcmp(arr,Y)==0)
				{
					sprintf(output,"y\n");
				}
				else if(strcmp(arr,Z)==0)
				{
					sprintf(output,"z\n");
				}
				else {
					sprintf(output,"Error\n");
				}
				sprintf(String,"decoded:");
				UART_putstring(String);
				UART_putstring(output);
				output[0] = '\0';
				arr[0] = '\0';
				edge2_2 = 0;
			}
			else{
				_delay_ms(10);
			}
		}
		
	}
  }
