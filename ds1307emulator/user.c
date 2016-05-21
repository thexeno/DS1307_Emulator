/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>    /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#endif

#include "user.h"

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

void InitApp(void)
{
    ANCON0 = 0xff;
    ANCON1 = 0b00011111;

  // Setup porte MSSP1 - i2c
    TRISBbits.TRISB4 = 1; //SCL1
    TRISBbits.TRISB5 = 1; //SDA1


    /*Inits RTC*/
    TRISCbits.TRISC0 = 1;
    TRISCbits.TRISC1 = 1;
    TRISBbits.TRISB1 = 0; // output RTCC
    EECON2 = 0x55;
    EECON2 = 0xAA;
    RTCCFGbits.RTCWREN = 1;
      // HALFSEC stato del secondo
    RTCCFGbits.RTCOE = 1;
    RTCCFGbits.RTCEN = 1;
    PADCFG1 = 0b00000101; // 2^16 su RTCC pin
    T1CONbits.T1OSCEN = 1;

    /*debug leds*/
    TRISDbits.TRISD6 = 0; // led1
    PORTDbits.RD6 = 0;
    TRISDbits.TRISD7 = 0; // led2
    PORTDbits.RD7 = 0;

    /* Enable interrupts */

    RCONbits.IPEN = 1;
    INTCONbits.GIEL = 1;
    INTCONbits.GIEH = 1;
}

