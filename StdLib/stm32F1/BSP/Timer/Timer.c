/*****************************************************************************
 @Project	: 
 @File 		: Timer.c
 @Details  	: STM32F103XX Timer driver               
 @Author	: lcgan
 @Hardware	: STM32
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  lcgan     XXXX-XX-XX  		Initial Release
   1.1  Liang    2017-03-01      Updated for Infrared (IrDA) transmission.
******************************************************************************/
#include <Common.h>
#include "Hal.h"
#include "Timer.h"
#include "pwm.h"


/*****************************************************************************
 Define
******************************************************************************/
#define TOTAL_TIMER				4U
#define NUM_32CYCLE_IRDA      	130U  //Number of 32 cycles for IrDA PWM 36KHz

/*****************************************************************************
 Type definiton
******************************************************************************/


/*****************************************************************************
 Global Variables
******************************************************************************/


/*****************************************************************************
 Local Variables
******************************************************************************/
static volatile PTIMER_HANDLE g_aTimerIrqHandle[TOTAL_TIMER] = { 0, 0, 0, 0 };


/*****************************************************************************
 Local Functions
******************************************************************************/
static void timer_IRQHandler( PTIMER_HANDLE pHandle );


/*****************************************************************************
 Implementation
******************************************************************************/
int TimerInit( PTIMER_HANDLE pHandle, uint8_t nTimer, uint32_t nFreq )
{
	TIM_TypeDef *timer;
	IRQn_Type 	irq;
	uint32_t 	clock;
	uint32_t 	count = 0U;
	uint32_t 	prescale = 0U;
	
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
			return TIMER_ERR_INVALID;
	}
	
	pHandle->pTimer = timer;
	pHandle->Irq = irq;
	
	g_aTimerIrqHandle[nTimer-1] = pHandle;
	
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
	
	/* Calculate auto reload counter and its prescale */
	count = clock / nFreq;
	while( count >= 0xffffU )
	{
		/* Prescale is needed since the count over 16 bits */
		count = clock / nFreq;
		
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
	
	/* the rest of the count store into auto reload counter register */
	timer->ARR = count;

	/* enable update generation interrupt */
	timer->EGR |= TIM_EGR_UG;
	
	/* Timer counting enable */
	timer->CR1 |= TIM_CR1_CEN | TIM_CR1_ARPE;
	
	/* Enable Timer interrupt */
	timer->DIER |= TIM_DIER_UIE;
	
	NVIC_EnableIRQ( irq );
	
	return TIMER_OK;
}


void 
TimerAddHook(
	PTIMER_HANDLE 		pHandle,
	PTIMER_HOOK 		pHook,
	TIMER_CB_UPDATE 	*pfUpdate
	)
{
	ASSERT( 0 != pHandle );
	ASSERT( 0 != pHook );
	ASSERT( 0 != pfUpdate );
	
	pHook->pfUpdate = pfUpdate;
	pHook->pNext = pHandle->pHeadHook;
	pHandle->pHeadHook = pHook;
}

static void timer_IRQHandler( PTIMER_HANDLE pHandle )
{
	TIM_TypeDef *timer = pHandle->pTimer;
	PTIMER_HOOK head;
	int status;

	ASSERT( 0 != pHandle );
	
	status = timer->SR;
	
	if( 0 != (status & TIM_SR_UIF) )
	{
		timer->SR &= ~TIM_SR_UIF;
		
		if( 0 != pHandle->pHeadHook )
		{
			/* Iterate a link list */
			for( head=pHandle->pHeadHook; 0 != head; head=head->pNext )
			{
				ASSERT( 0 != head->pfUpdate );
				head->pfUpdate();
			}
		}
	}
}


/*****************************************************************************
 Interrupt handling
******************************************************************************/
void TIM1_UP_TIM10_IRQHandler( void )
{
	timer_IRQHandler( g_aTimerIrqHandle[0] );
}

void TIM2_IRQHandler( void )
{
	timer_IRQHandler( g_aTimerIrqHandle[1] );
}


void TIM3_IRQHandler( void )
{
	timer_IRQHandler( g_aTimerIrqHandle[2] );
}


void TIM4_IRQHandler( void )
{
	timer_IRQHandler( g_aTimerIrqHandle[3] );
}









































