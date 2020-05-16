/*****************************************************************************
 @Project	: 
 @File 		: Can.c
 @Details  	:         
 @Author	: 
 @Hardware	: STM32
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
   
******************************************************************************/

#include <Common.h>
#include <string.h>
#include "CanStdConst.h"
#include "CanStd.h"

	
/*****************************************************************************
 Defines 
*****************************************************************************/
typedef CAN_TypeDef CAN;

#define TOTAL_PORT		1U

// Maximum allowed clock tolerance in 1/1024 steps
#define CAN_CLOCK_TOLERANCE             (15U)   // 15/1024 approx. 1.5 %

#define CAN1_FILTER_BANK_NUM            (14UL)


/****** CAN ID Frame Format codes *****/
#define ARM_CAN_ID_IDE_Pos              31UL
#define ARM_CAN_ID_IDE_Msk             (1UL    << ARM_CAN_ID_IDE_Pos)

/****** CAN Identifier encoding *****/
#define ARM_CAN_STANDARD_ID(id)        (id & 0x000007FFUL)                      ///< CAN identifier in standard format (11-bits)
#define ARM_CAN_EXTENDED_ID(id)       ((id & 0x1FFFFFFFUL) | ARM_CAN_ID_IDE_Msk)///< CAN identifier in extended format (29-bits)



/*****************************************************************************
 Public variables
*****************************************************************************/
static	CANSTD_STATUS	CanInit( void	*pHandle );
static	CANSTD_STATUS	CanSpeed(void *pHandle, uint32_t nSpeed );
static	CANSTD_STATUS	CanMode(void const *pHandle, CANSTD_MODE	Mode );
static	BOOL			CanMailBox(
							void	const	*pHandle,
							uint32_t		nId,
							uint32_t 		nMask,
							uint8_t 		nPriority,
							MAILBOXTYPE		Type,
							uint8_t 		nMB );
static void				CanAddFilter(
							void const		*pHandle,
							int				nIdx,
							uint32_t		nId,
							uint32_t		nMask );
static void				CanRemoveFilter( void const	*pHandle, int nIdx );
static CANSTD_STATUS	CanLoopBack( void const *pHandle, BOOL bEnable );
static BOOL				CanTransmit( void *pHandle, CAN_TXMSG const *pMsg );		
static BOOL				CanReceived( void const	*pHandle, uint8_t nIdx, PCAN_RXMSG  pMsg );
static BOOL				CanIsTxEnd( void const *pHandle );
static BOOL				CanIsTxReady( void const *pHandle );
static BOOL				CanIsRxReady( void const *pHandle );
static void 			CanTxEnd( void const *pHandle );
static void 			CanLockRx( void const *pHandle );
static void 			CanUnlockRx( void const *pHandle );
static void				CanLockTx( void const *pHandle );
static void				CanUnlockTx( void const *pHandle );


CANSTD_DRIVER const STM32F1_CAN_DRVIVER = 
{
	&CanInit,
	&CanSpeed,
	&CanMode,
	&CanMailBox,
    &CanAddFilter,
	&CanRemoveFilter,
	&CanLoopBack,
	&CanTransmit,
	&CanReceived,
	&CanIsTxEnd,
	&CanIsTxReady,
	&CanIsRxReady,
	&CanTxEnd,
	&CanLockRx,
	&CanUnlockRx,
	&CanLockTx,
	&CanUnlockTx
};


/*****************************************************************************
 Private types
*****************************************************************************/


/*****************************************************************************
 Private configuration variables
*****************************************************************************/


/*****************************************************************************
 Private variables
*****************************************************************************/
static volatile PCANSTD_HANDLE g_CanIrqHandles[TOTAL_PORT];


/*****************************************************************************
 Private callbacks forward declaration
*****************************************************************************/


