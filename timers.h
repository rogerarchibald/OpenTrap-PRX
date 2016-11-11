/*
 * timers.h
 *
 * Created: 4/28/15 9:20:42 PM
 *  Author: Roger
 */ 


#ifndef TIMERS_H_
#define TIMERS_H_

#define	u8	uint8_t
#define	u32	uint32_t
#define	u16	uint16_t

//////////////////////////////////////////////////////////
/*function prototypes*/
void Timer0_init(void); //Timer0 will be used for mS timer/button debouncing
void Timer2_init(void);	//Timer 2 will drive the buzzer



void press_detected(void);
void prev_press_detected(void);	//functions used to debounce the buttons
u8 get_switch_status(void);	//will get the current state of the buttons

//functions related to the buzzer coming from Timer2B/PD3

void changeTone(void);	//change hte alarm sound
void makeNoise(void);	//turn on the buzzer
void shutUpAYouFace(void);	//turn off the buzzer


#endif /* TIMERS_H_ */