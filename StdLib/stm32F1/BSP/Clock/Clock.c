/*****************************************************************************
 @Project	: 
 @File 		: Clock.c
 @Details  	: All Ports and peripherals configuration                    
 @Author	: 
 @Hardware	: 
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
   
******************************************************************************/

#include <Common.h>
#include "Clock.h"

/*****************************************************************************
 Define
******************************************************************************/
#define RCC_HSICALIBRATION_DEFAULT 0x10U /* Default HSI calibration trimming value */
#define RCC_PLLSOURCE_HSI_DIV2 0x00000000U

#ifndef RCC_CR_HSITRIM_Pos
#define RCC_CR_HSITRIM_Pos 3U
#endif

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
 Implementation
******************************************************************************/
void Clock8MInternalInit( void )
{
    RCC->CR |= RCC_CR_HSION;
    while ( 0 == ( RCC->CR & RCC_CR_HSIRDY ) )
    {
    };

    /* Adjusts the Internal High Speed oscillator (HSI) calibration value. */
    RCC->CR &= ~RCC_CR_HSITRIM;
    RCC->CR |= RCC_HSICALIBRATION_DEFAULT << RCC_CR_HSITRIM_Pos;

    RCC->CFGR &= ~RCC_CFGR_SW;

    /* OFF PLL clock during configuration */
    RCC->CR &= ~RCC_CR_PLLON;
    while ( 0 != ( RCC->CR & RCC_CR_PLLRDY ) )
    {
    };

    RCC->CFGR |= RCC_CFGR_SW_PLL;

    /* NOTE:
	The max freq can run with internal RC is 64MHz
	*/
    ClockSetWaitStates( 64000000 );

    RCC->CFGR &= ~RCC_CFGR_HPRE;

    /* HCLK = SYSCLK */
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

    /* PCLK2 = HCLK */
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

    /* PCLK1 = HCLK */
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

    /* PLL source divided by 2 and multiply by 16 */
    RCC->CFGR &= ~RCC_CFGR_PLLSRC;
    RCC->CFGR &= ~RCC_CFGR_PLLMULL;
    RCC->CFGR |= RCC_CFGR_PLLMULL16;

    /* ON PLL clock during configuration */
    RCC->CR |= RCC_CR_PLLON;
    while ( 0 == ( RCC->CR & RCC_CR_PLLRDY ) )
    {
    };
}


void Clock8MExternalInit( void )
{
    RCC->CR |= RCC_CR_HSEON;
    while ( 0 == ( RCC->CR & RCC_CR_HSERDY ) )
    {
    };

    RCC->CFGR &= ~RCC_CFGR_SW;

    /* OFF PLL clock during configuration */
    RCC->CR &= ~RCC_CR_PLLON;
    while ( 0 != ( RCC->CR & RCC_CR_PLLRDY ) )
    {
    };

    RCC->CFGR |= RCC_CFGR_SW_PLL;

    /* Set flash read/write latency 2 wait state */
    ClockSetWaitStates( 72000000 );

    RCC->CFGR &= ~RCC_CFGR_HPRE;

    /* HCLK = SYSCLK */
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

    /* PCLK2 = HCLK */
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

    /* PCLK1 = HCLK */
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

    /* PLL source multiply by 9 */
    RCC->CFGR |= RCC_CFGR_PLLSRC;
    RCC->CFGR &= ~RCC_CFGR_PLLMULL;
    RCC->CFGR |= RCC_CFGR_PLLMULL9;

    /* USB 72MHz/1.5 = 48 */
    RCC->CFGR &= ~RCC_CFGR_USBPRE;

    /* ON PLL clock during configuration */
    RCC->CR |= RCC_CR_PLLON;
    while ( 0 == ( RCC->CR & RCC_CR_PLLRDY ) )
    {
    };
}


void ClockSetWaitStates( uint32_t nSysClock )
{
    FLASH->ACR &= ~FLASH_ACR_LATENCY;

    if ( nSysClock <= 24000000U )
    {
        FLASH->ACR |= FLASH_ACR_LATENCY_0;
    }
    else if ( nSysClock <= 48000000U )
    {
        FLASH->ACR |= FLASH_ACR_LATENCY_1;
    }
    else if ( nSysClock <= 72000000U )
    {
        FLASH->ACR |= FLASH_ACR_LATENCY_2;
    }
    else
    {
        ASSERT( FALSE );
    }
}



