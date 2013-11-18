/******************** (C) COPYRIGHT 2008 Osung LST *****************************
* File Name          : key_proc.c
* Author             : Advanced Research Team
* Version            : V2.2.1
* Date               : 09/22/2008
* Description        : key processing procedure 
********************************************************************************
*
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"
#include "key_proc.h"

#include "mem_heap.h"
#include "queue.h"
#include "calltrace.h"
#include "timer_proc.h"
#include "gpio.h"
#include "key_proc_handler.h"



/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
queue_type_t            g_key_q;

key_cmd_handler_t      g_KeyCmdHandler[KEY_CMD_MAX-1] = { \
  {KEY_CMD_INIT,                   KeyProc_Init_Handler},
  {KEY_CMD_IR_INPUT,               KeyProc_Key_Handler}
};



/* Private define ------------------------------------------------------------*/
/* system heap memory */
extern mem_heap_type           g_SystemHeap;


/*******************************************************************************
* Function Name  : KeyProc_MsgQInit
* Description    : Initialize key procedure variables and something.
* Input          : None.
* Return         : None.
*******************************************************************************/
void KeyProc_MsgQInit(void)
{
  /* Queue configuration */
  Q_Init(&g_key_q,"KeyProc",KEY_MAX_Q_SIZE);
}



/*******************************************************************************
* Function Name  : KeyProc_QPut
* Description    : put the data into key proc's msg Q.
* Input          : None.
* Return         : None.
*******************************************************************************/
void KeyProc_QPut(u8* str)
{
  Q_PutElement(&g_key_q,(int)str);
}


/*******************************************************************************
* Function Name  : KeyProc_ParsePkt
* Description    : parsing packet from other proc.
* Input          : None.
* Return         : None.
*******************************************************************************/
void KeyProc_ParsePkt(u8* str)
{
  u32            i;
  u32            pkt_len = 0;
  KeyCmdHandler  p_Fun   = NULL;

  key_proc_req_t   *p_req;


  p_req = (key_proc_req_t*)str;

  for(i = 0; i < (KEY_CMD_MAX-1); i++)
  {
    if( g_KeyCmdHandler[i].cmd == p_req->cmd )
    {
      p_Fun = g_KeyCmdHandler[i].fun;
      break;
    }
  }

  if(p_Fun)
  {
    p_Fun(str,pkt_len );
  }
  else
  {
	CallTrace(TRACE_APP_LOW,"KeyProc: Unknown CMD Error!\n\r");
  }

}

/*******************************************************************************
* Function Name  : KeyProc
* Description    : Key processing handler
* Input          : None.
* Return         : None.
*******************************************************************************/
void KeyProc(void)
{

  u8 *p_str;

  while(1)
  {
    if (Q_IsEmpty(&g_key_q))
    {
      break;
    }
    else
    {
      p_str = (u8*)Q_GetElement(&g_key_q);

      /* parsing command */
	  KeyProc_ParsePkt(p_str);

      Free(&g_SystemHeap,p_str);
    }
  }
}



