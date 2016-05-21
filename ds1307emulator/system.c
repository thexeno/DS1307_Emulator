/******************************************************************************/
/*Files to Include                                                            */
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

#include "system.h"


void mainOsc(_t_pllmode pll_m, _t_clksource clk_s)
{
    OSCCON = 0b01100000;
    if (clk_s == CLK_PRI) {
        OSCCON &= ~(0x03);
    } else if (clk_s == CLK_SEC){
        OSCCON |= 0x01;
    } else if (clk_s == CLK_INT){
        OSCCON |= 0x03;
    }

    if (pll_m == PLL_ON) {
         OSCTUNEbits.PLLEN = 1;
         __delay_ms(2);
     }
     else OSCTUNEbits.PLLEN = 0;


}

void secondOsc(_t_soscmode sosc)
{
    if (sosc == SOSC_ON){
        T1CONbits.T1OSCEN = 1;
    } else T1CONbits.T1OSCEN = 0;
}