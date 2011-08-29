// coding: utf-8

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "can.h"
#include "config.h"

volatile uint8_t _flags, dauer;
uint8_t incrementator;

ISR(INT1_vect){
	_flags |= 0x01;
}

ISR(TIMER2_COMP_vect){
	if (dauer > 0){
		dauer--;
	} else {
		dauer = 10;
		_timer1_toggle();
	}
}

uint8_t _sec(uint8_t data0){
	can_t msg;
	msg.id = CHAOSCONTROLID_EP;
	msg.flags.rtr = 0;
	msg.flags.extended = 1;
	msg.length = 1;
	msg.data[0] = data0;
	return can_send_message(&msg);
}

prog_uint8_t can_filter[] = {
	// Group 0
	MCP2515_FILTER_EXTENDED(CHAOSCONTROLID_EP),		// Filter 0
	MCP2515_FILTER_EXTENDED(0b00000000000000000000000000000),		// Filter 1

	// Group 1
	MCP2515_FILTER_EXTENDED(0b11111111111111111111111111111),		// Filter 2
	MCP2515_FILTER_EXTENDED(0b11111111111111111111111111111),		// Filter 3
	MCP2515_FILTER_EXTENDED(0b11111111111111111111111111111),		// Filter 4
	MCP2515_FILTER_EXTENDED(0b11111111111111111111111111111),		// Filter 5

	MCP2515_FILTER_EXTENDED(0b00000000000111111111110000000),		// Mask 0 (for group 0)
	MCP2515_FILTER_EXTENDED(0b11111111111111111111111111111),		// Mask 1 (for group 1)

};

void _timer1_on(uint16_t ocr1a){
	OCR1A = ocr1a;
	TCCR1A |= (1<<COM1A0);
  	TCCR1B |= (1<<WGM12)|(1<<CS11);
	DDRD   |= (1<<PD5);
}

void _timer1_off(void){
	TCCR1A &= ~(1<<COM1A0);
	TCCR1B &= ~((1<<WGM12)|(1<<CS11));
	DDRD   &= ~(1<<PD5);
}

void _timer1_toggle(void){
	DDRD ^= (1<<PD5);
}

void _timer2_init(void){
	OCR2 = 155;
	TCCR2 |= (1<<WGM21)|(1<<CS21)|(1<<CS20);
	TIMSK |= (1<<OCIE2);
}

void init(void) {
	//PA0-1, PB0-3 als Ausgänge
	DDRA |= (1<<PA0)|(1<<PA1);
	DDRB |= (1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3);
	DDRD &= ~(1<<PD3); //PD3 ist der Eingang für INT1

	//JTAG ausschalten, indem man zwe Mal hintereinander das Bit setzt
	MCUCSR |= (1<<JTD);
	MCUCSR |= (1<<JTD);

	//MCP2515 aktivieren, funktioniert ohne Delays nicht
	can_init(BITRATE_1_MBPS);
	CAN_INIT_DELAY;
	can_static_filter(can_filter);
	CAN_INIT_DELAY;
	can_set_mode(LOOPBACK_MODE);
	CAN_INIT_DELAY;

	//Interrupts aktivieren, jetzt kein _delay_* mehr!
	_timer2_init();
	_timer1_on(0x007f);

	GICR |= (1<<INT1);
	sei();
}

int main(void) {
	init();
	_sec(incrementator++);
	while(1) {
		PORTA ^= (1<<PA0);
		if (_flags & 0x01){
			_flags &= ~0x01;
			can_t canmsg;
			if(can_get_message(&canmsg) && (canmsg.length > 0)){
				PORTA ^= (1<<PA1);
				PORTB = (PORTB & 0xf0) | (canmsg.data[(canmsg.length)-1] & 0x0f); //Obere 4 Bits: Inhalt von PORTB, untere 4 Bits: untere 4 Bits des letzten Bytes der Cannachricht
				_sec(incrementator++);
			}
		}
	}
	return 0;
}
