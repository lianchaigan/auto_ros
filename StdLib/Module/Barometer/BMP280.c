/*****************************************************************************
 @Project	: 
 @File 		: BMP280.c
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
#include "I2c.h"
#include "BMP280.h"


/*****************************************************************************
 Define
******************************************************************************/
#define SEA_LEVEL_HPA 1013.25f


/*****************************************************************************
 Type definition
******************************************************************************/


/*****************************************************************************
 Global Variables
******************************************************************************/


/*****************************************************************************
 Local Variables
******************************************************************************/
static I2C_HOOK g_I2cHook;
static volatile BOOL g_bI2cDone = FALSE;


/*****************************************************************************
 Local Functions
******************************************************************************/


/*****************************************************************************
 callback Functions
******************************************************************************/
static void bmp_cbI2cDone( void );


/*****************************************************************************
 Implementation
******************************************************************************/
void BarometerInit( PBARO_HANDLE pHandle, PI2C_HANDLE pI2cHandle )
{
	uint8_t buf[24];

    ASSERT( 0 != pHandle );
	ASSERT( 0 != pI2cHandle );

	I2cAddHook( pI2cHandle, &g_I2cHook, bmp_cbI2cDone );

	I2cRead( pI2cHandle, B280_ADDRESS, B280_REG_CHIPID, buf, 1U );
	while( FALSE == g_bI2cDone );
    g_bI2cDone = FALSE;

	if( B280_CHIPID != buf[0] )
	{
		ASSERT( FALSE );
	}

//	buf[0] = 0xb6;
//	I2cWrite( pI2cHandle, B280_ADDRESS, B280_REG_SOFTRESET, buf, 1U );
//	while( FALSE == g_bI2cDone );
//    g_bI2cDone = FALSE;
//	while( FALSE == I2cIsDone( pI2cHandle ) );

    pHandle->pI2c = pI2cHandle;

	I2cRead( pI2cHandle, B280_ADDRESS, B280_REG_DIG_T1, buf, 24U );
	while( FALSE == g_bI2cDone );
    g_bI2cDone = FALSE;
	while( FALSE == I2cIsDone( pI2cHandle ) );

	pHandle->aCalT[0] = (buf[1]<<8) | buf[0];
	pHandle->aCalT[1] = (buf[3]<<8) | buf[2];
	pHandle->aCalT[2] = (buf[5]<<8) | buf[4];

	pHandle->aCalP[0] = (buf[7]<<8) | buf[6];
	pHandle->aCalP[1] = (buf[9]<<8) | buf[8];
	pHandle->aCalP[2] = (buf[11]<<8) | buf[10];
	pHandle->aCalP[3] = (buf[13]<<8) | buf[12];
	pHandle->aCalP[4] = (buf[15]<<8) | buf[14];
	pHandle->aCalP[5] = (buf[17]<<8) | buf[16];
	pHandle->aCalP[6] = (buf[19]<<8) | buf[18];
	pHandle->aCalP[7] = (buf[21]<<8) | buf[20];
	pHandle->aCalP[8] = (buf[23]<<8) | buf[22];

	buf[0] = 0x3f;
	I2cWrite( pI2cHandle, B280_ADDRESS, B280_REG_CTRL, buf, 1U );
	while( FALSE == g_bI2cDone );
    g_bI2cDone = FALSE;
   while( FALSE == I2cIsDone( pI2cHandle ) );
}

void BarometerUpdate( PBARO_HANDLE pHandle )
{
	uint8_t buf[8];

    ASSERT( 0 != pHandle );

	I2cRead( pHandle->pI2c, B280_ADDRESS, B280_REG_PRESSUREDATA, buf, 6U );
	while( FALSE == g_bI2cDone );
    g_bI2cDone = FALSE;
    	while( FALSE == I2cIsDone( pHandle->pI2c ) );

    pHandle->nPresRaw = (buf[0]<<12) | (buf[1]<<4) | ((buf[2]>>4)&0x0f);
    pHandle->nTempRaw = (buf[3]<<12) | (buf[4]<<4) | ((buf[5]>>4)&0x0f);
}


uint32_t BarometerPresure( PBARO_HANDLE pHandle )
{
	int64_t pressure = 0;
	int64_t var1;
	int64_t var2;

	ASSERT( 0 != pHandle );

	var1 = ((int64_t)(pHandle->nTempConst)) - 128000;
	var2 = var1 * var1 * (int64_t)pHandle->aCalT[B280_P6];
	var2 = var2 + ((var1 * (int64_t)pHandle->aCalT[B280_P5]) << 17);
	var2 = var2 + (((int64_t)pHandle->aCalT[B280_P4]) << 35);
	var1 = ((var1 * var1 * (int64_t)pHandle->aCalT[B280_P3]) >> 8)
	+ ((var1 * (int64_t)pHandle->aCalT[B280_P2]) << 12);
	var1 = ((INT64_C(0x800000000000) + var1) * ((int64_t)pHandle->aCalT[B280_P1]))>>33;

	if( var1 != 0 )
	{
			pressure = 1048576 - pHandle->nPresRaw;
			pressure = (((pressure << 31) - var2) * 3125) / var1;

			var1 = (((int64_t)pHandle->aCalT[B280_P9]) * (pressure >> 13) * (pressure >> 13))>>25;

			var2 = (((int64_t)pHandle->aCalT[B280_P8]) * pressure) >> 19;

			pressure = ((pressure + var1 + var2) >> 8) + (((int64_t)pHandle->aCalT[B280_P7])<<4);
	}
	else
	{
		pressure = 0;
	}

	pHandle->nPres = (uint32_t)pressure;

	return pHandle->nPres;
}


int32_t BarometerTemperature( PBARO_HANDLE pHandle )
{
	int32_t var1;
	int32_t var2;


	ASSERT( 0 != pHandle );

	var1 = ((((pHandle->nTempRaw>>3) - ((int32_t)pHandle->aCalT[B280_T1]<<1)))
	* ((int32_t)pHandle->aCalT[B280_T2])) >> 11;
	var2 = (((((pHandle->nTempRaw >> 4) - ((int32_t)pHandle->aCalT[B280_T1]))
	* ((pHandle->nTempRaw>> 4) - ((int32_t)pHandle->aCalT[B280_T1]))) >> 12)
	* ((int32_t)pHandle->aCalT[B280_T3])) >> 14;

	pHandle->nTempConst = var1 + var2;
	pHandle->nTemp = (pHandle->nTempConst * 5 + 128) >> 8;


	return pHandle->nTemp;
}


uint32_t BarometerAltitute( PBARO_HANDLE pHandle )
{
	uint32_t alt;
    
    ASSERT( 0 != pHandle );

	return alt;
}


/*****************************************************************************
 Callback functions
******************************************************************************/
static void bmp_cbI2cDone( void )
{
	g_bI2cDone = TRUE;
}


/*****************************************************************************
 Local functions
******************************************************************************/


/*****************************************************************************
 Interrupt functions
******************************************************************************/





















