/**
  ******************************************************************************
  * @file           : usbd_conf.c
  * @version        : v2.0_Cube
  * @brief          : This file implements the board support package for the USB device library
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <Common.h>
#include "usbd_def.h"
#include "usbd_core.h"
#include "usbd_customhid.h"
 #include "stm32f1xx_hal_pcd.h"

 extern volatile BOOL g_bUSBStart;

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

static PCD_HANDLE *g_pHpcdIrqHandle = 0;
static USBD_EndpointTypeDef g_EpIn[15];
static USBD_EndpointTypeDef g_EpOut[15]; 

/* USER CODE END PV */

void _Error_Handler(char * file, int line);

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* Private functions ---------------------------------------------------------*/

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

void HAL_PCDEx_SetConnectionState(PCD_HANDLE *hpcd, uint8_t state);

/*******************************************************************************
                       LL Driver Callbacks (PCD -> USB Device Library)
*******************************************************************************/
/* MSP Init */

void HAL_PCD_MspInit(PCD_HANDLE* pcdHandle)
{
  if(pcdHandle->Instance==USB)
  {
  /* USER CODE BEGIN USB_MspInit 0 */

  /* USER CODE END USB_MspInit 0 */
    /* Peripheral clock enable */
    RCC->APB1ENR |= RCC_APB1ENR_USBEN;

    /* Peripheral interrupt init */
    NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 0);
    NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
  /* USER CODE BEGIN USB_MspInit 1 */

  /* USER CODE END USB_MspInit 1 */
  }
}

void HAL_PCD_MspDeInit(PCD_HANDLE* pcdHandle)
{
  if(pcdHandle->Instance==USB)
  {
  /* USER CODE BEGIN USB_MspDeInit 0 */

  /* USER CODE END USB_MspDeInit 0 */
    /* Peripheral clock disable */
    RCC->APB1ENR &= ~RCC_APB1ENR_USBEN;

    /* Peripheral interrupt Deinit*/
    NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);

  /* USER CODE BEGIN USB_MspDeInit 1 */

  /* USER CODE END USB_MspDeInit 1 */
  }
}

/**
  * @brief  Setup stage callback
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_SetupStageCallback(PCD_HANDLE *hpcd)
{
  USBD_LL_SetupStage((USBD_HANDLE*)hpcd->pUSBHandle, (uint8_t *)hpcd->Setup);

  g_bUSBStart = TRUE;
}

 /*
  * @brief  Data Out stage callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint number
  * @retval None
  */
void HAL_PCD_DataOutStageCallback(PCD_HANDLE *hpcd, uint8_t epnum)
{
  USBD_LL_DataOutStage((USBD_HANDLE*)hpcd->pUSBHandle, epnum, hpcd->OUT_ep[epnum].xfer_buff);
}

/**
  * @brief  Data In stage callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint number
  * @retval None
  */
void HAL_PCD_DataInStageCallback(PCD_HANDLE *hpcd, uint8_t epnum)
{
  USBD_LL_DataInStage((USBD_HANDLE*)hpcd->pUSBHandle, epnum, hpcd->IN_ep[epnum].xfer_buff);
}

/**
  * @brief  SOF callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_SOFCallback(PCD_HANDLE *hpcd)
{
  USBD_LL_SOF((USBD_HANDLE*)hpcd->pUSBHandle);
}

/**
  * @brief  Reset callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_ResetCallback(PCD_HANDLE *hpcd)
{ 
  USBD_SpeedTypeDef speed = USBD_SPEED_FULL;

  /* Set USB current speed. */
  switch (hpcd->Init.speed)
  {
  case PCD_SPEED_FULL:
    speed = USBD_SPEED_FULL;
    break;
	
  default:
    speed = USBD_SPEED_FULL;
    break;    
  }
  USBD_LL_SetSpeed((USBD_HANDLE*)hpcd->pUSBHandle, speed);
  
  /* Reset Device. */
  USBD_LL_Reset((USBD_HANDLE*)hpcd->pUSBHandle);
}

