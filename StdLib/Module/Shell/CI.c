/*****************************************************************************
 @Project	: 
 @File 		: CI.c
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
#include "CI.h"


/*****************************************************************************
 Define
******************************************************************************/


/*****************************************************************************
 Type definition
******************************************************************************/


/*****************************************************************************
 Global Variables
******************************************************************************/
extern char GetKey( void );
extern void FlushInBuf( void );


/*****************************************************************************
 Local Variables
******************************************************************************/
static char		sBuf[64];
static int		g_nCnt	= 0;
static PCI_CMD	g_pList = 0;


/*****************************************************************************
 Implementation
******************************************************************************/
void CiInit( void *pList )
{
	ASSERT( 0 != pList );
	
	g_pList = (PCI_CMD)pList;
	CI_TRACE("SEEMS >");
}


void CiParse( void )
{
	char c = 0;
	CI_ARG args;
	int i = 0;
	const char *token;
	char delim[2] = " ";
	CI_CMD *cur = 0;
		
    while( 0 != GetKey() )
    {
        c = getchar();
	
    #ifdef _UART_TRACE
		if( c == '\n' )
		{
			continue;
		}

        if( c == '\r' )
    #else
        if( c == '\n' )
    #endif
        {
            args.Args[0]	= 0;
            args.Args[1]	= 0;
            args.Args[2]	= 0;
            args.Args[3]	= 0;
		
            sBuf[g_nCnt]	= 0;
            g_nCnt 			= 0;
            i 				= 0;
	
		
            token = strtok( sBuf, delim );
		
            if( 0 != token )
            {
                while( 0 != (&g_pList)[i] )
                {
                    cur = (CI_CMD *)(((int *)g_pList)[i]);
				
                    if( 0 == cur )
                    {
                        CI_TRACE("\r\nInvalid Command\r\n");
                        CI_TRACE("SEEMS >");
                        memset( sBuf, 0, sizeof(sBuf) );
                        break;
                    }
				
                    i++;
				
                    while( 0 != cur->sCmd )
                    {
                        if( 0 == strcmp( token, cur->sCmd ) )
                        {
                            i = 0;
                            for(;;)
                            {
                                token = strtok( 0, delim );
							
                                if( 0 == token )
                                {
                                    break;
                                }
                                else
                                {
                                    args.Args[i++] = token;
                                }
                            }
                            CI_TRACE("\r\n");
                            cur->pFunc( &args );
                            CI_TRACE("SEEMS >");
                            memset( sBuf, 0, sizeof(sBuf) );
							FlushInBuf();
                            return;
                        }
  
					
                        cur++;
                    }                     
                }

                /* Not available command reach here */
                CI_TRACE("\r\nInvalid Command\r\n");
                CI_TRACE("SEEMS >");
                memset( sBuf, 0, sizeof(sBuf) );
            }
            else
            {
                CI_TRACE("\r\nSEEMS >");
            }
        }
        else
        {
            if( 0 != c )
            {
                sBuf[g_nCnt++] = c;

    #ifdef _UART_TRACE
 //               CI_TRACE( "%c", c );
    #endif
            }
        }
    }
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






















