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
#define TOTAL_I2C			3U

#define HSI_CLOCK		    16000000U /* For STM32F746 is 16MHz */

#define CLK_SEL_APB1	0U
#define CLK_SEL_SYS		1U
#define CLK_SEL_HSI		2U


/*****************************************************************************
 Type definition
******************************************************************************/
typedef struct _BIT_TIME
{
	uint8_t Presc;
	uint8_t Scll;
	uint8_t Sclh;
	uint8_t Sdadel;
	uint8_t Scldel;
}BIT_TIME, *PBIT_TIME;


/*****************************************************************************
 Global Variables
******************************************************************************/


/*****************************************************************************
 constant Variables
******************************************************************************/
static BIT_TIME const BIT_TIME_TAB[4] =
{
	{ 0x03, 0xc7, 0xc3, 0x02, 0x04 },
	{ 0x03, 0x13, 0x0f, 0x02, 0x04 },
	{ 0x01, 0x09, 0x03, 0x02, 0x03 },
	{ 0x00, 0x04, 0x02, 0x00, 0x02 }
};

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
	I2C_SPEED 		Speed 
	)
{
	I2C_TypeDef 	*i2c;
	IRQn_Type 		irq;
	uint32_t 		temp = 0;
	int				idx;
	
	ASSERT( 0 != pHandle );
	ASSERT( 0 != nPort );


	switch( nPort )
	{
		case 1:
			i2c = I2C1;
			irq = I2C1_EV_IRQn;

            /* All I2C force to use HSI clock to reduce complexity in speed calculation */
			RCC->DCKCFGR2 &= ~RCC_DCKCFGR2_I2C1SEL_Msk;
            RCC->DCKCFGR2 |= (CLK_SEL_HSI<<RCC_DCKCFGR2_I2C1SEL_Pos);
		break; 
		
		case 2:
			i2c = I2C2;
			irq = I2C2_EV_IRQn;

            /* All I2C force to use HSI clock to reduce complexity in speed calculation */
			RCC->DCKCFGR2 &= ~RCC_DCKCFGR2_I2C2SEL_Msk;
            RCC->DCKCFGR2 |= (CLK_SEL_HSI<<RCC_DCKCFGR2_I2C2SEL_Pos);
		break;

		case 3:
			i2c = I2C3;
			irq = I2C3_EV_IRQn;

            /* All I2C force to use HSI clock to reduce complexity in speed calculation */
			RCC->DCKCFGR2 &= ~RCC_DCKCFGR2_I2C3SEL_Msk;
            RCC->DCKCFGR2 |= (CLK_SEL_HSI<<RCC_DCKCFGR2_I2C3SEL_Pos);
		break;

		
		default:
			return I2C_STS_PORT_INVALID;
	}

		
	/* Store callback function for ADC interrupt use */
	pHandle->pI2c = i2c;
	pHandle->bRepeatStart = FALSE;
	pHandle->nTxCount = 0;
	pHandle->nRxCount = 0;
	
	g_pI2cIrqHandles[nPort-1] = pHandle;
	
	i2c->CR1 = 0;
	
	i2c->CR1 &= ~I2C_CR1_PE;
	while( 0 != (i2c->CR1 & I2C_CR1_PE) );
	
	i2c->OAR1 &= ~I2C_OAR1_OA1EN;
	i2c->OAR2 &= ~I2C_OAR2_OA2EN;
	i2c->CR2 |= I2C_CR2_AUTOEND | I2C_CR2_NACK;
	i2c->CR2 &= ~I2C_CR2_ADD10;

    /* WARNING All I2c timming is base on HSI 16MHz */

	switch( Speed )
	{
		case I2C_SPEED_10KHZ:
			idx = 0;
		break;

        case I2C_SPEED_100KHZ:
			idx = 1;
		break;

		case I2C_SPEED_400KHZ:
			idx = 2;
		break;

		case I2C_SPEED_1000KHZ:
			idx = 3;
		break;

		default:
			return I2C_STS_SPEED_INVALID;
	}

	temp = 0;
	temp = (BIT_TIME_TAB[idx].Scll<<I2C_TIMINGR_SCLL_Pos) 
		|(BIT_TIME_TAB[idx].Sclh<<I2C_TIMINGR_SCLH_Pos)
		|(BIT_TIME_TAB[idx].Sdadel<<I2C_TIMINGR_SDADEL_Pos)
		|(BIT_TIME_TAB[idx].Sdadel<<I2C_TIMINGR_SCLDEL_Pos)
		|(BIT_TIME_TAB[idx].Presc<<I2C_TIMINGR_PRESC_Pos);
	i2c->TIMINGR = temp;

	/* Enable SPI */
	i2c->CR1 |= I2C_CR1_PE;
	
	NVIC_EnableIRQ( irq );
	
	return I2C_STS_OK;
}


void I2cSetRiseTimeMax( PI2C_HANDLE pHandle, int nRiseNanoSec )
{

	ASSERT( 0 != pHandle );
    UNUSE( pHandle );
    UNUSE( nRiseNanoSec );

    /* Not applicable */
}


