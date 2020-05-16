/*****************************************************************************
 @Project	: 
 @File 		: i2c.c
 @Details  	: 
 @Author		: lcgan
 @Hardware	: 
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
   
******************************************************************************/

#include <Common.h>
#include "Hal.h"
#include "i2c.h"


/*****************************************************************************
 Define
******************************************************************************/
#define TOTAL_I2C					2U
#define RISE_TIME_DEFAULT_NANO_S	300		/* Default Rising time 300ns */


/*****************************************************************************
 Type definition
******************************************************************************/


/*****************************************************************************
 Global Variables
******************************************************************************/


/*****************************************************************************
 constant Variables
******************************************************************************/


/*****************************************************************************
 Local Variables
******************************************************************************/

static volatile PI2C_HANDLE	g_pI2cIrqHandles[TOTAL_I2C];


/*****************************************************************************
 Local functions
******************************************************************************/


/*****************************************************************************
 Callback functions
******************************************************************************/


/*****************************************************************************
 Implementation 
******************************************************************************/
int
I2cInit(
	PI2C_HANDLE 	pHandle,
	uint8_t 		nPort,
	uint32_t 		nBusSpeed
	)
{
	I2C_TypeDef 	*i2c;
	IRQn_Type 		irq;
	uint32_t		clock;
	int				tmp;
	
	ASSERT( 0 != pHandle );
	
	switch( nPort )
	{
		case 0:
			i2c = I2C1;
			irq = I2C1_EV_IRQn;
#ifdef _DEBUG
	DBGMCU->CR |= DBGMCU_CR_DBG_I2C1_SMBUS_TIMEOUT;
#endif
		break;
		
		case 1:
			i2c = I2C2;
			irq = I2C2_EV_IRQn;
#ifdef _DEBUG
	DBGMCU->CR |= DBGMCU_CR_DBG_I2C2_SMBUS_TIMEOUT;
#endif
		break;
		
		default:
			return I2C_STS_PORT_INVALID;
	}

		
	/* Store callback function for ADC interrupt use */
	pHandle->pI2c = i2c;
	pHandle->bRepeatStart = FALSE;
	pHandle->nTxCount = 0;
	pHandle->nRxCount = 0;
	
	g_pI2cIrqHandles[nPort] = pHandle;
	
	i2c->CR1 = 0;
	
	i2c->CR1 &= ~I2C_CR1_PE;


	/* Retrieve Bus prescaler to get APB speed */
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

	i2c->CR2 &= ~(I2C_CR2_FREQ | I2C_CR2_LAST);
	i2c->CR2 |= (clock / 1000000);

	/*	Fm mode:
		If DUTY = 0:
		Thigh = CCR * TPCLK1
		Tlow = 2 * CCR * TPCLK1 */

	tmp = clock /(3 * nBusSpeed);

	i2c->CCR |= I2C_CCR_FS | tmp;

	/* Set rising time to default 300ns */
	i2c->TRISE = (RISE_TIME_DEFAULT_NANO_S / (1000000000/clock)) + 1;

	/* Enable I2C controller */
	i2c->CR1 |= I2C_CR1_PE;
	
	/* Enable interrupt */
	NVIC_EnableIRQ( irq );
	
	return I2C_STS_OK;
}


void I2cSetRiseTimeMax( PI2C_HANDLE pHandle, int nRiseNanoSec )
{
	I2C_TypeDef *i2c;
	ASSERT( 0 != pHandle );
	
	i2c = (I2C_TypeDef *)pHandle->pI2c;

    /* Disable I2C controller */
	i2c->CR1 &= ~I2C_CR1_PE;

	i2c->TRISE = (nRiseNanoSec / (1000000000/pHandle->nClock)) + 1;

    /* Enable I2C controller */
	i2c->CR1 |= I2C_CR1_PE;
}


BOOL I2cIsDone( PI2C_HANDLE pHandle )
{
	uint32_t status;
	I2C_TypeDef *i2c;
	ASSERT( 0 != pHandle );
	
	i2c = (I2C_TypeDef *)pHandle->pI2c;
	status = i2c->SR1;
	UNUSE( status );

	return ( 0 != (i2c->SR2 & I2C_SR2_BUSY) )? FALSE : TRUE;
}


