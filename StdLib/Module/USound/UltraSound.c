/*****************************************************************************
 @Project	: 
 @File 		: UltraSound.c
 @Details  	: Ultrasonic driver               
 @Author	: 
 @Hardware	: 
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
   
******************************************************************************/

#include <Common.h>
#include "Hal.h"
#include "Timer.h"
#include "UltraSound.h"


/*****************************************************************************
 Define
******************************************************************************/
#define US_ECHO_TIMEOUT			18U	/* ms, for 300cm->17.46ms, */
#define TOTAL_USONIC			8U
#define US_DEFAULT_DIST_MAX		20000U

#define US_TIMER_FREQ_MIN		10000U	/* 10KHz */


/*****************************************************************************
 Type definition
******************************************************************************/


/*****************************************************************************
 Global Variables
******************************************************************************/


/*****************************************************************************
 Local Variables
******************************************************************************/

static TIMER_HOOK			g_TimerHook;
static PUSONIC_HANDLE		g_pusonicIrqHandles[TOTAL_USONIC]; /* these handle pointers are used in ISR */
static double				g_dTimerUsConst = 0;	/* Timer period in us */
static int					g_nTriggerConst = 0;	/* trigger timeout at least 10us */


/*****************************************************************************
 Local functions
******************************************************************************/
static void sonic_cbOnTimer( void );


/*****************************************************************************
 Implementation
******************************************************************************/
int
USonicInit(
	PTIMER_HANDLE 	pTimer,
	int				nTimerFreq
	)
{
	int cnt;
	
	ASSERT( 0 != pTimer );
	ASSERT( 0 != nTimerFreq );

	/* Timer run too slow. insufficient to use */
	if( nTimerFreq < US_TIMER_FREQ_MIN )
	{
		/* Return error code */
		return USONIC_TIMER_FREQ_INVALID;
	}
	
	/* Reset handles to 0 */
	for( cnt=0; cnt<TOTAL_USONIC; cnt++ )
	{
		g_pusonicIrqHandles[cnt] = 0;
	}

	g_dTimerUsConst = 1000000.0 / nTimerFreq; /* timer period in us. Time per tick */

	if( g_dTimerUsConst > 10.0 )
	{
		/* It is fine for trigger time more than 10us that required */
		g_nTriggerConst = 1;
	}
	else
	{
		/* If timer run faster to 10us, we need to scale back using incremental delay */
		g_nTriggerConst = 10 / g_dTimerUsConst;
	}
	
	/* Add a timer callback */
	TimerAddHook(
		pTimer,
		&g_TimerHook,
		sonic_cbOnTimer );
}


int 
UsonicAddDevice(
	PUSONIC_HANDLE 			pHandle,
	uint8_t 				nIndex,
	USONIC_CB_ON_TRIGGER	*pfOnTrigger,
	USONIC_CBI_READY 		*pfOnReady
	)
{
	ASSERT( 0 != pHandle );
	ASSERT( 0 != pfOnTrigger );
	ASSERT( 0 != pfOnReady );
	
	/* Check if sensor index is valid or within range */
	if( nIndex > TOTAL_USONIC )
	{
		/* not in range. Return error code */
		return USONIC_INVALID;
	}
	
	if( 0 != g_pusonicIrqHandles[nIndex] )
	{
		/* handles has been initialized. Overwrite is not allowed */
		return USONIC_INUSE;
	}
	
	/* Set handle variables  to default */
	pHandle->nDistance 		= 0;
	pHandle->nTimeout 		= 0;
	pHandle->nTimeoutConst	= (int)((double)(US_ECHO_TIMEOUT*1000) / g_dTimerUsConst);
	pHandle->nTimerTick 	= 0;
	pHandle->nTrigger 		= 0;
	pHandle->bValidEdge		= FALSE;
	pHandle->nDistanceMax	= US_DEFAULT_DIST_MAX;
	pHandle->pfOnTrigger 	= pfOnTrigger;
	pHandle->pfOnReady 		= pfOnReady;
	
	g_pusonicIrqHandles[nIndex] = pHandle;
	
	return USONIC_SUCCESS;
}


