/*****************************************************************************
 @Project	: 
 @File 		: PololuV5.c
 @Details  	:             
 @Author	: lcgan
 @Hardware	: STM32
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
   
******************************************************************************/
#include <Common.h>
#include <math.h>
#include "Hal.h"
#include "Timer.h"
#include "i2c.h"
#include "PololuV3Regs.h"
#include "PololuV3.h"


/******************************************************************************
 Define
******************************************************************************/
#define GYRO_SLAVE_ADDR  (0x6bU<<1U)
#define ACCEL_MAG_SLAVE_ADDR (0x1dU<<1U)


/******************************************************************************
 Global variables
******************************************************************************/


/******************************************************************************
 Typedef
******************************************************************************/


/******************************************************************************
 Local Const variables
******************************************************************************/


/******************************************************************************
 Local variables
******************************************************************************/
static volatile BOOL 	g_bI2cDone 		= FALSE;
static volatile BOOL 	g_bDataReady 	= FALSE;

static PI2C_HANDLE		g_pI2cHandle 	= 0;
static I2C_HOOK			g_I2cHook;

static uint8_t 			g_aBuf[64];

static TIMER_HOOK		g_hTimerHook;
static int				g_nIntervalConst = 0;
static volatile int		g_nInterval = 0;


/******************************************************************************
Callback functions
******************************************************************************/
static void mpu_cbI2cOnDone( void );
static void mpu_TimerTick( void );


/*****************************************************************************
 Local function
*****************************************************************************/
static void main_Delayms( int ms );
static BOOL mpu_WaitReady( void );


/*****************************************************************************
 Implementations
*****************************************************************************/
IMU_POLOLU_STS
ImuPololuInit(
	PI2C_HANDLE		pHandle,
	PTIMER_HANDLE	pTimerHandle,
	int				nTimerFreq,
	int				nRespTimeoutMs
	)
{
	BOOL done;
	ASSERT( 0 != pHandle );
	ASSERT( 0 != pTimerHandle );
	ASSERT( 0 != nTimerFreq  );

	g_nIntervalConst = (nTimerFreq * nRespTimeoutMs) / 1000; /* Here we only need 200Hz or 5ms tick */
	g_nInterval = g_nIntervalConst;

	TimerAddHook(
		pTimerHandle,
		&g_hTimerHook,
		mpu_TimerTick );

	I2cAddHook(
		pHandle,
		&g_I2cHook,
		mpu_cbI2cOnDone );
	
	g_pI2cHandle = pHandle;

    /* Read Gyroscope ID and verify  */
  	g_aBuf[0] = 0;
	I2cRead( g_pI2cHandle, GYRO_SLAVE_ADDR, L3GD_GYRO_WHO_AM_I, g_aBuf, 1U );
	if( FALSE == mpu_WaitReady() )
	{
		return IMU_POLOLU_STS_I2C_NO_RESP;
	}

    if( 0xd7U != g_aBuf[0] )
    {
        return IMU_POLOLU_INVALID_ID;
    }

    /* Read Accelerometer and Magnetometer ID and verify  */
	g_aBuf[0] = 0;
	I2cRead( g_pI2cHandle, ACCEL_MAG_SLAVE_ADDR, LSM3_AMAG_WHO_AM_I, g_aBuf, 1U );
	if( FALSE == mpu_WaitReady() )
	{
		return IMU_POLOLU_STS_I2C_NO_RESP;
	}

    if( 0x49U  != g_aBuf[0] )
    {
        return IMU_POLOLU_INVALID_ID;
    }    

    
  	g_aBuf[0] = 0x00; /* Disable Low ODR */
	I2cWrite( g_pI2cHandle, GYRO_SLAVE_ADDR, L3GD_GYRO_LOW_ODR, g_aBuf, 1U );
	if( FALSE == mpu_WaitReady() )
	{
		return IMU_POLOLU_STS_I2C_NO_RESP;
	}

  	g_aBuf[0] = 0x80; /* FS = 00 (+/- 250 dps full scale) */
	I2cWrite( g_pI2cHandle, GYRO_SLAVE_ADDR, L3GD_GYRO_CTRL4, g_aBuf, 1U );
	if( FALSE == mpu_WaitReady() )
	{
		return IMU_POLOLU_STS_I2C_NO_RESP;
	}

  	g_aBuf[0] = 0x8f; /* ODR = 400Hz, cutoff = 20Hz */
	I2cWrite( g_pI2cHandle, GYRO_SLAVE_ADDR, L3GD_GYRO_CTRL1, g_aBuf, 1U );
	if( FALSE == mpu_WaitReady() )
	{
		return IMU_POLOLU_STS_I2C_NO_RESP;
	}

    g_aBuf[0] = 0xc0; /* AFS = 0 (+/- 2 g full scale) */
	I2cWrite( g_pI2cHandle, ACCEL_MAG_SLAVE_ADDR, LSM3_AMAG_CTRL2, g_aBuf, 1U );
	if( FALSE == mpu_WaitReady() )
	{
		return IMU_POLOLU_STS_I2C_NO_RESP;
	}

    g_aBuf[0] = 0x8f; /* ODR = 400Hz */
	I2cWrite( g_pI2cHandle, ACCEL_MAG_SLAVE_ADDR, LSM3_AMAG_CTRL1, g_aBuf, 1U );
	if( FALSE == mpu_WaitReady() )
	{
		return IMU_POLOLU_STS_I2C_NO_RESP;
	}


	return IMU_POLOLU_STS_OK;
}


