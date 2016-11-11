//
//  NRF24_lib.c
//  
//
//  Created by Roger on 5/12/15.
//
//

#include "NRF24_lib.h"
#define	u8	uint8_t

static u8 CFGValues = 0x7C;	//7C where I'm going to start this.  Two things that will be tweaked are: Bit0 which determines if we're PTX or PRX, this is determined in 'main.c'.  Also bit 1 is the powerup, have functions called PowerUp and PowerDn to control this.
u8 Addy [] = {0x52, 0x6F, 0x67, 0x65, 0x72}; //Believe that as long as we're looking at only one TX/RX combo than only need one address

//this will only be called just prior to initializing the NRF in case 'PRF' has been defined in Main.
void enable_PRX (void){
CFGValues |= 0x01;
}


//This is the lowest level function which requires something above it to enable the CSN line.  It just writes a byte to the SPI data register and waits until it's been clocked through
void sendbyte (u8 sendit){
    SPDR = sendit;                  //write 'sendit' to the data register.
    while(!(SPSR & (1<<SPIF) ));    //wait for transmission to complete.
}

//Sending a single byte and not requiring a response.  sendbyte is a sub-function
void sendcmd (u8 byteout){
    clr_csn;
    sendbyte(byteout);
    set_csn;
}

u8 readRXWidth(void){
	clr_csn;
	sendbyte(R_RX_PL_WID);	//command to read the payload width
	sendbyte(0x55);			//dummy byte while clocking out pl width
	set_csn;
	return	SPDR;		//return RX Width
}


void flushTx(void){
	sendcmd(FLUSH_TX);
}

void flushRx(void){
	sendcmd(FLUSH_RX);
}

//Read the STATUS register which is clocked out as the first byte of any SPI transfer is clocked in.  This is accomplished here by sending the 'NOP' (0xFF) command to the NRF
u8 readStatus (void){
    clr_csn;
    sendbyte(0xff); //probably want to define this as 'nop' in NRF-specific Library file
    set_csn;
    return SPDR;
}


/*Function below here will Read registers where 'Register' is the 5-bit address of the register which is defined in another library.  'destination' is a pointer to where these will go and size is the number of bytes that need to be read from that register. */

void readReg(u8 Register, u8 * destination, u8 size){
clr_csn;
sendbyte (Register); //write the adress of the register we're writing to.
    if (1==size) {
        sendbyte(0x55);		//55 is a dummy byte just to clock something out
       *destination = SPDR;
        goto regreadbail;            //if it's only a single-byte read then read the reg and bail.
    }
    for (u8 i = 0; i < size; i++){
        sendbyte(0x55);
    destination[i] = SPDR;
}
regreadbail:    //if it's a single-byte read then come here after reading it.
set_csn;
}



/*    Here Register is the 5-bit address of the register which is defined in other library, but in this case it will need to be OR’d with 0x20 to make it the write command.*/

void writeRegMult(u8 Register, u8 * source, u8 size){
clr_csn;
sendbyte((Register | 0x20));
    for (u8 i = 0; i < size; i++){
        sendbyte(source[i]);
    }
    set_csn;
}


void writeRegSing(u8 Register, u8 data){
	clr_csn;
	sendbyte((Register | 0x20));	//'OR register address with 0x20 to indicate it's a write command
	sendbyte(data);
	set_csn;
}




//Check and Clear specific flags in the Status Register for RX_DR, TX_DS, and MAX_RT

u8	check_Flag (u8 flag){
u8	temp = readStatus();
if (temp & flag){
	return 1;
}else{
	return 0;
	}
}

void clear_Flag	(u8 flag){
u8 temp = readStatus();
temp |= flag;
writeRegSing(STATUS,flag);	
}






/* following functions are for sending/retrieving payloads from the NRF.  */


void sendPayLoad(u8 TXorAWK, u8 * plsource, u8 size){
flushTx();		//clear the buffer first
clr_csn;
sendbyte(TXorAWK);		//this will be either the command to W_TX_PAYLOAD (0xA0) or the command to W_ACK_PAYLOAD(0xAX) where nibble X = 1PPP where P = pipe address.
for (int i = 0; i < size; i++){
	sendbyte(plsource[i]);
}
set_csn;
if(TXorAWK == W_TX_PAYLOAD){	//if I'm here because of writing a PTX payload and not an awk-pak payload, pulse CE
	ten_uS_ce_Pulse();	//10uS pulse on Chip Enable to make it transmit.  Will probably work this into Timers later on.
}
}


void receivePayLoad(u8 * payload_dest, u8 size){
	clr_csn;
	sendbyte(R_RX_PAYLOAD);	//command to receive paylaod
	for(int i =0; i < size; i ++){
		sendbyte(0x55);	//dummy byte just to send something
		payload_dest[i] = SPDR;
	}
	set_csn;
}



void get_RX_Data(u8 *destination){
	u8 width = readRXWidth();
	//TODO: If Width is above 32(MAX FOR CHIP) or above whatever max I set in software, flag an error
	receivePayLoad(destination, width);
	flushRx();
	clear_Flag(RX_DR);
}




void powerUp (void){
	CFGValues |= 0x02;
	writeRegSing(CONFIG, CFGValues);
	_delay_ms(5);	//a bit of a delay for this thing to settle down before trying to send data.
}

void powerDown(void){
	CFGValues &= ~(0x02);
	writeRegSing(CONFIG, CFGValues);
}


void ten_uS_ce_Pulse(void){
	set_ce;
	_delay_us(11);	//yeah, it's eleven, just to be safe
	clr_ce;
}


void initialize_NRF(void){
set_csn;
clr_ce;				//clear Chip Enable in case not already done.
writeRegSing(CONFIG, CFGValues);	//initially powered down, CFGValues will be setup for either TX or RX by main depending on whether or not PRX is defined
writeRegSing(SETUP_RETR, 0x77);	//2mS between retries, 7 total retries
writeRegSing(RF_SETUP, 0x06);	//1MBPS, zero TX attenuation
writeRegMult(RX_ADDR_P0, Addy, 5);
writeRegMult(TX_ADDR, Addy, 5);		//Making TX and RX addresses the same
writeRegSing(DYNPD, 0x01);	//enable Dynamic Payload for RX Pipe
writeRegSing(FEATURE, 0x06);	//enablying Dynamic Payload Length. and awk-pack payload
clear_Flag(ALL_FLAG);
flushTx();
flushRx();
powerUp();		//will probably only want this in the Receiver once things are squared away.  TX will be powered up to TX data and then powered down in order to conserve battery

}


