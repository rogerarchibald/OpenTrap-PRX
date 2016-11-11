/*
 * timer.c
 *Will use this file to increment the mS timer and eventually to control the PWM for the LEDs
 * Created: 4/28/15 9:21:53 PM
 *  Author: Roger
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>	
#include "timers.h"	
#include "NRF24_lib.h"
#include "ADC.h"


static u32 mSecond = 0;
static u8	buzzerOn = 0;	//will use this as a flag 
static u8	twofiftymsroll = 250;



#define debounce_time 20	//how many milliseconds after a button is pressed will I consider it to be pressed.  Will go with 20 as a default start.
#define button_bitmask 0x70	//will be used to compare PIND to the buttons at play. 
//following variables are for button debouncing...Definitely need to clean up the code below.   buttons for debouncing on PD4, PD5 and PD6.  Buttons are on PIND 0BBB 0000 -> 0x70
static u8 tempvalButtons = button_bitmask;
static u32 PD4time = 0;
static u32 PD5time = 0;
static u32 PD6time = 0;	//these will be used to determine how long after the last button press should I keep 'tempvalButtons' saying that the button is pressed.
static u16 downtimePD4 = 0;
static u16 downtimePD5 = 0;
static u16 downtimePD6 = 0;	//these will be used to count how long the button has been pressed.







//Timer0 compare ISR will fire every mS...Using this to debounce buttons and using the 250mS rollover to 2-tone the alarm.
ISR(TIMER0_COMPA_vect){
	mSecond ++;
	twofiftymsroll --;
	if ((PIND & button_bitmask) != button_bitmask){
		press_detected();
	}	//end of seeing if a button is pressed right now.

	if((tempvalButtons & button_bitmask) != button_bitmask){
		prev_press_detected();
	}	//end of checking if there was a previous button press detected
	if (0 == twofiftymsroll){
		twofiftymsroll = 250;
		start_ADC_conv();	//every 250mS read teh ADC
		if(buzzerOn){
			changeTone();	//This function will change the tone of the alarm...Will do this 4 times a second to start, may need to up it.
		}
		
	}
}




void Timer2_init(void) {
OCR2B = 100;		//make this off before we start
OCR2A = 100;	//OCR2A will set the period, OCR2B will set the DC
TCCR2A = 0x21;	//setting OC2B in non-inverting PWM output and timer mode as phase-corrected PWM with OCR2A as top (still need to set a bit in TCCR2B for this)
TCCR2B = 0x0B;	//WGM22 and setting a /32 prescaler.

}


void Timer0_init(void){
OCR0A = 125;	//will set OCR0a as Top...8MHz clock /64 presaler *125 = 1K	
TCCR0A = 0x02;	//Setting timer in CTC mode with OCR0A as top
TCCR0B = 0x03;	//setting the /64 prescaler
//set up ISR
TIFR0 |= 0x02;	//clear interrupt flag prior to enabling interrupt
TIMSK0 |= 0x02;	//enable Interrupt on OCR0A compare.
}





//following functions will handle debouncing of buttons....Should really clean this up and use an array + indexer/for loop rather than the multiple if statements. 

void press_detected(void){	//if I sense any of the buttons are low
	
	if(!(PIND & 0x40)){	//check each of the buttons: 0x40 == PD6 == AWKnowledge alarm
		tempvalButtons &= ~(0x40);	//clear its bit in tempval if the button is pressed
		downtimePD6 ++;	//increment counter of how long it's been pressed.
		shutUpAYouFace();	//turn off Buzzer
		led3_on;		//turn on LED2 which is indication that we're shutting up.
		PD6time = (mSecond + 0x2BF20); //for the awknowledge I'll make a button press push back the alarm for 3 minutes == 180000 == 0x2BF20
		
	}
	if(!(PIND & 0x20)){	//check each of the buttons: 0x20 == PD5 == Aux(camera shutter release)
		tempvalButtons &= ~(0x20);	//clear its bit in tempval if the button is pressed
		downtimePD5 ++;	//increment counter of how long it's been pressed.
		PD5time = (mSecond + debounce_time);
	}
	if(!(PIND & 0x10)){	//check each of the buttons: 0x10 == PD4 == set trap
		tempvalButtons &= ~(0x10);	//clear its bit in tempval if the button is pressed
		downtimePD4 ++;	//increment counter of how long it's been pressed.
		PD4time = (mSecond + debounce_time);
	}
}	//end of what to do if I sense there’s a button pressed and then check each of the 3 buttons individually.


void prev_press_detected(void){	//if any of the button outputs is currently low because of a previously detected button press
	
		if (mSecond >= PD6time){
			tempvalButtons |= 0x40;
			downtimePD6 = 0;	
			led3_off;	//turn off LED indicating that we're going silent.
		}

				if (mSecond >= PD5time){
				tempvalButtons |= 0x20;
				downtimePD5 = 0;
				}
				
		if (mSecond >= PD4time){
			tempvalButtons |= 0x10;
			downtimePD4 = 0;
		}


}	//end of checking if any button presses were previously detected.



u8 get_switch_status(void){
	u8 pop_button_stat = 0;	

	if (!(tempvalButtons & 0x10)){	//If tempvalButtons bit 4 is cleared, send signal to set the trap.
		pop_button_stat |= 0x80;	//say to set the trap
	}
	if (!(tempvalButtons & 0x20)){
		pop_button_stat |= 0x10;	//shutter release
	}
	if(!(PINB & 0x02)){		//read the light switch input and if it's low (i.e. the switch is actuated) set the bit in pop_button_stat which will go to PTX to say turn on the lights
		pop_button_stat |= 0x40;
		}
		
	return pop_button_stat;	
	
}




//functions related to the buzzer
void changeTone(void){	//change hte alarm sound
if(OCR2A == 200){
	OCR2A = 100;
}else{
	OCR2A = 200;}
	OCR2B = (OCR2A/2);
}	//current settings are sufficiently annoying, might play with them a bit.
	
	
	
	
	
void makeNoise(void){
	if(tempvalButtons & 0x40){
	buzzerOn = 1;	//set flag as on
	TCCR2A |= 0x20;	//enable output
	}	//turn on the buzzer if this was called and I'm not within the 3-minute window that's blocked out after the AWK button is pressed
	}
	
	
	
	
void shutUpAYouFace(void){
	buzzerOn = 0;
	TCCR2A &= ~(0x20);	//clearing COM2B1 to disable output
	}	//turn off the buzzer