/*****************************************************************************
 Private helpers forward declaration
*****************************************************************************/
static CANSTD_STATUS can_SetBitrate( CAN *pCan, int32_t nBitrate );
static CANSTD_STATUS can_SetBittime(
						CAN		*pCan,
						int32_t Bitrate,
						int32_t Clock,
						int32_t ProgSeg,
						int32_t PhaseSeg1,
						int32_t PhaseSeg2,
						int nSjw );


/*****************************************************************************
 Implementations
*****************************************************************************/
static  CANSTD_STATUS CanInit( void *pHandle )
{
	uint8_t			cnt = 0;
	CAN				*can;
	IRQn_Type		irq;
	CANSTD_BITTIME 	param;
	int32_t			mcr = 0;
    CANSTD_STATUS	res;
	int				retry = 0;
	
	CANSTD_HANDLE *handle = (CANSTD_HANDLE *)pHandle;
	ASSERT( 0 != pHandle );
	
	/* Determined base port pointer and 
	   Enable the clock to the selected UART */
	switch( handle->nPort )
	{
		case 1:
			can = CAN1;
            handle->TxIRQn = USB_HP_CAN1_TX_IRQn;
			handle->RxIRQn = USB_LP_CAN1_RX0_IRQn;
		break;

		default:
			return CAN_STD_INVALID_PORT;
	}
	
	/* Store uart parameters */
	handle->pCAN  = can;
	
	/* Keep a copy of handle for the port */
	g_CanIrqHandles[handle->nPort - 1] = handle;

	/* Ensure CAN Bus Power fully reset */
	RCC->APB1RSTR |= RCC_APB1RSTR_CAN1RST;
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	RCC->APB1RSTR &= ~RCC_APB1RSTR_CAN1RST;

	/* Reset CAN controller */
    can->MCR = CAN_MCR_RESET;
    while ( 0U != (can->MCR & CAN_MCR_RESET) ){};

	/* Exit from sleep mode */
	can->MCR &= ~CAN_MCR_SLEEP;

	/* Request initialization */
	can->MCR |= CAN_MCR_INRQ;

	/* Wait the acknowledge */
	while( 0 == (can->MSR&CAN_MSR_INAK) )
	{
		retry++;
		if( retry > 1000 )
		{
			return CAN_STD_HW_TRX_NO_RESP;
		}
	}

	/* Set the time triggered communication mode */
	can->MCR &= ~CAN_MCR_TTCM;

	/* Set the automatic bus-off management */
	can->MCR &= ~CAN_MCR_ABOM;

    /* Set the automatic wake-up mode */
    can->MCR &= ~CAN_MCR_AWUM;
  
    /* Set the no automatic retransmission */
    can->MCR &= ~CAN_MCR_NART;

	can->MCR &= ~CAN_MCR_DBF;

	/* Debug use only */
	//can->BTR |= CAN_BTR_LBKM;

	/* Initialize filter banks */
	can->FMR   =  CAN_FMR_FINIT | (CAN1_FILTER_BANK_NUM << 8);

	can->FA1R &= ~0xffffffffU;     /* Put all filters in inactive mode */
	can->FM1R = 0U;					/* Initialize all filters mode */
	can->FS1R &= ~0xffffffffU;     /* Initialize all filters scale configuration */

	for( cnt=0; cnt<CAN1_FILTER_BANK_NUM; cnt++ )
	{
		can->sFilterRegister[cnt].FR1 = 0;
		can->sFilterRegister[cnt].FR2 = 0;
	}

    CAN1->FFA1R = 0;
    CAN1->FA1R = 3;
    
    /* Set the bit timing register */
    res = can_SetBitrate( can, handle->nSpeed );
	if( CAN_STD_SUCCESS != res )
	{
		return res;
	}

	can->IER =  CAN_IER_TMEIE  |  /* Enable Interrupts */
				CAN_IER_FMPIE0 |
				CAN_IER_FOVIE0 |
				CAN_IER_FMPIE1 |
				CAN_IER_FOVIE1 |
				CAN_IER_EWGIE  |
				CAN_IER_EPVIE  |
				CAN_IER_BOFIE  |
				CAN_IER_ERRIE  ;

    /* Request leave initialization */
	can->MCR &= ~CAN_MCR_INRQ;

	/* Wait the acknowledge */
	while( 0 != (can->MSR&CAN_MSR_INAK) );
	
	/* Enable UART interrupt for TX & RX */
	NVIC_ClearPendingIRQ ( handle->TxIRQn );
	NVIC_EnableIRQ       ( handle->TxIRQn );

	NVIC_ClearPendingIRQ ( USB_LP_CAN1_RX0_IRQn );
	NVIC_EnableIRQ       ( USB_LP_CAN1_RX0_IRQn );

    NVIC_EnableIRQ       ( CAN1_RX1_IRQn );
    
	
	return CAN_STD_SUCCESS;
}


