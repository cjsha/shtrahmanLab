

/**
 * main.c
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/pwm.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/debug.h"

#define C4   (uint32_t)261.63         //0
#define Db4  (uint32_t)277.18         //1
#define Dnat4   (uint32_t)293.66         //2
#define Eb4  (uint32_t)311.13         //3
#define E4   (uint32_t)329.63         //4
#define Fnat4   (uint32_t)349.63         //5
#define Gb4  (uint32_t)369.99         //6
#define G4   (uint32_t)392.00         //7
#define Ab4  (uint32_t)415.30         //8
#define A4   (uint32_t)440.00
#define Bb4  (uint32_t)466.16
#define B4   (uint32_t)493.88
#define C5   (uint32_t)523.25
#define Db5  (uint32_t)554.37
#define Dnat5 (uint32_t)587.33
#define Eb5  (uint32_t)622.25

uint32_t trainingPhase;
uint32_t tones[] = {C4,Db4,Dnat4,Eb4,E4,Fnat4,Gb4,G4,Ab4,A4,Bb4,B4,C5,Db5,Dnat5,Eb5};
uint32_t song[] = {C4, E4, G4, C5};
uint32_t templateSong1[] = {C4, E4, G4, C5};
uint32_t templateSong[] = {0, 3, 6, 11};
char songIndeces[];
uint32_t maxDev,minDev;
uint32_t numberOfTrials;
uint32_t play = 0;
uint32_t phase;
uint32_t noTones = 0;
uint32_t songLength = 6;

char hexChars[10];

void enableUART(void);
void UART0IntHandler(void);

int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    enableUART();

    while(1){

    }
    return 0;
}
void enableUART(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinConfigure(GPIO_PA0_U0RX);  // PA6 as the Reciever Line
    GPIOPinConfigure(GPIO_PA1_U0TX);   // PA7 as the Transmitter Line
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
        UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
    IntEnable(INT_UART0);
    // Receiver Interrupts: When a single character has been received
    // Receiver Timeout: Generated when a character has been received,
    // and a second character has not been received within a 32-Bit Period
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

}

void UART0IntHandler(void){
    int j = 0;
    int i = 0;
    uint32_t ui32Status;
    ui32Status = UARTIntStatus(UART0_BASE, true); //get interrupt status
    UARTIntClear(UART0_BASE, ui32Status); //clear the asserted interrupts
    while(UARTCharsAvail(UART0_BASE)) //loop while there are chars
    {
       //UARTCharPutNonBlocking(UART0_BASE, UARTCharGetNonBlocking(UART0_BASE)); //echo character
       hexChars[j] = UARTCharGetNonBlocking(UART0_BASE);
       //for debug
       UARTCharPut(UART0_BASE, hexChars[j]);
       UARTCharPut(UART0_BASE, ' ');
       UARTCharPut(UART0_BASE, j);
       UARTCharPut(UART0_BASE, '\n');
       ++j;
//       if(hexChars[j]== 0xFF){
//           ++j;
//       }
       /*If GUI is trying to edit settings*/
       if(0x11 == hexChars[0]){
               trainingPhase = (uint32_t)hexChars[1];

        }

    }

    /*If user is trying to resume behavior*/
    if(0x55 == hexChars[0]){
        play = 0;
        UARTCharPut(UART0_BASE,hexChars[0]);
        UARTCharPut(UART0_BASE,'0');
        UARTCharPut(UART0_BASE, '\n');


    }
    if(0x56 == hexChars[0]){
        play = 1;
        UARTCharPut(UART0_BASE,hexChars[0]);
        UARTCharPut(UART0_BASE,'1');
        UARTCharPut(UART0_BASE, '\n');

    }
    if(0x11 == hexChars[0] && j > 7){
        if(hexChars[1] < 5){
            phase = (int)hexChars[1];
            UARTCharPut(UART0_BASE, phase);
            UARTCharPut(UART0_BASE, '\n');
        }
        //min/max deviations
        minDev = hexChars[2];
        maxDev = hexChars[3];
        //determine number of tones in songs
        for(i = 4; i < 10; ++i){
            if(hexChars[i] == 0xFF){
                ++noTones;
            }
        }
        songLength = songLength - noTones;
        UARTCharPut(UART0_BASE, songLength);
        UARTCharPut(UART0_BASE, '\n');
        //store song index into array
        for(i = 0; i < (songLength - 1); ++i){
            songIndeces[i] = (char)hexChars[i + 4];

            UARTCharPut(UART0_BASE, songIndeces[i]);
        }
        UARTCharPut(UART0_BASE, '\n');
        numberOfTrials = hexChars[10];

    }

}
