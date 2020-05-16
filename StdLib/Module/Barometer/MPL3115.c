/*****************************************************************************
 @Project	: 
 @File 		: MPL3115.c
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
#include "MPL3115.h"


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
    MPL_CTRL_REG1	Reg1;
    MPL_CTRL_REG3	Reg3;
    MPL_CTRL_REG4	Reg4;
    MPL_CTRL_REG5	Reg5;

    ASSERT( 0 != pHandle );
	ASSERT( 0 != pI2cHandle );

	I2cAddHook( pI2cHandle, &g_I2cHook, bmp_cbI2cDone );

	I2cRead( pI2cHandle, MPL_BARO_ADDRESS, MPL_REG_WHO_AM_I, buf, 1U );
	while( FALSE == g_bI2cDone );
    g_bI2cDone = FALSE;
	while( FALSE == I2cIsDone( pI2cHandle ) );

	if( 0xC4 != buf[0] )
	{
		ASSERT( FALSE );
	}

    pHandle->pI2c = pI2cHandle;

	Reg1.Ctrl = 0;
	Reg1.b.OST = 1;
	Reg1.b.OS = 7;
    Reg1.b.RAW = 0;
	Reg1.b.ALT = 1;
	Reg1.b.SBYB = 0;
	I2cWrite( pI2cHandle, MPL_BARO_ADDRESS, MPL_REG_CTRL_REG1, &Reg1.Ctrl, 1U );
	while( FALSE == g_bI2cDone );
    g_bI2cDone = FALSE;
	while( FALSE == I2cIsDone( pI2cHandle ) );

	/* Configure INT1&INT2 to Acctive low with pullup */
	Reg3.Output = 0;
	Reg3.b.PP_OD2 = MPL_BIT_PP_PULLUP;
	Reg3.b.IPOL2 = MPL_BIT_POL_ACT_LOW;
	Reg3.b.PP_OD1 = MPL_BIT_PP_PULLUP;
	Reg3.b.IPOL1 = MPL_BIT_POL_ACT_LOW;

	I2cWrite( pI2cHandle, MPL_BARO_ADDRESS, MPL_REG_CTRL_REG3, &Reg3.Output, 1U );
	while( FALSE == g_bI2cDone );
    g_bI2cDone = FALSE;
	while( FALSE == I2cIsDone( pI2cHandle ) );

	Reg4.PIE = 0;
	Reg4.b.bDRDY = 1;
	I2cWrite( pI2cHandle, MPL_BARO_ADDRESS, MPL_REG_CTRL_REG4, &Reg4.PIE, 1U );
	while( FALSE == g_bI2cDone );
    g_bI2cDone = FALSE;
	while( FALSE == I2cIsDone( pI2cHandle ) );

    Reg5.Pin = 0;
	I2cWrite( pI2cHandle, MPL_BARO_ADDRESS, MPL_REG_CTRL_REG5, &Reg5.Pin, 1U );
	while( FALSE == g_bI2cDone );
    g_bI2cDone = FALSE;
	while( FALSE == I2cIsDone( pI2cHandle ) );

	buf[0] = MPL_BIT_PT_DATA_CFG_TDEFE | MPL_BIT_PT_DATA_CFG_PDEFE | MPL_BIT_PT_DATA_CFG_DREM;
    I2cWrite( pI2cHandle, MPL_BARO_ADDRESS, MPL_REG_PT_DATA_CFG, buf, 1U );
	while( FALSE == g_bI2cDone );
    g_bI2cDone = FALSE;
	while( FALSE == I2cIsDone( pI2cHandle ) );


	Reg1.b.SBYB = 1;

	I2cWrite( pI2cHandle, MPL_BARO_ADDRESS, MPL_REG_CTRL_REG1, &Reg1.Ctrl, 1U );
	while( FALSE == g_bI2cDone );
    g_bI2cDone = FALSE;
    	while( FALSE == I2cIsDone( pI2cHandle ) );
}

void BarometerUpdate( PBARO_HANDLE pHandle )
{
	uint8_t buf[5];
    MPL_CTRL_REG1	Reg1;

	I2cRead( pHandle->pI2c, MPL_BARO_ADDRESS, MPL_REG_INT_SOURCE, buf, 1U );
	while( FALSE == g_bI2cDone );
    g_bI2cDone = FALSE;
    	while( FALSE == I2cIsDone( pHandle->pI2c ) );

	I2cRead( pHandle->pI2c, MPL_BARO_ADDRESS, MPL_REG_PRESSURE_MSB, buf, 5U );
	while( FALSE == g_bI2cDone );
    g_bI2cDone = FALSE;
	while( FALSE == I2cIsDone( pHandle->pI2c ) );

	pHandle->nPres = ((buf[0]<<12) | (buf[1]<<4) | ((buf[2]>>4)&0x0f))&0xfffff;

    	Reg1.Ctrl = 0;
	Reg1.b.OST = 1;
	Reg1.b.OS = 7;
    Reg1.b.RAW = 0;
	Reg1.b.ALT = 1;
	Reg1.b.SBYB = 0;
	I2cWrite( pHandle->pI2c, MPL_BARO_ADDRESS, MPL_REG_CTRL_REG1, &Reg1.Ctrl, 1U );
	while( FALSE == g_bI2cDone );
    g_bI2cDone = FALSE;
	while( FALSE == I2cIsDone( pHandle->pI2c ) );
}


uint32_t BarometerPresure( PBARO_HANDLE pHandle )
{
	return pHandle->nPres;
}


int32_t BarometerTemperature( PBARO_HANDLE pHandle )
{

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





















