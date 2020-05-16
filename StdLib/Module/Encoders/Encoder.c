/*****************************************************************************
 @Project	: 
 @File 		: main.c
 @Details  	: Main entry               
 @Author	: 
 @Hardware	: 
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  lcgan     5 Nov 2016  		Initial Release
   2.0  lcgan	  19 Jul 2018		Update to support CCM. ShaftEncoderLinkCCM()
   
******************************************************************************/
#include <Common.h>
#include "Hal.h"
#include "Timer.h"
#include "Encoder.h"

#ifdef _CCM_MODE
	#include "Ccm.h"
#endif

/*****************************************************************************
 Define
******************************************************************************/
#define MAX_COUNT       0xffffU      /* 16 bit timer */
#define ROLLOVER_MID_COUNT  (0xffffU>>1)

/*****************************************************************************
 Type definition
******************************************************************************/


/*****************************************************************************
 Global Variables
******************************************************************************/


/*****************************************************************************
 Local Variables
******************************************************************************/
static TIMER_HOOK 			g_TimerHook;
static TIMER_HANDLE			*g_pTimerHandle = 0;
static int					g_TimerHz;
static volatile int 		g_nInterval 		= 0;
static int					g_nIntervalConst	= 0;
static ENC_CB_READY			*g_pfcbEncReady		= 0;
static double				g_RpmConst = 0.0;

static PSHAFT_ENC_HANDLE	g_pHeadHandle = 0;

static int					g_CPR = 0;
static int					g_GearRatio = 0;


/*****************************************************************************
 Callback functions prototypes
******************************************************************************/
static void enc_cbOnTimer( void );


/*****************************************************************************
 Local functions prototypes
******************************************************************************/
static int enc_CheckRolloverGetCount( PSHAFT_ENC_HANDLE pEnc );


/*****************************************************************************
 Implementation
******************************************************************************/
void
ShaftEncoderInit(
	PTIMER_HANDLE 	pTimerHandle,
	uint32_t 		nTimerHz,
	ENC_CB_READY 	*pfcbEncReady,
	uint32_t		nIntervalMs
	)
{
	ASSERT( 0 != pTimerHandle );

	g_pfcbEncReady = pfcbEncReady;
    g_pTimerHandle = pTimerHandle;
    g_TimerHz = nTimerHz;
	
    g_nIntervalConst = (int) (((double)nIntervalMs) / (double)(1000.0/(double)nTimerHz));
	g_nInterval = g_nIntervalConst;

	/* Add a timer callback for interval counting */
	TimerAddHook( pTimerHandle, &g_TimerHook, enc_cbOnTimer );
}


void
ShaftEncoderCfg(
    uint32_t 		nCPR,
	uint32_t 		nGearRatio,
	uint32_t		nIntervalMs
	)
{
 /* Output RPM = ((Pulses Received in 1 sec * 60) / PPR) / Gear Ratio
  example PPR = CPR = 64 Gear Ratio is 70:1 */
	g_CPR = nCPR;
	g_GearRatio = nGearRatio;
	g_RpmConst = (60000.0 / (double)nIntervalMs) / (double)(nCPR * nGearRatio);
}


void ShaftEncoderChangeInterval( uint32_t nIntervalMs )
{
 /* Output RPM = ((Pulses Received in 1 sec * 60) / PPR) / Gear Ratio
  PPR = 2*CPR = 2*48 (Pololu motor is 48CPR) x2 because using both encoder A&B
  Gear Ratio is 47:1 */
	
    TimerStop( g_pTimerHandle );

	g_RpmConst = (60000.0 / (double)nIntervalMs) / (double)(g_CPR * g_GearRatio);
    g_nIntervalConst = (int) (((double)nIntervalMs) / (double)(1000.0/(double)g_TimerHz));
	g_nInterval = g_nIntervalConst;

    TimerStart( g_pTimerHandle );
}


BOOL
ShaftEncoderAdd(
	PSHAFT_ENC_HANDLE	pHandle,
	int					nIndex,
	ENC_CB_THRES		*pfcbEncThresHit
	)
{
	ASSERT( 0 != pHandle );

	/* Reset variable to default 0 */
	memset( pHandle, 0, sizeof(SHAFT_ENC_HANDLE) );
	
	pHandle->nIndex = nIndex;
    pHandle->pCcm = 0;
	pHandle->pfThresHit = pfcbEncThresHit;

	/* Insert into link list */
	pHandle->pNext = g_pHeadHandle;
	g_pHeadHandle = pHandle;
	
	return TRUE;
}


void ShaftEncoderReset( PSHAFT_ENC_HANDLE pHandle )
{
	ASSERT( 0 != pHandle );

	pHandle->nFinalCnt = 0;
    pHandle->nTmpCnt = 0;
}


