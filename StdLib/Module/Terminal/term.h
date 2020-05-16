/*****************************************************************************
 @Project	: 
 @File 		: term.h
 @Details  	: 
 @Author	: 
 @Hardware	: 
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
   
******************************************************************************/

#ifndef __TERM_DOT_H__
#define __TERM_DOT_H__


/*****************************************************************************
 Define
******************************************************************************/
#define CMD_STX						0x30
#define CMD_ETX						0x31		
#define CMD_HOST_PKT_SIZE           10

#define RESP_DEV_CUR_XY				0x01
#define RESP_DEV_ACK				0x02
#define RESP_DEV_STS				0x03
#define RESP_DEV_MAP				0x04
#define RESP_DEV_SPEED				0x05
#define RESP_DEV_SETTING			0x06


/* PC Host terminal command sets */
#define CMD_HOST_START				0x01 
#define CMD_HOST_STOP				0x02 
#define CMD_HOST_MAP_SET			0x03 
#define CMD_HOST_SYS_RESET			0x04 
#define CMD_HOST_SET_REF			0x05 
#define CMD_HOST_SET_CAL_MODE		0x06 
#define CMD_HOST_SAVE				0x07
#define CMD_HOST_MAP_REQ			0x08
#define CMD_HOST_CHANGE_SPEED		0x09
#define CMD_HOST_CHANGE_PID			0x10

#define CMD_HOST_RESTORE			0x2E
#define CMD_HOST_REPORT_ON			0x2F

#define CMD_HOST_MOVE_FWD			0x30 
#define CMD_HOST_MOVE_BWD			0x31 
#define CMD_HOST_ROTATE_LEFT		0x32 
#define CMD_HOST_MOVE_FR			0x33 
#define CMD_HOST_MOVE_RIGHT			0x34 
#define CMD_HOST_MOVE_BR			0x35 
#define CMD_HOST_ROTATE_RIGHT		0x36 
#define CMD_HOST_MOVE_BL			0x37 
#define CMD_HOST_MOVE_LEFT			0x38 
#define CMD_HOST_MOVE_FL			0x39



/*****************************************************************************
 Type definition
******************************************************************************/
#pragma pack(push,1)

typedef  union _tagDev_RESP_PKT
{
    uint8_t aResp[18];

    struct
    {
        uint8_t		STX;
        uint8_t		Cmd;
        uint16_t	RPM;
        uint16_t	Interval;
  		uint16_t	Kp;		/* factor x100 */
        uint16_t	Ki;		/* factor x100 */
        uint16_t	Kd;		/* factor x100 */
        uint8_t		Res[5];
        uint8_t		ETX;
    }Setting;

    struct
    {
        uint8_t STX;
        uint8_t Cmd;
        uint8_t MoveState;
        int32_t nCurX;
        int32_t nCurY;
        uint8_t nPoint;
        int32_t nYaw;
        uint8_t bStop :1;
        uint8_t Runtime :7;
        uint8_t ETX;
    }Info;

    struct
    {
        uint8_t STX;
        uint8_t Cmd;
        int16_t Speed1;
        int16_t Speed2;
        int16_t Speed3;
        int16_t Speed4;
		int16_t	Roll;
        int16_t	Pitch;
        int16_t	Yaw;
        uint8_t Res;
        uint8_t ETX;
    }Param;
}DEV_RESP_PKT, *PDEV_RESP_PKT;


typedef union _tagHOST_REQ_PKT
{
	uint8_t aReq[10];

	struct
	{
		uint8_t		Cmd;
		uint8_t		Rsvd[9];
	}Cmd;

	struct
	{
		uint8_t		Cmd;
		uint32_t	nCount;
		uint8_t		Rsvd[5];
	}Speed;

	struct
	{
		uint8_t		Cmd;
		uint16_t	Kp;
        uint16_t	Ki;
        uint16_t	Kd;
        uint16_t	Interval;
		uint8_t		r0;
	}PID;

    struct
    {
        uint8_t		Cmd;
        uint8_t     Idx;
        uint8_t     XY[8];
    }Point;

    struct
    {
        uint8_t		Cmd;
        uint8_t     Idx;
        uint32_t    X;
        uint32_t    Y;
    }RefPoint;

    struct
    {
        uint8_t		Cmd;
        uint8_t     bON;
		uint8_t		Rsvd[8];
    }Cal;

}HOST_REQ_PKT, *PHOST_REQ_PKT;


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
void TerminalInit( void );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
char *TerminalParse( UART_HANDLE *pUartHandle );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
void TerminalAckToHost( UART_HANDLE *pUartHandle );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
void TerminalMapToHost( UART_HANDLE *pUartHandle, int nIdx, int nX, int nY );


#endif /* __TERM_DOT_H__ */









