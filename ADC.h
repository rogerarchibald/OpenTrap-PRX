/*
 * ADC.h
 *
 * Created: 6/13/15 1:22:20 PM
 *  Author: Roger
 */ 


#ifndef ADC_H_
#define ADC_H_

#include <avr/interrupt.h>
#include "NRF24_lib.h"
#include	"NRF24defs.h"


void ADC_Init(void);
void start_ADC_conv (void);
void shut_r_down(void);		//will call this if battery voltage is detected to be below some threshold.

#endif /* ADC_H_ */