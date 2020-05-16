/*****************************************************************************
 @Project	: 
 @File 		: rtc.c
 @Details  	: STM32F103XX rtc driver               
 @Author	: 
 @Hardware	: STM32
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  lcgan     XXXX-XX-X X  		Initial Release
******************************************************************************/
#include <Common.h>
#include "Hal.h"
#include "dma.h"


/*****************************************************************************
 Define
******************************************************************************/
#define TOTAL_DMA		2U
#define TOTAL_STREAM	7U


/*****************************************************************************
 Type definition
******************************************************************************/

#define DMA1_Stream0	DMA1_BASE
#define DMA1_Stream1	DMA1_Channel1_BASE
#define DMA1_Stream2	DMA1_Channel2_BASE
#define DMA1_Stream3	DMA1_Channel3_BASE
#define DMA1_Stream4	DMA1_Channel4_BASE
#define DMA1_Stream5	DMA1_Channel5_BASE
#define DMA1_Stream6	DMA1_Channel6_BASE
#define DMA1_Stream7	DMA1_Channel7_BASE


/*****************************************************************************
 Global Variables
******************************************************************************/


/*****************************************************************************
 Constant Variables
******************************************************************************/
static const DMA_Channel_TypeDef *DMA1_STREAM_TABLE[7] = 
{
	//DMA1_Stream0,
	DMA1_Stream1,
	DMA1_Stream2,
	DMA1_Stream3,
	DMA1_Stream4,
	DMA1_Stream5,
	DMA1_Stream6,
	DMA1_Stream7
};


/*****************************************************************************
 Local Variables
******************************************************************************/
static  DMA_HANDLE *g_pDma1IrqHandles[7];
static  DMA_HANDLE *g_pDma2IrqHandles[4];


/*****************************************************************************
 Local Functions
******************************************************************************/


/*****************************************************************************
 Implementation
******************************************************************************/
void 
DmaInit(
	DMA_HANDLE	*hHandle,
	int			nDMA,
	uint8_t		nStream,
	DMA_CONFIG	Direction,
	DMA_CONFIG	MemIncrement,
    DMA_CONFIG	MemDataSize,
	DMA_CONFIG	PeriIncrement,
	DMA_CONFIG	PeriphDataSize
	)
{
	DMA_TypeDef *dma;
    DMA_Channel_TypeDef *stream;
	uint32_t tmp;

	ASSERT( 0 != hHandle );
	ASSERT( 0 != nDMA );
	ASSERT( nDMA<=TOTAL_DMA );
    ASSERT( nStream<=TOTAL_STREAM );

	switch( nDMA )
	{
		case 1:
			dma = DMA1;
            stream = (DMA_Channel_TypeDef *)DMA1_STREAM_TABLE[nStream-1];

            g_pDma1IrqHandles[nStream-1] = hHandle;
		break;

		default:
		break;
	}

	hHandle->pDma = dma;
	hHandle->nStream = nStream;
	hHandle->pStream = stream;
	hHandle->Direction = Direction;

	/* Disable DMA during configuration */
    stream->CCR &= ~DMA_CCR_EN;

	tmp = stream->CCR;

	/* Clear PL, MSIZE, PSIZE, MINC, PINC, CIRC and DIR bits */
	tmp &= ~(DMA_CCR_CIRC
			| DMA_CCR_MSIZE
			| DMA_CCR_PSIZE
			| DMA_CCR_MINC  
			| DMA_CCR_PINC   
			| DMA_CCR_CIRC   
			| DMA_CCR_DIR
			| DMA_CCR_MEM2MEM
			| DMA_CCR_DIR);

	/* Configure direction */
	switch( Direction )
	{
		case DMA_DIR_MEM_TO_PERI:
			tmp |= DMA_CCR_DIR;
		break;

		case DMA_DIR_PERI_TO_MEM:
			tmp &= ~DMA_CCR_DIR;
		break;

		case DMA_DIR_MEM_TO_MEM:
			tmp |= DMA_CCR_MEM2MEM;
		break;

		default:
		break;
	}

	/* Configure data alignment */
	switch( PeriphDataSize )
	{
    	case DMA_DATA_ALIGN_BYTE:
			tmp &= ~(DMA_CCR_PSIZE_0 | DMA_CCR_PSIZE_1);
		break;

    	case DMA_DATA_ALIGN_HALF_WORD:
			tmp |= DMA_CCR_PSIZE_0;
		break;

    	case DMA_DATA_ALIGN_WORD:
			tmp |= DMA_CCR_PSIZE_1;
		break;

		default:
		break;
	}

	switch( MemDataSize )
	{
    	case DMA_DATA_ALIGN_BYTE:
			tmp &= ~(DMA_CCR_MSIZE_0 | DMA_CCR_MSIZE_1);
		break;

    	case DMA_DATA_ALIGN_HALF_WORD:
			tmp |= DMA_CCR_MSIZE_0;
		break;

    	case DMA_DATA_ALIGN_WORD:
			tmp |= DMA_CCR_MSIZE_1;
		break;

		default:
		break;
	}

	/* configure data auto increment */
	switch( MemIncrement )
	{
		case DMA_MEM_INC_EN:
			tmp |= DMA_CCR_MINC;
		break;

		case DMA_MEM_INC_DIS:
			tmp &= ~DMA_CCR_MINC;
		break;

		default:
		break;
	}

	switch( PeriIncrement )
	{
		case DMA_PERI_INC_EN:
			tmp |= DMA_CCR_PINC;
		break;

		case DMA_PERI_INC_DIS:
			tmp &= ~DMA_CCR_PINC;
		break;

		default:
		break;
	}

	NVIC_EnableIRQ( DMA1_Channel7_IRQn );
	stream->CCR = tmp;
}


