/*****************************************************************************
 @Project	: SEP150 - Capsule
 @File 		: pwm.c
 @Details  	: pwm
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
#include "pwm.h"



/******************************************************************************
 Define
******************************************************************************/


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
PWMInit( PPWM_HANDLE pHandle, uint8_t nTimer, uint32_t nFreq, BOOL bIrqEn )
{
	TIM_TypeDef *timer;
	IRQn_Type 	irq;
	uint32_t 	clock;
	uint32_t 	count 		= 0U;
	uint32_t 	prescale 	= 0U;
	
	ASSERT( 0 != pHandle );
	ASSERT( 0 != nFreq );
	
	switch( nTimer )
	{
		case 1U:
			timer = TIM1;
			irq = TIM1_UP_IRQn;
		break;
		
		case 2U:
			timer = TIM2;
			irq = TIM2_IRQn;
		break;
		
		case 3U:
			timer = TIM3;
			irq = TIM3_IRQn;
		break;
		
		case 4U:
			timer = TIM4;
			irq = TIM4_IRQn;
		break;
		
		default:
			return PWM_ERR_INVALID;
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
	
	/* Calculate auto reload counter and its prescaler */
	count = clock / (nFreq >>1U);
	while( count >= 0xffffU )
	{
		/* Prescale is needed since the count over 16 bits */
		count = clock / (nFreq >>1U);
		
		/* try to increase presacaler */
		prescale++;
		
		/* devide again */
		count /= (prescale + 1);
	}

	if( 0 != prescale )
	{
		/* prescaler is applicable */
		timer->PSC = prescale;
	}
	
	timer->ARR = count;
	
	/* OFF all compare output */
	timer->CCMR1 = 0;
	timer->CCMR2 = 0;
	
	if( TRUE == bIrqEn )
	{
		/* enable update generation interrupt */
		timer->EGR |= TIM_EGR_UG;
		
		/* Enable Timer interrupt */
		timer->DIER |= TIM_DIER_UIE;
		
		NVIC_EnableIRQ( irq );
	}
	return PWM_OK;
}


void
PWMEnable(
	PPWM_HANDLE	pHandle,
	uint32_t	nChannel,
	int 		nDutyCycPercent,
	BOOL		bInverse
	)
{
	TIM_TypeDef *timer;
	
	ASSERT( 0 != pHandle );
	
	timer = (TIM_TypeDef *)pHandle->pTimer;
	
	switch( nChannel )
	{
		case 1:
			timer->CCMR1 &= ~TIM_CCMR1_CC1S;
			timer->CCMR1 |= TIM_CCMR1_OC1M;
		
			timer->CCR1 &= ~TIM_CCR1_CCR1;
			timer->CCR1 = (timer->ARR * nDutyCycPercent) / 100;
			if( TRUE == bInverse )
			{
				timer->CCER &= ~TIM_CCER_CC1P;
			}
			else
			{
				timer->CCER |= TIM_CCER_CC1P;
			}
			
			timer->CCER |= TIM_CCER_CC1E;
		break;
		
		case 2:
			timer->CCR2 &= ~TIM_CCR2_CCR2;
			timer->CCR2 = (timer->ARR * nDutyCycPercent) / 100;
			timer->CCMR1 |= TIM_CCMR1_OC2M;
		
			if( TRUE == bInverse )
			{
				timer->CCER &= ~TIM_CCER_CC2P;
			}
			else
			{
				timer->CCER |= TIM_CCER_CC2P;
			}
			
			timer->CCER |= TIM_CCER_CC2E;
		break;

		case 3:
			timer->CCR3 &= ~TIM_CCR3_CCR3;
			timer->CCR3 = (timer->ARR * nDutyCycPercent) / 100;
			timer->CCMR2 |= TIM_CCMR2_OC3M;
		
			if( TRUE == bInverse )
			{
				timer->CCER &= ~TIM_CCER_CC3P;
			}
			else
			{
				timer->CCER |= TIM_CCER_CC3P;
			}
			
			timer->CCER |= TIM_CCER_CC3E;
		break;

		case 4:
			timer->CCR4 &= ~TIM_CCR4_CCR4;
			timer->CCR4 = (timer->ARR * nDutyCycPercent) / 100;
			timer->CCMR2 |= TIM_CCMR2_OC4M;
		
			if( TRUE == bInverse )
			{
				timer->CCER &= ~TIM_CCER_CC4P;
			}
			else
			{
				timer->CCER |= TIM_CCER_CC4P;
			}
			
			timer->CCER |= TIM_CCER_CC4E;
		break;
			
		default:
		break;
	}
	
	timer->CR1 |= TIM_CR1_CEN | TIM_CR1_ARPE;
	timer->BDTR |= TIM_BDTR_MOE;
}


void
PWMDisable(
	PPWM_HANDLE	pHandle,
	uint32_t	nChannel
	)
{
	TIM_TypeDef *timer;
	
	ASSERT( 0 != pHandle );
	
	timer = (TIM_TypeDef *)pHandle->pTimer;
	
	switch( nChannel )
	{
		case 1:
			timer->CCMR1 &= ~TIM_CCMR1_OC1M;
		break;
		
		case 2:
			timer->CCMR1 &= ~TIM_CCMR1_OC2M;
		break;

		case 3:
			timer->CCMR2 &= ~TIM_CCMR2_OC3M;
		break;

		case 4:
			timer->CCMR2 &= ~TIM_CCMR2_OC4M;
		break;		
	}
}


















