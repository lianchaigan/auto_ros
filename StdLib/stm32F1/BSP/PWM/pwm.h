/*****************************************************************************
 @Project	: SEP150 - Capsule
 @File 		: pwm.h
 @Details  	: pwm
 @Author	: lcgan
 @Hardware	: mbed LPC1768
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release

******************************************************************************/

#ifndef __PWM_DOT_H__
#define __PWM_DOT_H__

/*****************************************************************************
 Define
******************************************************************************/

#define PWM_OK					0
#define PWM_ERR_INVALID 		-1


/*****************************************************************************
 Typedef 
******************************************************************************/
typedef struct _tagPwm_Handle
{
	void 		*pTimer;
}PWM_HANDLE, *PPWM_HANDLE;


/*****************************************************************************
 @Description 	: Intialize PWM with a full cycle of intended frequency
 
 @Param			: nFrequency - Requirend frequency in Hertz

 @Requirement	: The function does not take care Pin intialization. User shall
				  responsible to configure the pin as PWM. 
				  
 @Revision		:
******************************************************************************/
int PWMInit(
	PPWM_HANDLE	pHandle,
	uint8_t 	nTimer,
	uint32_t 	nFreq,
	BOOL		bIrqEn );


/*****************************************************************************
 @Description 	: 
 
 @Param			: 
				  
 
 @Revision		:
******************************************************************************/
void
PWMEnable(
	PPWM_HANDLE	pHandle,
	uint32_t	nChannel,
	int 		nDutyCycPercent,
	BOOL		bInverse );


/*****************************************************************************
 @Description 	: Enable PWM channel sperately. Duty cycle % must be specify
 
 @Param			: nChannel 			- PWM channel 1 to 6
				  nDutyCycPercent 	- Duty clycle %
 
 @Revision		:
******************************************************************************/
void
PWMDisable(
	PPWM_HANDLE	pHandle,
	uint32_t	nChannel );



#endif /* __PWM_DOC_H__ */




















































