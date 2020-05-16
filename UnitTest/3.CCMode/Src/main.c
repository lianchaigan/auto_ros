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
#include "Ccm.h"


/*****************************************************************************
 Define
******************************************************************************/
#define SYS_TICK_MS				500


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
static TIMER_HANDLE			g_Timer1Handle;
static TIMER_HOOK			g_TImer1Hook1;

static volatile int 		g_nSysTick 			= SYS_TICK_MS;
static volatile BOOL 		g_bSysTickReady 	= FALSE;

static BOOL					g_bBlueToggle		= FALSE;
static BOOL					g_bRedToggle		= FALSE;

static CCM_HANDLE			g_CcmHandle;


/*****************************************************************************
 Local functions
******************************************************************************/


/*****************************************************************************
 Callback functions
******************************************************************************/
static void main_cbTimer1Hook1( void );



/*****************************************************************************
 Implementation 
******************************************************************************/
int main( void )
{
	int res;

    BSPInit();
	
	TRACE( "System Boot up\r\n" );
    TRACE( "SystemCoreClock %dHz", SystemCoreClock );

	/* Generate interrupt each 1 ms as system tick */
	SysTick_Config( SystemCoreClock/1000 );

	res = TimerInit( &g_Timer1Handle, 1U, 20 ); 
	ASSERT( TIMER_OK == res );

	/* LED Blue hook to Timer 1 */
	TimerAddHook( &g_Timer1Handle, &g_TImer1Hook1, main_cbTimer1Hook1 );

	/* Start Timer */
    TimerStart( &g_Timer1Handle );

	res = CCMInit( &g_CcmHandle, 4U );
	ASSERT( CCM_OK == res );

	CCMEncoderMode( &g_CcmHandle, CCM_ENC_INPUT_AB );

	/* WARNING:
	 There is unlimited hook you can create per timer but more hook mean timer
	  interrupt need to iterate larger link list and it consume time. It is your judgement
	  base on your time profiling to determine how many hooks are allowed. 
	  Total time of all hooks cannot exceed timer period.
	*/

	/* Runtime for loop */
    for(;;)
    {
		/* TODO: */
		TRACE( "%d\r\n", CCMReadCount( &g_CcmHandle ) );
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

