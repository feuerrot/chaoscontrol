#ifndef BUFFER_H
#define BUFFER_H

#define BUFFER_SIZE 32 // muss 2^n betragen (8, 16, 32, 64 ...)
#define BUFFER_MASK (BUFFER_SIZE-1) // Klammern auf keinen Fall vergessen
#include "can.h"

struct {
	can_t   data[BUFFER_SIZE];
	uint8_t nextread;
	uint8_t nextwrite;
} buffer; // = {{}, 0, 0};

extern uint8_t bufferin(can_t canmsg);
extern uint8_t bufferout(can_t *canmsg);

#endif