void
I2cWrite(
	PI2C_HANDLE pHandle,
	uint8_t 	nSlaveAddr,
	uint8_t 	Register,
	void const 	*pData,
	uint8_t 	nByte
	)
{
	I2C_TypeDef *i2c;
	ASSERT( 0 != pHandle );
	
	i2c = (I2C_TypeDef *)pHandle->pI2c;
	
	pHandle->nSlaveAddr	= nSlaveAddr;
	pHandle->Register	= Register;
	pHandle->pData		= (uint8_t *)pData;
	pHandle->nByte		= nByte;
    pHandle->bWrite     = TRUE;
    pHandle->nRxCount = 0;
	pHandle->nTxCount = 0;
    pHandle->bNotEmpty = FALSE;

	/* Start */
	//i2c->OAR1 |= nSlaveAddr<<1;
	i2c->CR1 |= I2C_CR1_START;
	i2c->CR2 |= I2C_CR2_ITEVTEN;
}


void
I2cRead( 
	PI2C_HANDLE pHandle,
	uint8_t 	nSlaveAddr,
	uint8_t 	Register,
	void 		*pData,
	uint8_t 	nByte
	)
{
	I2C_TypeDef *i2c;
	ASSERT( 0 != pHandle );
	
	i2c = (I2C_TypeDef *)pHandle->pI2c;
	pHandle->nSlaveAddr	= nSlaveAddr;
	pHandle->Register	= Register;
	pHandle->pData		= (uint8_t *)pData;
	pHandle->nByte		= nByte;
	pHandle->bWrite     = FALSE;
    pHandle->bRepeatStartReq = TRUE;
    pHandle->nTxCount = 0;
    pHandle->nRxCount = 0;
	pHandle->nReadCount = 0;
    pHandle->bNotEmpty = FALSE;

	/* Start */
	i2c->CR1 |= I2C_CR1_ACK | I2C_CR1_START;
	i2c->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN;	
}


void 
I2cAddHook(
	PI2C_HANDLE 		pHandle,
	PI2C_HOOK 			pHook,
	I2C_CB_DONE 		*pfDone
	)
{
	ASSERT( 0 != pHandle );
	ASSERT( 0 != pHook );
	ASSERT( 0 != pfDone );
	
	pHook->pfDone 		= pfDone;
	pHook->pNext 		= pHandle->pHeadHook;
	pHandle->pHeadHook 	= pHook;
}


/*****************************************************************************
 Callback functions
******************************************************************************/


