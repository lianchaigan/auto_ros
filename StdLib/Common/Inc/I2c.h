/*****************************************************************************
 @Project	: 
 @File 		: i2c.h
 @Details  	:              
 @Author	: lcgan
 @Hardware	: 
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
   
******************************************************************************/


#ifndef __I2C_DOT_H__
#define __I2C_DOT_H__

/*****************************************************************************
 Define
******************************************************************************/
#define I2C_STS_OK					0
#define I2C_STS_PORT_INVALID		-1
#define I2C_STS_SPEED_INVALID		-2

/*****************************************************************************
 Type definition
******************************************************************************/
typedef void I2C_CB_DONE( void );

typedef enum _I2C_SPEED
{
	I2C_SPEED_10KHZ,
	I2C_SPEED_100KHZ,
    I2C_SPEED_400KHZ,
    I2C_SPEED_1000KHZ
}I2C_SPEED;

typedef struct _tagI2c_Hook
{
	I2C_CB_DONE 			*pfDone;
	struct _tagI2c_Hook 	*pNext;
}I2C_HOOK, *PI2C_HOOK;


typedef struct _tagI2C_Handle
{
	void 				*pI2c;
	uint8_t				nSlaveAddr;
	uint8_t				Register;
	volatile uint8_t	*pData;
	int					nByte;
	int					nLeft;
	volatile int		nTxCount;
	volatile int		nRxCount;
    volatile int		nReadCount;
	int					nClock;
	
	uint8_t				bRepeatStart    :1;
    uint8_t				bWrite          :1;
	uint8_t                             :6;

	PI2C_HOOK			pHeadHook;
}I2C_HANDLE, *PI2C_HANDLE;


/******************************************************************************
 Global functions
******************************************************************************/


/******************************************************************************
 @Description 	: Initialize I2c with parameters and callback

 @param			: nPort - 1 to 3
 
 @revision		: 1.0.0
 
******************************************************************************/
int I2cInit(
		PI2C_HANDLE 	pHandle,
		uint8_t 		nPort,
		I2C_SPEED 		nBusSpeed );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
void I2cSetRiseTimeMax( PI2C_HANDLE pHandle, int nRiseNanoSec );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
void I2cWrite(
	PI2C_HANDLE 	pHandle,
	uint8_t 		nSlaveAddr,
	uint8_t 		Register,
	void const 		*pData,
	int				nByte );


/******************************************************************************
 @Description 	:

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
void I2cRead( 
	PI2C_HANDLE pHandle,
	uint8_t 	nSlaveAddr,
	uint8_t 	Register,
	void 		*pData,
	int			nByte );


/******************************************************************************
 @Description 	:

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
void
I2cAddHook(
	PI2C_HANDLE 		pHandle,
	PI2C_HOOK 			pHook,
	I2C_CB_DONE 		*pfDone );


/******************************************************************************
 @Description 	:

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
BOOL I2cIsDone( PI2C_HANDLE pHandle );


#endif /* __I2C_DOT_H__ */

