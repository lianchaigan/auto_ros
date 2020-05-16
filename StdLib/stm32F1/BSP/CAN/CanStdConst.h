/*****************************************************************************
 @Project	: 
 @File 		: CanStdConst.h
 @Details  	:             
 @Author	: 
 @Hardware	: STM32
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
   
******************************************************************************/

#ifndef __CANSTDCONST_DOT_H__
#define __CANSTDCONST_DOT_H__


/*****************************************************************************
 Public types
*****************************************************************************/
typedef enum _tagCANConfigFlags
{
	CAN_CONFIG_DEFAULT,
	CAN_CONFIG_MSG_TYPE_BIT,
	CAN_CONFIG_STD_MSG,
	CAN_CONFIG_XTD_MSG,
	CAN_CONFIG_MSG_BITS,
	CAN_CONFIG_ALL_MSG,
	CAN_CONFIG_ALL_VALID_MSG
}CANCONFIGFLAGS, *PCANCONFIGFLAGS;


typedef enum _tagCAN_TX_MSG_FLAGS
{
	CAN_TX_PRIORITY_BITS,
 	CAN_TX_PRIORITY_0,
 	CAN_TX_PRIORITY_1,
 	CAN_TX_PRIORITY_2,
 	CAN_TX_PRIORITY_3,

	CAN_TX_FRAME_BIT,
	CAN_TX_STD_FRAME,
	CAN_TX_XTD_FRAME,

	CAN_TX_RTR_BIT,
	CAN_TX_NO_RTR_FRAME,
	CAN_TX_RTR_FRAME
}CAN_TX_MSG_FLAGS, *PCAN_TX_MSG_FLAGS;


typedef enum _tagCanMsgFlags
{
	CAN_MSG_STD_FRAME = 0x00,
	CAN_MSG_XTD_FRAME = 0x01,
	CAN_MSG_RTR_FRAME = 0x02,
	CAN_MSG_INVALID	  = 0x04
}CANMSGFLAGS, *PCANMSGFLAGS;


typedef enum _tagCANMask
{
	CAN_MASK_MB0,
	CAN_MASK_MB1,
	CAN_MASK_MB2,
	CAN_MASK_MB3,
	CAN_MASK_MB4,
	CAN_MASK_MB5,
	CAN_MASK_MB6,
	CAN_MASK_MB7,
	CAN_MASK_MB8,
	CAN_MASK_MB9,
	CAN_MASK_MB10,
	CAN_MASK_MB11,
	CAN_MASK_MB12,
	CAN_MASK_MB13,
	CAN_MASK_MB14,
	CAN_MASK_MB15	
}CANMASK;


typedef enum _tagCanTypeFrame
{
	DATA_FRAME,
	REMOTE_FRAME
}CANTYPEFRAME;

typedef enum _tagMailboxType
{
	MB_DISABLE 		= 0,
	MB_RX 			= 1,
	MB_RX_OVERWRITE = 2,
	MB_TX 			= 3
}MAILBOXTYPE, *PMAILBOXTYPE;

typedef enum 	_tagCANFilter
{
	CAN_FILTER_MB0 	= 0,
	CAN_FILTER_MB1 	= 1,
	CAN_FILTER_MB2 	= 2,
	CAN_FILTER_MB3 	= 3,
	CAN_FILTER_MB4 	= 4,
	CAN_FILTER_MB5 	= 5,
	CAN_FILTER_MB6 	= 6,
	CAN_FILTER_MB7 	= 7,
	CAN_FILTER_MB8 	= 8,
	CAN_FILTER_MB9 	= 9,
	CAN_FILTER_MB10 = 10,
	CAN_FILTER_MB11 = 11,
	CAN_FILTER_MB12 = 12,
	CAN_FILTER_MB13 = 13,
	CAN_FILTER_MB14 = 14,
	CAN_FILTER_MB15 = 15
}CANFILTER, *PCANFILTER;


typedef enum 	_tagCANErrorFlags
{
	FLEXCAN_ERROR_SUCCESS,
	FLEXCAN_ERROR_CODE_BASE,
	FLEXCAN_ERROR_INVALID_BAUD,
	FLEXCAN_ERROR_PENDING_TX,
	FLEXCAN_ERROR_TX_DONE,
	FLEXCAN_ERROR_TX_ERR,
	FLEXCAN_ERROR_PARAM_OUT_OF_RANGE
}CANERRORFLAGS, *PCANERRORFLAGS;


typedef enum 	_tagCANSTD_MODE
{
	CAN_MODE_NORMAL,
	CAN_MODE_LISTEN,
	CAN_MODE_CONFIG,
	CAN_MODE_LOOP_INT,
    CAN_MODE_LOOP_EXT,
 	CAN_MODE_DISABLE,
}CANSTD_MODE, 	*PCANSTD_MODE;


typedef struct 	_tagCANStatus
{
	uint8_t 	nRxOvfl;
	uint8_t 	nTxBusOff;
	uint8_t		nTxBusPassive;
	uint8_t 	nRxBusPassive;
	uint8_t 	nTxWarning;
	uint8_t 	nRxWarning;
	uint8_t 	nInvalidMsg;
	uint8_t 	nRxBufOvfl;
	uint8_t 	nTxLostArb;
	uint8_t 	nTxError;
}CANSTATUS, 	*PCANSTATUS;

typedef struct	_tagCANSTD_BITTIME
{
	uint32_t	BITRATE;
	uint8_t		TQUANTUM;
	uint8_t		PRESDIV;
	uint8_t		PROPSEG;
	uint8_t		SJW;
	uint8_t		PSEG1;
	uint8_t		PSEG2;
}CANSTD_BITTIME, *PCANSTD_BITTIME;


typedef struct _CANParamList
{
	CANSTD_BITTIME	Param[32];
	uint32_t		TQDefault;
}CANPARAMLIST, *PCANPARAMLIST;


#pragma pack(push,1)

typedef union _tagCANMsgId
{
	uint32_t Id;
	
	struct
	{
		uint8_t b0;
		uint8_t b1;
		uint8_t b2;
		uint8_t b3;
	}b;
}CANMSGID, *PCANMSGID;


typedef struct _tagCAN_MSG
{
	CANMSGID		ID;
	uint8_t 		nDLC;
	uint8_t 		data[8];
	CANMSGFLAGS		format;
}CAN_MSG, *PCAN_MSG;


typedef struct 	_tagCAN_TxMsg
{
	CAN_MSG		Msg;
    uint8_t 	priority;
}CAN_TXMSG, *PCAN_TXMSG;


typedef struct 	_tagCAN_RxMsg
{
 	CAN_MSG		Msg;
	uint64_t 	qwTimeStamp;
	
}CAN_RXMSG, *PCAN_RXMSG;


typedef struct _tagCANEmptyMsg
{
 	CANMSGID 	MsgId;
	uint8_t 	nDLC;
	uint8_t 	fFlags;
}CANEMPTYMSG, *PCANEMPTYMSG;

#pragma pack(pop)


#endif /* __CANSTDCONST_DOT_H__ */




























