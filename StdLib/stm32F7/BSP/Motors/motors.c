/*****************************************************************************
 @Project	: 
 @File 		: motors.c
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
#include "Hal.h"
#include "pwm.h"
#include "Motors.h"

#ifdef _PID_EN
	#include "PID.h"
#endif

/*****************************************************************************
 Define
******************************************************************************/
#define DEFAULT_DUTY			10


/*****************************************************************************
 Type definition
******************************************************************************/


/*****************************************************************************
 Global Variables
******************************************************************************/


/*****************************************************************************
 Local Variables
******************************************************************************/
static PWM_HANDLE 		g_PwmHandle;

#ifdef _PID_EN
	static PID_HANDLE		g_PIDHandles[TOTAL_MOTOR];
#endif


/*****************************************************************************
 Local functions
******************************************************************************/


/*****************************************************************************
 Callback functions
******************************************************************************/


/*****************************************************************************
 Implementation 
******************************************************************************/
void MotorsInit( void )
{
	/* NOTE:
	 PWM for Motor speed control Initialization.
	 20kHz so it will not generate a human hear able sound */

	/* WARNING:
	 Make sure and check what is the maximum frequency can feed into Motor driver board.
	 It may vary from different type of motor driver board */
	PWMInit( &g_PwmHandle, 8U, 20000U );
}

#ifdef _PID_EN
void 
MotorsPIDConfig(
	int32_t nDefaultDutyCycle,
	int32_t nTargetSpeed,
	int32_t nPidSamplingMs,
	int32_t nDutyCycleMin,
	int32_t nDutyCycleMax,
    float	dOutputFactor
	)
{
	memset( g_PIDHandles, 0, sizeof(g_PIDHandles) );

	/* Initialize 4 PID for each motor */
	PIDCtrlInit( &g_PIDHandles[0], &g_PwmHandle, 1, nDefaultDutyCycle, nPidSamplingMs, FALSE );
	PIDCtrlInit( &g_PIDHandles[1], &g_PwmHandle, 2, nDefaultDutyCycle, nPidSamplingMs, FALSE );
	PIDCtrlInit( &g_PIDHandles[2], &g_PwmHandle, 3, nDefaultDutyCycle, nPidSamplingMs, FALSE );
	PIDCtrlInit( &g_PIDHandles[3], &g_PwmHandle, 4, nDefaultDutyCycle, nPidSamplingMs, FALSE );
	
	/* Set desired speed(in term of encoder count) for each motor */
	PIDCtrlSetTargetValue( &g_PIDHandles[0], nTargetSpeed );
	PIDCtrlSetTargetValue( &g_PIDHandles[1], nTargetSpeed);
	PIDCtrlSetTargetValue( &g_PIDHandles[2], nTargetSpeed);
	PIDCtrlSetTargetValue( &g_PIDHandles[3], nTargetSpeed );
	
	PIDCtrlOutputConfig( &g_PIDHandles[0], nDutyCycleMin, nDutyCycleMax, dOutputFactor );
	PIDCtrlOutputConfig( &g_PIDHandles[1], nDutyCycleMin, nDutyCycleMax, dOutputFactor );
	PIDCtrlOutputConfig( &g_PIDHandles[2], nDutyCycleMin, nDutyCycleMax, dOutputFactor );
	PIDCtrlOutputConfig( &g_PIDHandles[3], nDutyCycleMin, nDutyCycleMax, dOutputFactor );
		
	/*PID tunning Parameters */
	/* NOTE:
	Based on Embedded Robotics, Thomas, This is optimized PID.
	 Refer to page 90, Ki = (Kp*interval)/Ti
	*/

	PIDCtrlSetParam( &g_PIDHandles[0], PID_CONST_KP, PID_CONST_KI, PID_CONST_KD );
	PIDCtrlSetParam( &g_PIDHandles[1], PID_CONST_KP, PID_CONST_KI, PID_CONST_KD );
	PIDCtrlSetParam( &g_PIDHandles[2], PID_CONST_KP, PID_CONST_KI, PID_CONST_KD );
	PIDCtrlSetParam( &g_PIDHandles[3], PID_CONST_KP, PID_CONST_KI, PID_CONST_KD );
}


void MotorsSetPIDParams( double Kp, double Ki, double Kd )
{
	PIDCtrlSetParam( &g_PIDHandles[0], Kp, Ki, Kd );
	PIDCtrlSetParam( &g_PIDHandles[1], Kp, Ki, Kd );
	PIDCtrlSetParam( &g_PIDHandles[2], Kp, Ki, Kd );
	PIDCtrlSetParam( &g_PIDHandles[3], Kp, Ki, Kd );
}



