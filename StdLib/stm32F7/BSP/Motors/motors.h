/*****************************************************************************
 @Project	: 
 @File 		: motors.h
 @Details  	:              
 @Author	: 
 @Hardware	: 
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
   
******************************************************************************/


#ifndef __MOTORS_DOT_H__
#define __MOTORS_DOT_H__

/*****************************************************************************
 Define
******************************************************************************/
#define TOTAL_MOTOR					4

#define PID_CONST_KP				0.3
#define PID_CONST_KI				0.1
#define PID_CONST_KD				0.1


/* Combination wheel control */
#define MOTORS_MOVE_FORWARD()				M1_DIR_CTRL_ON();	\
											M2_DIR_CTRL_ON();	\
											M3_DIR_CTRL_ON();	\
											M4_DIR_CTRL_ON();

#define MOTORS_MOVE_BACKWARD()				M1_DIR_CTRL_OFF();	\
											M2_DIR_CTRL_OFF();	\
											M3_DIR_CTRL_OFF();	\
											M4_DIR_CTRL_OFF();

#define MOTORS_MOVE_RIGHT()					M1_DIR_CTRL_ON(); 	\
											M2_DIR_CTRL_OFF();	\
											M3_DIR_CTRL_ON();	\
											M4_DIR_CTRL_OFF();
									
#define MOTORS_MOVE_LEFT()					M1_DIR_CTRL_OFF(); 	\
											M2_DIR_CTRL_ON();	\
											M3_DIR_CTRL_OFF();	\
											M4_DIR_CTRL_ON()
									
									
#define MOTORS_MOVE_FRONT_LEFT()			M1_DIR_CTRL_OFF(); 	\
											M2_DIR_CTRL_ON();	\
											M3_DIR_CTRL_OFF();	\
											M4_DIR_CTRL_ON()

#define MOTORS_MOVE_FRONT_RIGHT()			M1_DIR_CTRL_ON(); 	\
											M2_DIR_CTRL_OFF();	\
											M3_DIR_CTRL_ON();	\
											M4_DIR_CTRL_OFF()
									
#define MOTORS_MOVE_BACK_LEFT()				M1_DIR_CTRL_OFF(); 	\
											M2_DIR_CTRL_OFF();	\
											M3_DIR_CTRL_OFF();	\
											M4_DIR_CTRL_OFF()
									
#define MOTORS_MOVE_BACK_RIGHT()			M1_DIR_CTRL_OFF(); 	\
											M2_DIR_CTRL_OFF();	\
											M3_DIR_CTRL_OFF();	\
											M4_DIR_CTRL_OFF()

#define MOTORS_ROTATE_LEFT()				M1_DIR_CTRL_OFF(); 	\
											M2_DIR_CTRL_OFF();	\
											M3_DIR_CTRL_ON();	\
											M4_DIR_CTRL_ON()

#define MOTORS_ROTATE_RIGHT()				M1_DIR_CTRL_ON(); 	\
											M2_DIR_CTRL_ON();	\
											M3_DIR_CTRL_OFF();	\
											M4_DIR_CTRL_OFF()


#define MOTORS_STOP()						M1_PWM_OUT_DIS();	\
											M2_PWM_OUT_DIS();	\
											M3_PWM_OUT_DIS();	\
											M4_PWM_OUT_DIS();

#define MOTORS_RUN()						M1_PWM_OUT_EN();	\
											M2_PWM_OUT_EN();	\
											M3_PWM_OUT_EN();	\
											M4_PWM_OUT_EN();


/*****************************************************************************
 Type definition
******************************************************************************/


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
void MotorsInit( void );


#ifdef _PID_EN
/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
void MotorsPIDConfig(
		int32_t nDefaultDutyCycle,
		int32_t nTargetSpeed,
		int32_t nPidSamplingMs,
		int32_t nDutyCycleMin,
		int32_t nDutyCycleMax,
		float	dOutputFactor );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
void MotorsSetPIDParams( double Kp, double Ki, double Kd );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
void MotorsRunPID( int nIndex, int nSpeed );
#endif

/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
void MotorsPIDReset( void );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
void MotorsSetTargetSpeed( int nSpeed );

void Motor1SetTargetSpeed( int nSpeed );
void Motor2SetTargetSpeed( int nSpeed );
void Motor3SetTargetSpeed( int nSpeed );
void Motor4SetTargetSpeed( int nSpeed );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
void MotorsSetDecreaseStopSpeed( int nDecPercent );


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
void MotorsAllEnable( int nDutyClyce );
void Motors1Enable( int nDutyClyce );
void Motors2Enable( int nDutyClyce );
void Motors3Enable( int nDutyClyce );
void Motors4Enable( int nDutyClyce );

void MotorsMoveFront( int nDutyClyce );
void MotorsMoveBack( int nDutyClyce );
void MotorsMoveLeft( int nDutyClyce );
void MotorsMoveRight( int nDutyClyce );
void MotorsMoveFrontLeft( int nDutyClyce );
void MotorsMoveBackLeft( int nDutyClyce );
void MotorsMoveFrontRight( int nDutyClyce );
void MotorsMoveBackRight( int nDutyClyce );
void MotorsRotateLeft( int nDutyClyce );
void MotorsRotateRight( int nDutyClyce );
void MotorsStop( void );

#endif /* __MOTORS_DOT_H__ */