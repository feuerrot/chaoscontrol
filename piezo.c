// coding: utf-8
#include <avr/io.h>
#include <avr/interrupt.h>
#include "piezo.h"

volatile uint8_t dauer, dauercnt;

ISR(TIMER2_COMP_vect){
	if (dauercnt > 0){
		dauercnt--;
	} else {
		dauercnt = dauer;
		_piezo_toggle();
	}
}

void _timer2_init(void){
	OCR2 = 155;
	TCCR2 |= (1<<WGM21)|(1<<CS21)|(1<<CS20);
	TIMSK |= (1<<OCIE2);
}

void _piezo_init(uint16_t freq, uint8_t period){
	dauer = period;
	OCR1A = freq;
	TCCR1B |= (1<<WGM12)|(1<<CS11);
	_timer2_init();
}

void _piezo_on(void){
	DDRD   |= (1<<PD5);
	TCCR1A |= (1<<COM1A0);
	
}

void _piezo_off(void){
	DDRD   &= ~(1<<PD5);
	TCCR1A &= ~(1<<COM1A0);
}

void _piezo_toggle(void){
	DDRD   ^= (1<<PD5);
	TCCR1A ^= (1<<COM1A0);
}
