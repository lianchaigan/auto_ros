/*****************************************************************************
 @Project	: 
 @File 		: PololuV3Regs.h
 @Details  	: 
 @Author	: 
 @Hardware	: 
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
   
******************************************************************************/

#ifndef __POLOLUV3REGS_DOT_H__
#define __POLOLUV3REGS_DOT_H__


/*****************************************************************************
 Define
******************************************************************************/
/* ST L3GD20H gyroscope registers */
#define L3GD_GYRO_WHO_AM_I 		0x0F
#define L3GD_GYRO_CTRL1 		0x20
#define L3GD_GYRO_CTRL2 		0x21
#define L3GD_GYRO_CTRL3 		0x22
#define L3GD_GYRO_CTRL4 		0x23
#define L3GD_GYRO_CTRL5 		0x24
#define L3GD_GYRO_REFERENCE 	0x25
#define L3GD_GYRO_OUT_TEMP 		0x26
#define L3GD_GYRO_STATUS 		0x27
#define L3GD_GYRO_OUT_X_L 		0x28
#define L3GD_GYRO_OUT_X_H 		0x29
#define L3GD_GYRO_OUT_Y_L 		0x2A
#define L3GD_GYRO_OUT_Y_H 		0x2B
#define L3GD_GYRO_OUT_Z_L 		0x2C
#define L3GD_GYRO_OUT_Z_H 		0x2D
#define L3GD_GYRO_FIFO_CTRL 	0x2E
#define L3GD_GYRO_FIFO_SRC 		0x2F
#define L3GD_GYRO_IG_CFG 		0x30
#define L3GD_GYRO_IG_SRC 		0x31
#define L3GD_GYRO_IG_THS_XH 	0x32
#define L3GD_GYRO_IG_THS_XL 	0x33
#define L3GD_GYRO_IG_THS_YH 	0x34
#define L3GD_GYRO_IG_THS_YL 	0x35
#define L3GD_GYRO_IG_THS_ZH 	0x36
#define L3GD_GYRO_IG_THS_ZL 	0x37
#define L3GD_GYRO_IG_DURATION 	0x38
#define L3GD_GYRO_LOW_ODR 		0x39

/* ST Accelerometer & Magnetometer registers */
#define LSM3_AMAG_TEMP_OUT_L 	0x05
#define LSM3_AMAG_TEMP_OUT_H 	0x06
#define LSM3_AMAG_STATUS_M 		0x07
#define LSM3_AMAG_OUT_X_L_M 	0x08
#define LSM3_AMAG_OUT_X_H_M 	0x09
#define LSM3_AMAG_OUT_Y_L_M 	0x0A
#define LSM3_AMAG_OUT_Y_H_M 	0x0B
#define LSM3_AMAG_OUT_Z_L_M 	0x0C
#define LSM3_AMAG_OUT_Z_H_M 	0x0D
#define LSM3_AMAG_WHO_AM_I 		0x0F
#define LSM3_AMAG_INT_CTRL_M 	0x12
#define LSM3_AMAG_INT_SRC_M 	0x13
#define LSM3_AMAG_INT_THS_L_M 	0x14
#define LSM3_AMAG_INT_THS_H_M 	0x15
#define LSM3_AMAG_OFFSET_X_L_M 	0x16
#define LSM3_AMAG_OFFSET_X_H_M 	0x17
#define LSM3_AMAG_OFFSET_Y_L_M 	0x18
#define LSM3_AMAG_OFFSET_Y_H_M 	0x19
#define LSM3_AMAG_OFFSET_Z_L_M 	0x1A
#define LSM3_AMAG_OFFSET_Z_H_M 	0x1B 
#define LSM3_AMAG_REFERENCE_X 	0x1C
#define LSM3_AMAG_REFERENCE_Y 	0x1D
#define LSM3_AMAG_REFERENCE_Z 	0x1E
#define LSM3_AMAG_CTRL0 		0x1F
#define LSM3_AMAG_CTRL1 		0x20
#define LSM3_AMAG_CTRL2 		0x21
#define LSM3_AMAG_CTRL3 		0x22
#define LSM3_AMAG_CTRL4 		0x23
#define LSM3_AMAG_CTRL5 		0x24
#define LSM3_AMAG_CTRL6 		0x25
#define LSM3_AMAG_CTRL7 		0x26
#define LSM3_AMAG_STATUS_A 		0x27
#define LSM3_AMAG_OUT_X_L_A 	0x28
#define LSM3_AMAG_OUT_X_H_A 	0x29
#define LSM3_AMAG_OUT_Y_L_A 	0x2A
#define LSM3_AMAG_OUT_Y_H_A 	0x2B
#define LSM3_AMAG_OUT_Z_L_A 	0x2C
#define LSM3_AMAG_OUT_Z_H_A 	0x2D
#define LSM3_AMAG_FIFO_CTRL 	0x2E
#define LSM3_AMAG_FIFO_SRC 		0x2F
#define LSM3_AMAG_IG_CFG1 		0x30
#define LSM3_AMAG_IG_SRC1 		0x31
#define LSM3_AMAG_IG_THS1 		0x32
#define LSM3_AMAG_IG_DUR1 		0x33
#define LSM3_AMAG_IG_CFG2 		0x34
#define LSM3_AMAG_IG_SRC2 		0x35
#define LSM3_AMAG_IG_THS2 		0x36
#define LSM3_AMAG_IG_DUR2 		0x37
#define LSM3_AMAG_CLICK_CFG 	0x38
#define LSM3_AMAG_CLICK_SRC 	0x39
#define LSM3_AMAG_CLICK_THS 	0x3A
#define LSM3_AMAG_TIME_LIMIT 	0x3B
#define LSM3_AMAG_TIME_LATENCY 	0x3C
#define LSM3_AMAG_TIME_WINDOW 	0x3D
#define LSM3_AMAG_ACT_THS 		0x3E
#define LSM3_AMAG_ACT_DUR 		0x3F


/*****************************************************************************
 Type definition
******************************************************************************/
#pragma push
#pragma pack(1)
typedef union _tagMPU_A_G_Data
{
	uint8_t Data[14];
	
	struct
	{
		uint8_t AX_H;
		uint8_t AX_L;
		uint8_t AY_H;
		uint8_t AY_L;
		uint8_t AZ_H;
		uint8_t AZ_L;
		
		uint8_t T_H;
		uint8_t T_L;
		
		uint8_t GX_H;
		uint8_t GX_L;
		uint8_t GY_H;
		uint8_t GY_L;
		uint8_t GZ_H;
		uint8_t GZ_L;
	}b;
}MPU_A_G_DATA, *PMPU_A_G_DATA;
#pragma pop


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


#endif /* __POLOLUV5REGS_DOT_H__ */









