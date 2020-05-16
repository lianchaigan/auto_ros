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

#ifndef	__CANSTD_DOT_H__
#define	__CANSTD_DOT_H__


/*****************************************************************************
 Defines 
*****************************************************************************/

/* Result status */
#define	CAN_STD_SUCCESS				0					/* Success */
#define	CAN_STD_FAILED				-1					/* Failed */
#define	CAN_STD_INVALID_HANDLE		-2					/* Failed */
#define	CAN_STD_INVALID_PORT		-3					/* Invalid port */
#define	CAN_STD_INVALID_MODE		-4					/* Invalid Mode */
#define	CAN_STD_INVALID_CLOCK		-5					/* Invalid Bitrate */
#define	CAN_STD_INVALID_BITRATE		-6					/* Invalid Bitrate */
#define	CAN_STD_INVALID_PROGSEG		-7					/* Invalid Bittime */
#define	CAN_STD_INVALID_PHASEG1		-8					/* Invalid Bittime */
#define	CAN_STD_INVALID_PHASEG2		-9					/* Invalid Bittime */
#define	CAN_STD_INVALID_SJW			-10					/* Invalid Bittime */
#define	CAN_STD_BUFFER_FULL			-11					/* Buffer full */
#define	CAN_STD_HW_TRX_NO_RESP		-12


/* Error status */
#define	CAN_STD_E_NONE				0x00000000U			/* No error */
#define	CAN_STD_E_OVERFLOW			0x00000001U			/* Rx overflow */
#define	CAN_STD_E_PARITY			0x00000002U			/* Parity error */
#define	CAN_STD_E_FRAME				0x00000004U			/* Framing error */


/*****************************************************************************
 Public types
*****************************************************************************/

typedef int32_t					CANSTD_STATUS;		/* result status */

/* Uart Hardware driver function prototypes */
typedef	CANSTD_STATUS	CAN_INIT( void	*pHandle );
typedef	CANSTD_STATUS	CAN_SPEED(void  *pHandle, uint32_t nSpeed );
typedef	CANSTD_STATUS	CAN_MODE(void const *pHandle, CANSTD_MODE	Mode );
typedef	BOOL			CAN_MAILBOX(
							void	const	*pHandle,
							uint32_t		nId,
							uint32_t 		nMask,
							uint8_t 		nPriority,
							MAILBOXTYPE		Type,
							uint8_t 		nMB );
typedef void CAN_ADD_FILTER(
							void const	*pHandle,
							int			nIdx,
							uint32_t	nId,
							uint32_t 	nMask );
typedef void CAN_REMOVE_FILTER( void const	*pHandle, int nIdx );
typedef	CANSTD_STATUS	CAN_LOOPBACK( void const *pHandle, BOOL bEnable );
typedef	 BOOL			CAN_TRANSMIT( void *pHandle, CAN_TXMSG const *pMsg );		
typedef BOOL			CAN_RECEIVED( void const *pHandle, uint8_t nIdx, PCAN_RXMSG  pMsg );
typedef BOOL			CAN_ISTXEND( void const *pHandle );
typedef BOOL			CAN_IS_TXREADY( void const *pHandle );
typedef BOOL			CAN_IS_RXREADY( void const *pHandle );
typedef void 			CAN_TXEND( void const *pHandle );
typedef void 			CAN_LOCK_RX( void const *pHandle );
typedef void 			CAN_UNLOCK_RX( void const *pHandle );
typedef void			CAN_LOCK_TX( void const *pHandle );
typedef void			CAN_UNLOCK_TX( void const *pHandle );

/* Uart Hardware driver functions */
typedef struct s_tag_SXMUART_DRIVER
{
	CAN_INIT			*pfInit;
	CAN_SPEED			*pfSpeed;
	CAN_MODE			*pfSetMode;
	CAN_MAILBOX			*pfSetMailBox;
    CAN_ADD_FILTER      *pfAddFilter;
	CAN_REMOVE_FILTER	*pfRemoveFilter;
	CAN_LOOPBACK		*pfLookback;
	CAN_TRANSMIT		*pfWrite;
	CAN_RECEIVED		*pfRead;
	CAN_ISTXEND			*pfIsTxEnd;
	CAN_IS_TXREADY		*pfIsTxReady;
	CAN_IS_RXREADY		*pfIsRxReady;
	CAN_TXEND			*pfTxEnd;
	CAN_LOCK_RX			*pfRxLock;
	CAN_UNLOCK_RX		*pfRxUnlock;
	CAN_LOCK_TX			*pfTxLock;
	CAN_UNLOCK_TX		*pfTxUnlock;
}CANSTD_DRIVER, *PCANSTD_DRIVER;