void MotorsRunPID( int nIndex, int nSpeed )
{
	PIDOnTimer( &g_PIDHandles[nIndex], nSpeed );
}
#endif


void MotorsAllEnable( int nDutyClyce )
{
	/* Start all motor speed control PWM */
	PWMEnable( &g_PwmHandle, 1, nDutyClyce, FALSE );
	PWMEnable( &g_PwmHandle, 2, nDutyClyce, FALSE );
	PWMEnable( &g_PwmHandle, 3, nDutyClyce, FALSE );
	PWMEnable( &g_PwmHandle, 4, nDutyClyce, FALSE );
    MOTORS_RUN();
}


void Motors1Enable( int nDutyClyce )
{
	/* Start all motor speed control PWM */
	PWMEnable( &g_PwmHandle, 1, nDutyClyce, FALSE );
    M1_PWM_OUT_EN();
}


void Motors2Enable( int nDutyClyce )
{
	/* Start all motor speed control PWM */
	PWMEnable( &g_PwmHandle, 2, nDutyClyce, FALSE );
    M2_PWM_OUT_EN();
}

void Motors3Enable( int nDutyClyce )
{
	/* Start all motor speed control PWM */
	PWMEnable( &g_PwmHandle, 3, nDutyClyce, FALSE );
    M3_PWM_OUT_EN();
}

void Motors4Enable( int nDutyClyce )
{
	/* Start all motor speed control PWM */
	PWMEnable( &g_PwmHandle, 4, nDutyClyce, FALSE );
    M4_PWM_OUT_EN();
}


void MotorsMoveFront( int nDutyClyce  )
{
	MOTORS_MOVE_FORWARD(); /* Set direction */

	/* Start all motor speed control PWM */
	PWMEnable( &g_PwmHandle, 1, nDutyClyce, FALSE );
	PWMEnable( &g_PwmHandle, 2, nDutyClyce, FALSE );
	PWMEnable( &g_PwmHandle, 3, nDutyClyce, FALSE );
	PWMEnable( &g_PwmHandle, 4, nDutyClyce, FALSE );
    MOTORS_RUN();
}

void MotorsMoveBack( int nDutyClyce  )
{
	MOTORS_MOVE_BACKWARD(); /* Set direction */

	/* Start all motor speed control PWM */
	PWMEnable( &g_PwmHandle, 1, nDutyClyce, FALSE );
	PWMEnable( &g_PwmHandle, 2, nDutyClyce, FALSE );
	PWMEnable( &g_PwmHandle, 3, nDutyClyce, FALSE );
	PWMEnable( &g_PwmHandle, 4, nDutyClyce, FALSE );
    MOTORS_RUN();
}


void MotorsMoveLeft( int nDutyClyce  )
{
	MOTORS_MOVE_LEFT(); /* Set direction */

	/* Start all motor speed control PWM */
	PWMEnable( &g_PwmHandle, 1, nDutyClyce, FALSE );
	PWMEnable( &g_PwmHandle, 2, nDutyClyce, FALSE );
	PWMEnable( &g_PwmHandle, 3, nDutyClyce, FALSE );
	PWMEnable( &g_PwmHandle, 4, nDutyClyce, FALSE );
    MOTORS_RUN();
}


void MotorsMoveRight( int nDutyClyce  )
{
	MOTORS_MOVE_RIGHT(); /* Set direction */

	/* Start all motor speed control PWM */
	PWMEnable( &g_PwmHandle, 1, nDutyClyce, FALSE );
	PWMEnable( &g_PwmHandle, 2, nDutyClyce, FALSE );
	PWMEnable( &g_PwmHandle, 3, nDutyClyce, FALSE );
	PWMEnable( &g_PwmHandle, 4, nDutyClyce, FALSE );
    MOTORS_RUN();
}


void MotorsMoveFrontLeft( int nDutyClyce  )
{
	MOTORS_MOVE_FRONT_LEFT(); /* Set direction */
	PWMEnable( &g_PwmHandle, 2, nDutyClyce, FALSE );
	PWMEnable( &g_PwmHandle, 4, nDutyClyce, FALSE );
    M2_PWM_OUT_EN();
    M4_PWM_OUT_EN();
}


void MotorsMoveBackLeft( int nDutyClyce  )
{
	MOTORS_MOVE_BACK_LEFT(); /* Set direction */
	PWMEnable( &g_PwmHandle, 1, nDutyClyce, FALSE );
	PWMEnable( &g_PwmHandle, 3, nDutyClyce, FALSE );
    M1_PWM_OUT_EN();
    M3_PWM_OUT_EN();
}


