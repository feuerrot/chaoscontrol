// coding: utf-8
#ifndef	_74HC595_H
#define	_74HC595_H
#include <avr/io.h>

#define STCP_PORT	PORTC
#define STCP_DDR	DDRC
#define STCP_PIN	PC0
#define OE_PORT		PORTC
#define OE_DDR		DDRC
#define OE_PIN		PC1

extern void _74hc595_init(void);
extern void _74hc595_send(uint8_t data);
extern void _74hc595_output(uint8_t state);

#endif
