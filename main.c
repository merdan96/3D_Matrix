/*
 * 3D Matrix.c
 *
 * Created: 22-Nov-20 2:48:21 PM
 * Author : MohamedMerdan
 */ 
#define F_CPU 8000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

#include "USART.h"
#include "SPI.h"
#include "ports.h"
#include "matrix_display.h"

extern uint8_t matrix[8][8];
volatile uint16_t ticks = 0;
uint16_t cycle = 0;

int main(void)
{
	serialBegin(F_CPU, 9600);
	SPISetup();
	pinMode(&PORTB, 0, OUTPUT);
	pinWrite(&PORTB, 0, 0);
	//line(7,7,0,0, 0, 7);
	//serialPrintf("hey there\n");
	/*
	uint8_t l = 6;
	l = (4 - l/2);
	uint8_t w = 6;
	w = (4 - w/2);
	uint8_t h = 7;
	line(l, w, h, (7 - l), w,h);
	line((7 - l), w, h, (7 - l), (7 - w),h);
	line((7 - l), (7 - w), h, l, (7 - w), h);
	line(l, (7 - w), h, l, w, h);
	*/
	//line(l,0,h,l,w,h);
	//line(l,w,h,0,w,h);
	//line(0,w,h,0,0,h);
	
	TCCR0 = (1 << WGM01) | (1 << CS00) | (1 << CS01);
	OCR0 = 50;
	TIMSK = (1 << OCIE0);
	
	int i = 0;
	uint8_t s = 2;
	int8_t d = 1;
	//int8_t d1 = 1;
	//uint8_t h = 7;
	uint16_t tick_prev = 0;
	sei();
    while (1) 
    {
		if(cycle > 249){
			clearMatrix();
			uint8_t l = s;
			l = (4 - l/2);
			uint8_t w = s;
			w = (4 - w/2);
			for(uint8_t h = 0;h < 8;h++){
				line(l, w, h, (7 - l), w,h);
				line((7 - l), w, h, (7 - l), (7 - w),h);
				line((7 - l), (7 - w), h, l, (7 - w), h);
				line(l, (7 - w), h, l, w, h);
			}
			
			if(d == 1){
				s = (s == 8) ?  2 : (s + 2);
			}
			else{
				s = (s == 2) ? 8 : (s - 2);
			}
			if(s == 2 || s == 8){
				d = !d;
			}
			cycle = 0;
		}
		if(ticks != tick_prev){
			uint8_t row  = i / 8;
			uint8_t col  = i % 8;
			SPITransmit((1 << row));
			SPITransmit(~(1 << col));
			SPITransmit(~matrix[row][col]);
			pinWrite(&PORTB, 0, 0);
			_delay_us(1);
			pinWrite(&PORTB, 0, 1);
			
			i = (i < 63) ? (i + 1) : 0;	
			tick_prev = ticks;
		}
    }
}

ISR(TIMER0_COMP_vect) {
	ticks++;
	//cycle++;
	//cycle = (cycle == 2500) ? 0 : (cycle + 1);
}