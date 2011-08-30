// coding: utf-8
#ifndef	CONFIG_H
#define	CONFIG_H

//Eigene #defines
#define CAN_INIT_DELAY			_delay_ms(10)

//Config von libcan
#define	SUPPORT_EXTENDED_CANID	1
#define	SUPPORT_TIMESTAMPS		0
#define	SUPPORT_MCP2515			1
#define	MCP2515_CS				B,4
#define	MCP2515_INT				D,3

#endif	// CONFIG_H
