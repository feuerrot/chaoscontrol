// coding: utf-8

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "can.h"
#include "config.h"
#include "chaoscontrol.h"

volatile uint8_t dauer;
volatile struct {
	unsigned iCAN:1;	//Flag für neue CAN-Nachricht
//	unsigned iFOO:42;	//Flag für frische Pizza
} flags;

uint8_t incrementator;

cc_id_t canid;

uint8_t _sec(uint8_t data0, cc_id_t id){
	can_t msg;
	msg.id = cc_id_to_int(id);
	msg.flags.rtr = 0;
	msg.flags.extended = 1;
	msg.length = 1;
	msg.data[0] = data0;
	return can_send_message(&msg);
}

prog_uint8_t can_filter[] = {
	// Group 0
	MCP2515_FILTER_EXTENDED(0b00000000000000000000000000000),		// Filter 0
	MCP2515_FILTER_EXTENDED(0b00000000000000000000000000000),		// Filter 1

	// Group 1
	MCP2515_FILTER_EXTENDED(0b11111111111111111111111111111),		// Filter 2
	MCP2515_FILTER_EXTENDED(0b11111111111111111111111111111),		// Filter 3
	MCP2515_FILTER_EXTENDED(0b11111111111111111111111111111),		// Filter 4
	MCP2515_FILTER_EXTENDED(0b11111111111111111111111111111),		// Filter 5

	MCP2515_FILTER_EXTENDED(0b00000000000000000000000000000),		// Mask 0 (for group 0)
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

	//JTAG ausschalten, indem man zwei Mal hintereinander das Bit setzt
	MCUCSR |= (1<<JTD);
	MCUCSR |= (1<<JTD);

	//MCP2515 aktivieren, funktioniert ohne Delays nicht
	can_init(BITRATE_1_MBPS);
	CAN_INIT_DELAY;
	can_static_filter(can_filter);
	CAN_INIT_DELAY;
	can_set_mode(LOOPBACK_MODE);
	CAN_INIT_DELAY;

	canid.idFrom = 0x7ff;
	canid.idTo = 0x7ff;
	canid.idFlagService = 0x3f;

	//Interrupts aktivieren, jetzt kein _delay_* mehr!
	_timer2_init();
	_timer1_on(0x007f);

	GICR |= (1<<INT1);
	sei();
}

int main(void) {
	init();
	_sec(incrementator++, canid);
	while(1) {
		PORTA ^= (1<<PA0);
		if (flags.iCAN){
			flags.iCAN = 0;
			can_t canmsg;
			if(can_get_message(&canmsg) && (canmsg.length > 0)){
				PORTA ^= (1<<PA1);
				PORTB = (PORTB & 0xf0) | (canmsg.data[(canmsg.length)-1] & 0x0f); //Obere 4 Bits: Inhalt von PORTB, untere 4 Bits: untere 4 Bits des letzten Bytes der Cannachricht
				_sec(incrementator++, canid);
			}
		}
	}
	return 0;
}

ISR(INT1_vect){
	flags.iCAN = 1;
}

ISR(TIMER2_COMP_vect){
	if (dauer > 0){
		dauer--;
	} else {
		dauer = 10;
		_timer1_toggle();
	}
}

