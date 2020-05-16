/*****************************************************************************
 @Project	: 
 @File 		: Timer.h
 @Details  	: STM32F103XX Timer driver               
 @Author	: 
 @Hardware	: 
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
   
******************************************************************************/

#ifndef __TIMER_DOC_H__
#define __TIMER_DOC_H__

/*****************************************************************************
 Define
******************************************************************************/

#define TIMER_OK				0
#define TIMER_ERR_INVALID 		-1
#define PA_PWM_CH1				0
#define PA_PWM_CH2				1
#define PA_PWM_CH3				2
#define PA_PWM_CH4				3


/*****************************************************************************
 Type definiton
******************************************************************************/
typedef void TIMER_CB_UPDATE( void );

typedef struct _tagTimer_Hook
{
	TIMER_CB_UPDATE 		*pfUpdate;
	struct _tagTimer_Hook 	*pNext;
}TIMER_HOOK, *PTIMER_HOOK;


typedef struct _tagTimer_Handle
{
	void 				*pTimer;
	int					Irq;
	PTIMER_HOOK			pHeadHook;
}TIMER_HANDLE, *PTIMER_HANDLE;


/******************************************************************************
 Global functions
******************************************************************************/


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
int TimerInit( PTIMER_HANDLE pHandle, uint8_t nTimer, uint32_t nFreq );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
void 
TimerAddHook(
	PTIMER_HANDLE 		pHandle,
	PTIMER_HOOK 		pHook,
	TIMER_CB_UPDATE 	*pfUpdate );


#endif /* __TIMER_DOC_H__ */