void MotorsMoveBackRight( int nDutyClyce  )
{
	MOTORS_MOVE_BACK_RIGHT(); /* Set direction */
	PWMEnable( &g_PwmHandle, 2, nDutyClyce, FALSE );
	PWMEnable( &g_PwmHandle, 4, nDutyClyce, FALSE );
    M2_PWM_OUT_EN();
    M4_PWM_OUT_EN();
}


void MotorsMoveFrontRight( int nDutyClyce  )
{
	MOTORS_MOVE_FRONT_RIGHT(); /* Set direction */
	PWMEnable( &g_PwmHandle, 1, nDutyClyce, FALSE );
	PWMEnable( &g_PwmHandle, 3, nDutyClyce, FALSE );
    M1_PWM_OUT_EN();
    M3_PWM_OUT_EN();
}


void MotorsRotateRight( int nDutyClyce )
{
	MOTORS_ROTATE_RIGHT(); /* Set direction */

	/* Start all motor speed control PWM */
	PWMEnable( &g_PwmHandle, 1, nDutyClyce, FALSE );
	PWMEnable( &g_PwmHandle, 2, nDutyClyce, FALSE );
	PWMEnable( &g_PwmHandle, 3, nDutyClyce, FALSE );
	PWMEnable( &g_PwmHandle, 4, nDutyClyce, FALSE );
    MOTORS_RUN();
}


void MotorsRotateLeft( int nDutyClyce )
{
	MOTORS_ROTATE_LEFT(); /* Set direction */

	/* Start all motor speed control PWM */
	PWMEnable( &g_PwmHandle, 1, nDutyClyce, FALSE );
	PWMEnable( &g_PwmHandle, 2, nDutyClyce, FALSE );
	PWMEnable( &g_PwmHandle, 3, nDutyClyce, FALSE );
	PWMEnable( &g_PwmHandle, 4, nDutyClyce, FALSE );
	MOTORS_RUN();
}


#ifdef _PID_EN
void MotorsPIDReset( void )
{
	PIDRestart( &g_PIDHandles[0], FALSE );
	PIDRestart( &g_PIDHandles[1], FALSE );
	PIDRestart( &g_PIDHandles[2], FALSE );
	PIDRestart( &g_PIDHandles[3], FALSE );
}


void MotorsSetTargetSpeed( int nSpeed )
{
	/* Set desired speed(in term of encoder count) for each motor */
	PIDCtrlSetTargetValue( &g_PIDHandles[0], nSpeed );
	PIDCtrlSetTargetValue( &g_PIDHandles[1], nSpeed );
	PIDCtrlSetTargetValue( &g_PIDHandles[2], nSpeed );
	PIDCtrlSetTargetValue( &g_PIDHandles[3], nSpeed );
}


void Motor1SetTargetSpeed( int nSpeed )
{
	/* Set desired speed(in term of encoder count) for each motor */
	PIDCtrlSetTargetValue( &g_PIDHandles[0], nSpeed );
}


void Motor2SetTargetSpeed( int nSpeed )
{
	/* Set desired speed(in term of encoder count) for each motor */
	PIDCtrlSetTargetValue( &g_PIDHandles[1], nSpeed );
}


void Motor3SetTargetSpeed( int nSpeed )
{
	/* Set desired speed(in term of encoder count) for each motor */
	PIDCtrlSetTargetValue( &g_PIDHandles[2], nSpeed );
}


void Motor4SetTargetSpeed( int nSpeed )
{
	/* Set desired speed(in term of encoder count) for each motor */
	PIDCtrlSetTargetValue( &g_PIDHandles[3], nSpeed );
}


void MotorsSetDecreaseStopSpeed( int nDecPercent )
{
	PIDCtrlSetDecreaseRateAtSpeed0( &g_PIDHandles[0], nDecPercent );
	PIDCtrlSetDecreaseRateAtSpeed0( &g_PIDHandles[1], nDecPercent );
	PIDCtrlSetDecreaseRateAtSpeed0( &g_PIDHandles[2], nDecPercent );
	PIDCtrlSetDecreaseRateAtSpeed0( &g_PIDHandles[3], nDecPercent );
}
#endif


void MotorsStop( void )
{
	MOTORS_STOP(); /* Set direction */

	/* Start all motor speed control PWM */
	PWMDisable( &g_PwmHandle, 1 );
	PWMDisable( &g_PwmHandle, 2 );
	PWMDisable( &g_PwmHandle, 3 );
	PWMDisable( &g_PwmHandle, 4 );
}

/*****************************************************************************
 Callback functions
******************************************************************************/


/*****************************************************************************
 Local functions
******************************************************************************/


/*****************************************************************************
 Interrupt functions
******************************************************************************/


















