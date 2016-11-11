/*
 * commspec.h
 *
 * Created: 12/20/15 4:31:18 PM
 *  Author: Roger
 Outline the overall operation of the Remote Controlled(NRF24L01+ comms driven) animal trap.
 
 The PTX will initiate comms to the remote, sending some information in the following 2-byte format where the first byte is a bitfield and the second byte is not initially going to be used, but will put it in the first round implementation so it's there if I decide it's needed later.  Format for the two bytes will be:
 *****************************************************************
 
 Status bitfield:
 PTX - PRX 7:0	PBT10101
 bit 7 = PIR Status (1 = motion detected, 0 = clear)
 bit 6 = Battery Low (0 = above threshold, 1 = below threshold)
 bit 5 = trap status (1 = trap has been set, 0 = trap is armed)
 bit 4 = 1, always 1
 bit 3 = 0, always 0
 Bit 2 = 1, always 1
 Bit 1 = 0, always 0
 Bit 0 = 1, always 1.
 Second sent byte will be 0x55, unless I decide I need it later.
 
 ******************************************************************
 PIR status will come from state machine that is controlled via interrupts from PIR input driving INT0/PD2
 
 Battery Low will need the ADC to measure ADC7, where I'm bringing in a resistor-divided input.  Initially planning to use a 7.2V battery.Will read ADC input and if it's below limit will set this flag.
 
 Trap status will be set at the end of the 'set trap' routine as an indication that it's been run (as of right now I don't have a way to verify that it physically set the trap)
 

********************************************************************

The PRX will be reading button/switch inputs and communicating with the PTX.  It's data will also be two bytes, with the following format:

PRX - PTX bitfield info:
bit 7 = set trap command (0 = don't set, 1 = set)
bit 6 = lights (0 = lights off, 1 = lights on)
bit 5 = 1, will always read 1
bit 4 = shutter release (0 = don't take picture, 1 = take picture)
bits 3:0 = 1010
 
Second sent byte will be 0xAA, unless lights are on in which case it will be the PWM value.

********************************************************************

Set Trap will be triggered by a button. If the PTX sees this bit set it will check if the trap is currently 'active'.  If it's not, will launch the function to set the trap.

Lights are an ancillary that's currently going off the 6-pin right-angle header on the receiver board.  Will be able to externally connect some LED's and have this signal turn them on. Will just have a single pot on the PRX that's read by the ADC.  If the pot value is above a minimum threshold it will trigger the lights, otherwise bit 6 will be clear.  This saves a button.

Shutter release is also an ancillary of the 6-pin header.  Could connect a camera there and have the remote trigger shutter release. 
 
 #$#$#$#$#$#$#$#$ #$#$#$#$#$#$#$#$ #$#$#$#$#$#$#$#$ #$#$#$#$#$#$#$#$ #$#$#$#$#$#$#$#$ #$#$#$#$#$#$#$#$ #$#$#$#$#$#$#$#$
 Actual operation of this deal:
 
 At startup, the PTX (trap) will reset its transmit byte to the default 0x15: bits 4, 2 and 0 will always be set, bits 3 and 1  will always be clear.  Need to fill in the other bits at the start:
 For bit 7 will check the PIR state which is controlled via state machine.  If bit 7 of PIR state is set, this indicates 'cat_in_trap'.  Currently this will stay hi for 20 seconds once activated
 For bit 6 will call function to read previous result of ADC conversion and start a new conversion.  The bit will always represent the status of the battery voltage at last transmission.  A conversion may take up to 200uS when first arming ADC, so it's important to ensure that the conversion has completed before going back to sleep after sending data
 
 For bit 5 will have a status bit set whenever the trap is set, will pull this into bit 5 here.
 Bit 4 always 1
 Bit 3 always 0
 Bit 2 always 1
 Bit 1 always 0
 Bit 0 always 1
 
 Once all the bitfields are checked and appropriate bits set, will send packet of the bitfield followed by 0x55.   
 
 
 
 Have 5 LED's across the PRX.  Their status is the following:
 
 LED1	- Remote Low Battery (read by onboard LED)
 LED2	- Trap Low Battery (bit 6 of PTX packet)
 LED3	- Silence! (if the AWK button has been pressed to shut up the alarm, and that state hasn't yet timed out)
 LED4	- Motion detected (bit 7 of PTX packed)
 LED5	- Comms (will toggle every time a packet is sent
 
 */ 


#ifndef COMMSPEC_H_
#define COMMSPEC_H_





#endif /* COMMSPEC_H_ */