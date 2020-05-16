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
#include "PololuV5Regs.h"
#include "PololuV5.h"


/******************************************************************************
 Define
******************************************************************************/
#define MAGNETOMETER_SLAVE_ADDR  (0x1eU<<1U)
#define ACCELEROMETER_SLAVE_ADDR (0x6bU<<1U)


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

  	g_aBuf[0] = 0;
	I2cRead( g_pI2cHandle, ACCELEROMETER_SLAVE_ADDR, LSM6D_WHO_AM_I, g_aBuf, 1U );
	if( FALSE == mpu_WaitReady() )
	{
		return IMU_POLOLU_STS_I2C_NO_RESP;
	}

    ASSERT( 0x69U == g_aBuf[0] );

	main_Delayms( 5 );

	g_aBuf[0] = 0x50; /* Filter 400Hz, accl -/+2g, Ouput data rate 1.66KHz */
	I2cWrite( g_pI2cHandle, ACCELEROMETER_SLAVE_ADDR, LSM6D_CTRL1_XL, g_aBuf, 1U );
	if( FALSE == mpu_WaitReady() )
	{
		return IMU_POLOLU_STS_I2C_NO_RESP;
	}

	main_Delayms( 5 );

	g_aBuf[0] = 0x50; /* Filter 400Hz, accl 2000dps, Ouput data rate 1.66KHz */
	I2cWrite( g_pI2cHandle, ACCELEROMETER_SLAVE_ADDR, LSM6D_CTRL2_G, g_aBuf, 1U );
	if( FALSE == mpu_WaitReady() )
	{
		return IMU_POLOLU_STS_I2C_NO_RESP;
	}
	
    main_Delayms( 5 );

	g_aBuf[0] = 0x04;
	I2cWrite( g_pI2cHandle, ACCELEROMETER_SLAVE_ADDR, LSM6D_CTRL3_C, g_aBuf, 1U );
	if( FALSE == mpu_WaitReady() )
	{
		return IMU_POLOLU_STS_I2C_NO_RESP;
	}
	
    main_Delayms( 5 );

	g_aBuf[0] = 0x80;
	I2cWrite( g_pI2cHandle, ACCELEROMETER_SLAVE_ADDR, LSM6D_CTRL4_C, g_aBuf, 1U );
	if( FALSE == mpu_WaitReady() )
	{
		return IMU_POLOLU_STS_I2C_NO_RESP;
	}

	main_Delayms( 5 );

	g_aBuf[0] = 0x6e;
	I2cWrite( g_pI2cHandle, ACCELEROMETER_SLAVE_ADDR, LSM6D_FIFO_CTRL5, g_aBuf, 1U );
	if( FALSE == mpu_WaitReady() )
	{
		return IMU_POLOLU_STS_I2C_NO_RESP;
	}

	main_Delayms( 5 );

	g_aBuf[0] = 0x74U;
	I2cWrite( g_pI2cHandle, MAGNETOMETER_SLAVE_ADDR, LIS_CTRL_REG1, g_aBuf, 1U );
	if( FALSE == mpu_WaitReady() )
	{
		return IMU_POLOLU_STS_I2C_NO_RESP;
	}

	main_Delayms( 5 );

	g_aBuf[0] = 0x0cU;
	I2cWrite( g_pI2cHandle, MAGNETOMETER_SLAVE_ADDR, LIS_CTRL_REG4, g_aBuf, 1U );
	if( FALSE == mpu_WaitReady() )
	{
		return IMU_POLOLU_STS_I2C_NO_RESP;
	}

	main_Delayms( 5 );

	g_aBuf[0] = 0x40U;
	I2cWrite( g_pI2cHandle, MAGNETOMETER_SLAVE_ADDR, LIS_CTRL_REG5, g_aBuf, 1U );
	if( FALSE == mpu_WaitReady() )
	{
		return IMU_POLOLU_STS_I2C_NO_RESP;
	}

	main_Delayms( 5 );

	g_aBuf[0] = 0x00;
	I2cWrite( g_pI2cHandle, MAGNETOMETER_SLAVE_ADDR, LIS_CTRL_REG2, g_aBuf, 1U );
	if( FALSE == mpu_WaitReady() )
	{
		return IMU_POLOLU_STS_I2C_NO_RESP;
	}

	main_Delayms( 5 );

	g_aBuf[0] = 0x01;
	I2cWrite( g_pI2cHandle, MAGNETOMETER_SLAVE_ADDR, LIS_CTRL_REG3, g_aBuf, 1U );
	if( FALSE == mpu_WaitReady() )
	{
		return IMU_POLOLU_STS_I2C_NO_RESP;
	}
	
	return IMU_POLOLU_STS_OK;
}


IMU_POLOLU_STS ImuPololuReadUntilComplete( uint8_t Register, void *pData, uint32_t nSize )
{
	IMU_POLOLU_STS res = IMU_POLOLU_STS_OK;

	ASSERT( 0 != g_pI2cHandle );

	I2cRead( g_pI2cHandle, ACCELEROMETER_SLAVE_ADDR, Register, pData, nSize );
    if( FALSE == mpu_WaitReady() )
	{
		res =  IMU_POLOLU_STS_I2C_NO_RESP;
	}

	return res;
}


IMU_POLOLU_STS ImuPololuReadData( PIMU_POLOLU_DATA pData )
{
	ASSERT( 0 != g_pI2cHandle );
	ASSERT( 0 != pData );

	g_bI2cDone = FALSE;

	g_aBuf[0] = 0;
	I2cRead( g_pI2cHandle, ACCELEROMETER_SLAVE_ADDR, LSM6D_STATUS_REG, g_aBuf, 1U );
	if( FALSE == mpu_WaitReady() )
	{
		return IMU_POLOLU_STS_I2C_NO_RESP;
	}

	__NOP();
	__NOP();
			
	if( 0x03 == (g_aBuf[0] & 0x03) )
	{
		g_aBuf[0] = 0;
		I2cRead( g_pI2cHandle, ACCELEROMETER_SLAVE_ADDR, LSM6D_OUTX_L_G, g_aBuf, 12U );
		if( FALSE == mpu_WaitReady() )
		{
			return IMU_POLOLU_STS_I2C_NO_RESP;
		}
				
		pData->nGyroX = g_aBuf[0] | (g_aBuf[1]<<8);
		pData->nGyroY = g_aBuf[2] | (g_aBuf[3]<<8);
		pData->nGyroZ = g_aBuf[4] | (g_aBuf[5]<<8);
        pData->nAcclX = g_aBuf[6] | (g_aBuf[7]<<8);
		pData->nAcclY = g_aBuf[8] | (g_aBuf[9]<<8);
		pData->nAcclZ = g_aBuf[10] | (g_aBuf[11]<<8);
	}

	return IMU_POLOLU_STS_OK;
}


BOOL ImuPololuIsDone( void )
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


