static CANSTD_STATUS CanSpeed( void *pHandle, uint32_t nSpeed )
{
	CANSTD_STATUS res;
	CANSTD_HANDLE *handle = (CANSTD_HANDLE *)pHandle;
	CAN *can = (CAN *)handle->pCAN;

	ASSERT( 0 != pHandle );

	res = can_SetBitrate( can, nSpeed );
	if( TRUE == res )
	{
		handle->nSpeed = nSpeed;
	}

	return res;
}


static CANSTD_STATUS CanMode( void const *pHandle, CANSTD_MODE Mode )
{
	CANSTD_HANDLE const *handle = (CANSTD_HANDLE const *)pHandle;
	CAN *can = (CAN *)handle->pCAN;
	
    ASSERT( 0 != pHandle );

    /* Configure the UART's databit */
    switch( Mode )
	{
		case CAN_MODE_NORMAL:
			can->BTR &= ~(CAN_BTR_LBKM | CAN_BTR_SILM);

			can->MCR = CAN_MCR_ABOM |   /* Activate automatic bus-off */
						CAN_MCR_AWUM;    /* Enable automatic wakeup mode */
			
			while( 0U != (can->MSR & CAN_MSR_INAK) ); /* Wait to exit initialization mode */
			can->FMR &= ~CAN_FMR_FINIT;  /* Filter active mode */
		break;
    
		case CAN_MODE_LISTEN:
			can->MCR |=  CAN_MCR_INRQ;       /* Enter initialization mode */
			while( 0U != (can->MSR & CAN_MSR_INAK) );  /* Wait to enter initialization mode */
			
			can->BTR &= ~CAN_BTR_LBKM;            /* Deactivate loopback */
			can->BTR |= CAN_BTR_SILM;            /* Activate silent */

			can->MCR &= ~CAN_MCR_INRQ;            /* Deactivate initialization mode */
			while ( 0U != (can->MSR & CAN_MSR_INAK) );  /* Wait to exit initialization mode */
		break;
		
		case CAN_MODE_CONFIG:
			can->FMR |= CAN_FMR_FINIT;	 /* Filter initialization mode */
			can->MCR = CAN_MCR_INRQ;     /* Enter initialization mode */
			while ( 0U == (can->MSR&CAN_MSR_INAK) );  /* Wait to enter initialization mode */
		break;

		case CAN_MODE_LOOP_INT:
			can->MCR |= CAN_MCR_INRQ;            /* Enter initialization mode */
			while( 0U == (can->MSR&CAN_MSR_INAK) );  /* Wait to enter initialization mode */
			
			can->BTR |=  CAN_BTR_LBKM;            /* Activate loopback */
			can->BTR |=  CAN_BTR_SILM;            /* Activate silent */
			
			can->MCR &= ~CAN_MCR_INRQ;            /* Deactivate initialization mode */
			while( 0U != (can->MSR&CAN_MSR_INAK) );  /* Wait to exit initialization mode */
		break;

		case CAN_MODE_LOOP_EXT:
			can->MCR |=  CAN_MCR_INRQ;            /* Enter initialization mode */
			while ( 0U == (can->MSR&CAN_MSR_INAK) );  /* Wait to enter initialization mode */
			
			can->BTR &= ~CAN_BTR_SILM;            /* Deactivate silent */
			can->BTR |=  CAN_BTR_LBKM;            /* Activate loopback */
			
			can->MCR &= ~CAN_MCR_INRQ;            /* Deactivate initialization mode */
			while ( 0U != (can->MSR&CAN_MSR_INAK) );  /* Wait to exit initialization mode */
		break;

		case CAN_MODE_DISABLE:
		break;
		
		default:
			return CAN_STD_INVALID_MODE;
    }

    /* Enable CAN */

	return CAN_STD_SUCCESS;
}


