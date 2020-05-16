/*****************************************************************************
 @Project	: 
 @File 		: PID.h
 @Details  	:              
 @Author	: lcgan
 @Hardware	: 
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  lcgan     Oct 2016  		Initial Release
   2.0  lcgan     27 Jul 2018		Update PWM enable to allow duty cycle in double
									to increase resolution
   
******************************************************************************/


#ifndef __PID_DOT_H__
#define __PID_DOT_H__


/*****************************************************************************
 Define
******************************************************************************/


/*****************************************************************************
 Type definition
******************************************************************************/
typedef enum e_tagPID_STS
{
	PID_STS_HIT_MAX		= 2,
	PID_STS_HIT_MIN		= 1,
	PID_STS_OK			= 0,
	PID_ERR_INVALID_KP = -1
}PID_STS;


typedef struct _tagPID_Handle
{
	PPWM_HANDLE pPwmHandle;
	uint8_t 	nChannel;
	double		dDutyCyclePercent;
	
	int32_t 	nTargetValue;
	uint32_t 	nInterval;
	double		dStopRate;
	
	/* Constant for P, I, D */
	double 		dKp;
	double 		dKi;
	double 		dKd;
	
	/* Error for P, I, D */
	double 		dErrP;
	double 		dPrevErrP1;
	double 		dPrevErrP2;
	double		nPrevOutput;
	int32_t		nMin;
	int32_t		nMax;
	double		dFactor;
	double		dPWM;

    uint8_t		bInverse :1;
	uint8_t		Resvd :7;
}PID_HANDLE, *PPID_HANDLE;


/*****************************************************************************
 Macro
******************************************************************************/


/******************************************************************************
 Global functions
******************************************************************************/


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
void 
PIDCtrlInit(
	PPID_HANDLE pHandle,
	PPWM_HANDLE pPwmHandle,
	uint32_t 	nChannel,
	double		dDutyCyclePercent,
	uint32_t 	nInterval,
	BOOL		bInverse );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
void PIDRestart( PPID_HANDLE pHandle, BOOL bInverse );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
void PIDCtrlSetTargetValue( PPID_HANDLE pHandle, int32_t nValue );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
void PIDCtrlOutputConfig(
			PPID_HANDLE pHandle,
			int32_t 	nMin,
			int32_t		nMax,
			double		dFactor );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
PID_STS PIDOnTimer( PPID_HANDLE pHandle, int nFeedbackValue );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
void PIDCtrlSetParam(
		PPID_HANDLE pHandle,
		double 		Kp,
		double 		Ki,
		double 		Kd );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
void PIDCtrlSetDecreaseRateAtSpeed0( PPID_HANDLE pHandle, int nDecPercent );

#endif /* __PID_DOT_H__ */















