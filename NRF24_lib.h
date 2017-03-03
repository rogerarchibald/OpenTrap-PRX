//
//  NRF24_lib.h
//  
//
//  Created by Roger on 5/12/15.
//
//

#ifndef ____NRF24_lib__
#define ____NRF24_lib__


#define F_CPU 8000000
#define	u8	uint8_t
#include <util/delay.h>
#include <avr/io.h>
#include "NRF24_lib.h"
#include	"NRF24defs.h"

/*
 Here will put functions for running the NRF24L01+.  Will define a few macros for setting/clearing the CSN and CE bits, as well as functions for sending a single SPI byte, a single command, writing a register (registers are single or multi-byte), or reading a register (single or multi-byte).  All functions specific to reading/writing to the NRF will start with sending a command.  This will either do its job in one shot (i.e. flush_tx) or prime the NRF for the bytes to come (i.e. say that the next X bytes read/written on SPI are register Y).  This should work provided that CSN is maintained low throughout the multi-register write.
 */

/////////////Defining Macros Specific to setting bits on NRF and on the rest of board.///////
#define clr_ce  {PORTB &= ~(0x01);}
#define set_ce  {PORTB |= 0x01;}
#define clr_csn {PORTD &=   ~(0X80);}
#define set_csn {PORTD |= 0x80;}
#define led1_on	{PORTC |= 0x01;}
#define led1_off	{PORTC &= ~(0x01);}
#define led1_tog	{PORTC ^= 0x01;}	
#define led2_on	{PORTC |= 0x02;}
#define led2_off	{PORTC &= ~(0x02);}
#define led2_tog	{PORTC ^= 0x02;}	
#define led3_on	{PORTC |= 0x04;}
#define led3_off	{PORTC &= ~(0x04);}
#define led3_tog	{PORTC ^= 0x04;}
#define led4_on	{PORTC |= 0x08;}
#define led4_off	{PORTC &= ~(0x08);}
#define led4_tog	{PORTC ^= 0x08;}	
#define led5_on	{PORTC |= 0x10;}
#define led5_off	{PORTC &= ~(0x10);}
#define led5_tog	{PORTC ^= 0x10;}		
#define set_tp1	{PORTB |= 0x04;}
#define clr_tp1 {PORTB &= ~(0x04);}
#define tog_tp1 {PORTB ^= 0x04;}
/////////////More macros to come?

//called in case PRX is defined in main.
void enable_PRX (void);

/*Lower-level functions for raw transferring of data*/
void sendbyte   (u8 sendit);   //send individual byte
void sendcmd  (u8 byteout);    //Send individual byte, this will include wrapping for clearing/setting CSN at the start/end of each byte.

/*Write FF to the NRF and capture the resulting Status Register being written out.  check_Flag and clear_Flag will read/manipulate Interrupt Flag bits in Status*/
u8 readStatus  (void);
u8	check_Flag	(u8);
void clear_Flag	(u8);

/*Reading_and_Writing registers*/
//'Register' is the address of the required register defined in other library, *destination is a pointer to the address the data's going to, and size is how many bytes I need to read here.
void readReg(u8 Register, u8* destination, u8 size);
void writeRegMult(u8 Register, u8 * source, u8 size);	//This will be used when writing multiple bytes to a register
void writeRegSing(u8 Register, u8 data);	//use this when writing a single command to a register

//sengind and receiving payloads.  In sending will need to specify if it's writing the TX payload or the Auto_AWK payload.
void sendPayLoad(u8 TXorAWK, u8* plsource, u8 size);
void receivePayLoad(u8* payload_dest, u8 size);


//FOllowing functions will perform specific tasks and read/write to specific registers.

void ten_uS_ce_Pulse(void);	//need to pulse CE hi for 10uS to enable transmission

void powerUp(void);
void powerDown(void);

void initialize_NRF(void);

void flushTx(void);
void flushRx(void);
u8	readRXWidth(void);

void get_RX_Data(u8 *destination);	//this will call the function to read the RX width, will flag an error if RXWidth is above the maximum, then will download the data, flush the RX (is this necessary?) and clear the flag.  

#endif /* defined(____NRF24_lib__) */
