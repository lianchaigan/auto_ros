/*****************************************************************************
 @Project	: DPGUI
 @File 		:
 @Details  	:                  
 @Author	: lcgan
 @Hardware	: This is hardware depend. Different LCD need to update this file.
			  This driver purely use for 2.0" TFT LCD with ILI9225DS driver
 
 --------------------------------------------------------------------------
 @Revision	:
  Ver  	Author    	Date        	Changes
 --------------------------------------------------------------------------
   1.0  Name     XXXX-XX-XX  		Initial Release
   
******************************************************************************/
#include <Common.h>
#include "Hal.h"
#include "spim.h"
#include "LCD.h"
#include "gui.h"


/*****************************************************************************
 Define
******************************************************************************/
#define LCD_PHY_SIZE_X		176		/* Different LCD has different size in pixel */
#define LCD_PHY_SIZE_Y		220


/*****************************************************************************
 Type definition
******************************************************************************/


/*****************************************************************************
 Global Variables
******************************************************************************/


/*****************************************************************************
 Local Variables
******************************************************************************/
static PSPIM_HANDLE	g_pSpimHandle;
static volatile int	g_bSpiDone = 0;
static int			g_nScreenX = LCD_PHY_SIZE_X;
static int			g_nScreenY = LCD_PHY_SIZE_Y;
static LCD_CB_DONE	*g_pfLcdCbTransferDone = 0;
static uint8_t		g_Cmd;
static uint8_t		g_Data;
static char data[64];


/*****************************************************************************
 Callback functions
******************************************************************************/
static void lcd_cbSpiDone( void );


/*****************************************************************************
 Local functions
******************************************************************************/
static void lcd_WriteCmd( uint8_t Cmd );
static void lcd_WriteData( uint8_t Data );
static void lcd_Delayms( uint32_t ms );
static void lcd_WriteMultipleByte( void const *pData, int nSize );;
static void lcd_WriteData16( uint16_t Data );
void LCD_WriteReg( uint8_t Reg, uint16_t RegValue );

/*****************************************************************************
 Implementation
******************************************************************************/
void
LcdInit(
	PSPIM_HANDLE 		pSpimHAndle,
	LCD_ORIENTATION 	Orientation
	)
{
	ASSERT( 0 != pSpimHAndle );

	/* Keep SPI handle for later use */
	g_pSpimHandle = pSpimHAndle;
	
	/* Add a callback from SPI to notify SPI done */
	SpimAddCallbackTransferDone( pSpimHAndle, lcd_cbSpiDone );
	
	/* All the LCD initialization code normally come from LCD maker. 
	 Same ship might have different internal connection to LCD glass */

	/*******************
	 Hardware  Reset
	********************/
	LCD_CS_HIGH();
	LCD_RESET_HIGH();
	
	lcd_Delayms( 5U );
	
	LCD_RESET_LOW();
	
	lcd_Delayms( 5U );
	
	LCD_RESET_HIGH();
	
	lcd_Delayms( 5U );
	
    LCD_CS_HIGH();

	/*******************
	 ILI9225 init
	********************/
	LCD_WriteReg(0x01,0x011c);
	LCD_WriteReg(0x02,0x0100);
	LCD_WriteReg(0x03,0x1038);
	LCD_WriteReg(0x08,0x0808); // set BP and FP
	LCD_WriteReg(0x0B,0x1100); //frame cycle
	LCD_WriteReg(0x0C,0x0000); // RGB interface setting R0Ch=0x0110 for RGB 18Bit and R0Ch=0111for RGB16Bit
	LCD_WriteReg(0x0F,0x1401); // Set frame rate----0801
	LCD_WriteReg(0x15,0x0000); //set system interface
	LCD_WriteReg(0x20,0x0000); // Set GRAM Address
	LCD_WriteReg(0x21,0x0000); // Set GRAM Address

	/*******************
	 Power On sequence
	********************/
	lcd_Delayms(50);                         // Delay 50ms
	LCD_WriteReg(0x10,0x0800); // Set SAP,DSTB,STB----0A00
	LCD_WriteReg(0x11,0x1F3F); // Set APON,PON,AON,VCI1EN,VC----1038
	lcd_Delayms(50);                         // Delay 50ms
	LCD_WriteReg(0x12,0x0121); // Internal reference voltage= Vci;----1121
	LCD_WriteReg(0x13,0x006F); // Set GVDD----0066
	LCD_WriteReg(0x14,0x4349); // Set VCOMH/VCOML voltage----5F60

	/*******************
	 Set GRAM area
	********************/
	LCD_WriteReg(0x30,0x0000);
	LCD_WriteReg(0x31,0x00DB);
	LCD_WriteReg(0x32,0x0000);
	LCD_WriteReg(0x33,0x0000);
	LCD_WriteReg(0x34,0x00DB);
	LCD_WriteReg(0x35,0x0000);
	LCD_WriteReg(0x36,0x00AF);
	LCD_WriteReg(0x37,0x0000);
	LCD_WriteReg(0x38,0x00DB);
	LCD_WriteReg(0x39,0x0000);

	/**********************
	 Adjust the Gamma Curve
	***********************/
	LCD_WriteReg(0x50,0x0001);  //0400
	LCD_WriteReg(0x51,0x200B);  //060B
	LCD_WriteReg(0x52,0x0000);  //0C0A
	LCD_WriteReg(0x53,0x0404);  //0105
	LCD_WriteReg(0x54,0x0C0C);  //0A0C
	LCD_WriteReg(0x55,0x000C);  //0B06
	LCD_WriteReg(0x56,0x0101);  //0004
	LCD_WriteReg(0x57,0x0400);  //0501
	LCD_WriteReg(0x58,0x1108);  //0E00
	LCD_WriteReg(0x59,0x050C);  //000E




    if( LCD_PORTRAIT == Orientation ) /* Select portrait or landscape */
	{
		g_nScreenX = LCD_PHY_SIZE_X;
		g_nScreenY = LCD_PHY_SIZE_Y;		
		LCD_WriteReg(0x03,0x1030); 
	}
	else
	{
		g_nScreenX = LCD_PHY_SIZE_Y;
		g_nScreenY = LCD_PHY_SIZE_X;	
		LCD_WriteReg(0x03,0x1028);
	}

    	lcd_Delayms(50);
	LCD_WriteReg(0x07,0x1017);
	lcd_WriteCmd(0x22);

}


