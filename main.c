// coding: utf-8

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "can.h"

prog_uint8_t can_filter[] = {
	// Group 0
	MCP2515_FILTER(0),			// Filter 0
	MCP2515_FILTER(0),			// Filter 1

	// Group 1
	MCP2515_FILTER_EXTENDED(0),		// Filter 2
	MCP2515_FILTER_EXTENDED(0),		// Filter 3
	MCP2515_FILTER_EXTENDED(0),		// Filter 4
	MCP2515_FILTER_EXTENDED(0),		// Filter 5

	MCP2515_FILTER(0),				// Mask 0 (for group 0)
	MCP2515_FILTER_EXTENDED(0),		// Mask 1 (for group 1)
};

void init(void) {
	can_init(BITRATE_125_KBPS);
	can_set_mode(LOOPBACK_MODE);
	can_static_filter(can_filter);
	DDRD = (1<<DDD0);
}

int main(void) {
	init();

	can_t msg;

	msg.id = 0b000000000000;
	msg.flags.rtr = 0;
	msg.flags.extended = 1;
	msg.length = 4;
	msg.data[0] = 0xde;
	msg.data[1] = 0xad;
	msg.data[2] = 0xbe;
	msg.data[3] = 0xef;

	can_send_message(&msg);

	while(1) {

		if (can_check_message()) {
			if (can_get_message(&msg)) {
				if (msg.id==0x00){
					PORTD = (1<<PD0);
					msg.id += 0x01;
				} else if (msg.id==0x01) {
					PORTD = (0<<PD0);
					msg.id -= 0x01;
				}
				_delay_ms(100);
				can_send_message(&msg);
			}
		}
	}

	return 0;
}
