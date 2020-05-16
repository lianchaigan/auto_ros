/*****************************************************************************
 @Project	: SEP150 - Capsule
 @File 		: spim.c
 @Details  	: SPI Master hardware layer                    
 @Author	: lcgan
 @Hardware	: LPC1768
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
   
******************************************************************************/
#include <Common.h>
#include "math.h"
#include "Hal.h"

#ifdef _DMA_EN
	#include "Dma.h"
#endif

#include "spim.h"


/*****************************************************************************
 Define
******************************************************************************/
#define TOTAL_SPI			5U

#define SPI_CAST_8BIT( x )	(*(__IO uint8_t *)(x))


/*****************************************************************************
 Type definition
******************************************************************************/


/*****************************************************************************
 Global Variables
******************************************************************************/

/*****************************************************************************
 Const Local Variables
******************************************************************************/


/*****************************************************************************
 Local Variables
******************************************************************************/
static volatile PSPIM_HANDLE g_pSpiIrqhandles[TOTAL_SPI] = { 0, 0, 0, 0, 0 };

#ifdef _DMA_EN
	static void spi_cbDmaTxHalf( void *pContext );
	static void spi_cbDmaRxHalf( void *pContext );
	static void spi_cbDmaTxComp( void *pContext );
	static void spi_cbDmaRxComp( void *pContext );
	static void spi_cbDmaTxError( void *pContext );
	static void spi_cbDmaRxError( void *pContext );
#endif

/*****************************************************************************
 Local Helpers
******************************************************************************/
#ifdef _DMA_EN
	static void	spi_DmaTx( void *pHandle, void const *pData, int nSize );
	static void	spi_DmaRx( void *pHandle, void *pData, int nSize );
