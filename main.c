// coding: utf-8

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "chaoscontrol.h"
#include "can.h"
#include "config.h"

volatile struct {
	unsigned iCAN:1;	//Flag für neue CAN-Nachricht
} flags;
uint8_t incr;
cc_id_t canid;

char chars[]={
0b10100000,
0b10000110,
0b11101000,
0b10100100,
0b11101101,
0b11001101,
0b11101110,
0b10000101,
0b11000010,
0b11111111
};

char text[]={9,4,9,2,0,4,5,9,8,9,3,0,6,1};


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

void init(void) {
	//PA0-1, PB0-3 als Ausgänge
	DDRA |= (1<<PA0)|(1<<PA1);
	DDRB |= (1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3);
	DDRD &= ~(1<<PD3); //PD3 ist der Eingang für INT1

	//JTAG ausschalten, indem man zwei Mal hintereinander das Bit setzt
	MCUCSR |= (1<<JTD);
	MCUCSR |= (1<<JTD);

	//MCP2515 aktivieren, funktioniert ohne Delays nicht
	can_init(BITRATE_125_KBPS);
	CAN_INIT_DELAY;
	can_static_filter(can_filter);
	CAN_INIT_DELAY;
	can_set_mode(NORMAL_MODE);
	CAN_INIT_DELAY;


	//Interrupts aktivieren, jetzt kein _delay_* mehr!
	GICR |= (1<<INT1);
	sei();
}

int main(void) {
	init();
	cc_id_set(&canid, 0x7ff, 0x7ff, 0x00, 0x3f);
	while(1) {
		int i;
		for (i=0; i<14; i++){
			_sec(chars[text[i]], canid);
			_delay_ms(300);
		}
	}
	return 0;
}

ISR(INT1_vect){
	flags.iCAN = 1;
}