/**
  * @brief  Suspend callback.
  * When Low power mode is enabled the debug cannot be used (IAR, Keil doesn't support it)
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_SuspendCallback(PCD_HANDLE *hpcd)
{
    ASSERT( 0 != hpcd );

  /* Inform USB library that core enters in suspend Mode. */
  USBD_LL_Suspend((USBD_HANDLE*)hpcd->pUSBHandle);
  /* Enter in STOP mode. */
  /* USER CODE BEGIN 2 */
  if (hpcd->Init.low_power_enable)
  {
    /* Set SLEEPDEEP bit and SleepOnExit of Cortex System Control Register. */
    SCB->SCR |= (uint32_t)((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
  }
  /* USER CODE END 2 */
}

/**
  * @brief  Resume callback.
  * When Low power mode is enabled the debug cannot be used (IAR, Keil doesn't support it)
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_ResumeCallback(PCD_HANDLE *hpcd)
{
  /* USER CODE BEGIN 3 */

  /* USER CODE END 3 */
  USBD_LL_Resume((USBD_HANDLE*)hpcd->pUSBHandle);
}

/**
  * @brief  ISOOUTIncomplete callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint number
  * @retval None
  */
void HAL_PCD_ISOOUTIncompleteCallback(PCD_HANDLE *hpcd, uint8_t epnum)
{
  USBD_LL_IsoOUTIncomplete((USBD_HANDLE*)hpcd->pUSBHandle, epnum);
}

/**
  * @brief  ISOINIncomplete callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint number
  * @retval None
  */
void HAL_PCD_ISOINIncompleteCallback(PCD_HANDLE *hpcd, uint8_t epnum)
{
  USBD_LL_IsoINIncomplete((USBD_HANDLE*)hpcd->pUSBHandle, epnum);
}

/**
  * @brief  Connect callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_ConnectCallback(PCD_HANDLE *hpcd)
{
  USBD_LL_DevConnected((USBD_HANDLE*)hpcd->pUSBHandle);
}

/**
  * @brief  Disconnect callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_DisconnectCallback(PCD_HANDLE *hpcd)
{
  USBD_LL_DevDisconnected((USBD_HANDLE*)hpcd->pUSBHandle);
}

/*******************************************************************************
                       LL Driver Interface (USB Device Library --> PCD)
*******************************************************************************/

/**
  * @brief  Initializes the low level portion of the device driver.
  * @param  pdev: Device handle
  * @retval USBD status
  */

USBD_STS USBD_LL_Init( USBD_HANDLE *pUSBHandle, void *pHpcd_USB_FS )
{
    /* Init USB Ip. */
    /* Link the driver to the stack. */
    PCD_HANDLE *pcd = (PCD_HANDLE *)pHpcd_USB_FS;

    ASSERT( 0 != pUSBHandle );
    ASSERT( 0 != pHpcd_USB_FS );

    pcd->pUSBHandle = pUSBHandle;
    pUSBHandle->pPCDHandle = pHpcd_USB_FS;
    pUSBHandle->pEpIn = g_EpIn;
    pUSBHandle->pEpOut = g_EpOut;
    ASSERT( 0 != pUSBHandle->pPCDHandle );

    g_pHpcdIrqHandle = pcd;

    pcd->Instance = USB;
    pcd->Init.dev_endpoints = 8;
    pcd->Init.speed = PCD_SPEED_FULL;
    pcd->Init.ep0_mps = DEP0CTL_MPS_64;
    pcd->Init.low_power_enable = DISABLE;
    pcd->Init.lpm_enable = DISABLE;
    pcd->Init.battery_charging_enable = DISABLE;

    if (HAL_PCD_Init( pHpcd_USB_FS ) != HAL_OK)
    {
        ASSERT("PCD initialization Failed\r\n");
    }

    HAL_PCDEx_PMAConfig((PCD_HANDLE*)pUSBHandle->pPCDHandle , 0x00 , PCD_SNG_BUF, 0x18);
    HAL_PCDEx_PMAConfig((PCD_HANDLE*)pUSBHandle->pPCDHandle , 0x80 , PCD_SNG_BUF, 0x58);
    HAL_PCDEx_PMAConfig((PCD_HANDLE*)pUSBHandle->pPCDHandle , CUSTOM_HID_EPIN_ADDR , PCD_SNG_BUF, 0x98);
    HAL_PCDEx_PMAConfig((PCD_HANDLE*)pUSBHandle->pPCDHandle , CUSTOM_HID_EPOUT_ADDR , PCD_SNG_BUF, 0xD8);

    return USBD_OK;
}