#endif

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
	int				apb = 0;
	uint32_t		clock;
	int div;
	int prescale;
	PSPIM_HANDLE 	handle = (PSPIM_HANDLE)pHandle;

	ASSERT( 0 != pHandle );
	ASSERT( 0 != nSpeed );
    ASSERT( 0 != nPort );
	
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
		
		case 3:
			spi = SPI3;
			irq = SPI3_IRQn;
			apb = 1;
		break;
		
		case 4:
			spi = SPI4;
			irq = SPI4_IRQn;
			apb = 2;
		break;
	
		case 5:
			spi = SPI5;
			irq = SPI5_IRQn;
			apb = 2;
		break;
		default:
			return SPIM_E_INVALID_PORT;
	}
	
	handle->pSpi = spi;
	handle->Irq = irq;
	handle->Apb = apb;

	g_pSpiIrqhandles[nPort-1] = handle;		/* keep the handle for interrupt use */
	
	spi->CR1 &= ~SPI_CR1_SPE;
	spi->I2SCFGR &= ~SPI_I2SCFGR_I2SMOD;	/* SPI mode is selected */
	
	//tmp |= SPI_CR1_MSTR | SPI_CR1_SSM;		/* Master mode */
	tmp |= SPI_CR1_MSTR;	
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
	/* Different timer has different APB bus */
	if( 1 == apb )
	{
		/* Re-calculate current bus clock. User may change it in Hal.c */
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

			default:
			break;
		}
	}
	else if( 2 == apb )
	{
		/* Re-calculate current bus clock. User may change it in Hal.c */
		switch( RCC->CFGR & RCC_CFGR_PPRE2 )
		{
			case RCC_CFGR_PPRE2_DIV1:
				clock = SystemCoreClock;
			break;
		
			case RCC_CFGR_PPRE2_DIV2:
				clock = SystemCoreClock>>1U;
			break;
		
			case RCC_CFGR_PPRE2_DIV4:
				clock = SystemCoreClock>>2U;
			break;
		
			case RCC_CFGR_PPRE2_DIV8:
				clock = SystemCoreClock>>3U;
			break;
		
			case RCC_CFGR_PPRE1_DIV16:
				clock = SystemCoreClock>>4U;
			break;

			default:
			break;
		}
	}
	else
	{
		ASSERT( FALSE );
	}

	prescale = clock / nSpeed;
	div = 0;
	do
	{
		prescale /= 2;
		div++;
	}while( 1 != prescale );

	tmp &= ~SPI_CR1_BR;
	tmp |= (div<<SPI_CR1_BR_Pos);
	spi->CR1 = tmp;
	
	spi->CR2 |= SPI_CR2_SSOE;
    //spi->CR2 |= SPI_CR2_SSOE;
	spi->CR2 &= ~SPI_CR2_DS;
	
	switch( DATA_SIZE )
	{
		case SPI_DATA_SIZE_4:
			spi->CR2 |= (3U<<SPI_CR2_DS_Pos);
            handle->Datasize = 4;
		break;
		
		case SPI_DATA_SIZE_5:
			spi->CR2 |= (4U<<SPI_CR2_DS_Pos);
            handle->Datasize = 5;
		break;
		
		case SPI_DATA_SIZE_6:
			spi->CR2 |= (5U<<SPI_CR2_DS_Pos);
            handle->Datasize = 6;
		break;
		
		case SPI_DATA_SIZE_7:
			spi->CR2 |= (6U<<SPI_CR2_DS_Pos);
            handle->Datasize = 7;
		break;
		
		case SPI_DATA_SIZE_8:
			spi->CR2 |= (7U<<SPI_CR2_DS_Pos);
            handle->Datasize = 8;
		break;
		
		case SPI_DATA_SIZE_9:
			spi->CR2 |= (8U<<SPI_CR2_DS_Pos);
            handle->Datasize = 9;
            handle->bTransferWord = TRUE;
		break;
		
		case SPI_DATA_SIZE_10:
			spi->CR2 |= (9U<<SPI_CR2_DS_Pos);
            handle->Datasize = 10;
            handle->bTransferWord = TRUE;
		break;
		
		case SPI_DATA_SIZE_11:
			spi->CR2 |= (10U<<SPI_CR2_DS_Pos);
            handle->Datasize = 11;
            handle->bTransferWord = TRUE;
		break;
		
		case SPI_DATA_SIZE_12:
			spi->CR2 |= (11U<<SPI_CR2_DS_Pos);
            handle->Datasize = 12;
            handle->bTransferWord = TRUE;
		break;
		
		case SPI_DATA_SIZE_13:
			spi->CR2 |= (12U<<SPI_CR2_DS_Pos);
            handle->Datasize = 13;
            handle->bTransferWord = TRUE;
		break;
		
		case SPI_DATA_SIZE_14:
			spi->CR2 |= (13U<<SPI_CR2_DS_Pos);
            handle->Datasize = 14;
            handle->bTransferWord = TRUE;
		break;
		
		case SPI_DATA_SIZE_15:
			spi->CR2 |= (14U<<SPI_CR2_DS_Pos);
            handle->Datasize = 15;
            handle->bTransferWord = TRUE;
		break;
		
		case SPI_DATA_SIZE_16:
			spi->CR2 |= (15U<<SPI_CR2_DS_Pos);
            handle->Datasize = 16;
            handle->bTransferWord = TRUE;
		break;
		
		default:
			return SPIM_E_INVALID_DATASIZE;
	}
	
	if( handle->Datasize > 8 )
	{
		spi->CR2 &= ~SPI_CR2_FRXTH;
	}
	else
	{
		spi->CR2 |= SPI_CR2_FRXTH;
	}

	NVIC_EnableIRQ( irq );
	
	return SPIM_STS_OK;
}


void
SpimLinkDMA(
	void            *pHandle,
	void			*pDmaTxHandle,
	void			*pDmaRxHandle
	)
{
#ifdef _DMA_EN
	PSPIM_HANDLE handle = (PSPIM_HANDLE)pHandle;
    SPI_TypeDef *spi 	= (SPI_TypeDef *)handle->pSpi;
	DMA_HANDLE	*dmaTxHandle = (DMA_HANDLE *)pDmaTxHandle;
    DMA_HANDLE	*dmaRxHandle = (DMA_HANDLE *)pDmaRxHandle;

	ASSERT( 0 != pHandle );

	handle->pDmaTx = pDmaTxHandle;
	handle->pDmaRx = pDmaRxHandle;

	if( 0 != pDmaTxHandle )
	{
		dmaTxHandle->pLinkContext = handle;
		DmaAddCallback(
			pDmaTxHandle,
			spi_cbDmaTxHalf,
			spi_cbDmaTxComp,
			spi_cbDmaTxError );
	}


	if( 0 != pDmaRxHandle )
	{
		dmaRxHandle->pLinkContext = handle;
        DmaAddCallback(
			pDmaRxHandle,
			spi_cbDmaRxHalf,
			spi_cbDmaRxComp,
			spi_cbDmaRxError );
	}
#endif
}


