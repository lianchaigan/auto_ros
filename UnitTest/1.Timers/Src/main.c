/*****************************************************************************
 @Project	:
 @File 		: main.c
 @Details  	: Main entry
 @Author	: lcgan
 @Hardware	: STM32
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
******************************************************************************/
#include <Common.h>
#include "Hal.h"
#include "BSP.h"
#include "Timer.h"

/* NOTE: Please follows code style provided. All tabs is 4 space */

/*****************************************************************************
 Define
******************************************************************************/
#define SYS_TICK_MS 500 /* Software Timer with 500 ms */


/*****************************************************************************
 Type definition
******************************************************************************/


/*****************************************************************************
 Global Variables
******************************************************************************/


/*****************************************************************************
 Const Local Variables
******************************************************************************/


/*****************************************************************************
 Local Variables
******************************************************************************/
static TIMER_HANDLE g_Timer1Handle; /* This is timer 1 handle */
static TIMER_HOOK g_TImer1Hook1;    /* This is hook 1 to timer 1 */
static TIMER_HOOK g_TImer1Hook2;    /* This is hook 2 to timer 1 */
static TIMER_HOOK g_TImer1Hook3;    /* This is hook 3 to timer 1 */

static TIMER_HANDLE g_Timer2Handle; /* This is timer 2 handle */
static TIMER_HOOK g_TImer2Hook1;    /* This is hook 1 to timer 2 */

static volatile int g_nSysTick = SYS_TICK_MS;
static volatile BOOL g_bSysTickReady = FALSE;

static BOOL g_bBlueToggle = FALSE;
static BOOL g_bRedToggle = FALSE;


/*****************************************************************************
 Local functions
******************************************************************************/


/*****************************************************************************
 Callback functions
******************************************************************************/
static void main_cbTimer1Hook1( void );
static void main_cbTimer1Hook2( void );
static void main_cbTimer1Hook3( void );
static void main_cbTimer2Hook1( void );


/*****************************************************************************
 Implementation
******************************************************************************/
int main( void )
{
    BSPInit( );

    TRACE( "System Boot up\r\n" );
    TRACE( "SystemCoreClock %dHz", SystemCoreClock );

    /* Generate interrupt each 1 ms as system tick */
    SysTick_Config( SystemCoreClock / 1000 );

    TimerInit( &g_Timer1Handle, 1U, 1 );

    /* LED Blue first hook to Timer 1 */
    TimerAddHook( &g_Timer1Handle, &g_TImer1Hook1, main_cbTimer1Hook1 );

    /* LED Red second hook to Timer 1 */
    TimerAddHook( &g_Timer1Handle, &g_TImer1Hook2, main_cbTimer1Hook2 );

    /* Start timer 1 now */
    TimerStart( &g_Timer1Handle );

    /* WARNING:
     There is unlimited hook you can create per timer but more hooks mean timer
      interrupt need to iterate larger link list and it consumes more time. It
     is your judgement base on your time profiling to determine how many hooks
     are allowed. Total time of all hooks cannot exceed timer period.
    */

    /* Runtime for loop */
    for( ;; )
    {
        /* TODO: */
        if( FALSE != g_bSysTickReady )
        {
            g_bSysTickReady = FALSE;

            /* Every 500ms per tick */
        }
    }
}


/*****************************************************************************
 Callback functions
******************************************************************************/
static void main_cbTimer1Hook1( void )
{
    g_bBlueToggle = !g_bBlueToggle;
    LED_BLUE_SET( g_bBlueToggle );
}


static void main_cbTimer1Hook2( void )
{
    g_bRedToggle = !g_bRedToggle;
    LED_RED_SET( g_bRedToggle );
}


/*****************************************************************************
 Local functions
******************************************************************************/


/*****************************************************************************
 Interrupt functions
******************************************************************************/
void SysTick_Handler( void )
{
    /* NOTE:
    It is more efficient to compare to 0 for Cortex M
    */

    /* Provide system tick with a specified period
    decided by macro SYS_TICK_MS */
    if( 0 != g_nSysTick )
    {
        g_nSysTick--;

        if( 0 == g_nSysTick )
        {
            g_nSysTick = SYS_TICK_MS;
            g_bSysTickReady = TRUE;
        }
    }
}