IMU_POLOLU_STS ImuPololuReadData( PIMU_POLOLU_DATA pData )
{
	ASSERT( 0 != g_pI2cHandle );
	ASSERT( 0 != pData );

	g_bI2cDone = FALSE;

	g_aBuf[0] = 0;
	I2cRead( g_pI2cHandle, ACCEL_MAG_SLAVE_ADDR, LSM3_AMAG_STATUS_A, g_aBuf, 1U );
	if( FALSE == mpu_WaitReady() )
	{
		return IMU_POLOLU_STS_I2C_NO_RESP;
	}

	__NOP();
	__NOP();
			
	if( 0x80 == (g_aBuf[0] & 0x80) )
	{
		g_aBuf[0] = 0;
		I2cRead( g_pI2cHandle, ACCEL_MAG_SLAVE_ADDR, LSM3_AMAG_OUT_X_L_A | 0x80, g_aBuf, 6U );
		if( FALSE == mpu_WaitReady() )
		{
			return IMU_POLOLU_STS_I2C_NO_RESP;
		}
				
        pData->nAcclX = g_aBuf[0] | (g_aBuf[1]<<8);
		pData->nAcclY = g_aBuf[2] | (g_aBuf[3]<<8);
		pData->nAcclZ = g_aBuf[4] | (g_aBuf[5]<<8);
	}


    g_bI2cDone = FALSE;

	g_aBuf[0] = 0;
	I2cRead( g_pI2cHandle, GYRO_SLAVE_ADDR, L3GD_GYRO_STATUS, g_aBuf, 1U );
	if( FALSE == mpu_WaitReady() )
	{
		return IMU_POLOLU_STS_I2C_NO_RESP;
	}

	__NOP();
	__NOP();
			
	if( 0x80 == (g_aBuf[0] & 0x80) )
	{
		g_aBuf[0] = 0;
		I2cRead( g_pI2cHandle, GYRO_SLAVE_ADDR, L3GD_GYRO_OUT_X_L | 0x80, g_aBuf, 6U );
		if( FALSE == mpu_WaitReady() )
		{
			return IMU_POLOLU_STS_I2C_NO_RESP;
		}
				
        pData->nGyroX = g_aBuf[0] | (g_aBuf[1]<<8);
        pData->nGyroY = g_aBuf[2] | (g_aBuf[3]<<8);
        pData->nGyroZ = g_aBuf[4] | (g_aBuf[5]<<8);
	}


	return IMU_POLOLU_STS_OK;
}


static BOOL ImuPololuIsDone( void )
{
	return g_bI2cDone;
}


/*****************************************************************************
 Callback
*****************************************************************************/
static void mpu_cbI2cOnDone( void )
{
	g_bI2cDone = TRUE;
}


static void mpu_TimerTick( void )
{
	if( 0 != g_nInterval )
	{
		g_nInterval--;
	}
}


/*****************************************************************************
 Local helpers
*****************************************************************************/
static BOOL mpu_WaitReady( void )
{
	BOOL res = TRUE;

	g_nInterval = g_nIntervalConst;
    while( FALSE == g_bI2cDone )
	{
		if( 0 == g_nInterval )
		{
            res = FALSE;
			break;
		}
	}

    g_bI2cDone = FALSE;

	return res;
}


static void main_Delayms( int ms )
{
	volatile int cnt = (SystemCoreClock*ms)/1000;
	
	while( cnt-- )
	{
		__NOP();
	}
}


















