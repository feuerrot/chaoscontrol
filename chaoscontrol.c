// coding: utf-8
#include "chaoscontrol.h"
#include <stdint.h>
#include <stdbool.h>

uint32_t cc_id_to_int(cc_id_t canid){
	uint32_t ret;
	ret |= (canid.idFrom<<17);
	ret |= (canid.idTo<<6);
	ret |= (canid.idFlagPrio<<29);
	ret |=  canid.idFlagService;
	return ret;
}

cc_id_t int_to_cc_id(uint32_t canid){
	cc_id_t ret;
	ret.idFrom = ((canid>>17) & 0x7ff);
	ret.idTo   = ((canid>>6) & 0x7ff);
	ret.idFlagPrio = ((canid>>29) & 0x01);
	ret.idFlagService = (canid & 0x06);
	return ret;
}