void
SpimDeLinkDMA(
	void            *pHandle,
	void			*pDmaTxHandle,
	void			*pDmaRxHandle
	)
{
#ifdef _DMA_EN
	PSPIM_HANDLE handle = (PSPIM_HANDLE)pHandle;
    SPI_TypeDef *spi 	= (SPI_TypeDef *)handle->pSpi;
	DMA_HANDLE	*dmaTxHandle = (DMA_HANDLE *)pDmaTxHandle;
    DMA_HANDLE	*dmaRxHandle = (DMA_HANDLE *)pDmaRxHandle;

	ASSERT( 0 != pHandle );

	handle->pDmaTx = 0;
	handle->pDmaRx = 0;

    DmaStop( pDmaTxHandle );
    DmaStop( pDmaRxHandle );
	
#endif
}


int SpimSetDataSize( void *pHandle, SPIM_CFG DATA_SIZE )
{
    PSPIM_HANDLE 	handle = (PSPIM_HANDLE)pHandle;
	uint32_t		tmp;
	SPI_TypeDef 	*spi 	= (SPI_TypeDef *)handle->pSpi;

	ASSERT( 0 != pHandle );

	tmp = spi->CR1;
	spi->CR1 &= ~SPI_CR1_SPE;
	spi->CR2 &= ~SPI_CR2_DS;
	
	switch( DATA_SIZE )
	{
			case SPI_DATA_SIZE_4:
			spi->CR2 |= (3U<<SPI_CR2_DS_Pos);
            handle->Datasize = 4;
            handle->bTransferWord = FALSE;
		break;
		
		case SPI_DATA_SIZE_5:
			spi->CR2 |= (4U<<SPI_CR2_DS_Pos);
            handle->Datasize = 5;
            handle->bTransferWord = FALSE;
		break;
		
		case SPI_DATA_SIZE_6:
			spi->CR2 |= (5U<<SPI_CR2_DS_Pos);
            handle->Datasize = 6;
            handle->bTransferWord = FALSE;
		break;
		
		case SPI_DATA_SIZE_7:
			spi->CR2 |= (6U<<SPI_CR2_DS_Pos);
            handle->Datasize = 7;
            handle->bTransferWord = FALSE;
		break;
		
		case SPI_DATA_SIZE_8:
			spi->CR2 |= (7U<<SPI_CR2_DS_Pos);
            handle->Datasize = 8;
            handle->bTransferWord = FALSE;
		break;
		
		case SPI_DATA_SIZE_9:
			spi->CR2 |= (8U<<SPI_CR2_DS_Pos);
            handle->Datasize = 9;
            handle->bTransferWord = TRUE;
		break;
		
		case SPI_DATA_SIZE_10:
			spi->CR2 |= (9U<<SPI_CR2_DS_Pos);
            handle->Datasize = 10;
            handle->bTransferWord = TRUE;
		break;
		
		case SPI_DATA_SIZE_11:
			spi->CR2 |= (10U<<SPI_CR2_DS_Pos);
            handle->Datasize = 11;
            handle->bTransferWord = TRUE;
		break;
		
		case SPI_DATA_SIZE_12:
			spi->CR2 |= (11U<<SPI_CR2_DS_Pos);
            handle->Datasize = 12;
            handle->bTransferWord = TRUE;
		break;
		
		case SPI_DATA_SIZE_13:
			spi->CR2 |= (12U<<SPI_CR2_DS_Pos);
            handle->Datasize = 13;
            handle->bTransferWord = TRUE;
		break;
		
		case SPI_DATA_SIZE_14:
			spi->CR2 |= (13U<<SPI_CR2_DS_Pos);
            handle->Datasize = 14;
            handle->bTransferWord = TRUE;
		break;
		
		case SPI_DATA_SIZE_15:
			spi->CR2 |= (14U<<SPI_CR2_DS_Pos);
            handle->Datasize = 15;
            handle->bTransferWord = TRUE;
		break;
		
		case SPI_DATA_SIZE_16:
			spi->CR2 |= (15U<<SPI_CR2_DS_Pos);
            handle->Datasize = 16;
            handle->bTransferWord = TRUE;
		break;
		
		default:
			return SPIM_E_INVALID_DATASIZE;
	}
	
   spi->CR1 = tmp;

    return SPIM_STS_OK;
}