BOOL ShaftEncoderLinkCCM( PSHAFT_ENC_HANDLE pHandle, void *pCcmHandle )
{
	ASSERT( 0 != pHandle );
	ASSERT( 0 != pCcmHandle );

	pHandle->pCcm = pCcmHandle;

	return TRUE;
}


void ShaftEncoderSetThresCount( PSHAFT_ENC_HANDLE pHandle,int nCount )
{
	ASSERT( 0 != pHandle );
	
	pHandle->nThresCnt = nCount;
}


int ShaftEncoderReadCount( PSHAFT_ENC_HANDLE pHandle )
{
    /* If user set nInterval = 0, meant that timer will not run 
    and no callback to user as well. so we just read the count */

    if( 0 == pHandle->nInterval )
    {
        if( 0 == pHandle->pCcm )
        {
            pHandle->nFinalCnt = enc_CheckRolloverGetCount( pHandle );
        }
        else
        {
            pHandle->nTmpCnt = CCMReadCount( pHandle->pCcm );
            pHandle->nFinalCnt = enc_CheckRolloverGetCount( pHandle );
        }

        pHandle->nPrevCnt = pHandle->nTmpCnt;
    }

	return pHandle->nFinalCnt;
}


int ShaftEncoderReadRPM( PSHAFT_ENC_HANDLE pHandle )
{
	int rpm = (int)(pHandle->nFinalCnt*g_RpmConst);
	return rpm;
}


int ShaftEncoderCountToRpm( int nCount )
{
	int rpm = (int)(nCount*g_RpmConst);
	return rpm;
}


int ShaftEncoderRpmToCount( int nRpm )
{
	int count = (int)((double)nRpm/g_RpmConst);
	return count;
}


void ShaftEncoderISR( PSHAFT_ENC_HANDLE pHandle, BOOL bInputEncBSts )
{
	/* NOTE: 
	This Function is to support interrupt driven method.
	 This is not required for Compare Capture Mode.
	*/
	if( 0 == pHandle->pCcm )
	{
		(0 != bInputEncBSts)? pHandle->nTmpCnt++ : pHandle->nTmpCnt--;

		if( 0 != pHandle->nThresCnt )
		{
			pHandle->nThresCnt--;
			if( 0 == pHandle->nThresCnt  )
			{
				if( 0 != pHandle->pfThresHit )
				{
					pHandle->pfThresHit( 0 );
				}
			}
		}
	}
}


/*****************************************************************************
 Callback functions 
******************************************************************************/
static void enc_cbOnTimer( void )
{
    PSHAFT_ENC_HANDLE cur;

	/* NOTE: 
    In order to support timer sharing, Encoder timer will has its own prescaler to 
    Divide any timer frequency to desired period.
    */
	if( 0 != g_nInterval )
	{
		g_nInterval--;
		
		if( 0 == g_nInterval )
		{
			/* NOTE: User nIntervalMs Expired  */
			g_nInterval = g_nIntervalConst;	/* Reload interval for next use */

			if( 0 == g_pHeadHandle )
			{
				/* No encoders added */
				return;
			}
			
			for( cur=g_pHeadHandle; cur!=0; cur=cur->pNext )
			{
#ifdef _CCM_MODE
				if( 0 != cur->pCcm )
				{
					/* Get Count from Timer in CCM */
					cur->nTmpCnt = CCMReadCount( cur->pCcm );
				}
#endif
                /* Handle rollover to get final increment count */
                cur->nFinalCnt = enc_CheckRolloverGetCount( cur );
			}

			/* Callback to application every interval */
			if( 0 != g_pfcbEncReady )
			{
				g_pfcbEncReady();
			}
		}
	}
}


/*****************************************************************************
 Local functions 
******************************************************************************/
static int enc_CheckRolloverGetCount( PSHAFT_ENC_HANDLE pEnc )
{
    int count;

    ASSERT( 0 != pEnc );

    /* Handle rollover to get final increment count */
    if( abs(pEnc->nTmpCnt - pEnc->nPrevCnt) > ROLLOVER_MID_COUNT )
    {
        if( pEnc->nTmpCnt > pEnc->nPrevCnt )
        {
            count = -(MAX_COUNT - pEnc->nTmpCnt + pEnc->nPrevCnt);
        }
        else
        {
            count = (MAX_COUNT - pEnc->nTmpCnt + pEnc->nPrevCnt);
        }
    }

    pEnc->nPrevCnt = pEnc->nTmpCnt;
    
    return count;
}

/*****************************************************************************
 Interrupt
******************************************************************************/