static void CanAddFilter(
	void const	*pHandle,
	int			nIdx,
	uint32_t	nId,
	uint32_t 	nMask
	)
{
	uint32_t     fa1r, frx, fry, msk;
	uint8_t      bank, bank_end;
	int32_t      status;
	CANSTD_HANDLE *handle = (CANSTD_HANDLE *)pHandle;
	CAN *can = (CAN *)handle->pCAN;
	
    ASSERT( 0 != pHandle );
}


static void CanRemoveFilter(
	void const	*pHandle,
    int			nIdx
	)
{
	CANSTD_HANDLE *handle = (CANSTD_HANDLE *)pHandle;
	CAN *can = (CAN *)handle->pCAN;
	
    ASSERT( 0 != pHandle );  
}


static BOOL
CanMailBox(
	void	const	*pHandle,
	uint32_t		nId,
	uint32_t 		nMask,
	uint8_t 		nPriority,
	MAILBOXTYPE		Type,
	uint8_t 		nMB
	)
{
	BOOL res = FALSE;
	CANSTD_HANDLE *handle = (CANSTD_HANDLE *)pHandle;
	CAN *can = (CAN *)handle->pCAN;
	
    ASSERT( 0 != pHandle );

	return res;
}


static CANSTD_STATUS CanLoopBack( void const *pHandle, BOOL bEnable )
{
	CANSTD_HANDLE const *handle = (CANSTD_HANDLE const *)pHandle;
	CAN *can = (CAN *)handle->pCAN;
	
    ASSERT( 0 != pHandle );
				
	return CAN_STD_SUCCESS;
}


static BOOL CanTransmit( void *pHandle, CAN_TXMSG const *pMsg )
{
	BOOL res = FALSE;
	uint8_t mb;
	uint32_t cnt;
    uint32_t     tir = 0;
	int size;
	uint32_t payload;
	CANSTD_HANDLE *handle = (CANSTD_HANDLE *)pHandle;
	CAN *can = (CAN *)handle->pCAN;

	uint32_t *data = (uint32_t *)pMsg->Msg.data;
	
    ASSERT( 0 != pHandle );

	if( 0U != (can->sTxMailBox[2].TIR & CAN_TI0R_TXRQ) )
	{
		return FALSE;
	}

	if( 0U != (pMsg->Msg.ID.Id & ARM_CAN_ID_IDE_Msk) )
	{
		/* Extended Identifier */
		tir = (pMsg->Msg.ID.Id<<3) | CAN_TI0R_IDE;
	}
	else
	{
		/* Standard Identifier */
		tir = pMsg->Msg.ID.Id<<21U;
	}

	size = pMsg->Msg.nDLC;
	if( pMsg->Msg.nDLC > 8 )
	{
		size = 8U;
	}

#if 0
	if( msg_info->rtr != 0U )
	{   
		// If send RTR requested
		size = 0U;
		tir |= CAN_TI0R_RTR;

		ptr_CAN->sTxMailBox[obj_idx].TDTR &= ~CAN_TDT0R_DLC;
		ptr_CAN->sTxMailBox[obj_idx].TDTR |=  msg_info->dlc & CAN_TDT0R_DLC;
	} 
	else 
#endif
	{
		can->sTxMailBox[2].TDLR = *data++;
		can->sTxMailBox[2].TDHR = *data;

		can->sTxMailBox[2].TDTR &= ~CAN_TDT0R_DLC;
		can->sTxMailBox[2].TDTR |=  size & CAN_TDT0R_DLC;
	}

	can->sTxMailBox[2].TIR = tir | CAN_TI0R_TXRQ;    /* Activate transmit */
 
	return res;
}


