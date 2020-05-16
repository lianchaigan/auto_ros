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
#include "Pwm.h"
#include "motors.h"


/*****************************************************************************
 Define
******************************************************************************/
#define SYS_TICK_MS				500
#define DEFAULT_DUTY			20		/* Duty cycle in % */


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
static volatile int 		g_nSysTick 			= SYS_TICK_MS;
static volatile BOOL 		g_bSysTickReady		= FALSE;
static volatile BOOL 		g_bLEDToggle		= FALSE;


/*****************************************************************************
 Local functions
******************************************************************************/


/*****************************************************************************
 Callback functions
******************************************************************************/


/*****************************************************************************
 Implementation 
******************************************************************************/
/* NOTE: 
 This example is to use PWM to turn motors in 8 directions.
 This program is prepared based on Pin Assignment Table and its wheel direction
*/

int main( void )
{
	/* Board Support Package initialization */
    BSPInit();
	
	TRACE( "System Boot up\r\n" );
    TRACE( "SystemCoreClock %dHz", SystemCoreClock );

	/* WARNING:
	 System tick is called direct from ARM Core. Do not use for heavy processing.
	 System tick normally use for increase or decrease a counter or set/reset a flag.
	 Heavy processing in System Tick will caused MCU not responsive!
	*/
	/* Generate interrupt each 1 ms as system tick */
	SysTick_Config( SystemCoreClock/1000 );

    MotorsInit();

	/* NOTE: At start, Motors shall stops for Safety issue */
    MotorsStop();

	/* Move front. Immediately move the motor. 
	Please make sure vehicle is lifted up  */
	MotorsMoveFrontLeft( DEFAULT_DUTY );
    //MOTORS_MOVE_FORWARD();
    //Motors4Enable( DEFAULT_DUTY );

	/* TODO: */
	/* Students can test for the rest of the direction */


	/* Runtime for loop */
    for(;;)
    {
		/* TODO: */
		if( FALSE != g_bSysTickReady )
		{
			g_bSysTickReady = FALSE;

			/* Simply toggle Red LED */
			g_bLEDToggle = !g_bLEDToggle;
            LED_RED_SET( g_bLEDToggle );
		}
	}
}


/*****************************************************************************
 Callback functions
******************************************************************************/


/*****************************************************************************
 Local functions
******************************************************************************/


/*****************************************************************************
 Interrupt functions
******************************************************************************/
void SysTick_Handler( void )
{
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

