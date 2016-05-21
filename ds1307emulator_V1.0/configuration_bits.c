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


// Opzioni di configurazione.
// Qui di seguto ci sono le opzioni di configurazione del microcontrollore.
// Per conoscerne il significato consulatre il datasheet del PIC18F47J53
// alla sezione "SPECIAL FEATURES OF THE CPU" che fa riferimento ai
// registri di configurazione.


// Configurazione del tipo di oscillatore
// Oscillatore HS, PLL abilitato e HSPLL utilizzato dalla USB
#pragma config OSC = HSPLL

// Divisiore del PLL.
// Divide per 3 (Quarzo da 12 MHz)
#pragma config PLLDIV = 3

// Postscaler per il clock della CPU.
// Alla CPU viene mandato il clock senza divisioni
#pragma config CPUDIV = OSC1

// RESET in caso di stack overflow/underflow abilitato.
#pragma config STVREN = ON

// Watchdog disabilitato al RESET- Sarà poi possibile abilitarlo
// all' interno del programma tramite il bit SWDTEN
#pragma config WDTEN = OFF

// Divisore per il clock fornito al watchdog
// Clock di sistema diviso per 32768
#pragma config WDTPS = 32768

// Set di istruzioni esteso disabilitato
#pragma config XINST = OFF

// Protezione della memoria di programma.
// La memoria di programma non è protetta.
#pragma config CP0 = OFF

// Fail-Safe Clock Monitor disabilitato
#pragma config FCMEN = OFF

// Partenza a doppia velocità disabilitata
#pragma config IESO = OFF

// Clock per il Deep Sleep Watchdog Timer.
// Viene fornito il clock INTOSC/INTRC
#pragma config DSWDTOSC = INTOSCREF

// Clock per il Real Time Clock Calendar
// Il clock proviene dall' oscillatore del Timer 1
#pragma config RTCOSC = T1OSCREF

// BOR nello stato di Deep Sleep disabilitato
#pragma config DSBOREN = OFF

// Deep Sleep watchdog disabilitato
#pragma config DSWDTEN = OFF

// Deep Sleep watchdog postscaler impostato a 8192 (8,5 secondi)
#pragma config DSWDTPS = 8192

// Il bit di sicurezza IOL1WAY può essere attivato e disattivato
#pragma config IOL1WAY = OFF

// MSSP address masking a 7 bit (indirizzi I2C in funzionamento slave)
#pragma config MSSP7B_EN = MSK7

// Protizione delle pagine della FLASH abilitata
#pragma config WPDIS = OFF          //WPFP[5:0], WPEND, and WPCFG bits ignored

// La protezione va dalla pagina 0 alla 1 per proteggere il bootloader
#pragma config WPFP = PAGE_1        //Write Protect Program Flash Page 0
#pragma config WPEND = PAGE_0       //Start protection at page 0
#pragma config WPCFG = OFF          //Write/Erase last page protect Disabled


// Define che indica che verrà utilizzato il bootloader. Il PIERIN utilizza
// il bootloader quindi non è da modificiare
#define USE_HID_BOOTLOADER



/******************************************************************************/
/* Configuration Bits                                                         */
/*                                                                            */
/* Refer to 'HI-TECH PICC and PICC18 Toolchains > PICC18 Configuration        */
/* Settings' under Help > Contents in MPLAB X IDE for available PIC18         */
/* Configuration Bit Settings for the correct macros when using the C18       */
/* compiler.  When using the Hi-Tech PICC18 compiler, refer to the compiler   */
/* manual.pdf in the compiler installation doc directory section on           */
/* 'Configuration Fuses'.  The device header file in the HiTech PICC18        */
/* compiler installation directory contains the available macros to be        */
/* embedded.  The XC8 compiler contains documentation on the configuration    */
/* bit macros within the compiler installation /docs folder in a file called  */
/* pic18_chipinfo.html.                                                       */
/*                                                                            */
/* For additional information about what the hardware configurations mean in  */
/* terms of device operation, refer to the device datasheet.                  */
/*                                                                            */
/* General C18/XC8 syntax for configuration macros:                           */
/* #pragma config <Macro Name>=<Setting>, <Macro Name>=<Setting>, ...         */
/*                                                                            */
/* General HiTech PICC18 syntax:                                              */
/* __CONFIG(n,x);                                                             */
/*                                                                            */
/* n is the config word number and x represents the anded macros from the     */
/* device header file in the PICC18 compiler installation include directory.  */
/*                                                                            */
/* A feature of MPLAB X is the 'Generate Source Code to Output' utility in    */
/* the Configuration Bits window.  Under Window > PIC Memory Views >          */
/* Configuration Bits, a user controllable configuration bits window is       */
/* available to Generate Configuration Bits source code which the user can    */
/* paste into this project.                                                   */
/*                                                                            */
/******************************************************************************/

/* TODO Fill in your configuration bits here using the config generator.      */
