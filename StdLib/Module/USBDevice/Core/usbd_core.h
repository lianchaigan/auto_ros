/**
  ******************************************************************************
  * @file    usbd_core.h
  * @author  MCD Application Team
  * @version V2.4.2
  * @date    11-December-2015
  * @brief   Header file for usbd_core.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CORE_H
#define __USBD_CORE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbd_conf.h"
#include "usbd_def.h"
#include "usbd_ioreq.h"
#include "usbd_ctlreq.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */
  
/** @defgroup USBD_CORE
  * @brief This file is the Header file for usbd_core.c file
  * @{
  */ 


/** @defgroup USBD_CORE_Exported_Defines
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */
 

/**
  * @}
  */ 



/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */ 
#define USBD_SOF          USBD_LL_SOF
/**
  * @}
  */ 

/** @defgroup USBD_CORE_Exported_FunctionsPrototype
  * @{
  */ 
USBD_STS USBD_Init(USBD_HANDLE *pdev, void *pHpcd_USB_FS, USBD_DESC_DEF *pdesc, uint8_t id);
USBD_STS USBD_DeInit(USBD_HANDLE *pdev);
USBD_STS USBD_Start  (USBD_HANDLE *pdev);
USBD_STS USBD_Stop   (USBD_HANDLE *pdev);
USBD_STS USBD_RegisterClass(USBD_HANDLE *pdev, USBD_ClassTypeDef *pclass);

USBD_STS USBD_RunTestMode (USBD_HANDLE  *pdev); 
USBD_STS USBD_SetClassConfig(USBD_HANDLE  *pdev, uint8_t cfgidx);
USBD_STS USBD_ClrClassConfig(USBD_HANDLE  *pdev, uint8_t cfgidx);

USBD_STS USBD_LL_SetupStage(USBD_HANDLE *pdev, uint8_t *psetup);
USBD_STS USBD_LL_DataOutStage(USBD_HANDLE *pdev , uint8_t epnum, uint8_t *pdata);
USBD_STS USBD_LL_DataInStage(USBD_HANDLE *pdev , uint8_t epnum, uint8_t *pdata);

USBD_STS USBD_LL_Reset(USBD_HANDLE  *pdev);
USBD_STS USBD_LL_SetSpeed(USBD_HANDLE  *pdev, USBD_SpeedTypeDef speed);
USBD_STS USBD_LL_Suspend(USBD_HANDLE  *pdev);
USBD_STS USBD_LL_Resume(USBD_HANDLE  *pdev);

USBD_STS USBD_LL_SOF(USBD_HANDLE  *pdev);
USBD_STS USBD_LL_IsoINIncomplete(USBD_HANDLE  *pdev, uint8_t epnum);
USBD_STS USBD_LL_IsoOUTIncomplete(USBD_HANDLE  *pdev, uint8_t epnum);

USBD_STS USBD_LL_DevConnected(USBD_HANDLE  *pdev);
USBD_STS USBD_LL_DevDisconnected(USBD_HANDLE  *pdev);

/* USBD Low Level Driver */
USBD_STS  USBD_LL_Init (USBD_HANDLE *pdev, void *pHpcd_USB_FS );
USBD_STS  USBD_LL_DeInit (USBD_HANDLE *pdev);
USBD_STS  USBD_LL_Start(USBD_HANDLE *pdev);
USBD_STS  USBD_LL_Stop (USBD_HANDLE *pdev);
USBD_STS  USBD_LL_OpenEP  (USBD_HANDLE *pdev, 
                                      uint8_t  ep_addr,                                      
                                      uint8_t  ep_type,
                                      uint16_t ep_mps);

USBD_STS  USBD_LL_CloseEP (USBD_HANDLE *pdev, uint8_t ep_addr);   
USBD_STS  USBD_LL_FlushEP (USBD_HANDLE *pdev, uint8_t ep_addr);   
USBD_STS  USBD_LL_StallEP (USBD_HANDLE *pdev, uint8_t ep_addr);   
USBD_STS  USBD_LL_ClearStallEP (USBD_HANDLE *pdev, uint8_t ep_addr);   
uint8_t             USBD_LL_IsStallEP (USBD_HANDLE *pdev, uint8_t ep_addr);   
USBD_STS  USBD_LL_SetUSBAddress (USBD_HANDLE *pdev, uint8_t dev_addr);   
USBD_STS  USBD_LL_Transmit (USBD_HANDLE *pdev, 
                                      uint8_t  ep_addr,                                      
                                      uint8_t  *pbuf,
                                      uint16_t  size);

USBD_STS  USBD_LL_PrepareReceive(USBD_HANDLE *pdev, 
                                           uint8_t  ep_addr,                                      
                                           uint8_t  *pbuf,
                                           uint16_t  size);

uint32_t USBD_LL_GetRxDataSize  (USBD_HANDLE *pdev, uint8_t  ep_addr);  
void  USBD_LL_Delay (uint32_t Delay);

/**
  * @}
  */ 

#ifdef __cplusplus
}
#endif

#endif /* __USBD_CORE_H */

/**
  * @}
  */ 

/**
* @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/