void LCD_GetSize( int *pScreenX, int *pScreenY )
{
	*pScreenX = g_nScreenX;
	*pScreenY = g_nScreenY;	
}


void LCD_AddCallback( LCD_CB_DONE *pDone )
{
	g_pfLcdCbTransferDone = pDone;
}


void LCD_Reset( void )
{
	char data[4];
	
	LCD_WriteReg(0x36, g_nScreenX - 1);
	LCD_WriteReg(0x37, 0);
	LCD_WriteReg(0x38, g_nScreenY - 1);
	LCD_WriteReg(0x39, 0);
	LCD_WriteReg(0x20, 0);
	LCD_WriteReg(0x21, 0);
	lcd_WriteCmd(0x22);
}


void LCD_WriteData8( uint8_t Data )
{
	g_Data = Data;
	ASSERT( 0 != g_pSpimHandle );

	LCD_DC_HIGH();
	LCD_CS_LOW();
	
	SpimTransfer( g_pSpimHandle, &g_Data, 0, 1U );
	
	while( 0 == g_bSpiDone );
	g_bSpiDone = 0;

	LCD_CS_HIGH();  
}


void LCD_WriteDataPixel( void const *pData, int nSize )
{
	/* NOTE:
	This function is purely for LCD has been set in graphic pixel memory.
	It will only transfer pixel data from GUI library */
	ASSERT( 0 != g_pSpimHandle );

	LCD_DC_HIGH();
	LCD_CS_LOW();
	SpimTransfer( g_pSpimHandle, pData, 0, nSize );

	/* NOTE:
	SPI transfer can be blocking if wait until all data was sent.
	Or user specify a callback that it will not block the flow,
	it will callback to user when it is done 
	*/
	if( 0 == g_pfLcdCbTransferDone )
	{
		/* There is no callback, user wish to use blocking wait */
		while( 0 == g_bSpiDone );
		g_bSpiDone = 0;
		LCD_CS_HIGH(); 
	}
}


/*****************************************************************************
 Callback functions
******************************************************************************/
static void lcd_cbSpiDone( void )
{
	/* SPI transfer completed. Check user callback */
	if( 0 == g_pfLcdCbTransferDone )
	{
		/* Set polling flag */
		g_bSpiDone = 1;
	}
	else
	{
		/* Callback to user */
		g_pfLcdCbTransferDone();
        LCD_CS_HIGH();
	}
}


/*****************************************************************************
 Local functions
******************************************************************************/
static void lcd_WriteCmd( uint8_t Cmd )
{
	g_Cmd = Cmd;

	ASSERT( 0 != g_pSpimHandle );

	LCD_DC_LOW();
	LCD_CS_LOW();

	SpimTransfer( g_pSpimHandle, &g_Cmd, 0, 1U );
	
	while( 0 == g_bSpiDone );
	g_bSpiDone = 0;

	LCD_CS_HIGH();  
}


static void lcd_WriteData( uint8_t Data )
{
	g_Data = Data; 
	ASSERT( 0 != g_pSpimHandle );

	LCD_DC_HIGH();
	LCD_CS_LOW();

	SpimTransfer( g_pSpimHandle, &g_Data, 0, 1U );
	while( 0 == g_bSpiDone );
	g_bSpiDone = 0;

	LCD_CS_HIGH();  
}


static void lcd_WriteData16( uint16_t Data )
{
	data[0] = (uint8_t)(Data>>8U);
	data[1] = (uint8_t)(Data&0xffU);
	ASSERT( 0 != g_pSpimHandle );

	LCD_DC_HIGH();
	LCD_CS_LOW();

	SpimTransfer( g_pSpimHandle, data, 0, 2U );
	while( 0 == g_bSpiDone );
	g_bSpiDone = 0;

	LCD_CS_HIGH();  
}



static void lcd_WriteMultipleByte( void const *pData, int nSize )
{
	ASSERT( 0 != g_pSpimHandle );

	LCD_DC_HIGH();
	LCD_CS_LOW();
	SpimTransfer( g_pSpimHandle, pData, 0, nSize );
	while( 0 == g_bSpiDone );
	g_bSpiDone = 0;
	LCD_CS_HIGH(); 
}


void LCD_WriteReg( uint8_t Reg, uint16_t RegValue )
{	
	lcd_WriteCmd( Reg );  
	lcd_WriteData16( RegValue );	    		 
}	


static void lcd_Delayms( uint32_t ms )
{
	/* This not an accurate delay. */
	volatile int cnt = (SystemCoreClock*ms)/10000;
	
	while( cnt-- )
	{
		__NOP();
	}
}




















