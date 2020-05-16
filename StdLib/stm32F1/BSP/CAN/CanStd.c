/*****************************************************************************
 @Project	: 
 @File 		: CanStd.c
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
 Define
******************************************************************************/


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
static BOOL		can_TxPull	( PCANSTD_TXFIFO pFifo, PCAN_TXMSG pMsg );
static BOOL		can_TxPush	( PCANSTD_TXFIFO pFifo, CAN_TXMSG const *pMsg);

static BOOL		can_RxPull	( PCANSTD_RXFIFO pFifo, PCAN_RXMSG pMsg );
static BOOL		can_RxPush	( PCANSTD_RXFIFO pFifo, CAN_RXMSG const *pMsg );


/*****************************************************************************
 Implementation
******************************************************************************/
	
void
CanStdInit(
	PCANSTD_HANDLE			pHandle,
	uint8_t 				nPort,
	uint32_t 				nSpeed,
	void		 			*pTxBuf,
	uint32_t 				nTxSize,
	void 					*pRxBuf,
	uint32_t 				nRxSize
	)
{
	memset( pHandle, 0, sizeof( CANSTD_HANDLE ) );

	pHandle->nPort			= nPort;
	pHandle->nSpeed			= nSpeed;
	pHandle->pDriver		= 0;
	pHandle->TxFifo.nSize 	= nTxSize;
	pHandle->TxFifo.pBuf 	= pTxBuf;
	pHandle->RxFifo.nSize 	= nRxSize;
	pHandle->RxFifo.pBuf 	= pRxBuf;
}


CANSTD_STATUS
CanStdLinkDriver( 
	PCANSTD_HANDLE			pHandle,
	CANSTD_DRIVER const 	*pDriver )
{
	CANSTD_STATUS 	res 	= CAN_STD_INVALID_PORT;

	if( 0 != pHandle->pDriver )
	{
		/* Handle has been link to other driver */
		return CAN_STD_INVALID_HANDLE;
	}

	pHandle->pDriver = pDriver;
	res = pDriver->pfInit( pHandle );

	return res;
}


CANSTD_STATUS 
CanStdAddHook(
	PCANSTD_HANDLE		pHandle,
	CANSTD_CB_ONTX		*pfOnTx,
	CANSTD_CB_ONRX		*pfOnRx
	)
{
	ASSERT( 0 != pHandle );

	if( 0 != pfOnTx )
	{
		pHandle->pfHookOnTx = pfOnTx;
	}

	if( 0 != pfOnRx )
	{
		pHandle->pfHookOnRx = pfOnRx;
	}
	
	return CAN_STD_SUCCESS;
}



CANSTD_STATUS	
CanStdSpeed(
	PCANSTD_HANDLE		pHandle,
	uint32_t			nSpeed
	)
{
	CANSTD_DRIVER const *driver = pHandle->pDriver;
	ASSERT( 0 != pHandle );

	return driver->pfSpeed( pHandle, nSpeed );
}


CANSTD_STATUS
CanStdSetMode(
	CANSTD_HANDLE const		*pHandle, 
	CANSTD_MODE 			Mode 
	)
{
	CANSTD_DRIVER const *driver = pHandle->pDriver;
	ASSERT( 0 != pHandle );

	return driver->pfSetMode( pHandle, Mode );
}


BOOL
CanStdCfgMailBox(
	CANSTD_HANDLE const	*pHandle,
	uint32_t			nId,
	uint32_t 			nMask,
	uint8_t 			nPriority,
	MAILBOXTYPE			Type,
	uint8_t 			nMB
	)
{
	CANSTD_DRIVER const *driver = pHandle->pDriver;
	ASSERT( 0 != pHandle );

	return driver->pfSetMailBox( pHandle, nId, nMask, nPriority, Type, nMB );
}



CANSTD_STATUS
CanStdTransmit(
	PCANSTD_HANDLE 	pHandle,
	CAN_TXMSG const *pMsg
	)
{
	BOOL 		res 	= FALSE;
	CANSTD_DRIVER const *driver = pHandle->pDriver;
	ASSERT( 0 != pHandle );

	if( FALSE == driver->pfIsTxReady( pHandle ) )
	{
		/* Hardware FIFO full - push into s/w FIFO */
		driver->pfTxLock( pHandle );
		res = can_TxPush( &pHandle->TxFifo, pMsg );
		driver->pfTxUnlock( pHandle );

		if( FALSE == res )
		{
			/* s/w FIFO full */
			return CAN_STD_BUFFER_FULL;
		}
	}
	else
	{
		/* Have space - write into h/w fifo */
		driver->pfWrite( pHandle, pMsg );
	}
	
	return CAN_STD_SUCCESS;
}



BOOL
CanStdReceived(
	PCANSTD_HANDLE pHandle,
	PCAN_RXMSG  		pMsg
	)
{
	BOOL 			res		= FALSE;
	CANSTD_DRIVER const *driver = pHandle->pDriver;
	ASSERT( 0 != pHandle );
	
	driver->pfRxLock( pHandle );
	res = can_RxPull( &pHandle->RxFifo, pMsg );
	driver->pfRxUnlock( pHandle );

	return res;
}


BOOL
CanStdIsTxEmpty(
	CANSTD_HANDLE const *pHandle
	)
{
	CANSTD_DRIVER const *driver = pHandle->pDriver;
	ASSERT( 0 != pHandle );
	
	return driver->pfIsTxEnd( pHandle );
}


