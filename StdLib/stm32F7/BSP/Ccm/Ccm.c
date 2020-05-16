/*****************************************************************************
 @Project	: SEP150 - Capsule
 @File 		: ccm.c
 @Details  	: ccm
 @Author	: lcgan
 @Hardware	: stm32
 
 --------------------------------------------------------------------------
 @Revision	: 
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
******************************************************************************/

#include "Common.h"
#include "Hal.h"
#include "Ccm.h"



/******************************************************************************
 Define
******************************************************************************/
#define SMS_ENC_DISABLE				0x00U
#define SMS_ENC_MODE1_INPUTA		0x01U
#define SMS_ENC_MODE2_INPUTB		0x02U
#define SMS_ENC_MODE3_INPUTAB		0x03U
#define SMS_START_TRIG_MODE			0x06U
#define SMS_EXT_CLCK_MODE1			0x07U
#define SMS_START_RST_TRIG_MODE		0x80U

#define TS_TRIG_ITR0				0x00U
#define TS_TRIG_ITR1				0x01U
#define TS_TRIG_ITR2				0x02U
#define TS_TRIG_ITR3				0x03U
#define TS_TRIG_EDGE_A				0x04U
#define TS_FILTERED_TIM_IN1			0x05U
#define TS_FILTERED_TIM_IN2			0x06U
#define TS_EXTERNAL_TRIG_IN			0x07U


#define CCXS_OUTPUT					0x00U
#define CCXS_INPUT_B				0x01U
#define CCXS_INPUT_A				0x02U
#define CCXS_INPUT_TRC				0x03U


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
 Local functions
******************************************************************************/


/*****************************************************************************
 Implementation
******************************************************************************/

int
CCMInit(
	PCCM_HANDLE 	pHandle,
	uint8_t 		nTimer
	)
{
	TIM_TypeDef *timer;
	uint32_t 	clock		= 0U;
	
	
	switch( nTimer )
	{
		case 1U:
			timer = TIM1;
		break;
		
		case 2U:
			timer = TIM2;
		break;
		
		case 3U:
			timer = TIM3;
		break;
		
		case 4U:
			timer = TIM4;
		break;

		case 5U:
			timer = TIM5;
		break;

		case 6U:
			timer = TIM6;
		break;

		case 7U:
			timer = TIM7;
		break;

		case 8U:
			timer = TIM8;
		break;

		case 9U:
			timer = TIM9;
		break;	
			
		default:
			return CCM_ERR_TIM_INVALID;
	}

	if( (0 != (timer->CR1&TIM_CR1_CEN)) || (0 != (timer->CR1&TIM_CR1_ARPE)) )
	{
		/* Timer has been used for others. overlap is not possible */
		return CCM_ERR_TIM_USED;
	}
	
	pHandle->pTimer = timer;
	timer->CR1 &= ~(TIM_CR1_DIR 
					| TIM_CR1_OPM 
					| TIM_CR1_CKD 
					| TIM_CR1_CMS); /* Count up */
	
	switch( RCC->CFGR & RCC_CFGR_PPRE1 )
	{
		case RCC_CFGR_PPRE1_DIV1:
			clock = SystemCoreClock;
		break;
		
		case RCC_CFGR_PPRE1_DIV2:
			clock = SystemCoreClock>>1U;
		break;
		
		case RCC_CFGR_PPRE1_DIV4:
			clock = SystemCoreClock>>2U;
		break;
		
		case RCC_CFGR_PPRE1_DIV8:
			clock = SystemCoreClock>>3U;
		break;
		
		case RCC_CFGR_PPRE1_DIV16:
			clock = SystemCoreClock>>4U;
		break;
	}
	
    pHandle->nClock = clock;

	return CCM_OK;
}

