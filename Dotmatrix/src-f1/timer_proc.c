/******************** (C) COPYRIGHT 2008 Osung LST *****************************
* File Name          : pwr_detect.c
* Author             : Advanced Research Team
* Version            : V2.2.1
* Date               : 09/22/2008
* Description        : detect present value of VDD,VBL, IDD,IBL 
********************************************************************************
*
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"
#include "timer.h"
#include "timer_proc.h"
#include "mem_heap.h"
#include "queue.h"

#include "calltrace.h"



/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
timer_msg_type_t        g_SystemTimer;
queue_type_t            g_timerproc_q;



extern mem_heap_type           g_SystemHeap;

/* Private define ------------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Timer_Configuration
* Description    : Configure system timer
* Input          : None.
* Return         : None.
*******************************************************************************/
void Timer_Configuration(void)
{
  /* Initialize System Timer */
  TimerApp_Init(&g_SystemTimer, 0 ); 

  /* initialize timer proc Q */
  TimerProc_MsgQInit();

}

/*******************************************************************************
* Function Name  : TimerSet
* Description    : Set the timer
* Input          : None.
* Return         : None.
*******************************************************************************/
void TimerSet(int Timer_ID, int interval, timer_type_t type, T_CallBackFunc CallBackFunc)
{
  TimerApp_AddElement(&g_SystemTimer, (int)CallBackFunc, interval, Timer_ID, type);
  //CallTrace(TRACE_APP_WARN, "TimerSet: ID=%d Int=%d Fun=0x%x\r\n", Timer_ID, interval,CallBackFunc );
}

/*******************************************************************************
* Function Name  : TimerKill
* Description    : Remove Timer 
* Input          : None.
* Return         : None.
*******************************************************************************/
void TimerKill(int Timer_ID)
{
  TimerApp_RemoveElement(&g_SystemTimer, Timer_ID);
}


/*******************************************************************************
* Function Name  : TimerProc_MsgQInit
* Description    : Initialize Timer  procedure variables and something.
* Input          : None.
* Return         : None.
*******************************************************************************/
void TimerProc_MsgQInit(void)
{
  /* Queue configuration */
  Q_Init(&g_timerproc_q,"TimerProc",TIMER_MAX_Q_SIZE);

}

/*******************************************************************************
* Function Name  : TimerProc_QPut
* Description    : put the data into timer proc's msg Q.
* Input          : None.
* Return         : None.
*******************************************************************************/
void TimerProc_QPut(u8* str)
{
  Q_PutElement(&g_timerproc_q,(int)str);
}

/*******************************************************************************
* Function Name  : PattConProc_ParsePkt
* Description    : parsing packet from other proc.
* Input          : None.
* Return         : None.
*******************************************************************************/
void TimerProc_ParsePkt(u8* str)
{
  T_CallBackFunc    p_Fun = NULL;
  timer_proc_t      *p_req;


  p_req = (timer_proc_t*)str;

  switch(p_req->cmd)
  {
  case TIMER_CMD_RUN:
    if(p_req->pCallback)
    {
      p_req->pCallback(NULL);
    }
    if( p_req->remove_flag)
    {
      //CallTrace(TRACE_APP_ERROR, "Remove [%d] Timer \n\r", p_req->timer_id);
      TimerApp_RemoveElement(&g_SystemTimer, p_req->timer_id);
    }
    break;

  default:
    break;
  }
}


/*******************************************************************************
* Function Name  : TimerProc
* Description    : Timer process handler
* Input          : None.
* Return         : None.
*******************************************************************************/
void TimerProc(void)
{
  
  u8 *p_str;

  while(1)
  {
    if (Q_IsEmpty(&g_timerproc_q))
    {
      break;
    }
    else
    {
      p_str = (u8*)Q_GetElement(&g_timerproc_q);

      /* parsing command */
      TimerProc_ParsePkt(p_str);

      Free(&g_SystemHeap,p_str);
    }
  }
}


