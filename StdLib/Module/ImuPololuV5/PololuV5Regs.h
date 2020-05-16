/*****************************************************************************
 @Project	: 
 @File 		: PololuV5Regs.h
 @Details  	: 
 @Author	: 
 @Hardware	: 
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
   
******************************************************************************/

#ifndef __POLOLUV5REGS_DOT_H__
#define __POLOLUV5REGS_DOT_H__


/*****************************************************************************
 Define
******************************************************************************/
/* ST 6D accelerometer & gyroscope registers */
#define LSM6D_FUNC_CFG_ACCESS	0x01
#define LSM6D_FIFO_CTRL1		0x06
#define LSM6D_FIFO_CTRL2		0x07
#define LSM6D_FIFO_CTRL3		0x08
#define LSM6D_FIFO_CTRL4		0x09
#define LSM6D_FIFO_CTRL5		0x0A
#define LSM6D_ORIENT_CFG_G		0x0B
#define LSM6D_INT1_CTRL			0x0D
#define LSM6D_INT2_CTRL			0x0E
#define LSM6D_WHO_AM_I			0x0F
#define LSM6D_CTRL1_XL			0x10
#define LSM6D_CTRL2_G			0x11
#define LSM6D_CTRL3_C			0x12
#define LSM6D_CTRL4_C			0x13
#define LSM6D_CTRL5_C			0x14
#define LSM6D_CTRL6_C			0x15
#define LSM6D_CTRL7_G			0x16
#define LSM6D_CTRL8_XL			0x17
#define LSM6D_CTRL9_XL			0x18
#define LSM6D_CTRL10_C			0x19
#define LSM6D_WAKE_UP_SRC		0x1B
#define LSM6D_TAP_SRC			0x1C
#define LSM6D_D6D_SRC			0x1D
#define LSM6D_STATUS_REG		0x1E
#define LSM6D_OUT_TEMP_L		0x20
#define LSM6D_OUT_TEMP			0x21
#define LSM6D_OUTX_L_G			0x22
#define LSM6D_OUTX_H_G			0x23
#define LSM6D_OUTY_L_G			0x24
#define LSM6D_OUTY_H_G			0x25
#define LSM6D_OUTZ_L_G			0x26
#define LSM6D_OUTZ_H_G			0x27
#define LSM6D_OUTX_L_XL			0x28
#define LSM6D_OUTX_H_XL			0x29
#define LSM6D_OUTY_L_XL			0x2A
#define LSM6D_OUTY_H_XL			0x2B
#define LSM6D_OUTZ_L_XL			0x2C
#define LSM6D_OUTZ_H_XL			0x2D
#define LSM6D_FIFO_STATUS1		0x3A
#define LSM6D_FIFO_STATUS2		0x3B
#define LSM6D_FIFO_STATUS3		0x3C
#define LSM6D_FIFO_STATUS4		0x3D
#define LSM6D_FIFO_DATA_OUT_L	0x3E
#define LSM6D_FIFO_DATA_OUT_H	0x3F 
#define LSM6D_TIMESTAMP0_REG	0x40
#define LSM6D_TIMESTAMP1_REG	0x41
#define LSM6D_TIMESTAMP2_REG	0x42
#define LSM6D_STEP_TIMESTAMP_L	0x49
#define LSM6D_STEP_TIMESTAMP_H	0x4A
#define LSM6D_STEP_COUNTER_L	0x4B
#define LSM6D_STEP_COUNTER_H	0x4C
#define LSM6D_FUNC_SRC			0x53
#define LSM6D_TAP_CFG			0x58
#define LSM6D_TAP_THS_6D		0x59
#define LSM6D_UR2				0x5A
#define LSM6D_WAKE_UP_THS		0x5B
#define LSM6D_WAKE_UP_DUR		0x5C
#define LSM6D_FREE_FALL			0x5D

/* ST LIS33 Magnetometer registers */
#define LIS_WHO_AM_I 			0x0F
#define LIS_CTRL_REG1 			0x20
#define LIS_CTRL_REG2 			0x21
#define LIS_CTRL_REG3 			0x22
#define LIS_CTRL_REG4 			0x23
#define LIS_CTRL_REG5 			0x24
#define LIS_STATUS_REG 			0x27
#define LIS_OUT_X_L 			0x28
#define LIS_OUT_X_H				0x29
#define LIS_OUT_Y_L				0x2A
#define LIS_OUT_Y_H 			0x2B
#define LIS_OUT_Z_L 			0x2C
#define LIS_OUT_Z_H 			0x2D
#define LIS_TEMP_OUT_L 			0x2E
#define LIS_TEMP_OUT_H 			0x2F
#define LIS_INT_CFG 			0x30
#define LIS_INT_SRC 			0x31
#define LIS_INT_THS_L			0x32
#define LIS_INT_THS_H			0x33

/*****************************************************************************
 Type definition
******************************************************************************/
#pragma pack(push,1)
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
#pragma pack(pop)


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









