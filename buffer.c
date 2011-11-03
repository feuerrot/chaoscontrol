#include "can.h"
#include "buffer.h"

uint8_t bufferin(can_t canmsg){
	uint8_t next = ((buffer.nextwrite + 1) & BUFFER_MASK);
	if (buffer.nextread == next)
		return 0;
	buffer.data[buffer.nextwrite] = canmsg;
	buffer.nextwrite = next;
	return 1;
}

uint8_t bufferout(can_t *canmsg){
	if (buffer.nextread == buffer.nextwrite)
		return 0;
	*canmsg = buffer.data[buffer.nextread];
	buffer.nextread = (buffer.nextread+1) & BUFFER_MASK;
	return 1;
}
