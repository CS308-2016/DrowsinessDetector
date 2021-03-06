/*
* Author: Texas Instruments 
* Editted by: Saurav Shandilya, Vishwanathan Iyer 
	      ERTS Lab, CSE Department, IIT Bombay
* Description: This code will familiarize you with using GPIO on TMS4C123GXL microcontroller.
* Filename: lab-1.c
* Functions: setup(), ledPinConfig(), switchPinConfig(), main()
* Global Variables: none
*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))
/*
 ------ Global Variable Declaration
*/
/*
* Function Name: setup()
* Input: none
* Output: none
* Description: Set crystal frequency and enable GPIO Peripherals  
* Example Call: setup();
*/
void setup(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}
/*
* Function Name: ledPinConfig()
* Input: none
* Output: none
* Description: Set PORTF Pin 1, Pin 2, Pin 3 as output.
* Example Call: ledPinConfig();
*/
void ledPinConfig(void)
{
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);  // Pin-1 of PORT F set as output. Modifiy this to use other 2 LEDs.
}
/*
* Function Name: switchPinConfig()
* Input: none
* Output: none
* Description: Set PORTF Pin 0 and Pin 4 as input. Note that Pin 0 is locked.
* Example Call: switchPinConfig();
*/
void switchPinConfig(void)
{
	// Following two line removes the lock from SW2 interfaced on PORTF Pin0 -- leave this unchanged
	LOCK_F=0x4C4F434BU;
	CR_F=GPIO_PIN_0|GPIO_PIN_4;
	// GPIO PORTF Pin 0 and Pin4
	GPIODirModeSet(GPIO_PORTF_BASE,GPIO_PIN_4 | GPIO_PIN_0,GPIO_DIR_MODE_IN); // Set Pin-4 of PORT F as Input. Modifiy this to use another switch
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0);
	GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_4 | GPIO_PIN_0,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);
}
int sw2Status = 0;
int main(void)
{	
	// setup
	setup();
	// configure pin
	ledPinConfig();
	switchPinConfig();

	int pressed = 0;
	int state = -1;
	bool sw2pressed = false; // boolean to see if switch 2 is pressed

	while(1){
		// Delay 
		SysCtlDelay(20);

		// switch 2 is pressed
		if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)==0x00){
			if(!sw2pressed){
				sw2Status ++;
				sw2pressed = true;
			}
		} else {
			sw2pressed = false;
		}
		// switch 1 is pressed
		if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)!=0x00){
			pressed = 0;
		} else {
			if(pressed==0){
				state+=1;
				if(state>2)
					state = 0;
			}
			pressed = 1;
		}
		if(pressed == 1){
			switch(state){
				case 0:GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 2); break; // RED
				case 1:GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 4); break; // BLUE
				case 2:GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 8); break; // GREEN
			}
		}else{
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);	// TURN OF LED
		}
	}
}