static BOOL CanReceived( void const *pHandle, uint8_t nIdx, PCAN_RXMSG pMsg )
{
	uint32_t data;
	BOOL res = FALSE;
	CANTYPEFRAME 	type;
	CANSTD_HANDLE const *handle = (CANSTD_HANDLE const *)pHandle;
	CAN *can = (CAN *)handle->pCAN;

    ASSERT( 0 != pHandle );

	if( 0U != (can->sFIFOMailBox[0].RIR & CAN_RI0R_IDE) ) 
	{     
		/* Extended Identifier */
		pMsg->Msg.ID.Id = (0x1FFFFFFFUL & (can->sFIFOMailBox[0].RIR>>3)) | ARM_CAN_ID_IDE_Msk;
		pMsg->Msg.format = CAN_MSG_XTD_FRAME;
	}
	else
	{   /* Standard Identifier */
		pMsg->Msg.ID.Id = 0x07FFUL & (can->sFIFOMailBox[0].RIR>>21);
        pMsg->Msg.format = CAN_MSG_STD_FRAME;
	}

	if ((can->sFIFOMailBox[0].RIR & CAN_RI0R_RTR) != 0U)
	{
		pMsg->Msg.format |= CAN_MSG_RTR_FRAME;
	}

	pMsg->Msg.nDLC = can->sFIFOMailBox[0].RDTR & CAN_RDT0R_DLC;

	if( pMsg->Msg.nDLC>0U )
	{
		data = can->sFIFOMailBox[0].RDLR;
		memcpy( pMsg->Msg.data, &data, 4U );

		data = can->sFIFOMailBox[0].RDHR;
		memcpy( &pMsg->Msg.data[4], &data, 4U );

		res = TRUE;
	}

	switch(nIdx )
	{
		case 0:
			can->RF0R = CAN_RF0R_RFOM0;   /* Release FIFO 0 output mailbox */
		break;

		case 1:
			can->RF1R = CAN_RF1R_RFOM1;   /* Release FIFO 1 output mailbox */
		break;

		default:
		break;
	} 

	return res;
}


static BOOL CanIsTxEnd( void const *pHandle )
{
	CANSTD_HANDLE const *handle = (CANSTD_HANDLE const *)pHandle;
	CAN *can = (CAN *)handle->pCAN;

    ASSERT( 0 != pHandle );
	
    return FALSE;
}


static BOOL CanIsTxReady( void const *pHandle )
{
	CANSTD_HANDLE const *handle = (CANSTD_HANDLE const *)pHandle;
	CAN *can = (CAN *)handle->pCAN;

    ASSERT( 0 != pHandle );
	
    return ( (0U != (can->sTxMailBox[2].TIR & CAN_TI0R_TXRQ) )? FALSE : TRUE );
}


static BOOL CanIsRxReady( void const *pHandle )
{
	CANSTD_HANDLE const *handle = (CANSTD_HANDLE const *)pHandle;
	CAN *can = (CAN *)handle->pCAN;

    ASSERT( 0 != pHandle );
	
	return FALSE;
}


static void CanTxEnd( void const *pHandle )
{
	CANSTD_HANDLE const *handle = (CANSTD_HANDLE const *)pHandle;
	CAN *can = (CAN *)handle->pCAN;

    ASSERT( 0 != pHandle );
}


static void CanLockRx( void const *pHandle )
{
	CANSTD_HANDLE const *handle = (CANSTD_HANDLE const *)pHandle;
    ASSERT( 0 != pHandle );
	
	NVIC_DisableIRQ( (IRQn_Type)handle->RxIRQn );
}


