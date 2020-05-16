/*****************************************************************************
 @Project	: 
 @File 		: term.c
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
#include "Serial.h"
#include "term.h"


/*****************************************************************************
 Define
******************************************************************************/


/*****************************************************************************
 Type definition
******************************************************************************/
typedef struct _tag_CMD_INTERPRATE
{
	BOOL	bStart;
	int		nAcc;
	char	aBuf[32];
}CMD_INTERPRATE, *PCMD_INTERPRATE;


/*****************************************************************************
 Global Variables
******************************************************************************/


/*****************************************************************************
 Local Variables
******************************************************************************/
static CMD_INTERPRATE g_Cmd;


/*****************************************************************************
 Callback Functions
******************************************************************************/


/*****************************************************************************
 Local Functions
******************************************************************************/


/*****************************************************************************
 Implementation
******************************************************************************/
void TerminalInit( void )
{
    g_Cmd.bStart = FALSE;
    g_Cmd.nAcc = 0;
}


void TerminalAckToHost( UART_HANDLE *pUartHandle )
{
    ASSERT( 0 != pUartHandle );

    char buf[32];

    buf[0] = CMD_STX;
    buf[1] = RESP_DEV_ACK;
    buf[2] = 0;
    buf[17] = CMD_ETX;

    while( FALSE == SerialIsTxReady( pUartHandle ) ){}
    SerialWriteEx( pUartHandle, buf, 18U );
}


void TerminalMapToHost( UART_HANDLE *pUartHandle, int nIdx, int nX, int nY )
{
    ASSERT( 0 != pUartHandle );

    char buf[32];

    buf[0] = CMD_STX;
    buf[1] = RESP_DEV_MAP;
    buf[2] = nIdx;
    memcpy( &buf[3], &nX, 4U );
    memcpy( &buf[7], &nY, 4U );
    buf[17] = CMD_ETX;

    while( FALSE == SerialIsTxReady( pUartHandle ) ){}
    SerialWriteEx( pUartHandle, buf, 18U );
}


char *TerminalParse( UART_HANDLE *pUartHandle )
{
    char cdata;
    char *ptr = 0;

    ASSERT( 0 != pUartHandle );

    while( TRUE == SerialIsRxReady( pUartHandle ) )
    {
        cdata = SerialRead( pUartHandle );

        if( TRUE != g_Cmd.bStart )
        {
            if( CMD_STX == cdata )
            {
                g_Cmd.nAcc = 0;
                g_Cmd.bStart = TRUE;
            }

            continue;
        }
               
        if( CMD_HOST_PKT_SIZE != g_Cmd.nAcc )
        {
            g_Cmd.aBuf[g_Cmd.nAcc] = cdata;
            g_Cmd.nAcc++;

            if( g_Cmd.nAcc>CMD_HOST_PKT_SIZE )
            {
                g_Cmd.nAcc = 0;
            }

            continue;
        }
         
        g_Cmd.bStart = FALSE;
        g_Cmd.nAcc = 0;

        if( CMD_ETX == cdata )
        {
            ptr = g_Cmd.aBuf;
            return ptr;
        }
    }

    return ptr;
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






















