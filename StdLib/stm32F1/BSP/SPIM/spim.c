/*****************************************************************************
 @Project	: 
 @File 		: spim.c
 @Details  	: SPI Master hardware layer                    
 @Author	: lcgan
 @Hardware	: 
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
   
******************************************************************************/
#include <Common.h>
#include "math.h"
#include "Hal.h"
#include "spim.h"


/*****************************************************************************
 Define
******************************************************************************/
#define TOTAL_SPI			2U


/*****************************************************************************
 Type definition
******************************************************************************/


/*****************************************************************************
 Global Variables
******************************************************************************/

/*****************************************************************************
 Local Variables
******************************************************************************/
static volatile PSPIM_HANDLE g_pSpiIrqhandles[TOTAL_SPI] = { 0, 0 };


/*****************************************************************************
 Local Helpers
******************************************************************************/
static void SPI_Handler( PSPIM_HANDLE pHandle );


/*****************************************************************************
 Export functions
******************************************************************************/;


/*****************************************************************************
 Implementation
******************************************************************************/
int
SpimInit(
	void 		*pHandle,
	char 		nPort,
	int		 	nSpeed,
	SPIM_CFG	ClkInactive,
	SPIM_CFG	ClkEdge,
	SPIM_CFG	DATA_SIZE )
{
	IRQn_Type		irq;
	SPI_TypeDef 	*spi;
	int32_t			tmp = 0;
	int				apb;
	int				clock;
    int				div;
	int				prescale;
	PSPIM_HANDLE 	handle = (PSPIM_HANDLE)pHandle;
	
	ASSERT( 0 != pHandle );
	ASSERT( 0 != nSpeed );
	
	/* Check port */
	switch( nPort )
	{
		case 1:
			spi = SPI1;
			irq = SPI1_IRQn;
			apb = 2;
		break;
		
		case 2:
			spi = SPI2;
			irq = SPI2_IRQn;
			apb = 1;
		break;
		
		default:
			return SPIM_E_INVALID_PORT;
	}
	
	handle->pSpi = spi;
	handle->Irq = irq;
    handle->Apb = apb;
	
	g_pSpiIrqhandles[nPort-1] = handle;		/* keep the handle for interrupt use */
	
	spi->CR1 &= ~SPI_CR1_SPE;

	
	tmp |= SPI_CR1_MSTR;		/* Master mode */
	
	/* Set clock polarity */
	switch( ClkInactive )
	{
		case SPI_CLK_INACT_LOW:
			tmp &= ~SPI_CR1_CPOL;
		break;
		
		case SPI_CLK_INACT_HIGH:
			tmp |= SPI_CR1_CPOL;
		break;
		
		default:
			return SPIM_E_INVALID_CLK_POLARITY;
	}
	
	/* Set clock edge */
	switch( ClkEdge )
	{
		case SPI_CLK_RISING_EDGE:
			tmp &= ~SPI_CR1_CPHA;
		break;
		
		case SPI_CLK_FALLING_EDGE:
			tmp |= SPI_CR1_CPHA;
		break;
		
		default:
			return SPIM_E_INVALID_CLK_EDGE;
	}

	/* Speed */
    clock = SystemCoreClock;

	if( 1 == apb )
	{
		if( 0 != (RCC->CFGR & RCC_CFGR_PPRE1_2) )
		{
			switch( RCC->CFGR & RCC_CFGR_PPRE1)
			{
				case RCC_CFGR_PPRE1_DIV2:
					clock = clock>>1;
				break;

				case RCC_CFGR_PPRE1_DIV4:
					clock = clock>>2;
				break;

				case RCC_CFGR_PPRE1_DIV8:
					clock = clock>>3;
				break;

				case RCC_CFGR_PPRE1_DIV16:
					clock = clock>>4;
				break;

				default:
				break;
			}
		}
	}
	else
	{
		if( 0 != (RCC->CFGR & RCC_CFGR_PPRE2_2) )
		{
			switch( RCC->CFGR & RCC_CFGR_PPRE2)
			{
				case RCC_CFGR_PPRE2_DIV2:
					clock = clock>>1;
				break;

				case RCC_CFGR_PPRE2_DIV4:
					clock = clock>>2;
				break;

				case RCC_CFGR_PPRE2_DIV8:
					clock = clock>>3;
				break;

				case RCC_CFGR_PPRE2_DIV16:
					clock = clock>>4;
				break;

				default:
				break;
			}
		}
	}

	prescale = clock / nSpeed;
	div = 0;
	do
	{
		prescale /= 2;
		div++;

		
	}while( 0 != prescale );


	tmp &= ~SPI_CR1_BR;
	tmp |= ((div-1)&0x07)<<3U;
	spi->CR1 = tmp;
	
	spi->CR2 |= SPI_CR2_SSOE;
	
	switch( DATA_SIZE )
	{
		case SPI_DATA_SIZE_8:
			spi->CR1 &= ~SPI_CR1_DFF;
            handle->DataSize = 8;
		break;
		
		case SPI_DATA_SIZE_16:
			spi->CR1 |= SPI_CR1_DFF;
            handle->DataSize = 16;
		break;
		
		default:
			return SPIM_E_INVALID_DATASIZE;
	}
	
    spi->CR1 |= SPI_CR1_SPE;

	NVIC_EnableIRQ( irq );
	
	return SPIM_STS_OK;
}


