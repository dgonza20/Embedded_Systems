
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "buttons.h"
#include "inc/hw_nvic.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"

// Variable headers for LEDs and switches
#define RED_LED GPIO_PIN_1
#define BLUE_LED GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3
#define SW1 GPIO_PIN_4

//Switch Cases/ Current state
enum STATE {RED,GREEN,BLUE};
enum STATE current = RED;


 // This isr handler will rotate through the leds when called apon
 void interruptHandler(void)
 {

switch (current){

    case GREEN:
        // turn off red led
        GPIOPinWrite(GPIO_PORTF_BASE, RED_LED, 0x00);
        // turn on green led
        GPIOPinWrite(GPIO_PORTF_BASE, GREEN_LED, GREEN_LED);
        current=BLUE;
        break;

    case BLUE:
        // turn off green led
        GPIOPinWrite(GPIO_PORTF_BASE, GREEN_LED, 0x00);
        // turn on blue led
        GPIOPinWrite(GPIO_PORTF_BASE, BLUE_LED, BLUE_LED);
        current=RED;
        break;

    case RED:
        // turn off blue led
        GPIOPinWrite(GPIO_PORTF_BASE, BLUE_LED, 0x00);
        // turn on Red led
        GPIOPinWrite(GPIO_PORTF_BASE, RED_LED, RED_LED);
        current=GREEN;
        break;
    }

    // small delay for proccesor to catch up
    SysCtlDelay(10000);
    // Clears the interrupts source
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_4);
 }

 int main(void){

    // Sets clock frequency
    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    // Enable the GPIO port that is used for the on-board LED and switches.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    ButtonsInit();


    // Disable interrupts
    GPIOIntDisable(GPIO_PORTF_BASE, RED_LED | GREEN_LED | BLUE_LED | SW1);
    // Clears the interrupts sources
    GPIOIntClear(GPIO_PORTF_BASE, RED_LED | GREEN_LED | BLUE_LED | SW1);
    // enable Switch 1 as input
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,SW1);
    // pull up resistor
    GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_4,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    // enable leds as outputs
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED | GREEN_LED | BLUE_LED);
    // set the interrupt to when it senses a rising edge low to high
    GPIOIntTypeSet(GPIO_PORTF_BASE, SW1, GPIO_RISING_EDGE);
    // calls the code for when an interrupt happens
    GPIOIntRegister(GPIO_PORTF_BASE, interruptHandler);
    // Enable interrupts due to activity on switch 1
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_4);

    // loop to allow to loop through leds
    while(1){
        //infinite While Loop
      }
 }



