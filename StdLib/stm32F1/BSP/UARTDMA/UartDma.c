/******************************************************************************
 @filename		Uart.c

 @project		UART hardware driver
 
 @description		
 
 @version		1.0.0

 @revision		
******************************************************************************/

#include "Common.h"
#include "dma.h"
#include "SerialDma.h"
#include "Hal.h"


/******************************************************************************
 Define
******************************************************************************/
#define TOTAL_PORT				3U


/******************************************************************************
 Export functions
******************************************************************************/
static int UartInit( void *pHandle, uint8_t nPort, uint32_t nBaud, BOOL	bDMAEn );
static void UartSetIrqLevel( void const	*pHandle, uint32_t nLevel );
static int UartSetBaud( void const *pHandle, uint32_t nBaud );
static int UartSetCfg( void const *pHandle, UART_CFG Databit, UART_CFG Parity, UART_CFG Stopbit );
static int UartLoopback( void const *pHandle, int bEnable );
static void UartWrite( void const *pHandle, char Data );
static void	UartDmaTx( void *pHandle, void const *pData, int nSize );
static char UartRead( void const *pHandle );
static void	UartDmaRx( void *pHandle, void *pData, int nSize );
static int UartIsTxEnd( void const *pHandle );
static int UartIsTxReady( void const *pHandle );
static int UartIsRxReady( void const *pHandle );
static void UartTxEnd( void const *pHandle );
static void UartRxEnd( void const *pHandle );
static void UartLock( void const *pHandle );
static void UartUnlock( void const *pHandle );


/******************************************************************************
 Global variables
******************************************************************************/
UART_DRIVER const STM32F103X_UART_DRV = 
{
	UartInit,
	UartSetBaud,
	UartSetCfg,
	UartSetIrqLevel,
	UartLoopback,
	UartWrite,
	UartDmaTx,
	UartRead,
    UartDmaRx,
	UartIsTxEnd,
	UartIsTxReady,
	UartIsRxReady,
	UartTxEnd,
    UartRxEnd,
	UartLock,
	UartUnlock
};


/******************************************************************************
 Local const variables
******************************************************************************/
static int APB_PPRE[] = { 2, 4, 8, 16 };


/******************************************************************************
 Local variables
******************************************************************************/
static volatile PUART_HANDLE g_UartIrqHandles[TOTAL_PORT];
static volatile int g_bToggle = 0;

/******************************************************************************
 Implementations
******************************************************************************/
static int
UartInit(
	void		*pHandle,
	uint8_t		nPort,
	uint32_t	nBaud,
	BOOL		bDMAEn
	)
{
	USART_TypeDef 		*uart;
	IRQn_Type 			irq;
	int32_t				div;
	double 				frac;
	int32_t				fdr;
	int32_t				clock = 0;
	int32_t				prescale = 0;
	UART_HANDLE 		*handle = (UART_HANDLE *)pHandle;
	
	ASSERT( 0 != pHandle );
	ASSERT( 0 != nPort );
	

	/* Determined base port pointer and 
	   Enable the clock to the selected UART */
	switch( nPort )
	{
		case 1:
			uart = USART1;
			irq = USART1_IRQn;
		break;

		case 2:
			uart = USART2;
			irq = USART2_IRQn;
		break;

		case 3:
			uart = USART3;
			irq = USART3_IRQn;
		break;
			
		default:
			return UART_STS_NO_PORT;
	}
	
	/* Store UART parameters */
	handle->pUART 	= uart;
	handle->Irq 	= irq;

	/* Keep a copy of handle for the port */
	g_UartIrqHandles[nPort-1] = handle;
	
	/* Enable UART */
	uart->CR1 &= ~USART_CR1_UE;

    switch( nPort )
	{
		case 1:
			/* APB1 clock */
			prescale = (RCC->CFGR&RCC_CFGR_PPRE1)>>8U;

			if( 0 != (prescale&0x4) )
			{
				prescale &= ~0x04; 
				clock = SystemCoreClock / APB_PPRE[prescale];
			}
			else
			{
				clock = SystemCoreClock;
			}
		break;

		default:
			prescale = (RCC->CFGR&RCC_CFGR_PPRE2)>>11U;

			if( 0 != (prescale&0x4) )
			{
				prescale &= ~0x04; 
				clock = SystemCoreClock / APB_PPRE[prescale];
			}
			else
			{
				clock = SystemCoreClock;
			}
		break;
	}
	
	/* Calculate the clock frequency into UART */
	div = clock / (16* nBaud); 
	frac = (double)(clock) / (double)(16 * nBaud); 
	frac = frac - (double)div;
	fdr = frac*16.0;

	if( fdr<15 )
	{
		uart->BRR = (div<<4U) | fdr;
	}
	else
	{
		uart->BRR = div<<4U;
	}

	if( FALSE == bDMAEn )
	{
		/* Turn on transmit empty and receive ready interrupt */
		uart->CR1 |= USART_CR1_RXNEIE;

		/* Enable UART NVIC interrupt */
		NVIC_EnableIRQ( irq );
	}

	/* Turn on transmitter and receiver */
	uart->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;

	return UART_STS_OK;
}


