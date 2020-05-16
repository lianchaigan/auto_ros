/*****************************************************************************
 @Project	: 
 @File 		: BMP280.h
 @Details  	: 
 @Author	: 
 @Hardware	: 
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
   
******************************************************************************/

#ifndef __BMP280_DOT_H__
#define __BMP280_DOT_H__


/*****************************************************************************
 Define
******************************************************************************/
#define B280_ADDRESS                (0x76<<1)
#define B280_CHIPID                 0x58

#define B280_REG_DIG_T1             0x88
#define B280_REG_DIG_T2             0x8A
#define B280_REG_DIG_T3             0x8C

#define B280_REG_DIG_P1             0x8E
#define B280_REG_DIG_P2             0x90
#define B280_REG_DIG_P3             0x92
#define B280_REG_DIG_P4             0x94
#define B280_REG_DIG_P5             0x96
#define B280_REG_DIG_P6             0x98
#define B280_REG_DIG_P7             0x9A
#define B280_REG_DIG_P8             0x9C
#define B280_REG_DIG_P9             0x9E

#define B280_REG_CHIPID             0xD0
#define B280_REG_VERSION            0xD1
#define B280_REG_SOFTRESET          0xE0

#define B280_REG_CAL26              0xE1 /* R calibration stored in 0xE1-0xF0 */

#define B280_REG_CTRL               0xF4
#define B280_REG_CFG                0xF5
#define B280_REG_PRESSUREDATA       0xF7
#define B280_REG_TEMPDATA           0xFA

#define B280_T1						0
#define B280_T2						1
#define B280_T3						2

#define B280_P1						0
#define B280_P2						1
#define B280_P3						2
#define B280_P4						3
#define B280_P5						4
#define B280_P6						5
#define B280_P7						6
#define B280_P8						7
#define B280_P9						8


/*****************************************************************************
 Type definition
******************************************************************************/
typedef struct _tagBaro_Handle
{
    uint16_t aCalT[3];
    uint16_t aCalP[9];
    uint32_t nPresRaw;
    uint32_t nTempRaw;
    int32_t nTempConst;
	int32_t	 nTemp;
	uint32_t nPres;

    PI2C_HANDLE pI2c;

}BARO_HANDLE, *PBARO_HANDLE;


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
void BarometerInit( PBARO_HANDLE pHandle, PI2C_HANDLE pI2cHandle );

void BarometerUpdate( PBARO_HANDLE pHandle );
uint32_t BarometerPresure( PBARO_HANDLE pHandle );
int32_t BarometerTemperature( PBARO_HANDLE pHandle );

uint32_t BarometerAltitute( PBARO_HANDLE pHandle );

#endif /* __BMP280_DOT_H__ */









