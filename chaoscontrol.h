#ifndef CHAOSCONTROL_H
#define CHAOSCONTROL_H

#include <stdint.h>

//Eigene ID
uint16_t cc_id = 0x7ff;	//Maximal 2048 Nodes, sollte erstmal™ reichen…

typedef enum {
	SYSTEM = 0,		//Zugriff auf das System selber
	PROGRAM = 1,	//Daten an das laufende Programm
} cc_service_t;

typedef struct {
	unsigned idFrom:11;				//11 Bit Senderadresse
	unsigned idTo:11;				//11 Bit Empfängeradresse
	unsigned idFlagPrio:1;			// 1 Bit zur Priorisierung
	unsigned idFlagService:6;		// 6 Bit zur Auswahl des Service beim Empfänger
} cc_id_t;						//29 Bit für die komplette id

extern uint32_t cc_id_to_int(cc_id_t canid);
extern cc_id_t int_to_cc_id(uint32_t canid);

#endif