int SpimSetSpeed( void *pHandle, int nSpeed )
{
	int				clock;
    int				div;
	int				prescale;
	int				tmp;
    PSPIM_HANDLE 	handle = (PSPIM_HANDLE)pHandle;
    SPI_TypeDef 	*spi 	= handle->pSpi;

	ASSERT( 0 != pHandle );

	/* Speed */
    clock = SystemCoreClock;

	spi->CR1 &= ~SPI_CR1_SPE;

	if( 1 == handle->Apb )
	{
		if( 0 != (RCC->CFGR & RCC_CFGR_PPRE1_2) )
		{
			switch( RCC->CFGR & RCC_CFGR_PPRE1)
			{
				case RCC_CFGR_PPRE1_DIV2:
					clock = clock>>1;
				break;

				case RCC_CFGR_PPRE1_DIV4:
					clock = clock>>2;
				break;

				case RCC_CFGR_PPRE1_DIV8:
					clock = clock>>3;
				break;

				case RCC_CFGR_PPRE1_DIV16:
					clock = clock>>4;
				break;

				default:
				break;
			}
		}
	}
	else
	{
		if( 0 != (RCC->CFGR & RCC_CFGR_PPRE2_2) )
		{
			switch( RCC->CFGR & RCC_CFGR_PPRE2)
			{
				case RCC_CFGR_PPRE2_DIV2:
					clock = clock>>1;
				break;

				case RCC_CFGR_PPRE2_DIV4:
					clock = clock>>2;
				break;

				case RCC_CFGR_PPRE2_DIV8:
					clock = clock>>3;
				break;

				case RCC_CFGR_PPRE2_DIV16:
					clock = clock>>4;
				break;

				default:
				break;
			}
		}
	}

	prescale = clock / nSpeed;
	div = 0;
	do
	{
		prescale /= 2;
		div++;

		
	}while( 0 != prescale );

	tmp = spi->CR1;
	tmp &= ~SPI_CR1_BR;
	tmp |= ((div-1)&0x07)<<3U;
	spi->CR1 = tmp;

    clock = clock/ pow(2.0, div);

    spi->CR1 |= SPI_CR1_SPE;

	return clock;
}


int SpimSetDataSize( void *pHandle, SPIM_CFG DATA_SIZE )
{
	PSPIM_HANDLE handle = (PSPIM_HANDLE)pHandle;	
	SPI_TypeDef 	*spi 	= handle->pSpi;
	
	ASSERT( 0 != pHandle );

    spi->CR1 &= ~SPI_CR1_SPE;

	switch( DATA_SIZE )
	{
		case SPI_DATA_SIZE_8:
			spi->CR1 &= ~SPI_CR1_DFF;
            handle->DataSize = 8;
		break;
		
		case SPI_DATA_SIZE_16:
			spi->CR1 |= SPI_CR1_DFF;
            handle->DataSize = 16;
		break;
		
		default:
			return SPIM_E_INVALID_DATASIZE;
	}
	
    spi->CR1 |= SPI_CR1_SPE;

    return SPIM_STS_OK;
}