int SpimSetSpeed( void *pHandle, int nSpeed )
{
	int clock;
	int tmp;
	int prescale;
    int div;

	ASSERT( 0 != pHandle );
	ASSERT( 0 != nSpeed );

    PSPIM_HANDLE 	handle = (PSPIM_HANDLE)pHandle;
	SPI_TypeDef 	*spi 	= (SPI_TypeDef *)handle->pSpi;


	/* Different timer has different APB bus */
	if( 1 == handle->Apb )
	{
		/* Re-calculate current bus clock. User may change it in Hal.c */
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

			default:
			break;
		}
	}
	else if( 2 == handle->Apb )
	{
		/* Re-calculate current bus clock. User may change it in Hal.c */
		switch( RCC->CFGR & RCC_CFGR_PPRE2 )
		{
			case RCC_CFGR_PPRE2_DIV1:
				clock = SystemCoreClock;
			break;
		
			case RCC_CFGR_PPRE2_DIV2:
				clock = SystemCoreClock>>1U;
			break;
		
			case RCC_CFGR_PPRE2_DIV4:
				clock = SystemCoreClock>>2U;
			break;
		
			case RCC_CFGR_PPRE2_DIV8:
				clock = SystemCoreClock>>3U;
			break;
		
			case RCC_CFGR_PPRE1_DIV16:
				clock = SystemCoreClock>>4U;
			break;

			default:
			break;
		}
	}
	else
	{
		ASSERT( FALSE );
	}

	prescale = clock / nSpeed;
	div = 0;
	do
	{
		prescale /= 2;
		div++;

		
	}while( 0 != prescale );

	spi->CR1 &= ~SPI_CR1_SPE;

	tmp = spi->CR1;
	tmp &= ~SPI_CR1_BR;
	tmp |= ((div-1)<<SPI_CR1_BR_Pos);
	spi->CR1 = tmp;

	spi->CR1 |= SPI_CR1_SPE;

    clock = clock/ pow(2.0, div);

	return clock;
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
	SPI_TypeDef 	*spi 	= (SPI_TypeDef *)handle->pSpi;
	
	ASSERT( 0 != phandle );
	ASSERT( 0 != pTxBuf );
	ASSERT( 0 != nSize );
	
		
	handle->nTxCount	= 0;
	handle->nRxCount	= 0;

	while( 0 != (spi->SR&SPI_SR_BSY) )
	{
	}
	
	/* This driver need to modify if datasize more than 8 bits */
	if( FALSE == handle->bTransferWord )
	{
		handle->pRxBuf 	= (char *)pRxBuf;
		handle->pTxBuf 	= (char *)pTxBuf;
		handle->nSize	= nSize;

#ifdef _DMA_EN
		if( 0 != handle->pDmaRx )
		{
			if( 0 != pRxBuf )
			{
				spi_DmaRx( handle, handle->pRxBuf, nSize );
			}
			else
			{
				spi_DmaRx( handle, &handle->Dummy, nSize );
			}
		}

        if( 0 != handle->pDmaTx )
        {
            spi_DmaTx( handle, handle->pTxBuf, nSize );

            return;
        }
        else
#endif
        {
			spi->CR2 |= SPI_CR2_RXNEIE;
            SPI_CAST_8BIT( &spi->DR ) = handle->pTxBuf[0];
        }
	}
	else
	{
		handle->pRxWordBuf 	= (uint16_t *)pRxBuf;
		handle->pTxWordBuf 	= (uint16_t *)pTxBuf;
		handle->nSize	= nSize>>1;

#ifdef _DMA_EN
		if( 0 != handle->pDmaRx )
		{
			if( 0 != pRxBuf )
			{
				spi_DmaRx( handle, handle->pRxWordBuf, nSize );
			}
			else
			{
				spi_DmaRx( handle, &handle->Dummy, nSize );
			}
		}

        if( 0 != handle->pDmaTx )
        {
            spi_DmaTx( handle, handle->pTxWordBuf, nSize );
            
            return;
        }
        else
#endif
        {
			spi->CR2 |= SPI_CR2_RXNEIE;
            spi->DR = handle->pTxWordBuf[0];
        }
	}

	handle->nTxCount++;
	
	spi->CR2 |=  SPI_CR2_TXEIE;		/* Enable interrupts */
	spi->CR1 |= SPI_CR1_SPE;
}


/*****************************************************************************
 Callback functions
******************************************************************************/
#ifdef _DMA_EN
static void spi_cbDmaTxHalf( void *pContext )
{
}


static void spi_cbDmaRxHalf( void *pContext )
{
}


static void spi_cbDmaTxComp( void *pContext )
{

}


static void spi_cbDmaRxComp( void *pContext )
{
    PSPIM_HANDLE	handle 	= (PSPIM_HANDLE)pContext;

	/* Callback to application */
	if( 0 != handle->pfDone )	/* The pointer maybe 0 */
	{
		handle->pfDone();
	}
}


static void spi_cbDmaTxError( void *pContext )
{
}


static void spi_cbDmaRxError( void *pContext )
{
}
#endif

