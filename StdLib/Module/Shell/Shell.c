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
#include "Hal.h"
#include "CI.h"
#include "Shell.h"


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
extern CI_CMD CMD_IPS[];


/*****************************************************************************
 Local Variables
******************************************************************************/
static void ci_help( CI_ARG const *Args );
static void ci_about( CI_ARG const *Args );
static void ci_reboot( CI_ARG const *Args );
static void ci_led( CI_ARG const *Args );


CI_CMD CMD_HELP[] =
{
	{ "help",	&ci_help,	"Help Menu" },
    { "about", &ci_about, "General information" },
	{ "reboot", &ci_reboot, "Reboot whole system" },
	{ "led",	&ci_led,	"led <color> <0/1>, color= red or blue" },
	{ 0, 0, 0 }
};


PCI_CMD CMD_LIST[] =
{
	CMD_HELP,
	CMD_IPS,
	0
};


/*****************************************************************************
 Local Functions
******************************************************************************/


/*****************************************************************************
 Implementation
******************************************************************************/
void ShellInit( void )
{
	CI_TRACE( "Type \"help ?\" for more options\r\n" );
	
	CiInit( CMD_LIST );
}


/*****************************************************************************
 Callback functions
******************************************************************************/


/*****************************************************************************
 Local functions
******************************************************************************/
static void ci_help( CI_ARG const *Args )
{
	int i = 0;
	CI_CMD *cur;


	CI_TRACE( "***************************************\r\n" );
	CI_TRACE( "*        IPS Beacon Command Menu		 *\r\n" );
	CI_TRACE( "***************************************\r\n" );
	
	if( (0 == strcmp( "?", Args->Args[0] )) || (0 == Args->Args[0]) )
	{ 
		while( 0 != (&CMD_LIST)[i] )
		{
			cur = (CI_CMD *)(((int *)CMD_LIST)[i]);
			
			if( 0 == cur )
			{
				break;
			}
			
			i++;
			
			while( 0 != cur->sCmd )
			{
				CI_TRACE( "%s	:	%s\r\n", cur->sCmd, cur->sRemark );
				cur++;
			}
		}
	}
}


static void ci_about( CI_ARG const *Args )
{
	CI_TRACE( "IPS->Indoor Positioning System is required in SEP250 to follow map to multiple points\r\n" );
    CI_TRACE( "Type \"help ?\" to get the command menu\r\n" );
}


static void ci_reboot( CI_ARG const *Args )
{
	CI_TRACE( "Rebooting..." );
	NVIC_SystemReset();
}


static void ci_led( CI_ARG const *Args )
{
	if( (0 != Args->Args[0]) && (0 != Args->Args[1]) )
	{
		if( 0 == strcmp( "blue", Args->Args[0] ) )
		{
			if( 0 == strcmp( "0", Args->Args[1] ) )
			{
				LED_SET( FALSE );
			}
			else
			{
				LED_SET( TRUE );
			}
		}
	}
	else
	{
		CI_TRACE( "Error! Parameters invalid\r\n" );
	}
}


/*****************************************************************************
 Interrupt functions
******************************************************************************/






















