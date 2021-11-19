/*
 * ESE519_Lab4_Pong_Starter.c
 *
 * Created: 9/21/2021 21:21:21 AM
 * Author : J. Ye
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include "lib\ST7735.h"
#include "lib\LCD_GFX.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)
#include "inc\uart.h"
#include <avr/interrupt.h>


char String[25];
char Yourscore[10];
char Botscore[10];
int yourscore;
int botscore;
int pause = 10000;
int mode;

int PlayerPaddlePos = 59;
int BotPaddlePos = 59;
int Ball_x = 77;
int Ball_y = 59;
int dy = 3;
int dx = -1;
int botchange = 2;

void Initialize()
{
	lcd_init();
	cli();
	DDRD |= (1 << DDD3);//PD3 as output
	DDRD |= (1 << DDD2);//PD2 as output
	TCCR2B |= (1<<CS21);//start timer2 with prescaler = 256
	TCCR1B |= (1<<CS12);//start timer1 with prescaler = 256
	DDRD &= ~(1<< DDD5);//PD5 as input
	DDRD &= ~(1<< DDD7);//PD7 as input
	//TCCR1B |= (1<<CS10);
	//TIMSK2 |= (1<<TOIE2);//Enable timer2 ovf
	//DDRD &= ~(1<<DDD4);
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
	
}
void DrawPlayerPaddle(int pos)
{
	LCD_drawBlock(0,pos-12,3,pos+12,WHITE);//clear player paddle
	LCD_drawBlock(0,pos-10,3,pos+10,RED);
	
}
void DrawBotPaddle(int pos)
{
	LCD_drawBlock(156,pos-12,159,pos+12,WHITE);//clear bot paddle
	LCD_drawBlock(156,pos-10,159,pos+10,BLUE);
}
void BotPaddleMove(){
	BotPaddlePos = BotPaddlePos +botchange;
	DrawBotPaddle(BotPaddlePos);
	if(BotPaddlePos+10>=114){
		botchange = -2;
	}
	if(BotPaddlePos-10<= 1){
		botchange = 2;
	}
}
void DrawBall(int x,int y){
	LCD_drawCircle(x,y,5,BLACK);
}

void BallMove(int dx,int dy){
	//LCD_drawCircle(Ball_x,Ball_y,5,WHITE);//clear the previous ball
	LCD_drawBlock(Ball_x-5,Ball_y-5,Ball_x+5,Ball_y+5,WHITE);
	Ball_y = Ball_y+dy;
	Ball_x = Ball_x+dx;
	DrawBall(Ball_x,Ball_y);//draw the new ball
	}

void PlayerScore(){
	for(int i = 0;i<2;i++){
	TIMSK2 |= (1<<OCIE2A);//enable timer2 compa
	//OCR2A = 50;
	PORTD |= (1<<PORTD3);
	_delay_ms(30);
	PORTD &= ~(1<<PORTD3);
	TIMSK2 &= ~(1<<OCIE2A);//disable timer2 compa
	_delay_ms(30);
	}
}

void BotScore(){
	for(int i = 0;i<2;i++){
		PORTD |= (1<<PORTD2);
		TIMSK2 |= (1<<OCIE2A);//enable timer2 compa
		//OCR2A = 50;
		_delay_ms(30);
		PORTD &= ~(1<<PORTD2);
		TIMSK2 &= ~(1<<OCIE2A);//disable timer2 compa
		_delay_ms(30);
	}
}
void BallBounce(){
	srand(TCNT0);
	int sw;
	sw = rand()%4;
	sprintf(String,"%d\n",sw);
	UART_putstring(String);
	if(Ball_x >= 149){

			dx = -2;

		if((Ball_y<=BotPaddlePos+10)&&(Ball_y>=BotPaddlePos-10)){
			TIMSK2 |= (1<<TOIE2);//enable timer1 ovf
			_delay_ms(20);
			TIMSK2 &= ~(1<<TOIE2);//disable timer1 ovf
		}

	}
	if(Ball_x <= 9){

			dx = 2;
		
		if((Ball_y<=PlayerPaddlePos+10)&&(Ball_y>=PlayerPaddlePos-10)){
			TIMSK2 |= (1<<TOIE2);//enable timer1 ovf
			_delay_ms(20);
			TIMSK2 &= ~(1<<TOIE2);//disable timer1 ovf
		}
	}
	if(Ball_y >= 106){
		dy = -2;
		TIMSK0 |= (1<<TOIE0);//Enable timer0 ovf
		_delay_ms(20);
		TIMSK0 &= ~(1<<TOIE0);//disable timer0 ovf
	}
	if(Ball_y <= 5){

		dy = 2;

		TIMSK0 |= (1<<TOIE0);//Enable timer0 ovf
		_delay_ms(20);
		TIMSK0 &= ~(1<<TOIE0);//disable timer0 ovf
	}
}
void resetgame(){
	LCD_drawBlock(Ball_x-5,Ball_y-5,Ball_x+5,Ball_y+5,WHITE);//clear ball
	Ball_x = 77;
	Ball_y = 59;
	DrawBall(Ball_x,Ball_y);
}

void PlayerPaddleMove(){
	if(ADC > 1000){
		PlayerPaddlePos = PlayerPaddlePos-2;
		//_delay_ms(10);
		if(PlayerPaddlePos >= 108){
			PlayerPaddlePos = 108;
		}
		if(PlayerPaddlePos <= 10){
			PlayerPaddlePos = 10;
		}
		DrawPlayerPaddle(PlayerPaddlePos);
		
	}
	if(ADC < 100){
		PlayerPaddlePos = PlayerPaddlePos+2;
		//_delay_ms(10);
		if(PlayerPaddlePos >= 106){
			PlayerPaddlePos = 106;
		}
		if(PlayerPaddlePos <= 10){
			PlayerPaddlePos = 10;
		}
		DrawPlayerPaddle(PlayerPaddlePos);
		
	}
}

void ModeSel(){
	
}
void setup_gameboard()
{
	LCD_setScreen(WHITE);
	LCD_drawBlock(0,0,3,117,WHITE);//clear player paddle
	LCD_drawBlock(156,0,159,117,WHITE);//clear bot paddle
	LCD_drawBlock(0,119,159,127,WHITE);
	LCD_drawLine(0,118,159,118,BLACK);
	sprintf(Yourscore,"You:%d",yourscore);
	sprintf(Botscore, "Bot:%d",botscore);
	LCD_drawString(0,119,Yourscore,BLACK,WHITE);
	LCD_drawString(120,119,Botscore,BLACK,WHITE);
	DrawPlayerPaddle(PlayerPaddlePos);
	DrawBotPaddle(BotPaddlePos);
	DrawBall(Ball_x, Ball_y);
}

void ScoreCheck(){
	int Botlose1 = BotPaddlePos+10;
	int Botlose2 = BotPaddlePos-10;
	if(Ball_x>=148 && (Ball_y>Botlose1|| Ball_y<Botlose2)){//bot loses
		yourscore = yourscore+1;
		sprintf(Yourscore,"You:%d",yourscore);
		LCD_drawBlock(0,119,40,127, WHITE);
		LCD_drawString(0,119,Yourscore,BLACK,WHITE);
		PlayerScore();
		resetgame();
	}
	int Playerlose1 = PlayerPaddlePos+10;
	int Playerlose2 = PlayerPaddlePos-10;
	if(Ball_x<=10 && (Ball_y>Playerlose1|| Ball_y<Playerlose2)){//you loses
		botscore = botscore+1;
		sprintf(Botscore,"BOT:%d",botscore);
		LCD_drawBlock(100,119,159,127, WHITE);
		LCD_drawString(120,119,Botscore,BLACK,WHITE);
		BotScore();
		resetgame();
	}
	if(yourscore==3){
		LCD_drawBlock(Ball_x-5,Ball_y-5,Ball_x+5,Ball_y+5,WHITE);//clear ball
		LCD_drawString(70,50,"Game Over",BLACK,WHITE);
		LCD_drawString(70,60,"You Win!!",BLACK,WHITE);
		_delay_ms(1000);
		yourscore = 0;
		botscore = 0;
		setup_gameboard();
	}
	if(botscore==3){
		LCD_drawBlock(Ball_x-5,Ball_y-5,Ball_x+5,Ball_y+5,WHITE);//clear ball
		LCD_drawString(70,50,"Game Over",BLACK,WHITE);
		LCD_drawString(70,60,"Bot Win!!",BLACK,WHITE);
		_delay_ms(1000);
		yourscore = 0;
		botscore = 0;
		setup_gameboard();
	}
}
ISR(TIMER2_OVF_vect){
	PORTD ^= (1<<DDD4);
}
ISR(TIMER0_OVF_vect){
	PORTD ^= (1<<DDD4);
}
ISR(TIMER2_COMPA_vect){
	PORTD ^= (1<<DDD4);
}
ISR(TIMER1_OVF_vect){
	PORTD ^= (1<<DDD4);
}
int main(void)
{
	Initialize();
	setup_gameboard();	
	OCR2A = 10;
    while (1) 
    {
		
		
		PlayerPaddleMove();
		BotPaddleMove();
		//sprintf(String,"dx %d\n",dx);
		//UART_putstring(String);
		//sprintf(String,"dy %d\n",dy);
		//UART_putstring(String);
		BallMove(dx,dy);
		BallBounce();
		ScoreCheck();
		if(PIND & (1<<PIND5)){
			sprintf(String,"PD5 HIGH");
			UART_putstring(String);
			PlayerPaddlePos = PlayerPaddlePos-2;
			//_delay_ms(10);
			if(PlayerPaddlePos >= 108){
				PlayerPaddlePos = 108;
			}
			if(PlayerPaddlePos <= 10){
				PlayerPaddlePos = 10;
			}
			DrawPlayerPaddle(PlayerPaddlePos);
		}
		if(PIND & (1<<PIND7)){
			sprintf(String,"PD7 HIGH");
			UART_putstring(String);
			PlayerPaddlePos = PlayerPaddlePos+2;
			//_delay_ms(10);
			if(PlayerPaddlePos >= 106){
				PlayerPaddlePos = 106;
			}
			if(PlayerPaddlePos <= 10){
				PlayerPaddlePos = 10;
			}
			DrawPlayerPaddle(PlayerPaddlePos);
		}
		
	}
}