/**
  * @brief  De-Initializes the low level portion of the device driver.
  * @param  pdev: Device handle
  * @retval USBD status
  */
USBD_STS USBD_LL_DeInit(USBD_HANDLE *pdev)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_STS usb_status = USBD_OK;

  hal_status = HAL_PCD_DeInit(pdev->pPCDHandle);

  switch (hal_status) {
    case HAL_OK :
      usb_status = USBD_OK;
    break;
    case HAL_ERROR :
      usb_status = USBD_FAIL;
    break;
    case HAL_BUSY :
      usb_status = USBD_BUSY;
    break;
    case HAL_TIMEOUT :
      usb_status = USBD_FAIL;
    break;
    default :
      usb_status = USBD_FAIL;
    break;
  }
  return usb_status; 
}

/**
  * @brief  Starts the low level portion of the device driver.
  * @param  pdev: Device handle
  * @retval USBD status
  */
USBD_STS USBD_LL_Start(USBD_HANDLE *pdev)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_STS usb_status = USBD_OK;
 
  hal_status = HAL_PCD_Start( pdev->pPCDHandle );
     
  switch (hal_status) {
    case HAL_OK :
      usb_status = USBD_OK;
    break;
    case HAL_ERROR :
      usb_status = USBD_FAIL;
    break;
    case HAL_BUSY :
      usb_status = USBD_BUSY;
    break;
    case HAL_TIMEOUT :
      usb_status = USBD_FAIL;
    break;
    default :
      usb_status = USBD_FAIL;
    break;
  }
  return usb_status;
}

/**
  * @brief  Stops the low level portion of the device driver.
  * @param  pdev: Device handle
  * @retval USBD status
  */
USBD_STS USBD_LL_Stop(USBD_HANDLE *pdev)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_STS usb_status = USBD_OK;

  hal_status = HAL_PCD_Stop( pdev->pPCDHandle );

  switch (hal_status) {
    case HAL_OK :
      usb_status = USBD_OK;
    break;
    case HAL_ERROR :
      usb_status = USBD_FAIL;
    break;
    case HAL_BUSY :
      usb_status = USBD_BUSY;
    break;
    case HAL_TIMEOUT :
      usb_status = USBD_FAIL;
    break;
    default :
      usb_status = USBD_FAIL;
    break;
  }
  return usb_status;
}

/**
  * @brief  Opens an endpoint of the low level driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @param  ep_type: Endpoint type
  * @param  ep_mps: Endpoint max packet size
  * @retval USBD status
  */
USBD_STS USBD_LL_OpenEP(USBD_HANDLE *pdev, uint8_t ep_addr, uint8_t ep_type, uint16_t ep_mps)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_STS usb_status = USBD_OK;

  hal_status = HAL_PCD_EP_Open(pdev->pPCDHandle, ep_addr, ep_mps, ep_type);

  switch (hal_status) {
    case HAL_OK :
      usb_status = USBD_OK;
    break;
    case HAL_ERROR :
      usb_status = USBD_FAIL;
    break;
    case HAL_BUSY :
      usb_status = USBD_BUSY;
    break;
    case HAL_TIMEOUT :
      usb_status = USBD_FAIL;
    break;
    default :
      usb_status = USBD_FAIL;
    break;
  }

  return usb_status;
}

/**
  * @brief  Closes an endpoint of the low level driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval USBD status
  */
USBD_STS USBD_LL_CloseEP(USBD_HANDLE *pdev, uint8_t ep_addr)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_STS usb_status = USBD_OK;
  
  hal_status = HAL_PCD_EP_Close( pdev->pPCDHandle, ep_addr );
      
  switch (hal_status) {
    case HAL_OK :
      usb_status = USBD_OK;
    break;
    case HAL_ERROR :
      usb_status = USBD_FAIL;
    break;
    case HAL_BUSY :
      usb_status = USBD_BUSY;
    break;
    case HAL_TIMEOUT :
      usb_status = USBD_FAIL;
    break;
    default :
      usb_status = USBD_FAIL;
    break;
  }
  return usb_status;  
}