/*****************************************************************************
 Local functions
******************************************************************************/
#ifdef _DMA_EN
static void	spi_DmaTx( void *pHandle, void const *pData, int nSize )
{
	PSPIM_HANDLE	handle 	= (PSPIM_HANDLE)pHandle;
	SPI_TypeDef 	*spi 	= handle->pSpi;
	int size = nSize;

	ASSERT( 0 != pHandle );
    ASSERT( 0 != pData );
	ASSERT( 0 != nSize );

	spi->CR2 &= ~SPI_CR2_TXDMAEN;
    spi->CR1 &= ~SPI_CR1_SPE;

	spi->CR2 &= ~SPI_CR2_LDMATX;

	if( 16 == handle->Datasize )
	{
		if ((nSize & 0x1U) == 0x0U)
		{
			spi->CR2 &= ~SPI_CR2_LDMATX;
			size = nSize>>1;
		}
		else
		{
			spi->CR2 |= SPI_CR2_LDMATX;
			size = (nSize>>1) + 1;
		}
	}

	DmaStart( handle->pDmaTx, (uint32_t)pData, (uint32_t)(&spi->DR), size );
    spi->CR1 |= SPI_CR1_SPE;
    spi->CR2 |= SPI_CR2_TXDMAEN;
}


static void	spi_DmaRx( void *pHandle, void *pData, int nSize )
{
	PSPIM_HANDLE	handle 	= (PSPIM_HANDLE)pHandle;
	SPI_TypeDef 	*spi 	= handle->pSpi;
	int size = nSize;

	ASSERT( 0 != pHandle );
    ASSERT( 0 != pData );
	ASSERT( 0 != nSize );

	spi->CR2 &= ~SPI_CR2_RXDMAEN;
    spi->CR1 &= ~SPI_CR1_SPE;

	if( handle->Datasize > 8 )
	{
		spi->CR2 &= ~SPI_CR2_FRXTH;
	}
	else
	{
		spi->CR2 |= SPI_CR2_FRXTH;
	}

	spi->CR2 &= ~SPI_CR2_LDMATX;

	if( 16 == handle->Datasize )
	{
		if ((nSize & 0x1U) == 0x0U)
		{
			spi->CR2 &= ~SPI_CR2_LDMARX;
            size = nSize>>1;
		}
		else
		{
			spi->CR2 |= SPI_CR2_LDMARX;
			size = (size>>1) + 1;
		}
	}

	DmaStart( handle->pDmaRx, (uint32_t)(&spi->DR), (uint32_t)pData, size );

	spi->CR2 |= SPI_CR2_RXDMAEN;
}

#endif

static void SPI_Handler( PSPIM_HANDLE pHandle )
{
	SPI_TypeDef *spi = pHandle->pSpi;
	int status = spi->SR;
	uint16_t data;
	
	ASSERT( 0 != pHandle );
	
	//if( 0 == (status & SPI_SR_BSY) )
	{
		if( 0 != (status & SPI_SR_RXNE) )
		{
			data = spi->DR;

			if( FALSE == pHandle->bTransferWord )
			{
				if( 0 != pHandle->pRxBuf )
				{
					pHandle->pRxBuf[pHandle->nRxCount] = data;
				}
			}
			else
			{
            	if( 0 != pHandle->pRxWordBuf )
				{
					pHandle->pRxWordBuf[pHandle->nRxCount] = data;
				}
			}

			pHandle->nRxCount++;

			if( pHandle->nRxCount >= pHandle->nSize )
			{
				spi->CR2 &= ~SPI_CR2_RXNEIE;
				spi->CR1 &= ~SPI_CR1_SPE;
				
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
				if( FALSE == pHandle->bTransferWord )
				{
					/* Write to data register to transfer */
					SPI_CAST_8BIT( &spi->DR ) = pHandle->pTxBuf[pHandle->nTxCount];
				}
				else
				{
                	/* Write to data register to transfer */
					spi->DR = pHandle->pTxWordBuf[pHandle->nTxCount];
				}

				pHandle->nTxCount++;
			}
		}
	}
//	else
//	{
//		while(1);
//	}
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


void SPI3_IRQHandler( void )
{
	ASSERT( 0 != g_pSpiIrqhandles[2] );
	
	SPI_Handler( g_pSpiIrqhandles[2]  );
}


void SPI4_IRQHandler( void )
{
	ASSERT( 0 != g_pSpiIrqhandles[3] );
	
	SPI_Handler( g_pSpiIrqhandles[3] );
}


void SPI5_IRQHandler( void )
{
	ASSERT( 0 != g_pSpiIrqhandles[4] );
	
	SPI_Handler( g_pSpiIrqhandles[4] );
}


