void
DmaAddCallback(
	DMA_HANDLE	*hHandle,
	DMA_CB_HALF *pfcbHalf,
	DMA_CB_COMP *pfcbComp,
	DMA_CB_ERROR *pfcbError
	)
{
	ASSERT( 0 != hHandle );

	hHandle->pfHalf = pfcbHalf;
	hHandle->pfComp = pfcbComp;
	hHandle->pfError = pfcbError;
}


void
DmaStart(
	DMA_HANDLE	*hHandle,
	uint32_t	SrcAddr,
	uint32_t	DestAddr,
	uint32_t	nSize
	)
{
	DMA_TypeDef *dma;
	DMA_Channel_TypeDef *stream;

	ASSERT( 0 != hHandle );
	ASSERT( 0 != SrcAddr );
	ASSERT( 0 != DestAddr );
	ASSERT( 0 != nSize );

	dma = (DMA_TypeDef *)hHandle->pDma;
	stream = hHandle->pStream;

	/* Disable DMA during configuration */
    stream->CCR &= ~DMA_CCR_EN;

	/* Clear pending interrupts */
	dma->IFCR = 0x0fU<<((hHandle->nStream-1)*4);

	/* Configure DMA Channel data length */
	stream->CNDTR = nSize;

	/* Memory to Peripheral */

	switch( hHandle->Direction )
	{
		case DMA_DIR_MEM_TO_PERI:
			/* Configure DMA Channel destination address */
			stream->CPAR = DestAddr;

			/* Configure DMA Channel source address */
			stream->CMAR = SrcAddr;
		break;

		case DMA_DIR_PERI_TO_MEM:
			/* Configure DMA Channel source address */
			stream->CPAR = SrcAddr;

			/* Configure DMA Channel destination address */
			stream->CMAR = DestAddr;
		break;

		default:
		break;
	}

    /* Enable the transfer complete interrupt */
    /* Enable the transfer Error interrupt */
    /* Enable the Peripheral */
    stream->CCR |=  DMA_CCR_TCIE 
					| DMA_CCR_HTIE 
					| DMA_CCR_TEIE
					| DMA_CCR_EN;
}


/*****************************************************************************
 Callback functions
******************************************************************************/


/*****************************************************************************
 Local functions
******************************************************************************/
/**
  * @brief  Handles DMA interrupt request.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA Channel.  
  * @retval None
  */