int CCMEncoderMode( PCCM_HANDLE pHandle, CCM_ENC_INPUT Input )
{
	TIM_TypeDef *timer = (TIM_TypeDef *)pHandle->pTimer;
	
	ASSERT( 0 != pHandle );

	switch( Input )
	{
    	case CCM_ENC_INPUT_A:
			timer->SMCR &= ~(TIM_SMCR_SMS | TIM_SMCR_TS);

			/* Select the Capture Compare 1 and the Capture Compare 2 as input  */
			timer->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_CC2S);
			timer->CCMR1 |= (CCXS_INPUT_A<<TIM_CCMR1_CC1S_Pos);

			/* Set the Capture Compare 1 and the Capture Compare 2 prescalers and filters */
			timer->CCMR1 &= ~(TIM_CCMR1_IC1PSC | TIM_CCMR1_IC2PSC);
			timer->CCMR1 &= ~(TIM_CCMR1_IC1F | TIM_CCMR1_IC2F);

			timer->SMCR |= (SMS_ENC_MODE1_INPUTA<<TIM_SMCR_SMS_Pos);  /* TIM_SMCR_ECE Counting both edge */

			timer->CR1 |= TIM_CR1_CEN;
		break;

		case CCM_ENC_INPUT_B:
			timer->SMCR &= ~(TIM_SMCR_SMS | TIM_SMCR_TS);

			/* Select the Capture Compare 1 and the Capture Compare 2 as input  */
			timer->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_CC2S);
			timer->CCMR1 |= (CCXS_INPUT_B<<TIM_CCMR1_CC2S_Pos);

			/* Set the Capture Compare 1 and the Capture Compare 2 prescalers and filters */
			timer->CCMR1 &= ~(TIM_CCMR1_IC1PSC | TIM_CCMR1_IC2PSC);
			timer->CCMR1 &= ~(TIM_CCMR1_IC1F | TIM_CCMR1_IC2F);

			timer->SMCR |= (SMS_ENC_MODE2_INPUTB<<TIM_SMCR_SMS_Pos);  /* TIM_SMCR_ECE Counting both edge */

			timer->CR1 |= TIM_CR1_CEN;
		break;

		case CCM_ENC_INPUT_AB:
			timer->SMCR &= ~(TIM_SMCR_SMS | TIM_SMCR_TS);

			/* Select the Capture Compare 1 and the Capture Compare 2 as input  */
			timer->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_CC2S);
			timer->CCMR1 |= (CCXS_INPUT_A<<TIM_CCMR1_CC1S_Pos) | (CCXS_INPUT_B<<TIM_CCMR1_CC2S_Pos);

			/* Set the Capture Compare 1 and the Capture Compare 2 prescalers and filters */
			timer->CCMR1 &= ~(TIM_CCMR1_IC1PSC | TIM_CCMR1_IC2PSC);
			timer->CCMR1 &= ~(TIM_CCMR1_IC1F | TIM_CCMR1_IC2F);

			timer->SMCR |= (SMS_ENC_MODE3_INPUTAB<<TIM_SMCR_SMS_Pos);  /* TIM_SMCR_ECE Counting both edge */

			timer->CR1 |= TIM_CR1_CEN;
		break;

		default:
			return CCM_ERR_INPUT;
	}

	return CCM_OK;
}


int CCMInputMode( PCCM_HANDLE pHandle )
{
	TIM_TypeDef *timer = (TIM_TypeDef *)pHandle->pTimer;
	
	ASSERT( 0 != pHandle );

	timer->SMCR &= ~(TIM_SMCR_SMS | TIM_SMCR_TS);

	timer->SMCR |= (SMS_EXT_CLCK_MODE1<<TIM_SMCR_SMS_Pos)
					| (TS_FILTERED_TIM_IN1<<TIM_SMCR_TS_Pos);

	/* Select the Capture Compare 1 and the Capture Compare 2 as input  */
	timer->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_CC2S);
	timer->CCMR1 |= (CCXS_INPUT_A<<TIM_CCMR1_CC1S_Pos);

	timer->CR1 |= TIM_CR1_CEN;

    return CCM_OK;
}


void CCMSetThresholdCount( PCCM_HANDLE pHandle, int nCountHit )
{
	TIM_TypeDef *timer = (TIM_TypeDef *)pHandle->pTimer;
	
	ASSERT( 0 != pHandle );
	
    timer->CR1 &= ~TIM_CR1_CEN;
	timer->ARR = nCountHit;
    timer->CNT = 0;
	timer->CR1 |= TIM_CR1_CEN;
}



int16_t CCMReadCount( PCCM_HANDLE pHandle )
{
	int16_t count;
	TIM_TypeDef *timer = (TIM_TypeDef *)pHandle->pTimer;
	
	ASSERT( 0 != pHandle );
	
    timer->CR1 &= ~TIM_CR1_CEN;
	count = timer->CNT;
	timer->CR1 |= TIM_CR1_CEN;

	return count;
}


void CCMResetCount( PCCM_HANDLE pHandle )
{
	TIM_TypeDef *timer = (TIM_TypeDef *)pHandle->pTimer;
	
	ASSERT( 0 != pHandle );
	
    timer->CR1 &= ~TIM_CR1_CEN;
	timer->CNT = 0;
	timer->CR1 |= TIM_CR1_CEN;
}


int16_t CCMReadCountAutoReset( PCCM_HANDLE pHandle )
{
	int16_t count;
	TIM_TypeDef *timer = (TIM_TypeDef *)pHandle->pTimer;
	
	ASSERT( 0 != pHandle );
	
    timer->CR1 &= ~TIM_CR1_CEN;
	count = timer->CNT;
    timer->CNT = 0;
	timer->CR1 |= TIM_CR1_CEN;

	return count;
}


void
CCMEnable(
	PCCM_HANDLE	pHandle,
	uint32_t	nChannel
	)
{
	TIM_TypeDef *timer = (TIM_TypeDef *)pHandle->pTimer;
	
	ASSERT( 0 != pHandle );
	UNUSE( nChannel );

	timer->CR1 &= ~TIM_CR1_CEN;
	timer->CNT = 0;
    timer->CR1 |= TIM_CR1_CEN;
}


void
CCMDisable(
	PCCM_HANDLE	pHandle,
	uint32_t	nChannel
	)
{
	TIM_TypeDef *timer = (TIM_TypeDef *)pHandle->pTimer;

	ASSERT( 0 != pHandle );
	UNUSE( nChannel );

	timer->CR1 &= ~TIM_CR1_CEN;
	timer->CNT = 0;
}






