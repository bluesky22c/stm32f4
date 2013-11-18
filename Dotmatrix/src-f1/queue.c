/******************** (C) COPYRIGHT 2008 Osung LST *****************************
* File Name          : queue.c
* Author             : Advanced Research Team
* Version            : V2.2.1
* Date               : 09/22/2008
* Description        : Queue driver
********************************************************************************
*
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"
#include "queue.h"
#include "calltrace.h"
#include "mem_heap.h"
#include "mutex.h"



/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern mem_heap_type            g_SystemHeap;
/* Private define ------------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Q_Init
* Description    : Initialize Q type 
* Input          : None.
* Return         : None.
*******************************************************************************/
void Q_Init(queue_type_t* queue, char* name, int size)
{
  strncpy(queue->name, name, QUEUE_NAME_SIZE);

  queue->pHeadQ         = 0;
  queue->ulCount        = 0;
  
  queue->MemoryPoolSize = size;
  
  queue->Queue = Malloc(&g_SystemHeap, sizeof(int) * size );
  if( queue->Queue == NULL)
  {
    CallTrace(TRACE_DRV_ERROR,"Fail to malloc Queue \n\r");
  }
}

int Q_IsEmpty(queue_type_t* queue)
{
  return queue->ulCount ? FALSE:TRUE;
}


int Q_GetLength(queue_type_t* queue)
{
  return queue->ulCount;
}

int Q_GetElement(queue_type_t* queue)
{
  int result;

  Mutex_Lock();
  
  if (queue->ulCount > 0) 
  {
    result = queue->Queue[queue->pHeadQ];
  
    queue->pHeadQ = ( queue->pHeadQ + 1 ) % queue->MemoryPoolSize;
    
    queue->ulCount--;
    
    Mutex_UnLock();
    return result;
  }
  else
  {
    CallTrace(TRACE_DRV_ERROR,"%s : Message queue is empty\r\n", queue->name);
    Mutex_UnLock();
    return 0;
  }

}

bool Q_PutElement(queue_type_t* queue,int Content)
{
  
  Mutex_Lock();
  
  if (queue->ulCount >= queue->MemoryPoolSize-1)
  {
    CallTrace(TRACE_DRV_ERROR,"%s : queues overflowed (%d/%d)\r\n", queue->name, queue->ulCount, queue->MemoryPoolSize);
    Mutex_UnLock();
    return FALSE;
  }
  
  queue->Queue[( queue->pHeadQ + queue->ulCount ) % queue->MemoryPoolSize] = Content;
  queue->ulCount++;
  
  Mutex_UnLock();

  return TRUE;
}

int Q_LookAtHeadElement(queue_type_t* queue, int Reference)
{
  
  int l_Result;
  
  Mutex_Lock();
  
  l_Result = queue->Queue[queue->pHeadQ];
  
  Mutex_UnLock();
  
  return l_Result;
}

void Q_RemoveElement(queue_type_t* queue, int Content)
{
  int i, j;
  
  Mutex_Lock();
  
  for (i=0; i<queue->ulCount; i++) 
  {
    if (queue->Queue[( queue->pHeadQ + i ) % queue->MemoryPoolSize] == Content)
    {
      queue->ulCount--;
      for (j=i; j<queue->ulCount; j++) 
      {
        queue->Queue[( queue->pHeadQ + j ) % queue->MemoryPoolSize]= queue->Queue[( queue->pHeadQ + j + 1) % queue->MemoryPoolSize];
      }
      break;
    }
  }
  Mutex_UnLock();
}


