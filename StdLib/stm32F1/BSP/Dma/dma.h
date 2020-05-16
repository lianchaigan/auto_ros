/*****************************************************************************
 @Project	: 
 @File 		: dma.h
 @Details  	: dma
 @Author	: 
 @Hardware	: 
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
******************************************************************************/

#ifndef __DMA_DOT_H__
#define __DMA_DOT_H__

/*****************************************************************************
 Define
******************************************************************************/

#define DMA1_USART3_TX_CH			2U
#define DMA1_USART3_RX_CH			3U

#define DMA1_SPI1_RX_CH				2U
#define DMA1_SPI1_TX_CH				3U

#define DMA1_USART1_TX_CH			4U
#define DMA1_USART1_RX_CH			5U

#define DMA1_SPI2_RX_CH				4U
#define DMA1_SPI2_TX_CH				5U

#define DMA1_I2C2_TX_CH				4U
#define DMA1_I2C2_RX_CH				5U

#define DMA1_USART2_RX_CH			6U
#define DMA1_USART2_TX_CH			7U

#define DMA1_I2C1_TX_CH				6U
#define DMA1_I2C1_RX_CH				7U


/*****************************************************************************
 Typedef 
******************************************************************************/
typedef void DMA_CB_HALF( void *pContext );
typedef void DMA_CB_COMP( void *pContext );
typedef void DMA_CB_ERROR( void *pContext );

typedef enum _tagDma_Config
{
	DMA_DIR_MEM_TO_PERI,
	DMA_DIR_PERI_TO_MEM,
	DMA_DIR_MEM_TO_MEM,

	DMA_DATA_ALIGN_BYTE,
	DMA_DATA_ALIGN_HALF_WORD,
	DMA_DATA_ALIGN_WORD,

	DMA_MEM_INC_EN,
    DMA_MEM_INC_DIS,
	DMA_PERI_INC_EN,
	DMA_PERI_INC_DIS
}DMA_CONFIG;


typedef struct _tagDma_Handle
{
	void	*pDma;
    void	*pStream;
	int		nStream;

	void	*pLinkContext;

	DMA_CONFIG Direction;

	DMA_CB_HALF  *pfHalf;
	DMA_CB_COMP  *pfComp;
	DMA_CB_ERROR *pfError;
}DMA_HANDLE, *PDMA_HANDLE;


/*****************************************************************************
 @Description 	: 
 
 @Param			: 

 @Requirement	:  
				  
 @Revision		:
******************************************************************************/
void DmaInit(
		DMA_HANDLE	*hHandle,
		int			nPort,
		uint8_t		nStream,
		DMA_CONFIG	Direction,
		DMA_CONFIG	MemIncrement,
		DMA_CONFIG	MemDataSize,
		DMA_CONFIG	PeriIncrement,
		DMA_CONFIG	PeriphDataSize );


void DmaStart(
		DMA_HANDLE	*hHandle,
		uint32_t	SrcAddr,
		uint32_t	DestAddr,
		uint32_t	nSize );

void
DmaAddCallback(
	DMA_HANDLE	*hHandle,
	DMA_CB_HALF *pfcbHalf,
	DMA_CB_COMP *pfcbComp,
	DMA_CB_ERROR *pfcbError );

#endif /* __DMA_DOT_H__ */




















































