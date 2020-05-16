/*****************************************************************************
 @Project	: 
 @File 		: MPL3115.h
 @Details  	: 
 @Author	: 
 @Hardware	: 
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
   
******************************************************************************/

#ifndef __MPL3115_DOT_H__
#define __MPL3115_DOT_H__


/*****************************************************************************
 Define
******************************************************************************/
#define MPL_BARO_ADDRESS            (0x60<<1)


#define MPL_REG_STATUS				0x00
#define MPL_REG_PRESSURE_MSB		0x01
#define MPL_REG_PRESSURE_CSB		0x02
#define MPL_REG_PRESSURE_LSB		0x03
#define MPL_REG_TEMP_MSB			0x04
#define MPL_REG_TEMP_LSB			0x05
#define MPL_REG_DR_STATUS			0x06
#define MPL_REG_OUT_P_DELTA_MSB     0x07
#define MPL_REG_OUT_P_DELTA_CSB     0x08
#define MPL_REG_OUT_P_DELTA_LSB     0x09
#define MPL_REG_OUT_T_DELTA_MSB     0x0A
#define MPL_REG_OUT_T_DELTA_LSB     0x0B
#define MPL_REG_WHO_AM_I			0x0C
#define MPL_REG_INT_SOURCE			0x12
#define MPL_REG_PT_DATA_CFG			0x13
#define MPL_REG_BAR_IN_MSB			0x14
#define MPL_REG_BAR_IN_LSB			0x15

#define MPL_REG_CTRL_REG1			0x26
#define MPL_REG_CTRL_REG2			0x27
#define MPL_REG_CTRL_REG3			0x28
#define MPL_REG_CTRL_REG4			0x29
#define MPL_REG_CTRL_REG5			0x2A

/* Data Configuration bits */
#define MPL_BIT_PT_DATA_CFG_TDEFE	0x01
#define MPL_BIT_PT_DATA_CFG_PDEFE	0x02
#define MPL_BIT_PT_DATA_CFG_DREM	0x04

#define MPL_BIT_CTRL_REG1_SBYB		0x01
#define MPL_BIT_CTRL_REG1_OST		0x02
#define MPL_BIT_CTRL_REG1_RST		0x04
#define MPL_BIT_CTRL_REG1_RAW		0x40
#define MPL_BIT_CTRL_REG1_ALT		0x80
#define MPL_BIT_CTRL_REG1_BAR		0x00

#define MPL_BIT_CTRL_REG1_OS1		0x00
#define MPL_BIT_CTRL_REG1_OS2		0x08
#define MPL_BIT_CTRL_REG1_OS4		0x10
#define MPL_BIT_CTRL_REG1_OS8		0x18
#define MPL_BIT_CTRL_REG1_OS16      0x20
#define MPL_BIT_CTRL_REG1_OS32      0x28
#define MPL_BIT_CTRL_REG1_OS64      0x30
#define MPL_BIT_CTRL_REG1_OS128     0x38

/* MPL_REG_CTRL_REG3 Options */
#define MPL_BIT_POL_ACT_LOW			0
#define MPL_BIT_POL_ACT_HIGH		1

#define MPL_BIT_PP_PULLUP			0
#define MPL_BIT_PP_OPEN_DRAIN		1


/*****************************************************************************
 Type definition
******************************************************************************/
#pragma pack( push, 1 )
typedef union _tagMPL_CTRL_REG1
{
	uint8_t Ctrl;

	struct
	{
		uint8_t SBYB	:1;
		uint8_t OST		:1;
		uint8_t RST		:1;
		uint8_t OS		:3;
		uint8_t RAW		:1;
		uint8_t ALT		:1;
	}b;
} MPL_CTRL_REG1, *PMPL_CTRL_REG1;


typedef union _tagMPL_CTRL_REG3
{
	uint8_t Output;

	struct
	{
		uint8_t PP_OD2	:1;
		uint8_t IPOL2	:1;
		uint8_t RESV1	:2;
		uint8_t PP_OD1	:1;
		uint8_t IPOL1	:1;
		uint8_t RESV2	:2;
	}b;
} MPL_CTRL_REG3, *PMPL_CTRL_REG3;


typedef union _tagMPL_CTRL_REG4
{
	uint8_t PIE;

	struct
	{
		uint8_t bTCHG		:1;
		uint8_t bPCHG		:1;
		uint8_t bTTHres		:1;
		uint8_t bPThres		:1;
		uint8_t bTW			:1;
		uint8_t bPW			:1;
		uint8_t bFIFO		:1;
		uint8_t bDRDY		:1;

	}b;
} MPL_CTRL_REG4, *PMPL_CTRL_REG4;


typedef union _tagMPL_CTRL_REG5
{
	uint8_t Pin;

	struct
	{
		uint8_t bTCHGInt1	:1;
		uint8_t bPCHGInt1	:1;
		uint8_t bTTHresInt1	:1;
		uint8_t bPThresInt1	:1;
		uint8_t bTWInt1		:1;
		uint8_t bPWInt1		:1;
		uint8_t bFIFOInt1	:1;
		uint8_t bDRDYInt1	:1;

	}b;
} MPL_CTRL_REG5, *PMPL_CTRL_REG5;


#pragma pack( pop )

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

#endif /* __MPL3115_DOT_H__ */