void USonicTrigger( PUSONIC_HANDLE 	pHandle )
{
	ASSERT( 0 != pHandle );
	
	pHandle->nTimeout = 0;	/* Timeout only will be set after triggering completed */
	pHandle->nTrigger = g_nTriggerConst;  /* Reload trigger rising time */
	pHandle->bValidEdge = FALSE; /* reset edge detection for a new start */
	
	pHandle->pfOnTrigger( TRUE ); /* Callback to application to set trigger pin high */
}


void
USonicSetEchoTimeOut(
	PUSONIC_HANDLE	pHandle,
	int				nTimeoutMs,
	int				nDistMax
	)
{
	ASSERT( 0 != pHandle );
	
	/* Copy timeout to handle for the next use */
	pHandle->nTimeoutConst = (int)((double)(nTimeoutMs*1000) / g_dTimerUsConst);

	/* no changes if 0 */
	if( 0 != nDistMax )
	{
		pHandle->nDistanceMax = nDistMax;
	}
}


uint32_t USonicRead( PUSONIC_HANDLE pHandle )
{
	ASSERT( 0 != pHandle );

	return pHandle->nDistance;
}


void UsonicISR( PUSONIC_HANDLE 	pHandle, BOOL bEcho )
{
	uint32_t tmp;
	
	ASSERT( 0 != pHandle );

	if( 0 != bEcho )
	{
		pHandle->nTimerTick = 0;
		pHandle->bValidEdge = TRUE;
		return;
	}
	
	if( FALSE != pHandle->bValidEdge )
	{
		pHandle->bValidEdge = FALSE;
			
		/* If counter is 0, use previous value. This is sensor error */
		if( 0 != pHandle->nTimerTick )
		{
			/* Distance conversion with time per tick in us */
			tmp = (uint32_t)((((double)pHandle->nTimerTick * g_dTimerUsConst) / 58.0)*100);
				
			/* If more than 2cm. Sensor only able measure at least 4cm */
			if( tmp>200 )
			{
				pHandle->nDistance = tmp;
				pHandle->nTimeout = 0;
				pHandle->pfOnReady();
			}
		}
	}
}


/*****************************************************************************
 Callback functions
******************************************************************************/
static void sonic_cbOnTimer( void )
{
	int				cnt;
    PUSONIC_HANDLE	sensor;
	
	/* Iterates all available sensors */
	for( cnt=0; cnt<TOTAL_USONIC; cnt++ )
	{
		sensor = g_pusonicIrqHandles[cnt];

		if( 0 == sensor )
		{
			continue;	/* We also can break. If index is continuous */
		}
		
		/* Free running timer to increase software counters */
		sensor->nTimerTick++;
		
		/* Check if trigger is need to pull low */
		if( 0 != sensor->nTrigger )
		{
			sensor->nTrigger--;
			if( 0 == sensor->nTrigger )
			{
				/* Trigger time reached. Pull output to low */
				sensor->pfOnTrigger( FALSE );
				
				/* Activate timeout detection in case echo missing or too long */
				sensor->nTimeout = sensor->nTimeoutConst;
			}
		}
		
		/* Check any timeout in case distance too far.
		 Fix Ultrasonic module limitation */
		if( 0 != sensor->nTimeout )
		{
			sensor->nTimeout--;
			if( 0 == sensor->nTimeout )
			{
				sensor->nDistance = sensor->nDistanceMax;
			
				sensor->nTimerTick = 0;
				sensor->bValidEdge = FALSE;
			
				sensor->nTrigger = 1;
				sensor->bValidEdge = FALSE;
				sensor->pfOnTrigger( TRUE );
				
				sensor->pfOnReady();
			}
		}
	}
}


/*****************************************************************************
 Local functions
******************************************************************************/


/*****************************************************************************
 Interrupt functions
******************************************************************************/





















































