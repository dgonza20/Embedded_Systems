#include<stdint.h>
#include<stdbool.h>
#include"inc/hw_types.h"
#include"inc/hw_memmap.h"
#include"inc/hw_nvic.h"
#include"driverlib/gpio.h"
#include"driverlib/sysctl.h"
#include"driverlib/systick.h"
#include"buttons.h"
#include"inc/hw_gpio.h"
#include"driverlib/pin_map.h"

// Variable headers for LEDs and switches
#define BL1 GPIO_PIN_0
#define BL2 GPIO_PIN_1
#define BL3 GPIO_PIN_2
#define BL4 GPIO_PIN_3
#define GREEN GPIO_PIN_3
#define RED GPIO_PIN_1

//Switch Cases/ Current state
enum STATE {BLUE0, BLUE1, BLUE2, BLUE3};
enum STATE current = BLUE0;

// State for RED LED (crosswalk)
enum LIGHTSTATE {ON, OFF};
enum LIGHTSTATE cur = OFF;


struct SysTick_s {
    volatile uint32_t control;
    volatile uint32_t reload;
    volatile uint32_t count;
};
#define SysTick ((struct SysTick_s*)0xE000E010)

// TODO
// create a delay for x amount of seconds
void mydelay(uint32_t seconds) {
    uint32_t i;
    //uint8_t buttonState;
    for (i=0; i < seconds*5; i++) {
        SysTick->reload = 5*160000-1;
        SysTick->control |= 0x1;
        while ((SysTick->control & (1<<16)) == 0) {
//            buttonState = ButtonsPoll(0, 0);
//            if ((buttonState & ALL_BUTTONS) == LEFT_BUTTON) {
//                color_shown = REDLED;
//            }
//            if ((buttonState & ALL_BUTTONS) == RIGHT_BUTTON) {
//                color_shown = GREENLED;
//            }
        }
    }
    SysTick->control = 0x0;
}



// This isr handler will stop the position of the current LED and
// change the red to green or green to red
 void interruptHandler(void)
 {

switch (cur){

    case OFF:
        // turn off red led
        GPIOPinWrite(GPIO_PORTF_BASE, RED, 0x00);
        // turn on Green led
        GPIOPinWrite(GPIO_PORTF_BASE, GREEN, GREEN);
        // Want to hold position of current blue led
            switch(current){
                case BLUE0:
                    // keep first blue led ON
                    GPIOPinWrite(GPIO_PORTB_BASE, BL1, BL1);
                    // Turn off second led
                    GPIOPinWrite(GPIO_PORTB_BASE, BL2, 0x00);
                    break;
                case BLUE1:
                    // keep second led on
                    GPIOPinWrite(GPIO_PORTB_BASE, BL2, BL2);
                    // Turn off first led if on
                    GPIOPinWrite(GPIO_PORTB_BASE, BL1, 0x00);
                    // Turn off third led if on
                    GPIOPinWrite(GPIO_PORTB_BASE, BL3, 0x00);
                    break;
                case BLUE2:
                    // keep third blue led ON
                    GPIOPinWrite(GPIO_PORTB_BASE, BL3, BL3);
                    // Turn off second led if on
                    GPIOPinWrite(GPIO_PORTB_BASE, BL2, 0x00);
                    // Turn off fourth led if on
                    GPIOPinWrite(GPIO_PORTB_BASE, BL4, 0x00);
                    break;
                case BLUE3:
                    // keep fourth led on
                    GPIOPinWrite(GPIO_PORTB_BASE, BL4, BL4);
                    // Turn off third led if on
                    GPIOPinWrite(GPIO_PORTB_BASE, BL3, 0x00);
                    break;
                }
        cur=ON;
        break;

    case ON:
        // turn off green led
        GPIOPinWrite(GPIO_PORTF_BASE, GREEN, 0x00);
//        // turn on Red led
//        GPIOPinWrite(GPIO_PORTF_BASE, RED, RED);
        cur=OFF;
        break;
    }

    // small delay for proccesor to catch up
    SysCtlDelay(10000);
    // Clears the interrupts source
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_1);
 }




 int main(void){

    // Sets clock frequency to 80MHz
    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

    // Enable the GPIO port B and F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);


    // enable leds as outputs
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, BL1 | BL2 | BL3 | BL4 | GREEN | RED);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GREEN);


    // Disable interrupts
        GPIOIntDisable(GPIO_PORTF_BASE, RED | GREEN);
        // Clears the interrupts sources
        GPIOIntClear(GPIO_PORTF_BASE, RED | GREEN);
        // enable Red Led as input
        GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,RED);
//        // pull up resistor
        GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_1,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
        // set the interrupt to when it senses a rising or falling edge
        GPIOIntTypeSet(GPIO_PORTF_BASE, RED, GPIO_BOTH_EDGES);
        // calls the code for when an interrupt happens
        GPIOIntRegister(GPIO_PORTF_BASE, interruptHandler);
        // Enable interrupts due to activity on switch 1
        GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_1);


    // infinite loop
    while(1){

        // Red LED will remain on
        GPIOPinWrite(GPIO_PORTF_BASE, RED, RED);


        // rotate through Blue LEDS Forward
            // Turn on First LED
        GPIOPinWrite(GPIO_PORTB_BASE, BL1, BL1);
        current = BLUE1;
        // keep on for one second
        mydelay(1);
        // turn off First LED
        GPIOPinWrite(GPIO_PORTB_BASE, BL1, 0x00);
        // delay to keep off for 2 seconds
        mydelay(2);

            // Second LED
        GPIOPinWrite(GPIO_PORTB_BASE, BL2, BL2);
        current = BLUE2;
        // keep on for one second
        mydelay(1);
        // turn off Second LED
        GPIOPinWrite(GPIO_PORTB_BASE, BL2, 0x00);
        // delay to keep off for 2 seconds
        mydelay(2);

            // Third LED
        GPIOPinWrite(GPIO_PORTB_BASE, BL3, BL3);
        current = BLUE3;
        // keep on for one second
        mydelay(1);
        // turn off Third LED
        GPIOPinWrite(GPIO_PORTB_BASE, BL3, 0x00);
        // delay to keep off for 2 seconds
        mydelay(2);

            // Fourth LED
        GPIOPinWrite(GPIO_PORTB_BASE, BL4, BL4);
        current = BLUE2;
        // keep on for one second
        mydelay(1);
        // turn off Fourth LED
        GPIOPinWrite(GPIO_PORTB_BASE, BL4, 0x00);
        // delay to keep off for 2 seconds
        mydelay(2);

        // rotate through Blue LEDS Backwards
            // Third LED
        GPIOPinWrite(GPIO_PORTB_BASE, BL3, BL3);
        current = BLUE1;
        // keep on for one second
        mydelay(1);
        // turn off Third LED
        GPIOPinWrite(GPIO_PORTB_BASE, BL3, 0x00);
        // delay to keep off for 2 seconds
        mydelay(2);

            // Second LED
        GPIOPinWrite(GPIO_PORTB_BASE, BL2, BL2);
        current = BLUE0;
        // keep on for one second
        mydelay(1);
        // turn off Second LED
        GPIOPinWrite(GPIO_PORTB_BASE, BL2, 0x00);
        // delay to keep off for 2 seconds
        mydelay(2);

    }





 }
