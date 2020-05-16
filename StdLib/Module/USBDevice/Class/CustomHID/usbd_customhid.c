/**
  ******************************************************************************
  * @file    usbd_customhid.c
  * @author  MCD Application Team
  * @version V2.4.2
  * @date    11-December-2015
  * @brief   This file provides the CUSTOM_HID core functions.
  *
  * @verbatim
  *      
  *          ===================================================================      
  *                                CUSTOM_HID Class  Description
  *          =================================================================== 
  *           This module manages the CUSTOM_HID class V1.11 following the "Device Class Definition
  *           for Human Interface Devices (CUSTOM_HID) Version 1.11 Jun 27, 2001".
  *           This driver implements the following aspects of the specification:
  *             - The Boot Interface Subclass
  *             - Usage Page : Generic Desktop
  *             - Usage : Vendor
  *             - Collection : Application 
  *      
  * @note     In HS mode and when the DMA is used, all variables and data structures
  *           dealing with the DMA during the transaction process should be 32-bit aligned.
  *           
  *      
  *  @endverbatim
  *
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

/* Includes ------------------------------------------------------------------*/
#include "Common.h"
#include "usbd_customhid.h"
#include "usbd_desc.h"
#include "usbd_ctlreq.h"


static uint8_t  USBD_CUSTOM_HID_Init( void *pdev, uint8_t cfgidx );
static uint8_t  USBD_CUSTOM_HID_DeInit( void *pdev, uint8_t cfgidx );
static uint8_t  USBD_CUSTOM_HID_Setup( void *pdev, USBD_SetupReqTypedef *req );
static uint8_t  *USBD_CUSTOM_HID_GetCfgDesc( uint16_t *length );
static uint8_t  *USBD_CUSTOM_HID_GetDeviceQualifierDesc( uint16_t *length );
static uint8_t  USBD_CUSTOM_HID_DataIn (void *pdev, uint8_t epnum);
static uint8_t  USBD_CUSTOM_HID_DataOut (void *pdev, uint8_t epnum);
static uint8_t  USBD_CUSTOM_HID_EP0_RxReady (void  *pdev);


/** @defgroup USBD_CUSTOM_HID_Private_Variables
  * @{
  */ 
static USB_FS_CB_DATA_RDY *g_pcbDataReady = 0;


USBD_ClassTypeDef USBD_CUSTOM_HID = 
{
	USBD_CUSTOM_HID_Init,
	USBD_CUSTOM_HID_DeInit,
	USBD_CUSTOM_HID_Setup,
	NULL,							/* EP0_TxSent */  
	USBD_CUSTOM_HID_EP0_RxReady,	/* EP0_RxReady*/ /* STATUS STAGE IN */
	USBD_CUSTOM_HID_DataIn,			/* DataIn */
	USBD_CUSTOM_HID_DataOut,
	NULL,							/* SOF */
	NULL,
	NULL,      
	USBD_CUSTOM_HID_GetCfgDesc,
	USBD_CUSTOM_HID_GetCfgDesc, 
	USBD_CUSTOM_HID_GetCfgDesc,
	USBD_CUSTOM_HID_GetDeviceQualifierDesc,
	NULL
};


