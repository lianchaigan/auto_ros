/*****************************************************************************
 @Project	: 
 @File 		: 
 @Details  	: All Ports and peripherals configuration                    
 @Author	: 
 @Hardware	: 
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
   
******************************************************************************/

#ifndef __HAL_DOT_H__
#define __HAL_DOT_H__


/*****************************************************************************
 Define
******************************************************************************/
#define GPIO_MODE_INPUT 	0x00U
#define GPIO_MODE_OUTPUT 	0x01U
#define GPIO_MODE_AF		0x02U
#define GPIO_MODE_ANALOGUE	0x03U

#define GPIO_PULL_DIS		0x00U
#define GPIO_PULL_UP		0x01U
#define GPIO_PULL_DWN		0x02U

#define GPIO_SPEED_MIN		0x00U
#define GPIO_SPEED_MEDIUM	0x01U
#define GPIO_SPEED_HIGH 	0x02U
#define GPIO_SPEED_MAX 		0x03U

#define GPIO_PWM_AF1		0x01U
#define GPIO_PWM_AF2		0x02U
#define GPIO_I2C1_AF4		0x04U
#define GPIO_SPI1_AF5		0x05U
#define GPIO_SPI2_AF5		0x05U
#define GPIO_SPI3_AF6       0x06U
#define GPIO_USART2_AF7		0x07U
#define GPIO_UART7_AF8		0x08U


/*****************************************************************************
 Define - Pins
******************************************************************************/
#define PB_LED_BLUE			7U
#define PB_LED_RED			14U


/*****************************************************************************
 Type definition
******************************************************************************/


/*****************************************************************************
 Macro
******************************************************************************/
#define BIT( x )					(1U<<(x))

/* LEDs */
#define LED_RED_ON()				(GPIOB->BSRR = BIT(PB_LED_RED))
#define LED_RED_OFF()				(GPIOB->BSRR = (BIT(PB_LED_RED)<<16U))
#define LED_RED_SET( x )			((x)? LED_RED_ON() : LED_RED_OFF())

#define LED_BLUE_ON()				(GPIOB->BSRR = BIT(PB_LED_BLUE))
#define LED_BLUE_OFF()				(GPIOB->BSRR = (BIT(PB_LED_BLUE)<<16U))
#define LED_BLUE_SET( x )			((x)? LED_BLUE_ON() : LED_BLUE_OFF())


/******************************************************************************
 Global functions
******************************************************************************/


/******************************************************************************
 @Description 	: 

 @param			: 
 
 @revision		: 1.0.0
 
******************************************************************************/
void PortInit( void );



#endif /* __HAL_DOT_H__ */









