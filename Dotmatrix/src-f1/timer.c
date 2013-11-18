/******************** (C) COPYRIGHT 2008 Osung LST *****************************
* File Name          : timer.c
* Author             : Advanced Research Team
* Version            : V2.2.1
* Date               : 09/22/2008
* Description        : timer HAL driver
********************************************************************************
*
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"
#include "timer.h"
#include "mem_heap.h"
#include "mutex.h"
#include "calltrace.h"
#include "timer_proc.h"



/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern mem_heap_type           g_SystemHeap;

/* Private define ------------------------------------------------------------*/


/*******************************************************************************
* Function Name  : TimerApp_Init
* Description    : Initialize Timer variables  
* Input          : None.
* Return         : None.
*******************************************************************************/
void TimerApp_Init(timer_msg_type_t* pTimer, int sort)
{
  //reset object
  pTimer->pHead       = NULL;
  pTimer->pTail       = NULL;
  pTimer->iCount      = 0;
  pTimer->iSort       = sort; //if sort ==1 all of element will be sorted
}


/*******************************************************************************
* Function Name  : TimerApp_RemoveElement
* Description    : Remove the element in the map pointed to by the TimerID 
                        - TRUE if successful and FALSE if TimerID is not found
* Input          : None.
* Return         : None.
*******************************************************************************/
int TimerApp_RemoveElement(timer_msg_type_t* pTimer,int TimerID)
{

  timer_ele_msg_t* pElement;
  int pData = 0, i; 
  
  if(TimerApp_Empty(pTimer))
  {
    return 0;
  }

  Mutex_Lock();

  pElement = pTimer->pHead;

  for (i = 0; i<pTimer->iCount ; i++)
  {
    if (pElement == NULL) //check incase something has gone wrong
    {
      break;
    }
    
    if (pElement->Timer_ID == TimerID)
    {
      if (pElement->pPreviousListElement && pElement->pNextListElement) //An element in the middle of the queue
      {
        pElement->pPreviousListElement->pNextListElement = pElement->pNextListElement;
        pElement->pNextListElement->pPreviousListElement = pElement->pPreviousListElement;
      }
      else if (pElement->pPreviousListElement)//The tail of the queue
      {
        pElement->pPreviousListElement->pNextListElement = NULL;
        pTimer->pTail = pElement->pPreviousListElement;
      }			
      else if (pElement->pNextListElement)//The head of the queue
      {
        pElement->pNextListElement->pPreviousListElement = NULL;
        pTimer->pHead = pElement->pNextListElement;
      }
      else //I'm the only thing in the queue
      {
        pTimer->pHead = pTimer->pTail = NULL;
      }

      pData = pElement->DataElement;
      
      Free(&g_SystemHeap, pElement);
      
      pTimer->iCount--;
      break;
    }

    pElement = pElement->pNextListElement;
  }
  
  Mutex_UnLock();
  
  return pData;

}

