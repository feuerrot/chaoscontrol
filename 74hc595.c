// coding: utf-8
#include <avr/io.h>
#include <avr/interrupt.h>
#include "74hc595.h"

volatile uint8_t dauer, dauercnt;

void _74hc595_init(void){
	SPCR |= (1<<SPE)|(1<<MSTR);
	STCP_PORT &= ~(1<<STCP_PIN)
	STCP_DDR |= (1<<STCP_PIN);
	OE_PORT |= (1<<OE_PIN);
	OE_DDR |= (1<<OE_PIN);
}

void _74hc595_send(uint8_t data){
	while (!(SPSR & (1<<SPIF)));
	SPDR = data;
	while (!(SPSR & (1<<SPIF)));
	STCP_PORT ^= (1<<STCP_PIN);
	_delay_us(1);
	STCP_PORT ^= (1<<STCP_PIN);
}

void _74hc595_output(uint8_t state){
	if (state){
		OE_PORT &= ~(1<<OE_PIN);
	} else {
		OE_PORT |= (1<<OE_PIN);
	}
}