void
SpimAddCallbackTransferDone(
	void 				*phandle,
	SPIM_CB_TRFR_DONE 	*pfDone )
{
	PSPIM_HANDLE handle = (PSPIM_HANDLE)phandle;	
	
	ASSERT( 0 != phandle );
	ASSERT( 0 != pfDone );	
	
	handle->pfDone = pfDone;
}


void
SpimTransfer(
	void 		*phandle,
	void const 	*pTxBuf,
	void 		*pRxBuf,
	int 		nSize
	)
{
	PSPIM_HANDLE	handle 	= (PSPIM_HANDLE)phandle;
	SPI_TypeDef 	*spi 	= handle->pSpi;
	
	ASSERT( 0 != phandle );
	ASSERT( 0 != pTxBuf );
	ASSERT( 0 != nSize );
	

    switch( handle->DataSize )
    {
        case SPI_DATA_SIZE_8:
            handle->pRxBuf 	= (char *)pRxBuf;
            handle->pTxBuf 	= (char *)pTxBuf;
            handle->nSize	= nSize;
            *((__IO uint8_t *)(&spi->DR)) = handle->pTxBuf[0];
        break;

        case SPI_DATA_SIZE_16:
            handle->pRxWordBuf 	= (uint16_t *)pRxBuf;
            handle->pTxWordBuf 	= (uint16_t *)pTxBuf;
            handle->nSize	= nSize>>1U;
            *((__IO uint16_t *)(&spi->DR)) = handle->pTxWordBuf[0];
        break;
    }

	handle->nTxCount	= 0;
	handle->nRxCount	= 0;
	
	handle->nTxCount++;
	spi->CR2 |= SPI_CR2_RXNEIE | SPI_CR2_TXEIE;		/* Enable interrupts */
}


/*****************************************************************************
 Callback functions
******************************************************************************/


/*****************************************************************************
 Local functions
******************************************************************************/
static void SPI_Handler( PSPIM_HANDLE pHandle )
{
	SPI_TypeDef *spi = pHandle->pSpi;
	int status = spi->SR;
	
	ASSERT( 0 != pHandle );
	
	if( 0 == (status & SPI_SR_BSY) )
	{
		if( 0 != (status & SPI_SR_RXNE) )
		{
            if( SPI_DATA_SIZE_8 == pHandle->DataSize )
            {
                if( 0 != pHandle->pRxBuf )
                {
                    pHandle->pRxBuf[pHandle->nRxCount] = spi->DR;
                }
            }
            else
            {
                if( 0 != pHandle->pRxWordBuf )
                {
                    pHandle->pRxWordBuf[pHandle->nRxCount] = spi->DR;
                }  
            }

			pHandle->nRxCount++;
			
			if( pHandle->nRxCount >= pHandle->nSize )
			{
				spi->CR2 &= ~SPI_CR2_RXNEIE;

               	/* Callback to application */
				if( 0 != pHandle->pfDone )	/* The pointer maybe 0 */
				{
					pHandle->pfDone();
				}
			}
		}
		
		if( 0 != (status & SPI_SR_TXE) )
		{
			if( pHandle->nTxCount >= pHandle->nSize )
			{
				/* Transfer has completed */
				spi->CR2 &= ~SPI_CR2_TXEIE;	/* Disable interrupt */
			}
			else
			{
                if( SPI_DATA_SIZE_8 == pHandle->DataSize )
                {
                    /* Write to data register to transfer */
                    *((__IO uint8_t *)(&spi->DR)) = pHandle->pTxBuf[pHandle->nTxCount];
                }
                else
                {
                     /* Write to data register to transfer */
                    *((__IO uint16_t *)(&spi->DR)) = pHandle->pTxWordBuf[pHandle->nTxCount];
                }

				pHandle->nTxCount++;
			}
		}
	}
}


/*****************************************************************************
 Interrupt
******************************************************************************/
void SPI1_IRQHandler( void )
{
	ASSERT( 0 != g_pSpiIrqhandles[0] );
	
	SPI_Handler( g_pSpiIrqhandles[0] );
}


void SPI2_IRQHandler( void )
{
	ASSERT( 0 != g_pSpiIrqhandles[1] );
	
	SPI_Handler( g_pSpiIrqhandles[1] );
}


