/* FIFO */
typedef struct s_tagCanStd_TxFifo
{
	uint32_t			nSize;				/* Size of FIFO in count */
	volatile uint32_t	nHead;				/* Head pointer */
	volatile uint32_t	nTail;				/* Tail pointer */
	PCAN_TXMSG			pBuf;				/* Pointer to FIFO buffer */
}CANSTD_TXFIFO, *PCANSTD_TXFIFO;


typedef struct s_tagCanStd_RxFifo
{
	uint32_t			nSize;				/* Size of FIFO in count */
	volatile uint32_t	nHead;				/* Head pointer */
	volatile uint32_t	nTail;				/* Tail pointer */
	PCAN_RXMSG			pBuf;				/* Pointer to FIFO buffer */
}CANSTD_RXFIFO, *PCANSTD_RXFIFO;


/* Callback */
typedef	void 				CANSTD_CB_ONRX( CAN_RXMSG const *pData  );
typedef	void 				CANSTD_CB_ONTX( void );


typedef struct s_tagCANSTD_HANDLE
{
	uint8_t				nPort;
	uint32_t			nSpeed;
	CANSTD_DRIVER const	*pDriver;			/* H/W driver */

	volatile int32_t	dwError;			/* Error register */

	CANSTD_TXFIFO		TxFifo;				/* Tx FIFO */
	CANSTD_RXFIFO		RxFifo;				/* RX FIFO */
	uint32_t			nMbType;			/* bit that is 1 means Tx MB */
	uint8_t				TxMbIdx;
	uint8_t				RxMbIdx;
	int32_t				TxIRQn;
    int32_t				RxIRQn;
	void				*pCAN;				/* CAN hardware address */

    CAN_TXMSG			TxMsg;
    CAN_RXMSG			RxMsg;

	CANSTD_CB_ONTX		*pfHookOnTx;		/* Tx complete callback hook */
	CANSTD_CB_ONRX		*pfHookOnRx;		/* Rx receive callback hook */
}CANSTD_HANDLE, *PCANSTD_HANDLE;


/*****************************************************************************
 Public functions
*****************************************************************************/


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
void
CanStdInit(
	PCANSTD_HANDLE			pHandle,
	uint8_t 				nPort,
	uint32_t 				nSpeed,
	void		 			*pTxBuf,
	uint32_t 				nTxSize,
	void 					*pRxBuf,
	uint32_t 				nRxSize );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
CANSTD_STATUS
CanStdLinkDriver( 
	PCANSTD_HANDLE			pHandle,
	CANSTD_DRIVER const 	*pDriver );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
CANSTD_STATUS
CanStdAddHook(
	PCANSTD_HANDLE		pHandle,
	CANSTD_CB_ONTX		*pfOnTx,
	CANSTD_CB_ONRX		*pfOnRx );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
CANSTD_STATUS
CanStdSpeed(
	PCANSTD_HANDLE		pHandle,
	uint32_t			nSpeed );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
CANSTD_STATUS
CanStdSetMode(
	CANSTD_HANDLE const		*pHandle, 
	CANSTD_MODE 			Mode );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
BOOL
CanStdCfgMailBox(
	CANSTD_HANDLE const	*pHandle,
	uint32_t			nId,
	uint32_t 			nMask,
	uint8_t 			nPriority,
	MAILBOXTYPE			Type,
	uint8_t 			nMB );
	

/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
BOOL CanStdIsTxEmpty( CANSTD_HANDLE const *pHandle );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
BOOL CanStdIsTxReady( CANSTD_HANDLE const *pHandle );

/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
CANSTD_STATUS
CanStdTransmit(
	PCANSTD_HANDLE pHandle,
	CAN_TXMSG const *pMsg );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
BOOL CanStdIsRxReady( CANSTD_HANDLE const *pHandle );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
BOOL CanStdReceived( PCANSTD_HANDLE pHandle, PCAN_RXMSG  pMsg );





void CanStdTxIsr( PCANSTD_HANDLE pHandle );
void CanStdRxIsr( PCANSTD_HANDLE pHandle, uint8_t nIdx );


#endif	/* __CANSTD_DOT_H__ */















