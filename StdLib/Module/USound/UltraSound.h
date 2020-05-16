/*****************************************************************************
 @Project	: 
 @File 		: UltraSound.h
 @Details  	: Ultrasonic driver               
 @Author	: 
 @Hardware	: 
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
   
******************************************************************************/

#ifndef __ULTRASOUND_DOT_H__
#define __ULTRASOUND_DOT_H__


/*****************************************************************************
 Define - return codes
******************************************************************************/
#define USONIC_SUCCESS				0
#define USONIC_FAILED				-1
#define USONIC_INVALID				-2
#define USONIC_INUSE				-3
#define USONIC_TIMER_FREQ_INVALID	-4


/*****************************************************************************
 Type definition
******************************************************************************/
typedef void USONIC_CBI_READY( void );
typedef void USONIC_CB_ON_TRIGGER( BOOL bON );

typedef struct _tagUSONIC_HANDLE
{
	TIMER_HOOK				TimerHook;
	volatile uint32_t 		nTimerTick;
	volatile uint32_t		nTrigger;
	volatile int 			nTimeout;
	volatile int 			nTimeoutConst;
	volatile uint32_t		nDistance;
	volatile uint32_t		nDistanceMax;
	volatile BOOL 			bValidEdge;
	
	USONIC_CB_ON_TRIGGER 	*pfOnTrigger;
	USONIC_CBI_READY 		*pfOnReady;
}USONIC_HANDLE, *PUSONIC_HANDLE;


/*****************************************************************************
 Global Functions
******************************************************************************/


/******************************************************************************
 @Description 	: Intalizes Ultrasonic module with specified Timer

 @param			: pHandle - To Intiialized Timer pointer
				  nTimerFreq - Timer frequency. Must > 10KHz. Suggested 20KHz
 @return		: None

 @revision		: 1.0.0
 
******************************************************************************/
int USonicInit( PTIMER_HANDLE 	pHandle, int nTimerFreq );


/******************************************************************************
 @Description 	: Add ultrasonic sensors

 @param			: pHandle - To an empty ultrasonic pointer
				  nIndex - Ultrasonic number. Shall be unique
				  pfOnTrigger - callback trigger function pointer
				  pfOnReady - callback data ready function pointer
 
 @return		: codes. Refer to define portion
 @revision		: 1.0.0
 
******************************************************************************/
int
UsonicAddDevice(
	PUSONIC_HANDLE 			pHandle,
	uint8_t 				nIndex,
	USONIC_CB_ON_TRIGGER	*pfOnTrigger,
	USONIC_CBI_READY 		*pfOnReady );


/******************************************************************************
 @Description 	: Trigger ultrasonic to generate a sound pulse

 @param			: pHandle - To an initialized ultrasonic pointer
 
 @return		: None

 @revision		: 1.0.0
 
******************************************************************************/
void USonicTrigger( PUSONIC_HANDLE 	pHandle );


/******************************************************************************
 @Description 	: Set the maximum timeout if echo back signal not detected
				  after triggering

 @param			: pHandle - To an initialized ultrasonic pointer
				  nTimeoutMs - timeout in (ms)
				  nDistMax - limit Maximum distance in (cm) x100
 @return		: None

 @revision		: 1.0.0
 
******************************************************************************/
void USonicSetEchoTimeOut( PUSONIC_HANDLE pHandle, int nTimeoutMs, int nDistMax );


/******************************************************************************
 @Description 	: Read current distance in (cm) x100

 @param			: pHandle - To an initialized ultrasonic pointer

 @return		: distance in (cm) x100
 
 @revision		: 1.0.0
 
******************************************************************************/
uint32_t USonicRead( PUSONIC_HANDLE pHandle );


/******************************************************************************
 @Description 	: Ultrasonic echo signal ISR service during echo 
 				  rising and falling edge

 @param			: pHandle - To an initialized ultrasonic pointer
				: bEcho	  - Current echo input state

 @return		: None

 @revision		: 1.0.0
 
******************************************************************************/
void UsonicISR( PUSONIC_HANDLE pHandle, BOOL bEcho );


#endif /* __ULTRASOUND_DOT_H__*/







