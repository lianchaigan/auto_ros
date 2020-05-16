/*****************************************************************************
 @Project	: 
 @File 		: Move.h
 @Details  	:            
 @Author	: 
 @Hardware	: 
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
   
******************************************************************************/

#ifndef __MOVE_DOT_H__
#define __MOVE_DOT_H__


/*****************************************************************************
 Define
******************************************************************************/


/******************************************************************************
 Typedef 
******************************************************************************/
typedef enum _tagResp_State
{
	RESP_NONE = 0,
	RESP_STOP,
	RESP_STOP_DELAY,
	RESP_MOVE_FORWARD,
	RESP_MOVE_BACKWARD,
	RESP_MOVE_LEFT,
	RESP_MOVE_RIGHT,
	RESP_MOVE_FRONT_LEFT,
	RESP_MOVE_BACK_LEFT,
	RESP_MOVE_FRONT_RIGHT,
	RESP_MOVE_BACK_RIGHT,
    RESP_ROTATE_LEFT,
    RESP_ROTATE_RIGHT,
	RESP_ADJUST_RIGHT,
	RESP_ADJUST_LEFT,
	RESP_ADJUST_WAIT
}RESP_STATE_NUM;


typedef enum _tagMove_State
{
    MOVE_NONE = 0,
	MOVE_FWD,
	MOVE_BWD,
	MOVE_LEFT,
	MOVE_RIGHT,
	MOVE_FRONT_LEFT,
	MOVE_FRONT_RIGHT,
	MOVE_BACK_LEFT,
	MOVE_BACK_RIGHT,
	MOVE_ROTATE_LEFT,
	MOVE_ROTATE_RIGHT,
	MOVE_ADJUST_WAIT,
	MOVE_ADJUST_DONE,
	MOVE_STOP
}MOVE_STATE_NUM;


typedef struct _tagMotors_Resp
{
    volatile int const  	*pRespList;
	int						nStopDelayMs;
    int                     nState;
}MOTORS_RESP, *PMOTORS_RESP;

/******************************************************************************
 Global functions
******************************************************************************/


/******************************************************************************
 @Description 	: 
 @param			: 
 @revision		: 1.0.0
 
******************************************************************************/
extern int const g_aMoveFrontJobList[];
extern int const g_aMoveLeftJobList[];
extern int const g_aMoveRightJobList[];
extern int const g_aMoveFrontLeftJobList[];
extern int const g_aMoveFrontRightJobList[];
extern int const g_aMoveBackJobList[];
extern int const g_aMoveBackLeftJobList[];
extern int const g_aMoveBackRightJobList[];
extern int const g_aMoveStopJobList[];
extern int const g_aRotateLeftJobList[];
extern int const g_aRotateRightJobList[];

#endif /* __MOVE_DOT_H__ */























