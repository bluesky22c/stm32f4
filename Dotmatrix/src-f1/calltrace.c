/******************** (C) COPYRIGHT 2012 OP *****************************
* File Name          : calltrace.c
* Author             : bluesky22c
* Version            : V2.2.1
* Date               : 01/01/2012
* Description        : Serial output driver
********************************************************************************
*
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"
#include "calltrace.h"
#include "common.h"
#include "mem_heap.h"
#include "usart.h"

#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
unsigned int       g_TraceOnOffList[TRACE_MAX_INDEX] = {0, };

extern vu32        bDeviceState; /* USB device status */

/*******************************************************************************
* Function Name  : CallTrace_Init
* Description    : Initialize Calltrace index
* Input          : None.
* Return         : None.
*******************************************************************************/
void CallTrace_Init(void)
{
  u16 i;
  for( i=0; i<TRACE_MAX_INDEX; i++)
  {
    g_TraceOnOffList[i] = (GET_TRACE_TYPE(i)+TRACE_LVL_FULL);
  }
}

/*******************************************************************************
* Function Name  : CallTrace
* Description    : display messages through the USART
* Input          : None.
* Return         : None.
*******************************************************************************/
int CallTrace(int mask, char* format , ...)
{
  
  int             trace_idx = GET_TRACE_TYPE_INDEX(mask);
  unsigned char   trace_lvl = GET_TRACE_TYPE_LEVEL(mask);
  
  va_list args;


  if( (trace_idx < 1) || (trace_idx >= TRACE_MAX_INDEX) )
  {
    return 0;
  }

  
  if((g_TraceOnOffList[trace_idx]&trace_lvl)==0)
  {
    return 0;
  }

/*
  if( bDeviceState != CONFIGURED)
  {
    return 0;
  }
*/

  va_start( args, format );
  return PRINT( 0, format, args );

}