BOOL
CanStdIsTxReady(
	CANSTD_HANDLE const *pHandle
	)
{
	uint8_t	cnt = 0U;
	CANSTD_DRIVER const *driver = pHandle->pDriver;
	ASSERT( 0 != pHandle );
	
	driver->pfTxLock( pHandle );

	cnt = pHandle->TxFifo.nHead + 1U;

	if( cnt >= pHandle->TxFifo.nSize )
	{
		/* point back to the top of the FIFO ring buffer */
		cnt = 0U;
	}
	
	if( cnt == pHandle->TxFifo.nTail )
	{
		driver->pfTxUnlock( pHandle );
		return FALSE;					/* FIFO full - not ready */
	}

	driver->pfTxUnlock( pHandle );
	
	return TRUE;
}


BOOL
CanStdIsRxReady(
	CANSTD_HANDLE const *pHandle
	)
{
	ASSERT( 0 != pHandle );
	
	return (BOOL)((pHandle->RxFifo.nHead 
				!= pHandle->RxFifo.nTail) ? TRUE : FALSE);
}


/*****************************************************************************
 IRQ handler
*****************************************************************************/
void
CanStdTxIsr(
	PCANSTD_HANDLE pHandle 
	)
{
	CANSTD_DRIVER const *driver = pHandle->pDriver;
	ASSERT( 0 != pHandle );

	if( TRUE == can_TxPull( &pHandle->TxFifo, &pHandle->TxMsg ) )
	{
		/* Send it now */
		driver->pfWrite( pHandle, &pHandle->TxMsg );
	}
	else
	{
		/* Nothing else to Tx, disable IRQ */
		driver->pfTxEnd( pHandle );

		if( 0 != pHandle->pfHookOnTx )
		{
			pHandle->pfHookOnTx();
		}
	}
}


void CanStdRxIsr( PCANSTD_HANDLE pHandle, uint8_t nIdx )
{
	CANSTD_DRIVER const *driver = pHandle->pDriver;
	ASSERT( 0 != pHandle );
	
	if( TRUE == driver->pfRead( pHandle, nIdx, &pHandle->RxMsg ) )
	{
		if( FALSE == can_RxPush( &pHandle->RxFifo, &pHandle->RxMsg ) )
		{
			pHandle->dwError |= CAN_STD_E_OVERFLOW;
		}
		else
		{
			if( 0 != pHandle->pfHookOnRx )
			{
				pHandle->pfHookOnRx( &pHandle->RxMsg );
			}
		}
	}
}


/*****************************************************************************
 Local helpers
*****************************************************************************/

/* FIFO access */
static BOOL
can_TxPush(
	PCANSTD_TXFIFO 	pFifo,
	CAN_TXMSG const *pMsg
	)
{
	uint8_t	n;
	PCAN_TXMSG	pHead;
	
	
	ASSERT( 0 != pFifo );
	
	/* Write TxHead to nTemp */
	n = pFifo->nHead + 1U;
	
	if( n >= pFifo->nSize )
	{
		/* point back to the top of the FIFO ring buffer */
		n = 0U;
	}
	
	if( n == pFifo->nTail )
	{
		return FALSE;					/* FIFO full */
	}

	pHead = pFifo->pBuf + pFifo->nHead;

	memcpy(
		pHead,
		pMsg, 
		sizeof(CAN_TXMSG) );
	
	pFifo->nHead = n;					/* update head pointer */

	return TRUE;
}


static BOOL
can_TxPull(
	PCANSTD_TXFIFO 	pFifo,
	PCAN_TXMSG 		pMsg
	)
{
	
	ASSERT( 0 != pFifo );
	
	if( pFifo->nHead == pFifo->nTail )
	{
		/* Ring buffer is empty. Nothing left. */
		return FALSE;
	}

	memcpy( pMsg, &pFifo->pBuf[pFifo->nTail++], sizeof(CAN_TXMSG) );	/* get data from FIFO buffer */
	
	if( pFifo->nTail == pFifo->nSize )
	{
		pFifo->nTail = 0U;		/* point back to the top of FIFO ring buffer */
	}
		
	return TRUE;
}


/* FIFO access */
static BOOL
can_RxPush(
	PCANSTD_RXFIFO 	pFifo,
	CAN_RXMSG const *pMsg
	)
{
	uint8_t	n;
	PCAN_RXMSG	pHead;
	
	ASSERT( 0 != pFifo );
	
	/* Write TxHead to nTemp */
	n = pFifo->nHead + 1U;
	
	if( n >= pFifo->nSize )
	{
		/* point back to the top of the FIFO ring buffer */
		n = 0U;
	}
	
	if( n == pFifo->nTail )
	{
		return FALSE;					/* FIFO full */
	}
		
	pHead = pFifo->pBuf + pFifo->nHead;	/* copy data into FIFO */
	
	memcpy(
		pHead, 
		pMsg, 
		sizeof(CAN_RXMSG) );
	
	pFifo->nHead = n;					/* update head pointer */

	return TRUE;
}


static BOOL
can_RxPull(
	PCANSTD_RXFIFO 	pFifo,
	PCAN_RXMSG 		pMsg
	)
{
	ASSERT( 0 != pFifo );
	
	if( pFifo->nHead == pFifo->nTail )
	{
		/* Ring buffer is empty. Nothing left. */
		return FALSE;
	}

	memcpy( pMsg, &pFifo->pBuf[pFifo->nTail++], sizeof(CAN_RXMSG) );	/* get data from FIFO buffer */
	
	if( pFifo->nTail == pFifo->nSize )
	{
		/* Buffer full. point back to TOP */
		pFifo->nTail = 0U;
	}
		
	return TRUE;
}



