/*****************************************************************************
 Local functions
******************************************************************************/
static void i2c_Handler( PI2C_HANDLE pHandle, I2C_TypeDef *pI2C )
{
	PI2C_HOOK cur;
	uint32_t status = pI2C->SR1;
	uint32_t err = pI2C->SR2;

	ASSERT( 0 != pHandle );

    if( 0 != err )
    {
        err = 0;
    }

	if( 0 != (status & I2C_SR1_AF) )
	{
		pI2C->SR1 &= ~I2C_SR1_AF;

		/* Ack failed */
	}

	/* Start signal successfully generated. 
	At here We need to Decide read/write with slave address*/
	if( 0 != (status & I2C_SR1_SB) )
	{
		if( FALSE != pHandle->bRepeatStart )
		{
			pI2C->DR = pHandle->nSlaveAddr | 0x01;	/* Read */
		}
		else
		{
			pI2C->DR = pHandle->nSlaveAddr & 0xfe; /* Write */
		}

		pHandle->nTxCount = 0;
		pHandle->nRxCount = 0;
		
		return;
	}

	
	/* Address successfully generated. 
	 At here, we need to write slave memory address to access */
	if( 0 != (status & I2C_SR1_ADDR) )
	{
        if( FALSE != pHandle->bRepeatStart )
        {
            pHandle->bRepeatStart = FALSE;

            if( 1 == pHandle->nByte )
            {
                pI2C->CR1 &= ~I2C_CR1_ACK;
                pI2C->CR1 |= I2C_CR1_STOP;
				pHandle->bNotEmpty = TRUE;

				return;
            }
        }
        else
        {
            pI2C->DR = pHandle->Register;

			/* After writing slave memory address,
			 we need to get back an ACK, now exit and wait for ACK */
            return;
        }
	}

	
	/* Write direction implementation */
    if( TRUE == pHandle->bWrite )
    {
        if( (0 != (status & I2C_SR1_TXE)) || (0 != (status & I2C_SR1_BTF)) )
        {
            if( pHandle->nTxCount >= pHandle->nByte )
            {
                pI2C->CR1 |= I2C_CR1_STOP;
                pI2C->CR2 &= ~I2C_CR2_ITEVTEN;

                status = pI2C->SR1;

				//while( 0 != (pI2C->SR2 & I2C_SR2_BUSY) );
        
                /* Callback to application to notify I2C is done */
                if( 0 != pHandle->pHeadHook )
                {
                    /* Iterate a link list */
                    for( cur=pHandle->pHeadHook; 0 != cur; cur=cur->pNext )
                    {
                        ASSERT( 0 != cur->pfDone );
                        cur->pfDone();
                    }
                }
            }
            else
            {
                pI2C->DR = *pHandle->pData++;
                pHandle->nTxCount++;
            }
        }
	}
    else
    {
		/* If repeat start is required */
        if( TRUE == pHandle->bRepeatStartReq )
        {
            pHandle->bRepeatStartReq = FALSE;
            pHandle->bRepeatStart = TRUE;
            pI2C->CR1 |= I2C_CR1_START;

            return;
        }

        if( 0 != (status & (I2C_SR1_RXNE | I2C_SR1_BTF)) )
        {
			/* If the BTF flag set is due to Transmit register empty, we need to skip */
			if(0 == pHandle->nRxCount )
			{
				if( 0 != (status & I2C_SR1_BTF) )
				{
					return;
				}
			}

            pHandle->nRxCount++;

			if( pHandle->nByte > 3 )
			{
				switch( pHandle->nByte - pHandle->nRxCount )
				{
					case 0:
						*pHandle->pData++ = pI2C->DR;
						pHandle->nReadCount++;

                       // while( 0 != (pI2C->SR2 & I2C_SR2_BUSY) );
						*pHandle->pData++ = pI2C->DR;
						pHandle->nReadCount++;
					break;

					case 1:
						pI2C->CR1 &= ~I2C_CR1_ACK;
				 
						*pHandle->pData++ = pI2C->DR;
						pHandle->nReadCount++;

                        pI2C->CR1 |= I2C_CR1_STOP;
					break;

					default:
						*pHandle->pData++ = pI2C->DR;
						pHandle->nReadCount++;
					break;
				}
			}
			else if( 3 == pHandle->nByte )
            {
				switch( pHandle->nByte - pHandle->nRxCount )
				{
					case 0:
                        *pHandle->pData++ = pI2C->DR;
						pHandle->nReadCount++;
					break;

					case 1:
						pI2C->CR1 |= I2C_CR1_STOP;
                        *pHandle->pData++ = pI2C->DR;
						pHandle->nReadCount++;
					break;

					case 2:
						pI2C->CR1 &= ~I2C_CR1_ACK;
						*pHandle->pData++ = pI2C->DR;
						pHandle->nReadCount++;
					break;

					case 3:
						return;
					break;

					default:
                    	*pHandle->pData++ = pI2C->DR;
						pHandle->nReadCount++;
					break;
				}
#if 0
                if( pHandle->nRxCount == (pHandle->nByte - 1) )
                {
                    pI2C->CR1 &= ~I2C_CR1_ACK;
                    pI2C->CR1 |= I2C_CR1_STOP;
                }
#endif
            }
            else if( pHandle->nByte > 1 )
			{
				switch( pHandle->nByte - pHandle->nRxCount )
				{
					case 0:
                        *pHandle->pData++ = pI2C->DR;
						pHandle->nReadCount++;
					break;

					case 1:
                        pI2C->CR1 &= ~I2C_CR1_ACK;
						pI2C->CR1 |= I2C_CR1_STOP;
						*pHandle->pData++ = pI2C->DR;
						pHandle->nReadCount++;
					break;

					break;

					default:
					break;
				}	
			}		
        }

		if(  FALSE != pHandle->bNotEmpty  )
		{
			 pHandle->bNotEmpty = FALSE;
             pHandle->nReadCount++;
			 *pHandle->pData = pI2C->DR;
		}

		if( pHandle->nReadCount >= pHandle->nByte )
		{
			/* All bytes received done */
			pI2C->CR2 &= ~I2C_CR2_ITEVTEN;

           // while( 0 != (pI2C->SR2 & I2C_SR2_BUSY) );
		
			/* Callback to application to notify I2C is done */
			if( 0 != pHandle->pHeadHook )
			{
				/* Iterate a link list */
				for( cur=pHandle->pHeadHook; 0 != cur; cur=cur->pNext )
				{
					ASSERT( 0 != cur->pfDone );
					cur->pfDone();
				}
			}
		}
    }
}


/*****************************************************************************
 Interrupt functions
******************************************************************************/
void I2C1_EV_IRQHandler( void )
{
	i2c_Handler( g_pI2cIrqHandles[0], I2C1 );
}


void I2C2_EV_IRQHandler( void )
{
	i2c_Handler( g_pI2cIrqHandles[1], I2C2 );	
}














