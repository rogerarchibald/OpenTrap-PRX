/*
 * commspec.h
 *
 * Created: 12/20/15 4:31:18 PM
 *  Author: Roger
 Outline the overall operation of the Remote Controlled(NRF24L01+ comms driven) animal trap.
 
 The PTX will initiate comms to the remote, sending 3 bytes of info.  This new functionality in the Ultrasonic version is a change from the older versions where I just sent bitfields.  The 3 bytes will be:
 
 BB - Battery information
 CC - Counter info
 DD - Distance info

 
 Battery is a raw 8-bit value from the ADC.  The trap itself will set an LED if the battery voltage drops below ~ 6.8V (ADC < 217) which will still give me tonnes of heads up if the battery is starting to droop.
 
 Counter is just a rollover from 0-255 that will increment every transmission.  I can print off the counter from teh remote to get an idea of how many packets are being dropped if this value isn't sequential.
 
 Distance is how many timer rollovers have occured between firing the transmitter and getting a signal bounced back.  Each timer rollover takes 32uS...So this number/4.625 = distance in inches.  If no signal is received in 8mS (~4.5' of range) then this will stop listening and return a 0.
 

 ********************************************************************
 
 The PRX will be reading button/switch inputs and communicating with the PTX.  It's data will also be two bytes, with the following format:
 
 PRX - PTX bitfield info:
 bit 7 = set trap command (0 = don't set, 1 = set)
 bit 6 = lights (0 = lights off, 1 = lights on) I had initially intended to have a brightness option for the lights but I don't have a lot of real estate on the remote nor a free PWM channel.
 bit 4 = shutter release (0 = don't take picture, 1 = take picture)
 bits 3:0 = 1010
 
 Second sent byte will be 0xE2
 
 ********************************************************************
 
 Set Trap will be triggered by a button. If the PTX sees this bit set it will check if the trap is currently 'active'.  If it's not, will launch the function to set the trap.
 
 Light is a power LED tied to hte battery rail via a 24.9ohm resistor.  Its cathode is tied to an N-channel FET controlled by a PTX GPIO.
 
 Shutter release controls a BJT connected to a 1/8" stereo jack.  The sleeve is ground, the ring is open but there's a DNI resistor to give the option to tie it to the tip, and the tip has a 10K pullup to VCC and is on the collector of the BJT.  When shutter release is activated the BJT currently turns on for 1 second to ground the shutter release of an SLR camera.  This line could also be used as an auxiliary connector to connect some noise-maker to chase an unwanted animal out of hte trap (the SLR works well for this, but it's a pricey noisemaker.  The 'extra' pin on the power connector is tied to Battery In to allow connecting a 3rd power line to an extra device and then using shutter to provide a ground.
 
 #$#$#$#$#$#$#$#$ #$#$#$#$#$#$#$#$ #$#$#$#$#$#$#$#$ #$#$#$#$#$#$#$#$ #$#$#$#$#$#$#$#$ #$#$#$#$#$#$#$#$ #$#$#$#$#$#$#$#$
 Actual operation of this deal:
 
 At startup I'll prepare the various state machines on board and get cranking.  The whole deal works on a 50mS rollover:
 
 Start at 0..
 Every 25mS start the ultrasonic state machine.  A maximum of 8mS later the distance info will be available
 Every 50mS will transmit data to the remote, increment my rollover counter and reset my 50mS loop to zero...rinse and repeat.  Part of transmitting data involves checking to see if hte remote sent an awkpack (awknowledgement packet) and in this packet will be commands to set the trap, turn on the light or activate the shutter release.
 
 
 
 
 
 */


#ifndef COMMSPEC_H_
#define COMMSPEC_H_





#endif /* COMMSPEC_H_ */
