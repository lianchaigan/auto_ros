/*****************************************************************************
 @Project	: 
 @File 		: Flash.c
 @Details  	:                  
 @Author	: 
 @Hardware	: 
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
   
******************************************************************************/
#include <Common.h>
#include "Flash.h"


/*****************************************************************************
 Define
******************************************************************************/
#define FLASH_KEY1               ((uint32_t)0x45670123)
#define FLASH_KEY2               ((uint32_t)0xCDEF89AB)



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
 Local Functions
******************************************************************************/
static void flash_WaitForLastOperation( void );


/*****************************************************************************
 Implementation
******************************************************************************/
int FlashUnlock( void )
{
    int res = FLASH_STS_OK;
    int32_t tmpreg;

    __disable_irq();

  /* Read the ACR register */
  tmpreg = FLASH->ACR;  
  
  /* Sets the Latency value */
  tmpreg &= ~FLASH_ACR_LATENCY;
  tmpreg |= FLASH_ACR_LATENCY_2;
  
  /* Write the ACR register */
  FLASH->ACR = tmpreg;


    if( 0 != (FLASH->CR & FLASH_CR_LOCK) )
    {
        /* Authorize the FLASH Registers access */
        FLASH->KEYR = FLASH_KEY1;
        FLASH->KEYR = FLASH_KEY2;
    }
    else
    {
        res = FLASH_E_UNLOCK;
    }
  
    __enable_irq();
    return res; 
}


void FlashLock( void )
{
	__disable_irq();

	/* Set the Lock Bit to lock the FPEC and the CR of  Bank1 */
	FLASH->CR |= FLASH_CR_LOCK;

	__enable_irq();
}



int FlashErase( int nAddr )
{
    int idx;

    __disable_irq();

    flash_WaitForLastOperation();

    /* if the previous operation is completed, proceed to erase the page */
    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR = nAddr; 
    FLASH->CR  |= FLASH_CR_STRT;

    flash_WaitForLastOperation();

    /* Disable the PER Bit */
    FLASH->CR &= ~FLASH_CR_PER;

   __enable_irq();
}


int FlashProgram( uint32_t nAddress, void *pData, int nLen )
{
    int i;
    int ptr = 0;
    uint16_t *data = pData;
    int size = nLen>>1;

    ASSERT( 0 != pData );
    ASSERT( 0 != nLen );

    __disable_irq();

    /* Wait for last operation to be completed */
    flash_WaitForLastOperation();

         /* if the previous operation is completed, proceed to program the new first 
        half word */
      FLASH->CR |= FLASH_CR_PG;
    for( i=0; i<size; i++ )
    {


        *(__IO uint16_t*)(nAddress + ptr) = *data++;
        ptr += 2;
        /* Wait for last operation to be completed */
        flash_WaitForLastOperation();

    }


            /* If the program operation is completed, disable the PG Bit */
    FLASH->CR &= ~FLASH_CR_PG;

    __enable_irq();
}


/*****************************************************************************
 Local Helpers
******************************************************************************/
static void flash_WaitForLastOperation( void )
{
    while( (FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY );

    if( 0 != (FLASH->SR & (FLASH_SR_PGERR | FLASH_SR_WRPRTERR)) )
	{
		ASSERT( FALSE );
	}
}