void dma_IRQHandler( DMA_HANDLE	*hHandle )
{
	uint32_t			isr;
	uint32_t			ccr;
	DMA_TypeDef			*dma = (DMA_TypeDef *)hHandle->pDma;
	DMA_Channel_TypeDef *stream = (DMA_Channel_TypeDef *)hHandle->pStream;

	ASSERT( 0 != hHandle );

	isr = dma->ISR;
	ccr = stream->CCR;
  
	/* Half Transfer Complete Interrupt management ******************************/
	if((0 != (isr & (DMA_ISR_HTIF1<<((hHandle->nStream-1)*4)))) && (0 != (ccr & DMA_CCR_HTIE)))
	{
		/* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
		if( 0 == (ccr & DMA_CCR_CIRC) )
		{
			/* Disable the half transfer interrupt */
			stream->CCR &= ~DMA_CCR_HTIE;
		}

		/* Clear the half transfer complete flag */
		dma->IFCR = DMA_IFCR_CHTIF1<<((hHandle->nStream-1)*4);

		/* DMA peripheral state is not updated in Half Transfer */
		/* but in Transfer Complete case */

		if( 0 != hHandle->pfHalf )
		{
		  /* Half transfer callback */
			hHandle->pfHalf( hHandle->pLinkContext );
		}
	}

	/* Transfer Complete Interrupt management ***********************************/
	else if( (0 != (isr & (DMA_ISR_TCIF1<<((hHandle->nStream-1)*4)))) && (0 != (ccr & DMA_CCR_TCIE)))
	{
		if( 0 == (ccr & DMA_CCR_CIRC) )
		{
			/* Disable the transfer complete and error interrupt */
            stream->CCR &= ~(DMA_CCR_TEIE | DMA_CCR_TCIE);
		}

		/* Clear the transfer complete flag */
		dma->IFCR = DMA_IFCR_CTCIF1<<((hHandle->nStream-1)*4);

		if( 0 != hHandle->pfComp )
		{
			/* transfer done callback */
			hHandle->pfComp( hHandle->pLinkContext );
		}
	}

  /* Transfer Error Interrupt management **************************************/
  else if ((0 != (isr & (DMA_ISR_TEIF1<<((hHandle->nStream-1)*4)))) && (0 != (ccr & DMA_CCR_TEIE)))
  {
    /* When a DMA transfer error occurs */
    /* A hardware clear of its EN bits is performed */
    /* Disable ALL DMA IT */
    stream->CCR &= ~(DMA_CCR_TEIE | DMA_CCR_TCIE | DMA_CCR_HTIE);

    /* Clear all flags */
    dma->IFCR = DMA_IFCR_CGIF1<<((hHandle->nStream-1)*4);

	if( 0 != hHandle->pfError )
	{
		/* transfer done callback */
		hHandle->pfError( hHandle->pLinkContext );
	}
  }
  return;
}

/*****************************************************************************
 Interrupt handling
******************************************************************************/
void DMA1_Channel1_IRQHandler( void )
{
	dma_IRQHandler( g_pDma1IrqHandles[0] );
}

void DMA1_Channel2_IRQHandler( void )
{
	dma_IRQHandler( g_pDma1IrqHandles[1] );
}

void DMA1_Channel3_IRQHandler( void )
{
	dma_IRQHandler( g_pDma1IrqHandles[2] );
}

void DMA1_Channel4_IRQHandler( void )
{
	dma_IRQHandler( g_pDma1IrqHandles[3] );
}

void DMA1_Channel5_IRQHandler( void )
{
	dma_IRQHandler( g_pDma1IrqHandles[4] );
}

void DMA1_Channel6_IRQHandler( void )
{
	dma_IRQHandler( g_pDma1IrqHandles[5] );
}

void DMA1_Channel7_IRQHandler( void )
{
	dma_IRQHandler( g_pDma1IrqHandles[6] );
}

#if 0
void DMA2_Channel1_IRQHandler( void )
{
	dma_IRQHandler( g_pDma2IrqHandles[0] );
}


void DMA2_Channel2_IRQHandler( void )
{
	dma_IRQHandler( g_pDma2IrqHandles[1] );
}


void DMA2_Channel3_IRQHandler( void )
{
	dma_IRQHandler( g_pDma2IrqHandles[2] );
}


void DMA2_Channel4_5_IRQHandler( void )
{
	dma_IRQHandler( g_pDma2IrqHandles[3] );
}
#endif

























