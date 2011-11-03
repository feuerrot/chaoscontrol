// coding: utf-8

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "chaoscontrol.h"
#include "can.h"
#include "config.h"
#include "74hc595.h"
#include "buffer.h"

volatile struct {
	unsigned iCAN:1;			//Flag für neue CAN-Nachricht
	unsigned iErrorBufferIn:1;	//Flag für vollen Buffer
	unsigned iErrorBufferOut:1;	//Flag für leeren Buffer
//	unsigned iFOO:42;			//Flag für frische Pizza
} flags;
uint8_t incrementator;
cc_id_t canid;

uint8_t _createcanmessage(uint8_t data0, cc_id_t id){
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
	//PB235, PC01 als Ausgänge
	DDRB |= (1<<PB2)|(1<<PB3)|(1<<PB5);
	DDRD &= ~(1<<PD2); //PD2 ist der Eingang für INT1

	//Schieberegister initialisieren
	_74hc595_init();
	_74hc595_output(1);

	//MCP2515 aktivieren, funktioniert ohne Delays nicht
	can_init(BITRATE_125_KBPS);
	CAN_INIT_DELAY;
	can_static_filter(can_filter);
	CAN_INIT_DELAY;
	can_set_mode(NORMAL_MODE);
	CAN_INIT_DELAY;

	//Interrupts aktivieren, jetzt kein _delay_* mehr!
	GICR |= (1<<INT0);
	sei();
}

int main(void) {
    init();
    while(1) {
        if (flags.iCAN){
            flags.iCAN = 0;
            can_t canmsg;
            flags.iErrorBufferOut = !bufferout(&canmsg);
            if(!flags.iErrorBufferOut)
                _74hc595_send(canmsg.data[canmsg.length-1]);
        }
    }
    return 0;
}

ISR(INT0_vect){
    can_t canmsg;
    can_get_message(&canmsg);
    flags.iErrorBufferIn = !bufferin(canmsg);
    flags.iCAN = 1;
}

