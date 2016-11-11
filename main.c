/*
 * SPTry3.c
 *
 * Created: 5/9/15 6:07:47 PM
 *  Author: Roger
 */ 

#define	u8	uint8_t
#define F_CPU 8000000
#include <util/delay.h>
#include <avr/io.h>
#include<avr/interrupt.h>
#include "NRF24_lib.h"
#include	"NRF24defs.h"
#include	"timers.h"
#include "ADC.h"



//Here will setup some constants that the NRF_INIT function will use to determine how it needs to run the show.
#define	MAX_PACKET_LENGTH	10
//use following line if device is acting as PTX or PRX.  //out the define if used as PTX.  Will use #ifdef in initialization function 
#define	PRX

u8 datain [MAX_PACKET_LENGTH];	//easy temp place to receive data.  	
u8 dataout [] = {0x25, 0xAA};	//just an array of random stuff for starters, still trying to verify this works.




//OOPS!. I had routed the lightswitch to PB2 which is Slave Select not...If this pin is configured as an input and driven low, the SPI bus shits the bed.  The work-around I'm going with now is to use
//PB1 as the light switch.  The bad news here is that I'm going to lose TP1 and I've got to change some traces to make this happen.

int main(void)
{

DDRB = 0x2D;		//All of portB = outputs with exception of PB1(lightswitch) and this is shorted to PB2, PB5 (SCK), PB6-7 (XTAL).
/*PB0 = Chip Enable, PB1 = TP1(I lost TP1 because I had to make it the light switch input), PB3 = MOSI, PB4 = MISO, PB5 = SCK, PB6-7 = oscillator*/
DDRD |= 0x8E;	//PD7 = Chip Select Not, PD6-4 = buttons, PD3 = Buzzer, PD2 = power_kill, PD1 = TXO, PD0 = RXI 
PORTD |= 0x04;	//Drive PD2 hi as this will control the drive for the LDO enable signal and needs to be on once the 'power-on' button is released to keep power. 
DDRC |= 0x1F;	//PC0-PC4 == LED's, PC5 = Pot_in
DIDR0 |= 0x20;	//PC5 is used as analog in, disable digital IO on this pin.
for (u8 i = 0; i < 5; i ++){	//silly light show as it starts up
	PORTC = (1 << i);
	_delay_ms(100);
}	//end of light show
PORTC = 0;


ADC_Init();	//initialize ADC which includes enabling interrupt on conversion
sei();
//initialize SPI on AT328
SPCR = (1<<SPE)|(1<<MSTR);	

#ifdef PRX
enable_PRX();		//This will set a bit in a variable that's in the NRF24_lib.c and will be written to the NRF Config Register.
#endif

initialize_NRF();
set_ce;	//in test phase always on for PRX.  Will probably change this for battery conservation.
Timer0_init();	//initialize mS timer which will be used to time debouncing of buttons
Timer2_init();  //Timer2 will be used for the buzzer.
   
   
   while(1){   
	
	
	if(check_Flag(RX_DR)){
	
	get_RX_Data (datain);	//this will dump the data into the array 'datain'
	if(datain[0] & 0x80){		//if the Trap is Set
		makeNoise();	//make some noise and turn on the alarm
		led4_on;
	}else{
		led4_off;
		shutUpAYouFace();
		}		//end of what to do based on datain bit7, PIR status
		if(datain[0] & 0x40){
			led2_on;	//if low battery bit is set, turn on LED2
	}else{
		led2_off;	
	}	//end of checking battery statu

	//making awk-pak with payload
	dataout[0] = get_switch_status();	//get the current state of the buttons from function that's over in timer.c...It's with the timers since button debouncing is all done off hte millisecond timer.  Also the light switch status is read there. 
	sendPayLoad(W_ACK_PAYLOAD, dataout, 2);
	led5_tog;	//This LED is indication that we're talking.
	//TODO...look at datain and drive LEDs/launch alarm accordingly
	start_ADC_conv();	//This will have the ADC make two conversions, one for checking VIN for low battery and the other for whether or not the pot is high enough to enable the PWM...when both conversions are complete the ADC will call 'populate_dataout' and I'll get ready for the enxt round.
	}
	


	}//end of while(1)
}	//end of main


