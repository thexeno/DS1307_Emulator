/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/

/* TODO Define system operating frequency */

/* Microcontroller MIPs (FCY) */
#define SYS_FREQ        48000000L
#define FCY             SYS_FREQ/4
#define _XTAL_FREQ      SYS_FREQ

/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

/* Custom oscillator configuration funtions, reset source evaluation
functions, and other non-peripheral microcontroller initialization functions
go here. */

typedef enum {SOSC_ON, SOSC_OFF} _t_soscmode;
typedef enum {PLL_ON, PLL_OFF} _t_pllmode;
typedef enum {HIGH_PRI, LOW_PRI} _t_priority;
typedef enum {CLK_PRI, CLK_SEC, CLK_INT} _t_clksource;

void mainOsc(_t_pllmode, _t_clksource); /* Handles clock switching/osc initialization */
void secondOsc(_t_soscmode);