/**
  * @brief  Flushes an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval USBD status
  */
USBD_STS USBD_LL_FlushEP(USBD_HANDLE *pdev, uint8_t ep_addr)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_STS usb_status = USBD_OK;
  
  hal_status = HAL_PCD_EP_Flush( pdev->pPCDHandle, ep_addr );
      
  switch (hal_status) {
    case HAL_OK :
      usb_status = USBD_OK;
    break;
    case HAL_ERROR :
      usb_status = USBD_FAIL;
    break;
    case HAL_BUSY :
      usb_status = USBD_BUSY;
    break;
    case HAL_TIMEOUT :
      usb_status = USBD_FAIL;
    break;
    default :
      usb_status = USBD_FAIL;
    break;
  }
  return usb_status;  
}

/**
  * @brief  Sets a Stall condition on an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval USBD status
  */
USBD_STS USBD_LL_StallEP(USBD_HANDLE *pdev, uint8_t ep_addr)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_STS usb_status = USBD_OK;
  
  hal_status = HAL_PCD_EP_SetStall( pdev->pPCDHandle, ep_addr );

  switch (hal_status) {
    case HAL_OK :
      usb_status = USBD_OK;
    break;
    case HAL_ERROR :
      usb_status = USBD_FAIL;
    break;
    case HAL_BUSY :
      usb_status = USBD_BUSY;
    break;
    case HAL_TIMEOUT :
      usb_status = USBD_FAIL;
    break;
    default :
      usb_status = USBD_FAIL;
    break;
  }
  return usb_status;  
}

/**
  * @brief  Clears a Stall condition on an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval USBD status
  */
USBD_STS USBD_LL_ClearStallEP(USBD_HANDLE *pdev, uint8_t ep_addr)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_STS usb_status = USBD_OK;
  
  hal_status = HAL_PCD_EP_ClrStall( pdev->pPCDHandle, ep_addr );  
     
  switch (hal_status) {
    case HAL_OK :
      usb_status = USBD_OK;
    break;
    case HAL_ERROR :
      usb_status = USBD_FAIL;
    break;
    case HAL_BUSY :
      usb_status = USBD_BUSY;
    break;
    case HAL_TIMEOUT :
      usb_status = USBD_FAIL;
    break;
    default :
      usb_status = USBD_FAIL;
    break;
  }
  return usb_status; 
}

/**
  * @brief  Returns Stall condition.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval Stall (1: Yes, 0: No)
  */
uint8_t USBD_LL_IsStallEP(USBD_HANDLE *pdev, uint8_t ep_addr)
{
  PCD_HANDLE *hpcd = (PCD_HANDLE *)pdev->pPCDHandle;
  
  if((ep_addr & 0x80) == 0x80)
  {
    return hpcd->IN_ep[ep_addr & 0x7F].is_stall; 
  }
  else
  {
    return hpcd->OUT_ep[ep_addr & 0x7F].is_stall; 
  }
}

/**
  * @brief  Assigns a USB address to the device.
  * @param  pdev: Device handle
  * @param  dev_addr: Device address
  * @retval USBD status
  */
USBD_STS USBD_LL_SetUSBAddress(USBD_HANDLE *pdev, uint8_t dev_addr)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_STS usb_status = USBD_OK;
  
  hal_status = HAL_PCD_SetAddress( pdev->pPCDHandle, dev_addr );
     
  switch (hal_status) {
    case HAL_OK :
      usb_status = USBD_OK;
    break;
    case HAL_ERROR :
      usb_status = USBD_FAIL;
    break;
    case HAL_BUSY :
      usb_status = USBD_BUSY;
    break;
    case HAL_TIMEOUT :
      usb_status = USBD_FAIL;
    break;
    default :
      usb_status = USBD_FAIL;
    break;
  }
  return usb_status;  
}

/**
  * @brief  Transmits data over an endpoint.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @param  pbuf: Pointer to data to be sent
  * @param  size: Data size    
  * @retval USBD status
  */