/* USB CUSTOM_HID device Configuration Descriptor */
static uint8_t USBD_CUSTOM_HID_CfgDesc[USB_CUSTOM_HID_CONFIG_DESC_SIZ] =
{
	0x09,							/* bLength: Configuration Descriptor size */
	USB_DESC_TYPE_CONFIGURATION,	/* bDescriptorType: Configuration */
	USB_CUSTOM_HID_CONFIG_DESC_SIZ,
	/* wTotalLength: Bytes returned */
	0x00,
	0x01,         /*bNumInterfaces: 1 interface*/
	0x01,         /*bConfigurationValue: Configuration value*/
	0x00,         /*iConfiguration: Index of string descriptor describing the configuration*/
	0xA0,         /*bmAttributes: bus powered */
	0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/

	/* Descriptor of CUSTOM HID interface */
	/* 09 */
	0x09,						/*bLength: Interface Descriptor size*/
	USB_DESC_TYPE_INTERFACE,	/*bDescriptorType: Interface descriptor type*/
	0x00,         /*bInterfaceNumber: Number of Interface*/
	0x00,         /*bAlternateSetting: Alternate setting*/
	0x02,         /*bNumEndpoints*/
	0x03,         /*bInterfaceClass: CUSTOM_HID*/
	0x00,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
	0x00,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
	0,            /*iInterface: Index of string descriptor*/

	/* Descriptor of CUSTOM_HID */
	/* 18 */
	0x09,         /*bLength: CUSTOM_HID Descriptor size*/
	CUSTOM_HID_DESCRIPTOR_TYPE, /*bDescriptorType: CUSTOM_HID*/
	0x01,         /*bCUSTOM_HIDUSTOM_HID: CUSTOM_HID Class Spec release number*/
	0x01,
	0x00,         /*bCountryCode: Hardware target country*/
	0x01,         /*bNumDescriptors: Number of CUSTOM_HID class descriptors to follow*/
	0x22,         /*bDescriptorType*/
	USBD_CUSTOM_HID_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
	0x00,

	/* Descriptor of Custom HID endpoints */
	/* 27 */
	0x07,          /*bLength: Endpoint Descriptor size*/
	USB_DESC_TYPE_ENDPOINT, /*bDescriptorType:*/

	CUSTOM_HID_EPIN_ADDR,   /* bEndpointAddress: Endpoint Address (IN) */
	0x03,					/* bmAttributes: Interrupt endpoint */
	CUSTOM_HID_EPIN_SIZE,	/* wMaxPacketSize: 2 Byte max */
	0x00,
	0x01,					/* bInterval: Polling Interval (20 ms) */

	/* 34 */
	0x07,	         /* bLength: Endpoint Descriptor size */
	USB_DESC_TYPE_ENDPOINT,	/* bDescriptorType: */
	CUSTOM_HID_EPOUT_ADDR,  /*bEndpointAddress: Endpoint Address (OUT)*/
	0x03,	/* bmAttributes: Interrupt endpoint */
	CUSTOM_HID_EPOUT_SIZE,	/* wMaxPacketSize: 2 Bytes max  */
	0x00,
	0x01	/* bInterval: Polling Interval (20 ms) */
	/* 41 */
} ;

/* USB CUSTOM_HID device Configuration Descriptor */
static uint8_t USBD_CUSTOM_HID_Desc[USB_CUSTOM_HID_DESC_SIZ] =
{
	/* 18 */
	0x09,         /*bLength: CUSTOM_HID Descriptor size*/
	CUSTOM_HID_DESCRIPTOR_TYPE, /*bDescriptorType: CUSTOM_HID*/
	0x01,         /*bCUSTOM_HIDUSTOM_HID: CUSTOM_HID Class Spec release number*/
	0x01,
	0x00,         /*bCountryCode: Hardware target country*/
	0x01,         /*bNumDescriptors: Number of CUSTOM_HID class descriptors to follow*/
	0x22,         /*bDescriptorType*/
	USBD_CUSTOM_HID_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
	0x00
};

/* USB Standard Device Descriptor */
 static uint8_t USBD_CUSTOM_HID_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00
};


static uint8_t HID_MOUSE_ReportDesc[USBD_CUSTOM_HID_REPORT_DESC_SIZE] =
{
    0x06, 0xa0, 0xff,  // USAGE_PAGE (Vendor Defined Page 1)
    0x09, 0x01,        // USAGE (Vendor Usage 1)
    0xa1, 0x01,        // COLLECTION (Application)

	// The Input report
	0x09, 0x01,     	// Usage ID - vendor defined
	0x15, 0x00,     	// Logical Minimum (0)
	0x26, 0xFF, 0x00,   // Logical Maximum (255)
	0x75, 0x08,     	// Report Size (8 bits)
	0x95, 0x40,     	// Report Count (64 fields)
	0x81, 0x02,     	// Input (Data, Variable, Absolute)  
	
	// The Output report
	0x09, 0x01,     	// Usage ID - vendor defined
	0x15, 0x00,     	// Logical Minimum (0)
	0x26, 0xFF, 0x00,   // Logical Maximum (255)
	0x75, 0x08,     	// Report Size (8 bits)
	0x95, 0x40,     	// Report Count (64 fields)
	0x91, 0x02,      	// Output (Data, Variable, Absolute)  

	// The Feature report
	0x09, 0x02,     	// Usage ID - vendor defined
	0x15, 0x00,     	// Logical Minimum (0)
	0x26, 0xFF, 0x00,   // Logical Maximum (255)
	0x75, 0x08,     	// Report Size (8 bits)
	0x95, 0x40,     	// Report Count (64 fields)
	0xB1, 0x02,      	// Feature (Data, Variable, Absolute)  

    0xc0               // END_COLLECTION
}; 


/**
  * @}
  */ 

/** @defgroup USBD_CUSTOM_HID_Private_Functions
  * @{
  */ 

