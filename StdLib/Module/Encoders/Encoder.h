/*****************************************************************************
 @Project  : SEP200 2018 Fall
 @File     : Encoder.h
 @Details  : Encoder header file              
 @Author   : Liang
 @Hardware : STM32F722
 
 --------------------------------------------------------------------------
 @Revision  :
  Ver    Author      Date          Changes
 --------------------------------------------------------------------------
   1.0   lcgan      2018-09-01     Initial Release
   1.1   Liang      2018-10-04     Add more description. No any change on design

******************************************************************************/

#ifndef __ENCODER_DOT_H__
#define __ENCODER_DOT_H__


/*****************************************************************************
 Define
******************************************************************************/


/******************************************************************************
 Typedef 
******************************************************************************/
typedef void ENC_CB_READY( void );
typedef void ENC_CB_THRES( int Event );

typedef struct _tagShaftEncHandle
{
  int            nIndex;
  volatile int   nInterval;
  volatile int   nTmpCnt;
  int            nPrevCnt;
  volatile int   nThresCnt;
  volatile int   nFinalCnt;
  ENC_CB_THRES   *pfThresHit;

  void           *pCcm;
  struct _tagShaftEncHandle *pNext;
}SHAFT_ENC_HANDLE, *PSHAFT_ENC_HANDLE;



/******************************************************************************
 Global functions
******************************************************************************/


/******************************************************************************
 @Description : Intialize Shaft encoder module.
                User shall make sure encoder inputs and interrupts are configured
                The interval counter Max. value should be decide by the input
                argument nTimerHz and nIntervalMs.
                TimerAddHook should be called inside. 

 @param       : pTimerHandle  - timer handle that has been initalized
                nTimerHz      - Timer frequency
                *pfcbEncReady - Encoder ready call back function pointer/address
                nIntervalMs   - Encoder sampling interval:
                                If you want to read encoder's result in every 
                                100ms, you need set this value as 100.
                                ENCODER_SMPL_INTRVL_MS preprocessing directive
                                can be used for this parameter. 

 @return      : NULL
  
 @revision    : 1.0.0
 
******************************************************************************/
void ShaftEncoderInit(
  PTIMER_HANDLE   pTimerHandle,
  uint32_t        nTimerHz,
  ENC_CB_READY    *pfcbEncReady,
  uint32_t        nIntervalMs );


/******************************************************************************
 @Description : Configure Motor Encoder and gear parameters

 @param       : nCPR        - the encoder's count (clock cycles) per revolution,
                              or CPR in short.
                              This value is used to calculate RPM. 
                nGearRatio  - Motor gear ratio.
                nIntervalMs - This definition is same as the ShaftEncoderInit
                              function's nIntervalMs.
 
 @return      : NULL
  
 @revision    : 1.0.0
 
******************************************************************************/
void
ShaftEncoderCfg(
  uint32_t    nCPR,
  uint32_t    nGearRatio,
  uint32_t    nIntervalMs );


/******************************************************************************
 @Description : Calculate the motor speed (in the unit of RMP) by encoder's count.
                This function can let the application know the relationshio
                betwen the encoder's count and RPM.

 @param       : nCount - Encoder's count value.
 
 @return      : Motor RPM in int type.
  
 @revision    : 1.0.0
 
******************************************************************************/
int ShaftEncoderCountToRpm( int nCount );


/******************************************************************************
 @Description : Calculate the motor speed (in the unit of encoder's count) by RMP.
                This function can let the application know the relationshio
                betwen the encoder's count and RPM.
                This function is complementary to Description.

 @param       : nRpm - Motor RMP.
 
 @return      : Encoder's count.
  
 @revision    : 1.0.0
 
******************************************************************************/
int ShaftEncoderRpmToCount( int nRpm );


/******************************************************************************
 @Description : This function lets the application layer to dynamically change
                the encoder's counting interval.

 @param       : nIntervalMs - The new encoder counting period.
 
 @return      : NULL
  
 @revision    : 1.0.0
 
******************************************************************************/
void ShaftEncoderChangeInterval( uint32_t nIntervalMs );


/******************************************************************************
 @Description : This function add a new encoder into the resource struct.
                The current pHandle's pNext needs to be maintained to point to 
                the next encoder's pHandle address (link list), by using the 
                g_pHeadHandle variable.

 @param       : pHandle          - Handle for a encoder's resource struct
                nIndex           - ID for each encoder
                *pfcbEncThresHit - The pointer/address of the encoder signal
                                   rising (and/or falling) edge's interrupt
                                   call back function.
                                   This call back function is used to adjust
                                   the motor turn how many degrees, by checking
                                   the number of count from the encoder.
                                   This call back function can be used in 
                                   the orientation adjustment.
 
 @return      : TRUE: successful; FALSE: failed.
  
 @revision    : 1.0.0
 
******************************************************************************/
BOOL ShaftEncoderAdd( PSHAFT_ENC_HANDLE pHandle,int nIndex, ENC_CB_THRES *pfcbEncThresHit );


/******************************************************************************
 @Description : This function is for encoder's CCM mode.
                Not necessary to be implemented.

 @param       : 
 
 @return      : 
  
 @revision    : 1.0.0
 
******************************************************************************/
BOOL ShaftEncoderLinkCCM( PSHAFT_ENC_HANDLE pHandle, void *pCcmHandle );


/******************************************************************************
 @Description : Reset the encoder resource struct's nFinalCnt and nTmpCnt to 0.

 @param       : pHandle - Handle for a encoder's resource struct
 
 @return      : NULL
  
 @revision    : 1.0.0
 
******************************************************************************/
void ShaftEncoderReset( PSHAFT_ENC_HANDLE pHandle );


/******************************************************************************
 @Description : No used for the PID test.

 @param       : 
 
 @return      : NULL
  
 @revision    : 1.0.0
 
******************************************************************************/
void ShaftEncoderSetThresCount( PSHAFT_ENC_HANDLE pHandle,int nCount );


/******************************************************************************
 @Description : Return the encoder's result count value (nFinalCnt).

 @param       : pHandle - Handle for a encoder's resource struct
 
 @return      : Encoder's result count value
 
 @revision    : 1.0.0
 
******************************************************************************/
int ShaftEncoderReadCount( PSHAFT_ENC_HANDLE pHandle );


/******************************************************************************
 @Description : Return the recorded RMP value.

 @param       : pHandle - Handle for a encoder's resource struct
 
 @return      : RPM result.
 
 @revision    : 1.0.0
 
******************************************************************************/
int ShaftEncoderReadRPM( PSHAFT_ENC_HANDLE pHandle );


/******************************************************************************
 @Description : ISR implementation for all encoders' signal rising/falling edge
                interrupt.
                A counter (pHandle's nTmpCnt) should be maintained.

 @param       : pHandle       - Handle for a encoder's resource struct
                bInputEncBSts - Decide whether nTmpCnt is creased by 1, or 
                                decreased by 1, depending on the motor's direction. 
 
 @return      : .
 
 @revision    : 1.0.0
 
******************************************************************************/
void ShaftEncoderISR( PSHAFT_ENC_HANDLE pHandle, BOOL bInputEncBSts );


#endif /* __ENCODER_DOT_H__ */























