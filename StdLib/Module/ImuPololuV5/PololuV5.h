/*****************************************************************************
 @Project	: 
 @File 		: PololuV5.h
 @Details  	:              
 @Author	: lcgan
 @Hardware	: 
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
   
******************************************************************************/


#ifndef __POLOLUV5_DOT_H__
#define __POLOLUV5_DOT_H__

/*****************************************************************************
 Define
******************************************************************************/
#define IMU_POLOLU_STS_OK					0
#define IMU_POLOLU_STS_NOT_READY			-1
#define IMU_POLOLU_STS_I2C_NO_RESP			-2


/*****************************************************************************
 Type definition
******************************************************************************/
typedef int IMU_POLOLU_STS;

typedef struct _tagIMU_POLOLU_DATA
{
	int16_t nAcclX;
    int16_t nAcclY;
    int16_t nAcclZ;

	int16_t nGyroX;
	int16_t nGyroY;
	int16_t nGyroZ;

	int16_t nMagX;
	int16_t nMagY;
	int16_t nMagZ;
}IMU_POLOLU_DATA, *PIMU_POLOLU_DATA;


/******************************************************************************
 Global functions
******************************************************************************/

/******************************************************************************
 @Description 	:

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
IMU_POLOLU_STS ImuPololuInit(
					PI2C_HANDLE		pHandle,
					PTIMER_HANDLE	pTimerHandle,
					int				nTimerFreq,
					int				nRespTimeoutMs );


/******************************************************************************
 @Description 	:

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
IMU_POLOLU_STS ImuPololuReadUntilComplete( uint8_t Register, void *pData, uint32_t nSize );


/******************************************************************************
 @Description 	:

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
BOOL ImuPololuReadData( PIMU_POLOLU_DATA pData );


/******************************************************************************
 @Description 	:

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
BOOL ImuPololuIsDone( void );

#endif /* __POLOLUV5_DOT_H__ */











	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	