static void CanUnlockRx( void const *pHandle )
{
	CANSTD_HANDLE const *handle = (CANSTD_HANDLE const *)pHandle;
    ASSERT( 0 != pHandle );
	
	NVIC_EnableIRQ( (IRQn_Type)handle->RxIRQn );
}


static void CanLockTx( void const *pHandle )
{
	CANSTD_HANDLE const *handle = (CANSTD_HANDLE const *)pHandle;
    ASSERT( 0 != pHandle );
	
	NVIC_DisableIRQ( (IRQn_Type)handle->TxIRQn );
}


static void CanUnlockTx( void const *pHandle )
{
	CANSTD_HANDLE const *handle = (CANSTD_HANDLE const *)pHandle;
    ASSERT( 0 != pHandle );
	
	NVIC_EnableIRQ( (IRQn_Type)handle->TxIRQn );
}


static CANSTD_STATUS
can_SetBittime(
	CAN		*pCan,
	int32_t Bitrate,
	int32_t Clock,
	int32_t ProgSeg,
	int32_t PhaseSeg1,
	int32_t PhaseSeg2,
	int		nSjw
	)
{
	uint32_t tqnum;
	uint32_t mcr;
	uint32_t brp;
	uint32_t btr;

	if( ((ProgSeg + PhaseSeg1) < 1U) || ((ProgSeg + PhaseSeg1) > 16U) )
	{
		return CAN_STD_INVALID_PROGSEG;
	}

	if( (PhaseSeg2 < 1U) || (PhaseSeg2 > 8U) )
	{
		return CAN_STD_INVALID_PHASEG1;
	}

	if( (nSjw < 1U) || (nSjw > 4U) )
	{
		return CAN_STD_INVALID_PHASEG2; 
	}

	tqnum = 1U + ProgSeg + PhaseSeg1 + PhaseSeg2;

	if( Clock == 0U ) 
	{
		return CAN_STD_INVALID_CLOCK;
	}

	brp = Clock / (tqnum * Bitrate);
	if( brp>1024U ) 
	{
		return CAN_STD_INVALID_BITRATE;
	}

	if( Clock>(brp * tqnum * Bitrate) )
	{
		if( ((Clock - (brp * tqnum * Bitrate)) * 1024U) > CAN_CLOCK_TOLERANCE )
		{ 
			return CAN_STD_INVALID_BITRATE;
		}
	} 
	else if( Clock < (brp * tqnum * Bitrate) ) 
	{
		if( (((brp * tqnum * Bitrate) - Clock) * 1024U) > CAN_CLOCK_TOLERANCE )
		{ 
			return CAN_STD_INVALID_BITRATE; 
		}
	}

	mcr = pCan->MCR;
	pCan->MCR = CAN_MCR_INRQ;                  // Activate initialization mode
	while( 0U == (pCan->MSR&CAN_MSR_INAK) );    // Wait to enter initialization mode

	btr = pCan->BTR & 0xc0000000U; /* Reserved last 2 bits */
	btr |= ((brp - 1U) & CAN_BTR_BRP) | ((nSjw - 1U) << 24) | ((PhaseSeg2 - 1U) << 20) | ((ProgSeg + PhaseSeg1 - 1U) << 16);
	pCan->BTR = btr;
	pCan->MCR = mcr;                          // Return to previous mode

    return CAN_STD_SUCCESS;
}


static BOOL can_SetBitrate( CAN	*pCan, int32_t nBitrate )
{
	CANSTD_STATUS res;
	int32_t clock;

 	/* Re-calculate current APB1 bus clock. User may change it in Hal.c */
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

	if ((clock % (8U*nBitrate)) == 0U)
	{              
		/* If CAN base clock is divisible by 8 * nominal bitrate without remainder */
		res = can_SetBittime(
					pCan,
					nBitrate,
					clock,
					5U,
					1U,
					1U,
					1U );
	} 
	else if ((clock % (10U*nBitrate)) == 0U)
	{ 
		/* If CAN base clock is divisible by 10 * nominal bitrate without remainder */
		res = can_SetBittime(
				pCan,
				nBitrate,
				clock,
				7U,
				1U,
				1U,
				1U );
	} 
	else if ((clock % (12U*nBitrate)) == 0U)
	{                       
		// If CAN base clock is divisible by 12 * nominal bitrate without remainder
		res = can_SetBittime(
				pCan,
				nBitrate,
				clock,
				7U,
				2U,
				2U,
				2U );
	} 
	else 
	{
		res = CAN_STD_INVALID_BITRATE;
	}

	return res;
}


