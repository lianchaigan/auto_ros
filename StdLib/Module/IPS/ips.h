/*****************************************************************************
 @Project	: 
 @File 		: IPS.h
 @Details  	: 
 @Author	: 
 @Hardware	: 
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
   
******************************************************************************/

#ifndef __IPS_DOT_H__
#define __IPS_DOT_H__


/*****************************************************************************
 Define
******************************************************************************/
#define IPS_DIST_READY			1
#define IPS_SUCCESS				0
#define IPS_UWB_INIT_ERROR		-1
#define IPS_UWB_ERROR			-2
#define IPS_UWB_RX_TIMEOUT		-3
#define IPS_UWB_RX_MISMATCH		-4
#define IPS_UWB_IRQ_TIMEOUT		-5
				 

/*****************************************************************************
 Type definition
******************************************************************************/
typedef int IPS_STS;

#pragma pack(push,1)
typedef union _tagUWB_TX_MSG
{
	uint8_t Data[12];

	struct
	{
		uint16_t FrameCtrl;
		uint8_t	nSequenceNum;
		uint16_t PANID;
		uint16_t DestAddr;
		uint16_t SrcAddr;
		uint8_t Code;
		uint8_t Resvd[2];
	}b;
}UWB_TX_MSG, *PUWB_TX_MSG;


typedef union _tagUWB_RX_MSG
{
	uint8_t Data[20];

	struct
	{
		uint16_t FrameCtrl;
		uint8_t	nSequenceNum;
		uint16_t PANID;
		uint16_t DestAddr;
		uint16_t SrcAddr;
		uint8_t Code;
        uint32_t RxTimestamp;
		uint32_t TxTimestamp;
        uint16_t nCrc;
	}b;
}UWB_RX_MSG, *PUWB_RX_MSG;


typedef union _tagDATA_PKT
{
	uint8_t Data[28];

	struct
	{
		uint16_t	FrameCtrl;
		uint8_t		nSequenceNum;
		uint16_t	PANID;
		uint16_t	 DestAddr;
		uint16_t	SrcAddr;
		uint8_t		Code;
		int32_t		nDist1;
		int32_t		nDist2;
		int32_t		nDist3;
		int32_t		nDist4;
        uint16_t	nCRC;
	}b;

	struct
	{
		uint16_t	FrameCtrl;
		uint8_t		nSequenceNum;
		uint16_t	PANID;
		uint16_t	DestAddr;
		uint16_t	SrcAddr;
		uint8_t		Code;
		int32_t		nDist[4];
        uint16_t	nCRC;
	}a;
}DATA_PKT, *PDATA_PKT;

#pragma pack(pop)


/*****************************************************************************
 Macro
******************************************************************************/


/******************************************************************************
 Global functions
******************************************************************************/


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
IPS_STS IpsInit( SPIM_HANDLE *pSpimHandle );


/******************************************************************************
 @Description 	:  Set Beacon Authorization to group ID, 2 bytes

 @param			:	nBeacon - Beacon number, 1 to 3
					pID - point to 2 bytes Group ID
 
 @revision		: 1.0.0
 
******************************************************************************/
void IPSAuthID( int nBeacon, uint8_t *pID );


/******************************************************************************
 @Description 	:  Set Beacon address, 2 bytes

 @param			:	nBeacon - Beacon number, 1 to 3
					pAddr - point to 2 bytes address
 
 @revision		: 1.0.0
 
******************************************************************************/
void IPSSetAddr( int nBeacon, uint8_t *pAddr );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
void IPSSetReferencePoint( int nIndex, int nX, int nY );
void IPSGetReferencePoint( int nIndex, int *pX, int *pY );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
double IPSGetDistance( int nIndex );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
IPS_STS IpsTimer( void );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
void IPSystemSetIRQTimeout( int nTimeoutMs );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
void IPSSysTimeout1msTimer( void );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
void IPSystemGetCoordinate( double *pX, double *pY );

#endif /* __IPS_DOT_H__ */