static int
UartSetBaud(
	void const 	*pHandle,
	uint32_t 	nBaud
	)
{
	int32_t	div;
	UART_HANDLE const *handle = (UART_HANDLE const *)pHandle;
	USART_TypeDef *uart = (USART_TypeDef *)handle->pUART;
	
	ASSERT( 0 != pHandle );
	
	NVIC_DisableIRQ( (IRQn_Type)handle->Irq );
	
	div = SystemCoreClock / (16U * nBaud); 
	uart->BRR = div<<4U;
	
	uart->CR1 |= USART_CR1_RE | USART_CR1_TE;
	
#if 0
	/* Enable UART NVIC interrupt */
	NVIC_EnableIRQ( handle->Irq );
#endif
	return UART_STS_OK;
}


static int
UartSetCfg(
	void const 	*pHandle,
	UART_CFG 	Databit,
	UART_CFG 	Parity,
	UART_CFG 	Stopbit
	)
{
	UART_HANDLE const 	*handle = (UART_HANDLE const *)pHandle;
	USART_TypeDef 	*uart 	= (USART_TypeDef *)handle->pUART;

	ASSERT( 0 != pHandle );
	
    uart->CR1 &= ~USART_CR1_UE;
	uart->CR1 &= ~USART_CR1_PCE;
	uart->CR2 &= ~(USART_CR2_CPOL | USART_CR2_STOP);
	//uart->CR2 |= USART_CR2_CPHA;


    /* Configure the UART's databit */
    switch( Databit )
	{
		case UART_BITS_8:
			uart->CR1 &= ~USART_CR1_M;
		break;
			
		case UART_BITS_9:
			uart->CR1 |= USART_CR1_M;
		break;
		
		default:
			return UART_STS_NO_CFG;
    }
	
	/* Configure parity */
    switch( Parity )
	{
		case UART_NONE:
			uart->CR1 &= ~USART_CR1_PCE;	/* Disable parity */
		break;
			
		case UART_EVEN:
			uart->CR1 |= USART_CR1_PCE;
			uart->CR1 &= ~USART_CR1_PS;		/* Even parity */
		break;

		case UART_ODD:
			uart->CR1 |= USART_CR1_PCE
						| USART_CR1_PS;		/* Odd parity */
		break;

		default:
			return UART_STS_NO_CFG;
    }

	/* Configure stopbit */
	uart->CR2 &= ~USART_CR2_STOP;
    switch( Stopbit )
	{
		case UART_DEFAULT:
		case UART_ONE:
		break;
		
		case UART_TWO:
			uart->CR2 |= USART_CR2_STOP_1;
		break;
		
		default:
			return UART_STS_NO_CFG;
    }

	uart->CR1 |= USART_CR1_UE;
	return UART_STS_OK;
}


static void
UartSetIrqLevel(
	void const *pHandle,
	uint32_t 	nLevel
	)
{
	UART_HANDLE const *handle = (UART_HANDLE const *)pHandle;
	
	ASSERT( 0 != pHandle );
	
	NVIC_SetPriority( (IRQn_Type)handle->Irq, nLevel );
}


static int
UartLoopback(
	void const *pHandle,
	int 		bEnable
	)
{
	ASSERT( 0 != pHandle );
	
	/* No implemantation. LPC17XX not supported */
	return UART_STS_OK;
}


static void
UartWrite(
	void const 	*pHandle,
	char 		Data
	)
{
	UART_HANDLE const 	*handle = (UART_HANDLE const *)pHandle;
	USART_TypeDef 	*uart 	= (USART_TypeDef *)handle->pUART;

	ASSERT( 0 != pHandle );
	
	/* Write data to transmit holder register */
	uart->DR = Data;
	
	/* Enable interrupt */
	uart->CR1 |= USART_CR1_TXEIE;
}


static void	UartDmaTx( void *pHandle, void const *pData, int nSize )
{
	UART_HANDLE const 	*handle = (UART_HANDLE const *)pHandle;
    USART_TypeDef 	*uart 	= (USART_TypeDef *)handle->pUART;

	ASSERT( 0 != pHandle );
    ASSERT( 0 != pData );
	ASSERT( 0 != nSize );

	DmaStart( handle->pDmaTx, (uint32_t)pData, (uint32_t)(&uart->DR), nSize );

	uart->SR &= ~USART_SR_TC;
	uart->CR3 |= USART_CR3_DMAT;
}


static void	UartDmaRx( void *pHandle, void *pData, int nSize )
{
	UART_HANDLE const 	*handle = (UART_HANDLE const *)pHandle;
    USART_TypeDef 	*uart 	= (USART_TypeDef *)handle->pUART;

	ASSERT( 0 != pHandle );
    ASSERT( 0 != pData );
	ASSERT( 0 != nSize );

	DmaStart( handle->pDmaRx, (uint32_t)(&uart->DR), (uint32_t)pData, nSize );

	uart->SR &= ~USART_SR_ORE;
	uart->CR3 |= USART_CR3_DMAR;
}


