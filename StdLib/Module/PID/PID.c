/*****************************************************************************
 @Project	: 
 @File 		: PID.c
 @Details  	:              
 @Author	: lcgan
 @Hardware	: Independent
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
   
******************************************************************************/
#include <Common.h>
#include "Hal.h"
#include "pwm.h"
#include "Timer.h"
#include "PID.h"


/*****************************************************************************
 Define
******************************************************************************/
//#define _PID_DEBUG_EN


/*****************************************************************************
 Type definition
******************************************************************************/


/*****************************************************************************
 Global Variables
******************************************************************************/


/*****************************************************************************
 Local Variables
******************************************************************************/


/*****************************************************************************
 Callback functions prototypes
******************************************************************************/


/*****************************************************************************
 Local functions prototypes
******************************************************************************/


/*****************************************************************************
 Implementation
******************************************************************************/
void 
PIDCtrlInit(
	PPID_HANDLE pHandle,
	PPWM_HANDLE pPwmHandle,
	uint32_t 	nChannel,
	double		dDutyCyclePercent,
	uint32_t 	nInterval,
	BOOL		bInverse
	)
{
	ASSERT( 0 != pHandle );
	ASSERT( 0 != pPwmHandle );
	ASSERT( 0 != nInterval );
	
	pHandle->pPwmHandle 		= pPwmHandle;
	pHandle->nChannel 			= nChannel;
	pHandle->dDutyCyclePercent 	= dDutyCyclePercent;
	pHandle->dPWM 				= dDutyCyclePercent;
    pHandle->dStopRate			= 0.9; /* Decrease 30% by default */
	pHandle->dKp				= 0.0;
	pHandle->dKi				= 0.0;
	pHandle->dKd				= 0.0;
    pHandle->bInverse			= bInverse;
}


void
PIDCtrlSetParam(
	PPID_HANDLE pHandle,
	double 		Kp,
	double 		Ki,
	double 		Kd
	)
{
	ASSERT( 0 != pHandle );
	
	pHandle->dKp = Kp;
	pHandle->dKi = Ki;
	pHandle->dKd = Kd;
}


void PIDRestart( PPID_HANDLE pHandle, BOOL bInverse )
{
	ASSERT( 0 != pHandle );
	
	pHandle->dPrevErrP1 	= 0.0;
	pHandle->dPrevErrP2 	= 0.0;
	pHandle->nPrevOutput	= 0;
	pHandle->dPWM 			= pHandle->dDutyCyclePercent;
	pHandle->bInverse		= bInverse;
}


void
PIDCtrlSetTargetValue(
	PPID_HANDLE pHandle,
	int32_t 	nValue
	)
{
	ASSERT( 0 != pHandle );
	
	pHandle->nTargetValue = nValue;
}


void
PIDCtrlSetDecreaseRateAtSpeed0(
	PPID_HANDLE pHandle,
	int			nDecPercent
	)
{
	ASSERT( 0 != pHandle );

	pHandle->dStopRate = (double)(100 - nDecPercent) / 100.0;
}


void
PIDCtrlOutputConfig(
	PPID_HANDLE pHandle,
	int32_t 	nMin,
	int32_t		nMax,
	double		dFactor
	)
{
	ASSERT( 0 != pHandle );
	
	pHandle->nMin = nMin;
	pHandle->nMax = nMax;
	pHandle->dFactor = dFactor;
}


PID_STS
PIDOnTimer(
	PPID_HANDLE pHandle,
	int			nFeedbackValue
	)
{
	double output;
	PID_STS res = PID_STS_OK;

	ASSERT( 0 != pHandle );
	
	/* Kp cannot be 0, otherwise PID will not work */
	if( 0.0 == pHandle->dKp )
	{
		return PID_ERR_INVALID_KP;
	}
	
	/* NOTE:
	 If target value is 0, PID will take control to slow down the system before 
	 stop but this might be too slow.Here we implement a linear brake system such
	 that every interval we reduce to a specified percentage. 
	 This percentage can be decided base on mechanical behaviour */
	if( 0 == pHandle->nTargetValue )
	{
		if( 0 != ((int32_t)(pHandle->dPWM*100)) )
		{
			pHandle->dPWM = pHandle->dPWM*pHandle->dStopRate;

			if( 0 == ((int32_t)(pHandle->dPWM*100)) )
			{
				PWMDisable( pHandle->pPwmHandle, pHandle->nChannel );
				//LED_BLUE_ON();
			}
			else
			{
				PWMEnable(
					pHandle->pPwmHandle,
					pHandle->nChannel,
					pHandle->dPWM,
					pHandle->bInverse );
			}
		}

		return PID_STS_OK;
	}

	/* Refer to Embedded Robotics, Thomas Braunl, page 93 */
	/* Calculate proportional error value */
	pHandle->dErrP = pHandle->nTargetValue - nFeedbackValue;
	
	output = pHandle->nPrevOutput 
	+ (pHandle->dKp * (pHandle->dErrP - pHandle->dPrevErrP1)) 
	+ (pHandle->dKi * ((pHandle->dErrP + pHandle->dPrevErrP1) / 2.0)) /* Calculate Integral error */
	+ (pHandle->dKd * (pHandle->dErrP - (2.0*pHandle->dPrevErrP1) + pHandle->dPrevErrP2)); /* Calculate differential error */

	/* NOTE:
		dFactor is the constant that encoder count relates to PWM
	*/

	/* WARNING: nOutput no longer in encoder count after multiply by factor */
    output= output*pHandle->dFactor;


	output = MIN( output, pHandle->nMax );  /* PWM only can up to 100 */
	output = MAX( output, pHandle->nMin );

	/* WARNING: nPrevOutput need to be in encoder count, so we convert back */
	pHandle->nPrevOutput = output/pHandle->dFactor;
	pHandle->dPrevErrP2 = pHandle->dPrevErrP1;
	pHandle->dPrevErrP1 = pHandle->dErrP;

	/* compensate it */
	pHandle->dPWM = output;
	
	//TRACE( "%d\r\n", pHandle->nPWM );
	PWMEnable(
		pHandle->pPwmHandle,
		pHandle->nChannel,
		pHandle->dPWM,
		pHandle->bInverse );
		
#ifdef _PID_DEBUG_EN
	TRACE(
		"[ch%d] %d %f %f %f %d\r\n",
		pHandle->nChannel,
		nFeedbackValue,
		pHandle->dErrP,
		pHandle->dErrI,
		pHandle->dErrD,
		pHandle->nOutput );
#endif

	return res;
}
