void
I2cWrite(
	PI2C_HANDLE pHandle,
	uint8_t 	nSlaveAddr,
	uint8_t 	Register,
	void const 	*pData,
	int			nByte
	)
{
	I2C_TypeDef *i2c;
	int32_t reg;
	
	ASSERT( 0 != pHandle );
	ASSERT( nByte < 256 ); /* This driver not yet support more than 256 */

	i2c = (I2C_TypeDef *)pHandle->pI2c;
	
	pHandle->nSlaveAddr	= nSlaveAddr;
	pHandle->Register	= Register;
	pHandle->pData		= (uint8_t *)pData;
	pHandle->nTxCount 	= 0U;
	pHandle->bWrite		= TRUE;
	pHandle->nByte		= nByte + 1;
	pHandle->nLeft		= 0;

	if( pHandle->nByte > 255 )
	{
		pHandle->nLeft = pHandle->nByte - 255;
        pHandle->nByte = 255;
	}

	
	/* Start */
	reg = i2c->CR2;
	reg &= ~(I2C_CR2_SADD_Msk 
				| I2C_CR2_RD_WRN 
				| I2C_CR2_NBYTES_Msk 
				| I2C_CR2_NACK);
	i2c->CR2 = reg;

	reg = (pHandle->nByte<<I2C_CR2_NBYTES_Pos) 
				| (nSlaveAddr<<I2C_CR2_SADD_Pos)
                | I2C_CR2_AUTOEND;
				
	i2c->CR2 |= reg;

	i2c->CR2 |= I2C_CR2_START;
	
	i2c->CR1 |= I2C_CR1_TXIE | I2C_CR1_STOPIE;
}


void
I2cRead( 
	PI2C_HANDLE pHandle,
	uint8_t 	nSlaveAddr,
	uint8_t 	Register,
	void 		*pData,
	int			nByte
	)
{
	I2C_TypeDef *i2c;
	ASSERT( 0 != pHandle );
	ASSERT( nByte < 256 );
	
	i2c = (I2C_TypeDef *)pHandle->pI2c;
	pHandle->nSlaveAddr	= nSlaveAddr;
	pHandle->Register	= Register;
	pHandle->pData		= (uint8_t *)pData;
	pHandle->nByte		= nByte;
	pHandle->nRxCount	= 0U;
	pHandle->nTxCount 	= 0U;
	pHandle->bWrite		= FALSE;
	/* Start */
	
	i2c->CR2 &= ~(I2C_CR2_AUTOEND | I2C_CR2_RD_WRN);
	i2c->CR2 &= ~(I2C_CR2_SADD_Msk | I2C_CR2_NBYTES_Msk);
	i2c->CR2 |= (1U<<I2C_CR2_NBYTES_Pos) 
				| (nSlaveAddr<<I2C_CR2_SADD_Pos);
	
	i2c->CR2 |= I2C_CR2_START;
	
	i2c->CR1 |= I2C_CR1_RXIE | I2C_CR1_TXIE | I2C_CR1_TCIE;
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
	PI2C_HOOK	cur;
	uint32_t	status = pI2C->ISR;
    uint32_t	tmp;

	ASSERT( 0 != pHandle );

	/* If data is ready to read */
	if( 0 != (status & I2C_ISR_RXNE) )
	{
		*pHandle->pData++ = pI2C->RXDR;
		pHandle->nRxCount++;
	}

	
	if( 0 != (status & I2C_ISR_TC) )
	{
		/* TC only will set if NBYTES transfer done with AUTOEND disable */
		/* Here is the place to perform repeat start and update NBYTES */
		if( 0 == pHandle->nRxCount )
		{
			pI2C->CR1 &= ~I2C_CR1_TXIE;

			/* Here we disable the AUTOEND for read is because we want to
			 ensure last byte is properly read before STOP is generated */
			tmp = pI2C->CR2;
			tmp &= ~(I2C_CR2_NBYTES_Msk | I2C_CR2_AUTOEND); 
			tmp |= (pHandle->nByte<<I2C_CR2_NBYTES_Pos) 
						| I2C_CR2_RD_WRN;

			pI2C->CR2 = tmp;
			pI2C->CR2 |= I2C_CR2_START;

		}
		else
		{
			pI2C->CR1 |= I2C_CR1_STOPIE;
			pI2C->CR2 |= I2C_CR2_STOP;
		}
	}


	if( 0 != (status & I2C_ISR_STOPF) )
	{
		pI2C->ICR |= I2C_ICR_STOPCF;
		
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
		if( 0 != (status & I2C_ISR_TXIS) )
		{
			if( TRUE == pHandle->bWrite )
			{
				if( 0 == pHandle->nTxCount )
				{
					pI2C->TXDR = pHandle->Register;
				}
				else
				{
					pI2C->TXDR = *pHandle->pData++;
				}
							
				pHandle->nTxCount++;
			}
            else
            {
                if( 0 == pHandle->nTxCount )
				{
					pI2C->TXDR = pHandle->Register;
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


void I2C3_EV_IRQHandler( void )
{
	i2c_Handler( g_pI2cIrqHandles[2], I2C3 );	
}