USBD_STS USBD_LL_Transmit(USBD_HANDLE *pdev, uint8_t ep_addr, uint8_t *pbuf, uint16_t size)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_STS usb_status = USBD_OK;

  hal_status = HAL_PCD_EP_Transmit( pdev->pPCDHandle, ep_addr, pbuf, size );
     
  switch (hal_status) {
    case HAL_OK :
      usb_status = USBD_OK;
    break;
    case HAL_ERROR :
      usb_status = USBD_FAIL;
    break;
    case HAL_BUSY :
      usb_status = USBD_BUSY;
    break;
    case HAL_TIMEOUT :
      usb_status = USBD_FAIL;
    break;
    default :
      usb_status = USBD_FAIL;
    break;
  }
  return usb_status;    
}

/**
  * @brief  Prepares an endpoint for reception.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @param  pbuf: Pointer to data to be received
  * @param  size: Data size
  * @retval USBD status
  */
USBD_STS USBD_LL_PrepareReceive(USBD_HANDLE *pdev, uint8_t ep_addr, uint8_t *pbuf, uint16_t size)
{
  HAL_StatusTypeDef hal_status = HAL_OK;
  USBD_STS usb_status = USBD_OK;

  hal_status = HAL_PCD_EP_Receive( pdev->pPCDHandle, ep_addr, pbuf, size );
     
  switch (hal_status) {
    case HAL_OK :
      usb_status = USBD_OK;
    break;
    case HAL_ERROR :
      usb_status = USBD_FAIL;
    break;
    case HAL_BUSY :
      usb_status = USBD_BUSY;
    break;
    case HAL_TIMEOUT :
      usb_status = USBD_FAIL;
    break;
    default :
      usb_status = USBD_FAIL;
    break;
  }
  return usb_status; 
}

/**
  * @brief  Returns the last transfered packet size.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval Recived Data Size
  */
uint32_t USBD_LL_GetRxDataSize(USBD_HANDLE *pdev, uint8_t ep_addr)
{
  return HAL_PCD_EP_GetRxCount( (PCD_HANDLE *)pdev->pPCDHandle, ep_addr );
}

/**
  * @brief  Delays routine for the USB device library.
  * @param  Delay: Delay in ms
  * @retval None
  */
void USBD_LL_Delay(uint32_t Delay)
{
   UNUSE( Delay );
  //HAL_Delay(Delay);
  TRACE( "Delay not support\r\n" );
}

/**
  * @brief  Static single allocation.
  * @param  size: Size of allocated memory
  * @retval None
  */
void *USBD_static_malloc(uint32_t size)
{
  static uint32_t mem[(sizeof(USBD_CUSTOM_HID_HANDLE)/4+1)];/* On 32-bit boundary */

  UNUSE(size);

  return mem;
}

/**
  * @brief  Dummy memory free
  * @param  p: Pointer to allocated  memory address
  * @retval None
  */
void USBD_static_free(void *p)
{
    UNUSE( p );
}

/**
  * @brief Software Device Connection
  * @param hpcd: PCD handle
  * @param state: Connection state (0: disconnected / 1: connected)
  * @retval None
  */
void HAL_PCDEx_SetConnectionState(PCD_HANDLE *hpcd, uint8_t state)
{
     UNUSE( hpcd );

  /* USER CODE BEGIN 6 */
  if (state == 1)
  {
    /* Configure Low connection state. */

  }
  else
  {
    /* Configure High connection state. */

  }
  /* USER CODE END 6 */
}


void USB_LP_CAN1_RX0_IRQHandler( void )
{
  /* USER CODE BEGIN USB_LP_CAN1_RX0_IRQn 0 */

  /* USER CODE END USB_LP_CAN1_RX0_IRQn 0 */
  if( 0 != g_pHpcdIrqHandle )
  {
     //    ASSERT( 0x20000174 == g_pHpcdIrqHandle );
     HAL_PCD_IRQHandler( g_pHpcdIrqHandle );
  }
  /* USER CODE BEGIN USB_LP_CAN1_RX0_IRQn 1 */

  /* USER CODE END USB_LP_CAN1_RX0_IRQn 1 */
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
