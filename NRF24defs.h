/*
 * NRF24L0plus_defs.h
 *
 * Created: 5/17/15 6:39:42 PM
 *  Author: Roger
 */ 


#ifndef NRF24DEFS_H_
#define NRF24DEFS_H_

//Register Addresses:

#define CONFIG		0x00
#define EN_AA		0x01
#define EN_RXADDR	0x02
#define SETUP_AW	0x03
#define SETUP_RETR	0x04
#define RF_CH		0x05
#define	RF_SETUP	0x06
#define STATUS		0x07
#define	OBSERVE_TX	0x08
#define	RPD			0x09
#define	RX_ADDR_P0	0x0A
#define	RX_ADDR_P1	0x0B
#define	RX_ADDR_P2	0x0C
#define	RX_ADDR_P3	0x0D
#define	RX_ADDR_P4	0x0E
#define	RX_ADDR_P5	0x0F
#define	TX_ADDR		0x10
#define	RX_PW_P0	0x11
#define	RX_PW_P1	0x12
#define	RX_PW_P2	0x13
#define	RX_PW_P3	0x14
#define	RX_PW_P4	0x15
#define	RX_PW_P5	0x16
#define	FIFO_STATUS	0x17
#define	DYNPD		0x1C
#define	FEATURE		0x1D


//Commands...Leaving out a few commands (R_REG, W_REG) that are better handled without this i.e. R_REG is 0 |= with register addy.  
#define		R_RX_PAYLOAD		0x61
#define		W_TX_PAYLOAD		0xA0
#define		FLUSH_TX			0xE1
#define		FLUSH_RX			0xE2
#define		REUSE_TX_PL			0xE3
#define		R_RX_PL_WID			0x60
#define		W_ACK_PAYLOAD		0xA8		//the least significant nibble here is user-defined but for my use it will probably always be 8, since the 3 lsb's are 000 for Pipe 0
#define		W_TX_PAYLOAD_NOACK	0xB0	//will probably not use this
#define		NOP					0xFF

//Will use the following hex values to check/clear Interrupt flags in the STATUS Register.  Will use along with the 'check_Flag' and 'clear_Flag' functions
#define		RX_DR		0x40
#define		TX_DS		0x20
#define		MAX_RT		0x10
#define		ALL_FLAG	0x70







#endif /* NRF24L0PLUS_DEFS_H_ */
