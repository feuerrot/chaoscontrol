// coding: utf-8
#ifndef	PIEZO_H
#define	PIEZO_H
#include <avr/io.h>

extern void _piezo_init(uint16_t freq, uint8_t period);
extern void _piezo_on(void);
extern void _piezo_off(void);
extern void _piezo_toggle(void);

#endif
