#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "buttons.h"

#define GREEN_LED GPIO_PIN_3
#define SW1 GPIO_PIN_4
#define SW2 GPIO_PIN_0
#define RED_LED GPIO_PIN_1

struct SysTick_s {
    volatile uint32_t control;
    volatile uint32_t reload;
    volatile uint32_t count;
};
#define SysTick ((struct SysTick_s*)0xE000E010)

volatile uint8_t color_shown = RED_LED;

void mydelay(uint32_t seconds) {
    uint32_t i;
    uint8_t buttonState;
    for (i=0; i < seconds*5; i++) {
        SysTick->reload = 5*160000-1;
        SysTick->control |= 0x1;
        while ((SysTick->control & (1<<16)) == 0) {
            buttonState = ButtonsPoll(0, 0);
            if ((buttonState & ALL_BUTTONS) == LEFT_BUTTON) {
                color_shown = RED_LED;
            }
            if ((buttonState & ALL_BUTTONS) == RIGHT_BUTTON) {
                color_shown = GREEN_LED;
            }
        }
    }
    SysTick->control = 0x0;
}

int main(void) {
    // Set clock rate to 80MHz
    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

    // Enable the GPIO port that is used for the on-board LED.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, RED_LED | GREEN_LED | SW1 | SW2);
    ButtonsInit();

    while (1) {
        GPIOPinWrite(GPIO_PORTF_BASE, color_shown, color_shown);
        mydelay(4);
        GPIOPinWrite(GPIO_PORTF_BASE, RED_LED, 0x0);
        GPIOPinWrite(GPIO_PORTF_BASE, GREEN_LED, 0x0);
        mydelay(4);
    }
}
