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

#define GPIO_TIM1_AF1		0x01U
#define GPIO_TIM2_AF1		0x01U
#define GPIO_PWM_AF1		0x01U
#define GPIO_PWM_AF2		0x02U
#define GPIO_TIM4_AF2		0x02U
#define GPIO_PWM_AF3		0x03U
#define GPIO_I2C1_AF4		0x04U
#define GPIO_SPI1_AF5		0x05U
#define GPIO_SPI2_AF5		0x05U
#define GPIO_SPI3_AF6       0x06U
#define GPIO_USART2_AF7		0x07U
#define GPIO_UART7_AF8		0x08U


/*****************************************************************************
 Define - Pins
******************************************************************************/
#define PB_UWB_IRQ			6U

#define PE_M1_CTRL_IN1		2U
#define PE_M2_CTRL_IN1		3U
#define PE_M3_CTRL_IN1		4U
#define PE_M4_CTRL_IN1		5U

#define PC_M1_PWM_CH1		6U
#define PC_M2_PWM_CH2		7U
#define PC_M3_PWM_CH3		8U
#define PC_M4_PWM_CH4		9U

#define PG_M1_ENC_IN1		0U
#define PG_M1_ENC_IN2		1U
#define PG_M2_ENC_IN1		2U
#define PG_M2_ENC_IN2		3U
#define PG_M3_ENC_IN1		4U
#define PG_M3_ENC_IN2		5U
#define PG_M4_ENC_IN1		6U
#define PG_M4_ENC_IN2		7U

#define PB_LED_BLUE			7U
#define PB_LED_RED			14U

#define PD_USART2_TX_DBG	5U			
#define PD_USART2_RX_DBG	6U

#define PE_UART7_RX			7U
#define PE_UART7_TX			8U


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

/* Motors */
#define M1_DIR_CTRL_ON()			(GPIOE->BSRR = BIT(PE_M1_CTRL_IN1))
#define M1_DIR_CTRL_OFF()			(GPIOE->BSRR = (BIT(PE_M1_CTRL_IN1)<<16U))
#define M1_DIR_CTRL_SET( x )		((x)? M1_DIR_CTRL_ON() : M1_DIR_CTRL_OFF())

#define M1_PWM_OUT_EN()				GPIOC->MODER &= ~GPIO_MODER_MODER6;			\
									GPIOC->MODER |= (GPIO_MODE_AF<<GPIO_MODER_MODER6_Pos);							
#define M1_PWM_OUT_DIS()			GPIOC->MODER &= ~GPIO_MODER_MODER6;			\
									GPIOC->MODER |= (GPIO_MODE_INPUT<<GPIO_MODER_MODER6_Pos);								
#define M1_PWM_OUT_SET( x )			((x)? M1_PWM_OUT_EN() : M1_PWM_OUT_DIS())

#define M2_DIR_CTRL_ON()			(GPIOE->BSRR = BIT(PE_M2_CTRL_IN1))
#define M2_DIR_CTRL_OFF()			(GPIOE->BSRR = (BIT(PE_M2_CTRL_IN1)<<16U))
#define M2_DIR_CTRL_SET( x )		((x)? M2_DIR_CTRL_ON() : M2_DIR_CTRL_OFF())

#define M2_PWM_OUT_EN()				GPIOC->MODER &= ~GPIO_MODER_MODER7;			\
									GPIOC->MODER |= (GPIO_MODE_AF<<GPIO_MODER_MODER7_Pos);
#define M2_PWM_OUT_DIS()			GPIOC->MODER &= ~GPIO_MODER_MODER7;			\
									GPIOC->MODER |= (GPIO_MODE_OUTPUT<<GPIO_MODER_MODER7_Pos);
#define M2_PWM_OUT_SET( x )			((x)? M2_PWM_OUT_EN() : M2_PWM_OUT_DIS())

#define M3_DIR_CTRL_ON()			(GPIOE->BSRR = BIT(PE_M3_CTRL_IN1))
#define M3_DIR_CTRL_OFF()			(GPIOE->BSRR = (BIT(PE_M3_CTRL_IN1)<<16U))
#define M3_DIR_IN_SET( x )			((x)? M3_DIR_CTRL_ON() : M3_DIR_CTRL_OFF())

#define M3_PWM_OUT_EN()				GPIOC->MODER &= ~GPIO_MODER_MODER8;			\
									GPIOC->MODER |= (GPIO_MODE_AF<<GPIO_MODER_MODER8_Pos);
#define M3_PWM_OUT_DIS()			GPIOC->MODER &= ~GPIO_MODER_MODER8;			\
									GPIOC->MODER |= (GPIO_MODE_OUTPUT<<GPIO_MODER_MODER8_Pos);
#define M3_PWM_OUT_SET( x )			((x)? M3_PWM_OUT_EN() : M3_PWM_OUT_DIS())

#define M4_DIR_CTRL_ON()			(GPIOE->BSRR = BIT(PE_M4_CTRL_IN1))
#define M4_DIR_CTRL_OFF()			(GPIOE->BSRR = (BIT(PE_M4_CTRL_IN1)<<16U))
#define M4_DIR_CTRL_SET( x )		((x)? M4_DIR_CTRL_ON() : M4_DIR_CTRL_OFF())

#define M4_PWM_OUT_EN()				GPIOC->MODER &= ~GPIO_MODER_MODER9;			\
									GPIOC->MODER |= (GPIO_MODE_AF<<GPIO_MODER_MODER9_Pos);
#define M4_PWM_OUT_DIS()			GPIOC->MODER &= ~GPIO_MODER_MODER9;			\
									GPIOC->MODER |= (GPIO_MODE_OUTPUT<<GPIO_MODER_MODER9_Pos);
#define M4_PWM_OUT_SET( x )			((x)? M4_PWM_OUT_EN() : M4_PWM_OUT_DIS())

#define IN_M1_ENC_IN1()				((GPIOG->IDR & BIT(PG_M1_ENC_IN1))? TRUE : FALSE)
#define IN_M1_ENC_IN2()				((GPIOG->IDR & BIT(PG_M1_ENC_IN2))? TRUE : FALSE)

#define IN_M2_ENC_IN1()				((GPIOG->IDR & BIT(PG_M2_ENC_IN1))? TRUE : FALSE)
#define IN_M2_ENC_IN2()				((GPIOG->IDR & BIT(PG_M2_ENC_IN2))? TRUE : FALSE)

#define IN_M3_ENC_IN1()				((GPIOG->IDR & BIT(PG_M3_ENC_IN1))? TRUE : FALSE)
#define IN_M3_ENC_IN2()				((GPIOG->IDR & BIT(PG_M3_ENC_IN2))? TRUE : FALSE)

#define IN_M4_ENC_IN1()				((GPIOG->IDR & BIT(PG_M4_ENC_IN1))? TRUE : FALSE)
#define IN_M4_ENC_IN2()				((GPIOG->IDR & BIT(PG_M4_ENC_IN2))? TRUE : FALSE)



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









