/*
 * ADC.c
 *
 * Created: 6/13/15 1:09:27 PM
 *  Author: Roger
 
 
 Want to set up the ADC as 8-bit, will left-align the results and drop the Low register...8-bits is plenty of resolution for my needs here and it's easier to deal with.
 In this first setup I've got a resistor divider on VIN such where Rtop = 100K and Rbottom = 47K so the divide by = /3.12.  7.2Vin/3.12 = 2.3V.  VCC = 3.3V 2.3/3.3 * 256 = 178.
 */ 


#include "ADC.h"

#define threshold1 139
#define threshold2 122		//threshold1 is the level at which to turn on the low-battery indicator.  Threshold2 is critical low and will turn off micro.  Values of 139 and 122 correspond to 3.3 and 2.9v with a 220K -> 49K divider.  Will want to tweak values in future

void ADC_Init(void) {
	
	ADMUX = 0xE5;	// E is selecting Left Allign and reference voltage. LSNibble == 5 is for ADC5
	ADCSRA = 0x05;	//Set prescaler to 32.  
}



//initialize ADC conversion...currently calling this every 250mS from the mS timer ISR
void start_ADC_conv (void){
	ADCSRA |= 0xD0;	//enable ADC, start conversion and clear interrupt flag
	ADCSRA |= 0x08;	//enable ADC interrupt.
}


u8 getADCVal(void){
    return ADCH;
}



//ADC interrupt to check battery voltage and turn on LED if it's below threshold1.  Shut down micro if it's below threshold2
ISR(ADC_vect){
	if(ADCH > threshold1){
		led1_off;
		ADCSRA &= ~(0x88);	//clear the ADC enable bit to reduce power consumption, also disable ADC int.
	}else if(ADCH < threshold2){
		shut_r_down();
	}else{
		led1_on;
    }
	}


//will turn off the micro if the voltage is below threshold 2 to prevent drawing on an almost-dead battery.
void shut_r_down(void){
	for (int i = 0; i < 5; i++){
        led1_tog;
		_delay_ms(400);
	}
	PORTD = 0;
	//TODO make the power-control pin low once the flow is verified.
}