static void can_RxIrqHandler( CAN_TypeDef *pCan, int nIdx )
{
	uint32_t esr = CAN1->ESR;
	uint32_t ier = CAN1->IER;

	if( 0U != (CAN1->RF0R & CAN_RF0R_FOVR0) ) 
	{
		CAN1->RF0R = CAN_RF0R_FOVR0;      /* Clear overrun flag */
	}

	if( 0U != (CAN1->RF0R & CAN_RF0R_FMP0) ) 
	{
		/* Read Message */
         CanStdRxIsr( g_CanIrqHandles[0], nIdx );
	}

	/* Handle transition from from 'bus off', 
	' error active' state, or re-enable warning interrupt */
	if (((esr & CAN_ESR_BOFF) == 0U) && ((ier & CAN_IER_BOFIE) == 0U))
	{ 
		CAN1->IER |= CAN_IER_BOFIE;
	}

	if (((esr & CAN_ESR_EPVF) == 0U) && ((ier & CAN_IER_EPVIE) == 0U))
	{
		CAN1->IER |= CAN_IER_EPVIE;
	}

	if (((esr & CAN_ESR_EWGF) == 0U) && ((ier & CAN_IER_EWGIE) == 0U) 
	&& (((esr & CAN_ESR_TEC) >> 16) < 95U) && (((esr & CAN_ESR_REC) >> 24) == 95U))
	{
		CAN1->IER |= CAN_IER_EWGIE;
	}
}


/*****************************************************************************
 Interrupt Service Routines
*****************************************************************************/
void USB_HP_CAN1_TX_IRQHandler( void )
{
	uint32_t esr, ier;

	if( (CAN1->TSR & CAN_TSR_TXOK0) != 0U )
	{
		CAN1->TSR = CAN_TSR_RQCP0;          // Request completed on transmit mailbox 0
	}

	if ((CAN1->TSR & CAN_TSR_TXOK1) != 0U) 
	{
		CAN1->TSR = CAN_TSR_RQCP1;          // Request completed on transmit mailbox 1
	}

	if ((CAN1->TSR & CAN_TSR_TXOK2) != 0U) 
	{
		CAN1->TSR = CAN_TSR_RQCP2;          // Request completed on transmit mailbox 2
	}

	// Handle transition from from 'bus off', ' error active' state, or re-enable warning interrupt
	esr = CAN1->ESR;
	ier = CAN1->IER;
	if (((esr & CAN_ESR_BOFF) == 0U) && ((ier & CAN_IER_BOFIE) == 0U))
	{ 
		CAN1->IER |= CAN_IER_BOFIE;
	}

	if( ((esr & CAN_ESR_EPVF) == 0U) && ((ier & CAN_IER_EPVIE) == 0U) )
	{
		CAN1->IER |= CAN_IER_EPVIE;
	}

	if(((esr & CAN_ESR_EWGF) == 0U) && ((ier & CAN_IER_EWGIE) == 0U) 
	&& (((esr & CAN_ESR_TEC) >> 16) == 95U) && (((esr & CAN_ESR_REC) >> 24) < 95U))
	{
		CAN1->IER |= CAN_IER_EWGIE;
	}

    CanStdTxIsr( g_CanIrqHandles[0] );
}


void USB_LP_CAN1_RX0_IRQHandler( void )
{
	can_RxIrqHandler( CAN1, 0 );
}


void CAN1_RX1_IRQHandler( void )
{
	can_RxIrqHandler( CAN1, 1 );
}