/**
  * @brief  USBD_CUSTOM_HID_Init
  *         Initialize the CUSTOM_HID interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t USBD_CUSTOM_HID_Init( void *pdev, uint8_t cfgidx )
{
	uint8_t							ret = 0;
	USBD_CUSTOM_HID_HANDLE   *hhid;
	USBD_HANDLE						*usbd = (USBD_HANDLE *)pdev;

	UNUSE( cfgidx );

	/* Open EP IN */
	USBD_LL_OpenEP(
		usbd,
		CUSTOM_HID_EPIN_ADDR,
		USBD_EP_TYPE_INTR,
		CUSTOM_HID_EPIN_SIZE);  

	/* Open EP OUT */
	USBD_LL_OpenEP(
		usbd,
		CUSTOM_HID_EPOUT_ADDR,
		USBD_EP_TYPE_INTR,
		CUSTOM_HID_EPOUT_SIZE);

	usbd->pClassData = USBD_malloc( sizeof(USBD_CUSTOM_HID_HANDLE) );

	if(usbd->pClassData == NULL)
	{
		ret = 1; 
	}
	else
	{
		hhid = (USBD_CUSTOM_HID_HANDLE*) usbd->pClassData;
  
		hhid->state = CUSTOM_HID_IDLE;
		((USBD_CUSTOM_HID_ItfTypeDef *)usbd->pUserData)->Init();

		/* Prepare Out endpoint to receive 1st packet */ 
		USBD_LL_PrepareReceive(
			usbd,
			CUSTOM_HID_EPOUT_ADDR,
			hhid->Report_buf, 
			USBD_CUSTOMHID_OUTREPORT_BUF_SIZE );
	}
    
	return ret;
}

/**
  * @brief  USBD_CUSTOM_HID_Init
  *         DeInitialize the CUSTOM_HID layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_CUSTOM_HID_DeInit( void *pdev, uint8_t cfgidx )
{
    USBD_HANDLE *usbd = (USBD_HANDLE *)pdev;

    UNUSE( cfgidx );

    /* Close CUSTOM_HID EP IN */
    USBD_LL_CloseEP( usbd, CUSTOM_HID_EPIN_ADDR );

    /* Close CUSTOM_HID EP OUT */
    USBD_LL_CloseEP( usbd, CUSTOM_HID_EPOUT_ADDR );

    /* FRee allocated memory */
    if(usbd->pClassData != NULL)
    {
        ((USBD_CUSTOM_HID_ItfTypeDef *)usbd->pUserData)->DeInit();
        USBD_free(usbd->pClassData);
        usbd->pClassData = NULL;
    }

    return USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_Setup
  *         Handle the CUSTOM_HID specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t
USBD_CUSTOM_HID_Setup(
	void					*pdev, 
	USBD_SetupReqTypedef	*req
	)
{
	uint16_t	len		= 0;
	uint8_t		*pbuf	= NULL;
	USBD_HANDLE *usbd	= (USBD_HANDLE *)pdev;
	USBD_CUSTOM_HID_HANDLE *hhid = (USBD_CUSTOM_HID_HANDLE *)usbd->pClassData;

	switch( req->bmRequest & USB_REQ_TYPE_MASK )
	{
		case USB_REQ_TYPE_CLASS :  
			switch( req->bRequest )
			{
				case CUSTOM_HID_REQ_SET_PROTOCOL:
					hhid->Protocol = (uint8_t)(req->wValue);
				break;

				case CUSTOM_HID_REQ_GET_PROTOCOL:
					USBD_CtlSendData( usbd, (uint8_t *)&hhid->Protocol, 1 );    
				break;

				case CUSTOM_HID_REQ_SET_IDLE:
					hhid->IdleState = (uint8_t)(req->wValue >> 8);
				break;

				case CUSTOM_HID_REQ_GET_IDLE:
					USBD_CtlSendData( usbd, (uint8_t *)&hhid->IdleState, 1 );        
				break;      

				case CUSTOM_HID_REQ_SET_REPORT:
					hhid->IsReportAvailable = 1;
					USBD_CtlPrepareRx( usbd, hhid->Report_buf, (uint8_t)(req->wLength) );
				break;

				default:
					USBD_CtlError (usbd, req);
				return USBD_FAIL; 
			}

		break;

		case USB_REQ_TYPE_STANDARD:
			switch (req->bRequest)
			{
				case USB_REQ_GET_DESCRIPTOR: 
					if( req->wValue >> 8 == CUSTOM_HID_REPORT_DESC)
					{
						len = MIN(USBD_CUSTOM_HID_REPORT_DESC_SIZE , req->wLength);
						pbuf =  HID_MOUSE_ReportDesc;
					}
					else if( req->wValue >> 8 == CUSTOM_HID_DESCRIPTOR_TYPE)
					{
						pbuf = USBD_CUSTOM_HID_Desc;   
						len = MIN(USB_CUSTOM_HID_DESC_SIZ , req->wLength);
					}

					USBD_CtlSendData( usbd, pbuf, len );
				break;

				case USB_REQ_GET_INTERFACE :
					USBD_CtlSendData( usbd, (uint8_t *)&hhid->AltSetting, 1 );
				break;

				case USB_REQ_SET_INTERFACE :
					hhid->AltSetting = (uint8_t)(req->wValue);
				break;

				default:
				break;
			}
		break;

		default:
		break;
	}

	return USBD_OK;
}


