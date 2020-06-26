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

// Port Mapping
#define PB3 GPIO_PIN_3
#define PB2 GPIO_PIN_2
#define PB1 GPIO_PIN_1
#define PB0 GPIO_PIN_0
#define A 11363
#define B 10121
#define C 9560
#define D 8517
#define E 7587
#define F 7153
#define G 6377


#define SYSCTL_RCGC2          (*((volatile uint32_t *)0x400FE108))
#define GPIO_PORTB_LOCK       (*((volatile uint32_t *)0x40005520))
#define GPIO_PORTB_CR         (*((volatile uint32_t *)0x40005524))
#define GPIO_PORTB_AMSEL      (*((volatile uint32_t *)0x40005528))
#define GPIO_PORTB_PCTL       (*((volatile uint32_t *)0x4000552C))
#define GPIO_PORTB_DIR        (*((volatile uint32_t *)0x40005400))
#define GPIO_PORTB_AFSEL      (*((volatile uint32_t *)0x40005420))
#define GPIO_PORTB_DEN        (*((volatile uint32_t *)0x4000551C))
#define GPIO_PORTB_DR8R       (*((volatile uint32_t *)0x40005508))
#define GPIO_PORTB_DATA       (*((volatile uint32_t *)0x400053FC))
#define NVIC_ST_CTRL          (*((volatile uint32_t *)0xE000E010))
#define NVIC_ST_RELOAD        (*((volatile uint32_t *)0xE000E014))
#define NVIC_ST_CURRENT       (*((volatile uint32_t *)0xE000E018))
#define NVIC_SYS_PRI3         (*((volatile uint32_t *)0xE000ED20))

struct SysTick_s {
    volatile uint32_t control;
    volatile uint32_t reload;
    volatile uint32_t count;
};
#define SysTick ((struct SysTick_s*)0xE000E010)

// Intitalize 4 bit DAC
void InitDac(void) {

    volatile unsigned long delay;


    SYSCTL_RCGC2 |= 0x02;          // unlock port B clock

    delay = SYSCTL_RCGC2;          // allow time for clock to start

    GPIO_PORTB_LOCK |= 0x4C4F434B; // unlock GPIO Port B

    GPIO_PORTB_CR |= 0x0F;         // allow change to PB3-PB0

    GPIO_PORTB_AMSEL &= ~0x0F;     // disable analog function on PB3-PB0

    GPIO_PORTB_PCTL = 0x00;        // clear PCTL register on PB3-PB0

    GPIO_PORTB_DIR |= 0x0F;        // PB3-PB0 outputs

    GPIO_PORTB_AFSEL &= ~0x0F;     // disable alternate function on PB3-PB0

    GPIO_PORTB_DEN |= 0x0F;        // enable digital pins on PB3-PB0

    GPIO_PORTB_DR8R |= 0x0F;       // enable 8-mA drive select on PB3-PB0

}


// Writes output to DAC

void OutDac(unsigned long data){
    // Writes every single value from the SineWave array
    GPIO_PORTB_DATA = data;
}


// This creates a sine wave
const unsigned char SineWave[16] = {8, 10, 12, 14, 14, 14, 12, 10, 8, 6, 4, 2, 2, 2, 6};
unsigned char index =0;

// converts the frequency of the systick to the frequency of the soundwave
const unsigned long FREQ_SYSTICK_TO_WAVE[8] = {A, B, C, D, E, F, G};

// Initialize systick periodic interrupts and DAC
void InitSound(void) {

    // DAC Initialization

    InitDac();                     // call the InitDac() function in DAC.c

    index = 0;                     // init value of index is 0

    // SysTick Initialization

    NVIC_ST_CTRL = 0;            // disable SysTick during setup

    NVIC_ST_RELOAD = 0x00FFFFFF; // maximize the RELOAD value

    NVIC_ST_CURRENT = 0;         // any write to CURRENT clears it

    NVIC_ST_CTRL = 0x00000005;   // enable SysTick with core clock

    // priority 1

    NVIC_SYS_PRI3 = (NVIC_SYS_PRI3 & 0x00FFFFFF) | 0x20000000;

    NVIC_ST_CTRL = 0x0007;       // enable, source clock, and interrupts

//    SysTickEnable();
//    SysTick->control =0;
//    SysTick->reload=0x00FFFFFF;
//    SysTick->count = 0;
//    SysTick->control=
//    NVIC_ST_CURRENT =0;

}



/**

 * Changes SysTick periodic interrupts to start sound output

 *

 * @param  period  interrupt period

 *                 1 unit of period = 12.5 ns

 *                 maximum is 2^24 - 1

 *                 minimum is determined by the length of ISR

 */

void PlaySound(unsigned long period) {

    // reset the RELOAD value for different frequencies

    NVIC_ST_RELOAD = (period - 1) & 0x00FFFFFF;

}



/**

 * Stops outputing to DAC

 */

void StopSound(void) {

    GPIO_PORTB_DATA &= ~0x0F; // clear PB3-PB0

}



/**

 * SysTick interrupt service routine

 * Executed every (12.5 ns) * (period)

 *

 * @assumption    80-MHz clock

 */

void SysTick_Handler(void) {



        // only executes if there is non-zero input

        index = (index + 1) & 0x0F; // cycles from 0-15

        OutDac(SineWave[index]);   // output 1 data each interrupt



}

void DissableInterrupts(void);
void ClearInterrupts(void);
void EnableInterrupts(void);



int main(void){


    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);


        InitSound();        // initialize SysTick and DAC



        void EnableInterrupts(); // enable interrupts



        // Loop

        while (1) {






                // adjust the frequency similarly to C, D, E, G

                PlaySound(FREQ_SYSTICK_TO_WAVE[index]);



        }
}

