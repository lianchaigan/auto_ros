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

#include <Common.h>
#include "Move.h"


/*****************************************************************************
 Define
******************************************************************************/


/******************************************************************************
 Typedef 
******************************************************************************/


/******************************************************************************
 Const Variables 
******************************************************************************/
int const g_aMoveFrontJobList[] =
{ 
	RESP_STOP,
	RESP_STOP_DELAY,
	RESP_MOVE_FORWARD,
	0
};


int const g_aMoveLeftJobList[] =
{ 
	RESP_STOP,
	RESP_STOP_DELAY,
	RESP_MOVE_LEFT,
	0
};


int const g_aMoveRightJobList[] =
{ 
	RESP_STOP,
	RESP_STOP_DELAY,
	RESP_MOVE_RIGHT,
	0
};


int const g_aMoveFrontLeftJobList[] =
{ 
	RESP_STOP,
	RESP_STOP_DELAY,
	RESP_MOVE_FRONT_LEFT,
	0
};


int const g_aMoveFrontRightJobList[] =
{ 
	RESP_STOP,
	RESP_STOP_DELAY,
	RESP_MOVE_FRONT_RIGHT,
	0
};


int const g_aMoveBackJobList[] =
{ 
	RESP_STOP,
	RESP_STOP_DELAY,
	RESP_MOVE_BACKWARD,
	0
};


int const g_aMoveBackLeftJobList[] =
{ 
	RESP_STOP,
	RESP_STOP_DELAY,
	RESP_MOVE_BACK_LEFT,
	0
};


int const g_aMoveBackRightJobList[] =
{ 
	RESP_STOP,
	RESP_STOP_DELAY,
	RESP_MOVE_BACK_RIGHT,
	0
};


int const g_aMoveStopJobList[] =
{ 
	RESP_STOP,
	RESP_STOP_DELAY,
	0
};


int const g_aRotateLeftJobList[] =
{
	RESP_STOP,
	RESP_STOP_DELAY,
	RESP_ROTATE_LEFT,
	0
};


int const g_aRotateRightJobList[] =
{
	RESP_STOP,
	RESP_STOP_DELAY,
	RESP_ROTATE_RIGHT,
	0
};


/******************************************************************************
 Global functions
******************************************************************************/


/******************************************************************************
 @Description 	: 
 @param			: 
 @revision		: 1.0.0
 
******************************************************************************/

