/*******************************************************************************
* Function Name  : TimerApp_AddElement
* Description    : Add a element to the end of the map but only if it doesn't already exist  
* Input          : None.
* Return         : None.
*******************************************************************************/
void TimerApp_AddElement(timer_msg_type_t* pTimer, int pData, int iValue, int TimerID,  timer_type_t type)
{

  timer_ele_msg_t  *pElement      = NULL;
  timer_ele_msg_t  *pTempElement  = NULL;
  timer_ele_msg_t  *pSrchElement  = NULL;
  int i;


  TimerApp_RemoveElement(pTimer,TimerID);

  //create list entry based on data
  pElement = (timer_ele_msg_t*)Malloc(&g_SystemHeap, sizeof(timer_ele_msg_t));
  if( pElement == NULL)
  {
    CallTrace(TRACE_APP_WARN, "Fail to alloc memory \r\n");
    return;
  }

  pElement->DataElement          = pData;
  pElement->Timer_ID             = TimerID;
  pElement->nCont                = iValue;
  pElement->nBackCont            = iValue;
  pElement->Timer_type           = type;
  pElement->pNextListElement     = NULL;
  pElement->pPreviousListElement = NULL;

  Mutex_Lock();

  //put element in list
  if(pTimer->iCount == 0)
  {
    //special case - empty list
    pTimer->pHead = pTimer->pTail = pElement;
  }
  else
  {
    if(pTimer->iSort)
    {
      pTempElement= pTimer->pTail;
      pTimer->pTail = pElement;
      pTempElement->pNextListElement = pElement;
      pElement->pPreviousListElement = pTempElement;
    }
    else
    {
      pSrchElement = pTimer->pHead;
    
      for (i=0; i<pTimer->iCount; i++)
      {
        if (pSrchElement == 0) //check incase something has gone wrong
        {
          Free(&g_SystemHeap,pElement);
          CallTrace(TRACE_APP_WARN, "Fail to insert Timer \r\n");
          break;
        }
  
        if (pSrchElement->nCont > pElement->nCont ) //if existed data is bigger than new one,it would be changed
        {
          pTempElement = pSrchElement;
          
          pElement->pPreviousListElement = pTempElement->pPreviousListElement;
          pElement->pNextListElement = pTempElement;
          
          if( pTempElement->pPreviousListElement != NULL)
          {
            pTempElement->pPreviousListElement->pNextListElement = pElement;
          }
          pTempElement->pPreviousListElement = pElement;
          
          if( pSrchElement == pTimer->pHead)
          {
            pTimer->pHead = pElement;
          }
          break;
        }
        else
        {
          pSrchElement = pSrchElement->pNextListElement;
        }
      }
  
      /* if add element is longest data, m_pTail would be changed */
      if( i ==  pTimer->iCount )
      {
        pTempElement= pTimer->pTail;
        pTimer->pTail = pElement;
        pTempElement->pNextListElement = pElement;
        pElement->pPreviousListElement = pTempElement;
      }
    }
  }
  pTimer->iCount++;
  Mutex_UnLock();

}

/*******************************************************************************
* Function Name  : TimerApp_Empty
* Description    : Is the map empty
* Input          : None.
* Return         : None.
*******************************************************************************/
int TimerApp_Empty(timer_msg_type_t* pTimer)
{
  int Result = FALSE;
  
  Mutex_Lock();
 
  if (pTimer->iCount == 0)
  {
    Result = TRUE;
  }
  Mutex_UnLock();
  
  return Result;
}


/*******************************************************************************
* Function Name  : TimerApp_GetLength
* Description    : find the number of elements in the map
* Input          : None.
* Return         : None.
*******************************************************************************/
int TimerApp_GetLength(timer_msg_type_t* pTimer)
{
  int Count;
  
  Mutex_Lock();
  Count = pTimer->iCount;
  Mutex_UnLock();
  
  return Count;
}

/*******************************************************************************
* Function Name  : TimerTickISR
* Description    : Timer tick process handler
* Input          : None.
* Return         : None.
*******************************************************************************/
void TimerTickISR(timer_msg_type_t* pTimer)
{
  timer_ele_msg_t* pElement;
  int              pData = 0;
  int              i; 
  timer_proc_t     *p_req;


  if(TimerApp_Empty(pTimer))
  {
    return 0;
  }

  Mutex_Lock();

  pElement = pTimer->pHead;

  for (i = 0; i<pTimer->iCount ; i++)
  {
    if (pElement == NULL) //check incase something has gone wrong
    {
      break;
    }
    pElement->nCont--;
    
    if (pElement->nCont == 0)
    {
      /* building message for Pattern control procedure */
      p_req = (timer_proc_t*)Malloc(&g_SystemHeap, sizeof( timer_proc_t));
      if( !p_req )
      {
        CallTrace(TRACE_APP_ERROR, "TimerTickISR: Memory allocation failed !\n\r");
        return;
      }
      p_req->cmd         = TIMER_CMD_RUN;
      p_req->pCallback   = (T_CallBackFunc)pElement->DataElement;
      p_req->timer_id    = pElement->Timer_ID;
      
      if( pElement->Timer_type ==  TIMER_TYPE_PERIODIC)
      {
        pElement->nCont     = pElement->nBackCont;
        p_req->remove_flag  = FALSE;
      }
      else
      {
        p_req->remove_flag = TRUE;
      }
  
      /* q put */
      TimerProc_QPut((u8*)p_req);
    }
    pElement = pElement->pNextListElement;
  }
  Mutex_UnLock();
}