static char UartRead( void const *pHandle )
{
	UART_HANDLE const 	*handle = (UART_HANDLE const *)pHandle;
	USART_TypeDef 	*uart 	= (USART_TypeDef *)handle->pUART;

	ASSERT( 0 != pHandle );
	
	return uart->DR;
}


static int UartIsTxEnd( void const *pHandle )
{
	UART_HANDLE const 	*handle = (UART_HANDLE const *)pHandle;
	USART_TypeDef 	*uart 	= (USART_TypeDef *)handle->pUART;
	
	ASSERT( 0 != pHandle );
	
	return ((uart->CR1 & USART_CR1_TXEIE) ? FALSE : TRUE);
}


static int UartIsTxReady( void const *pHandle )
{
	UART_HANDLE const 	*handle = (UART_HANDLE const *)pHandle;
	USART_TypeDef 	*uart 	= (USART_TypeDef *)handle->pUART;
	
	ASSERT( 0 != pHandle );
	
    return (( 0 != (uart->SR & (USART_SR_TC | USART_SR_TXE)) ) ? TRUE : FALSE);
}


static int UartIsRxReady( void const *pHandle )
{
	UART_HANDLE const 	*handle = (UART_HANDLE const *)pHandle;
	USART_TypeDef 	*uart 	= (USART_TypeDef *)handle->pUART;
	
	ASSERT( 0 != pHandle );
	
    return ((uart->SR & USART_SR_RXNE) ? TRUE : FALSE);
}


static void UartTxEnd( void const *pHandle )
{
	UART_HANDLE const 	*handle = (UART_HANDLE const *)pHandle;
	USART_TypeDef 	*uart 	= (USART_TypeDef *)handle->pUART;
	
	ASSERT( 0 != pHandle );
	
	if( 0 != handle->pDmaTx )
	{
		/* Disable DMA Transmitter */
		uart->CR3 &= ~USART_CR3_DMAT;
	}
	else
	{
		/* Disable interrupt */
		uart->CR1 &= ~(USART_CR1_TXEIE | USART_CR1_TCIE);
	}
}


static void UartRxEnd( void const *pHandle )
{
	UART_HANDLE const 	*handle = (UART_HANDLE const *)pHandle;
	USART_TypeDef 	*uart 	= (USART_TypeDef *)handle->pUART;

	if( 0 != handle->pDmaRx )
	{
		/* Disable DMA Transmitter */
		uart->CR3 &= ~USART_CR3_DMAR;
	}
	else
	{
		/* Disable interrupt */
		uart->CR1 &= ~USART_CR1_RXNEIE;
	}
}


static void UartLock( void const *pHandle )
{
	UART_HANDLE const *handle = (UART_HANDLE const *)pHandle;
	
	ASSERT( 0 != pHandle );
	
	NVIC_DisableIRQ( (IRQn_Type)handle->Irq );
}


static void UartUnlock( void const *pHandle )
{
	UART_HANDLE const *handle = (UART_HANDLE const *)pHandle;
	
	ASSERT( 0 != pHandle );
	
	NVIC_EnableIRQ( (IRQn_Type)handle->Irq );
}


/******************************************************************************
 Interrupt Service Routines
******************************************************************************/
void USART1_IRQHandler( void )
{
	USART_TypeDef *uart = USART1;
	int status 	= uart->SR;
	
	ASSERT( 0 != g_UartIrqHandles[0] );
	
	/* Check for Tx IRQ */
	if( 0 != (status & USART_SR_TC) )
	{
		UartIsrTx( g_UartIrqHandles[0] );
	}

    /* Check for Rx IRQ */
	if( 0 != (status & (USART_SR_RXNE | USART_SR_ORE)) )
	{
		UartIsrRx( g_UartIrqHandles[0] );
	}
}


void USART2_IRQHandler( void )
{
	USART_TypeDef *uart = USART2;
	int status = uart->SR;
	
	ASSERT( 0 != g_UartIrqHandles[1] );

	/* Check for Tx IRQ */
	if( 0 != (status & USART_SR_TC) )
	{
		UartIsrTx( g_UartIrqHandles[1] );
	}
	
    /* Check for Rx IRQ */
	if( 0 != (status & USART_SR_RXNE) )
	{
		UartIsrRx( g_UartIrqHandles[1] );
	}
}


void USART3_IRQHandler( void )
{
	USART_TypeDef *uart = USART3;
	int status = uart->SR;
	
	ASSERT( 0 != g_UartIrqHandles[2] );
	
	/* Check for Tx IRQ */
	if( 0 != (status & USART_SR_TC) )
	{
		UartIsrTx( g_UartIrqHandles[2] );
	}

    /* Check for Rx IRQ */
	if( 0 != (status & USART_SR_RXNE) )
	{
		UartIsrRx( g_UartIrqHandles[2] );
	}
}




















