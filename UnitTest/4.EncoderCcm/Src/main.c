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
#include "IRQ.h"
#include "Timer.h"
#include "Pwm.h"
#include "Ccm.h"
#include "motors.h"
#include "Encoder.h"


/*****************************************************************************
 Define
******************************************************************************/
#define SYS_TICK_MS					500
#define DEFAULT_DUTY				20
#define TOTAL_MOTOR					4
#define TIMER5_TICK_HZ				10000U		/* 10KHz */
#define ENCODER_SMPL_INTRVL_MS		50U		/* 50ms */


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
static volatile BOOL 		g_bLEDToggle		= FALSE;

static TIMER_HANDLE			g_Timer5Handle;

static SHAFT_ENC_HANDLE		g_ShaftEncHandles[TOTAL_MOTOR];
static volatile BOOL		g_bEncoderData = FALSE;

static CCM_HANDLE			g_Ccm1Handle;
static CCM_HANDLE			g_Ccm2Handle;
static CCM_HANDLE			g_Ccm3Handle;
static CCM_HANDLE			g_Ccm4Handle;


/*****************************************************************************
 Local functions
******************************************************************************/
static void main_EncodersInit( void );


/*****************************************************************************
 Callback functions
******************************************************************************/
static void main_cbEncoderRdy( void );
static void main_CtrlUpdateWheelSpeed( void );


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

	TimerInit( &g_Timer5Handle, 9, TIMER5_TICK_HZ );

	main_EncodersInit();

    MotorsInit();

	/* NOTE: At start, Motors shall stops for Safety issue */
    MotorsStop();

	/* Move front */
	MotorsMoveFront( DEFAULT_DUTY );

    //MOTORS_MOVE_FORWARD(); /* Set direction */
   //Motors3Enable( DEFAULT_DUTY );

    TimerStart( &g_Timer5Handle );

    IRQInit();

	/* TODO: */
	/* Students can test for the rest of the direction */


	/* Runtime for loop */
    for(;;)
    {
        if( TRUE == g_bEncoderData )
		{
			/* Only Encoder data count is ready only will execute */
			g_bEncoderData = FALSE;
			main_CtrlUpdateWheelSpeed();
		}
	}
}


/*****************************************************************************
 Callback functions
******************************************************************************/
static void main_cbEncoderRdy( void )
{
	g_bEncoderData = TRUE;
}


/*****************************************************************************
 Local functions
******************************************************************************/
static void main_EncodersInit( void )
{
	int res;
	res = CCMInit( &g_Ccm1Handle, 1U );
	ASSERT( CCM_OK == res );

	res = CCMInit( &g_Ccm2Handle, 3U );
	ASSERT( CCM_OK == res );

	res = CCMInit( &g_Ccm3Handle, 2U );
	ASSERT( CCM_OK == res );

	res = CCMInit( &g_Ccm4Handle, 4U );
	ASSERT( CCM_OK == res );

	CCMEncoderMode( &g_Ccm1Handle, CCM_ENC_INPUT_AB );
	CCMEncoderMode( &g_Ccm2Handle, CCM_ENC_INPUT_AB );
    CCMEncoderMode( &g_Ccm3Handle, CCM_ENC_INPUT_AB );
	CCMEncoderMode( &g_Ccm4Handle, CCM_ENC_INPUT_AB );

	ShaftEncoderInit(
		&g_Timer5Handle,
		TIMER5_TICK_HZ,
		main_cbEncoderRdy,
		ENCODER_SMPL_INTRVL_MS );

	/* Add 4 available encoders */
	ShaftEncoderAdd( &g_ShaftEncHandles[0], 0, 0 );
	ShaftEncoderAdd( &g_ShaftEncHandles[1], 1, 0 );
	ShaftEncoderAdd( &g_ShaftEncHandles[2], 2, 0 );
	ShaftEncoderAdd( &g_ShaftEncHandles[3], 3, 0 );

    
	ShaftEncoderCfg(
		28,
		26,
		ENCODER_SMPL_INTRVL_MS );

	/* NOTE:
	If link to CCM, no encoder Input interrupt required.
	Please make sure remove from Hal.c */
    ShaftEncoderLinkCCM( &g_ShaftEncHandles[0], &g_Ccm1Handle );
    ShaftEncoderLinkCCM( &g_ShaftEncHandles[1], &g_Ccm2Handle );
    ShaftEncoderLinkCCM( &g_ShaftEncHandles[2], &g_Ccm3Handle );
	ShaftEncoderLinkCCM( &g_ShaftEncHandles[3], &g_Ccm4Handle );
}


static void main_CtrlUpdateWheelSpeed( void )
{
	int raw;
	int cnt;
    int rpm;

	TRACE("Encoder : ");
	for( cnt=0; cnt<TOTAL_MOTOR; cnt++ )
	{
		/* NOTE:
		You can read the encoder in RPM using ShaftEncoderReadRPM() 
		but ShaftEncoderCfg() need to be configured first */
		raw = ShaftEncoderReadCount( &g_ShaftEncHandles[cnt] );
        rpm = ShaftEncoderReadRPM( &g_ShaftEncHandles[cnt] );
		TRACE( "%d[%d] ", raw, rpm );
	}
	TRACE("\r\n");
}


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

			g_bLEDToggle = !g_bLEDToggle;
            LED_RED_SET( g_bLEDToggle );
		}
	}
}

