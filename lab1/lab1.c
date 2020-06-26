#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "buttons.h"


volatile int i;
volatile int j;
//delay function
void delay(float rate)
{

            for(i=0; i < rate ;i++)
            {
                //do nothing
            }
}

//blink function
void myBlinky(int color, float blink_rate, int times)
{


        for(j=0; j<times; j++)
        {
        GPIOPinWrite(GPIO_PORTF_BASE, color, color);
        delay(blink_rate);
        GPIOPinWrite(GPIO_PORTF_BASE, color, 0x0);
        delay(blink_rate);
        }
}

int main(void)
{
    //enabling the clock
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    // Check if the peripheral access is enabled.
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
        //checking
    }

    // enable the GPIO pin for digital function.
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);

    ButtonsInit();

    int color=GPIO_PIN_1;

    float rate_of_blink;
    int timesss;
    while(1)
    {
       if(color==GPIO_PIN_1)
       { // Yay konsa color hai?
           //pin 1 red
           //pin 2 blue Chechk
           rate_of_blink= 1000000;
           timesss=5;
           color=GPIO_PIN_2;
       }
       else if(color==GPIO_PIN_2)
       {
           rate_of_blink= 500000;
           timesss=10;
           color=GPIO_PIN_1;
       }

       myBlinky(color,rate_of_blink,timesss);
    }
}