/**
  * @brief  USBD_CUSTOM_HID_SendReport 
  *         Send CUSTOM_HID Report
  * @param  pdev: device instance
  * @param  buff: pointer to report
  * @retval status
  */
uint8_t
USBD_CUSTOM_HID_SendReport(
	USBD_HANDLE *pdev, 
	uint8_t		*report,
	uint16_t	len
	)
{
	USBD_CUSTOM_HID_HANDLE *hhid = (USBD_CUSTOM_HID_HANDLE*)pdev->pClassData;

	if( pdev->dev_state == USBD_STATE_CONFIGURED )
	{
		if( hhid->state == CUSTOM_HID_IDLE )
		{
			hhid->state = CUSTOM_HID_BUSY;
			USBD_LL_Transmit (
				pdev, 
				CUSTOM_HID_EPIN_ADDR,                                      
				report,
				len );
		}
	}
	return USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_GetCfgDesc 
  *         return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_CUSTOM_HID_GetCfgDesc (uint16_t *length)
{
  *length = sizeof (USBD_CUSTOM_HID_CfgDesc);
  return USBD_CUSTOM_HID_CfgDesc;
}

/**
  * @brief  USBD_CUSTOM_HID_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_CUSTOM_HID_DataIn( void *pdev, uint8_t epnum )
{
    USBD_HANDLE *usbd = (USBD_HANDLE *)pdev;

    UNUSE( epnum );

    /* Ensure that the FIFO is empty before a new transfer, this condition could 
    be caused by  a new transfer before the end of the previous transfer */
    ((USBD_CUSTOM_HID_HANDLE *)usbd->pClassData)->state = CUSTOM_HID_IDLE;

    return USBD_OK;
}



void HAL_PCD_DataOutAddCallback( USB_FS_CB_DATA_RDY *pcbReady )
{
	g_pcbDataReady = pcbReady;
}


static uint8_t  USBD_CUSTOM_HID_DataOut( void *pdev, uint8_t epnum )
{
    USBD_HANDLE *usbd = (USBD_HANDLE *)pdev;
    USBD_CUSTOM_HID_HANDLE     *hhid = (USBD_CUSTOM_HID_HANDLE*)usbd->pClassData;  

    UNUSE( epnum );

    if( 0 != g_pcbDataReady )
    {
        g_pcbDataReady();
    }
  
  ((USBD_CUSTOM_HID_ItfTypeDef *)usbd->pUserData)->OutEvent(hhid->Report_buf[0], 
                                                            hhid->Report_buf[1],
                                                            hhid->Report_buf );
    
    USBD_LL_PrepareReceive(
        usbd, 
        CUSTOM_HID_EPOUT_ADDR,
        hhid->Report_buf, 
        USBD_CUSTOMHID_OUTREPORT_BUF_SIZE );

  return USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_EP0_RxReady
  *         Handles control request data.
  * @param  pdev: device instance
  * @retval status
  */
uint8_t USBD_CUSTOM_HID_EP0_RxReady(void *pdev)
{
  USBD_HANDLE *usbd = (USBD_HANDLE *)pdev;
  USBD_CUSTOM_HID_HANDLE     *hhid = (USBD_CUSTOM_HID_HANDLE*)usbd->pClassData;  

  if (hhid->IsReportAvailable == 1)
  {
    ((USBD_CUSTOM_HID_ItfTypeDef *)usbd->pUserData)->OutEvent(hhid->Report_buf[0], 
                                                              hhid->Report_buf[1],
                                                              hhid->Report_buf );
    hhid->IsReportAvailable = 0;      
  }

  return USBD_OK;
}

/**
* @brief  DeviceQualifierDescriptor 
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
static uint8_t  *USBD_CUSTOM_HID_GetDeviceQualifierDesc (uint16_t *length)
{
  *length = sizeof (USBD_CUSTOM_HID_DeviceQualifierDesc);
  return USBD_CUSTOM_HID_DeviceQualifierDesc;
}

/**
* @brief  USBD_CUSTOM_HID_RegisterInterface
  * @param  pdev: device instance
  * @param  fops: CUSTOMHID Interface callback
  * @retval status
  */
uint8_t
USBD_CUSTOM_HID_RegisterInterface(
    USBD_HANDLE   *pdev, 
    USBD_CUSTOM_HID_ItfTypeDef *fops
    )
{

    ASSERT( 0 != pdev );
    ASSERT( 0 != fops );

    pdev->pUserData = (void *)fops;  

    return USBD_OK;
}
/**
  * @}
  */ 


/**
  * @}
  */ 


/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
