/******************** (C) COPYRIGHT 2008 Osung LST *****************************
* File Name          : map.c
* Author             : Advanced Research Team
* Version            : V2.2.1
* Date               : 09/22/2008
* Description        : MAP driver
********************************************************************************
*
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"
#include "map.h"
#include "mem_heap.h"
#include "mutex.h"

/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern mem_heap_type           g_SystemHeap;

/* Private define ------------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Map_Init
* Description    : Initialize MAP variables  
* Input          : None.
* Return         : None.
*******************************************************************************/
void Map_Init(map_msg_type_t* pMap, int sort)
{
  //reset object
  pMap->pHead       = NULL;
  pMap->pTail       = NULL;
  pMap->iCount      = 0;
  pMap->iSort       = sort; //if sort ==1 all of element will be sorted
}


/*******************************************************************************
* Function Name  : Map_RemoveElement
* Description    : Remove the element in the map pointed to by the pKey 
                        - TRUE if successful and FALSE if pKey is not found
* Input          : None.
* Return         : None.
*******************************************************************************/
int Map_RemoveElement(map_msg_type_t* pMap,int pKey)
{

  map_ele_msg_t* pElement;
  int pData = 0, i; 
  
  Mutex_Lock();
  
  pElement = pMap->pHead;

  for (i=0;i<pMap->iCount; i++)
  {
    if (pElement == NULL) //check incase something has gone wrong
    {
      break;
    }
    
    if (pElement->KeyElement == pKey)
    {
      if (pElement->pPreviousListElement && pElement->pNextListElement) //An element in the middle of the queue
      {
        pElement->pPreviousListElement->pNextListElement = pElement->pNextListElement;
        pElement->pNextListElement->pPreviousListElement = pElement->pPreviousListElement;
      }
      else if (pElement->pPreviousListElement)//The tail of the queue
      {
        pElement->pPreviousListElement->pNextListElement = NULL;
        pMap->pTail = pElement->pPreviousListElement;
      }			
      else if (pElement->pNextListElement)//The head of the queue
      {
        pElement->pNextListElement->pPreviousListElement = NULL;
        pMap->pHead = pElement->pNextListElement;
      }
      else //I'm the only thing in the queue
      {
        pMap->pHead = pMap->pTail = NULL;
      }

      pData = pElement->DataElement;
      
      Free(&g_SystemHeap, pElement);
      
      pMap->iCount--;
      break;
    }
    pElement = pElement->pNextListElement;
  }
  
  Mutex_UnLock();
  
  return pData;
}

/*******************************************************************************
* Function Name  : Map_RemoveData
* Description    : Remove the element in the map by the pData 
*                      - TRUE if successful and FALSE if pKey is not found
* Input          : None.
* Return         : None.
*******************************************************************************/
int Map_RemoveData(map_msg_type_t* pMap,int pData)
{
 
  int pKey = 0, i;
  map_ele_msg_t* pElement = pMap->pHead;

  Mutex_Lock();
 
  for (i=0;i<pMap->iCount;i++)
  {
    if (pElement == NULL) //check incase something has gone wrong
    {
      break;
    }
  
    if (pElement->DataElement == pData)
    {
      if (pElement->pPreviousListElement && pElement->pNextListElement) //An element in the middle of the queue
      {
        pElement->pPreviousListElement->pNextListElement = pElement->pNextListElement;
        pElement->pNextListElement->pPreviousListElement = pElement->pPreviousListElement;
      }
      else if (pElement->pPreviousListElement)//The tail of the queue
      {
        pElement->pPreviousListElement->pNextListElement = NULL;
        pMap->pTail = pElement->pPreviousListElement;
      }			
      else if (pElement->pNextListElement)//The head of the queue
      {
        pElement->pNextListElement->pPreviousListElement = NULL;
        pMap->pHead = pElement->pNextListElement;
      }
      else //I'm the only thing in the queue
      {
        pMap->pHead = pMap->pTail = NULL;
      }

      pKey = pElement->KeyElement;

      Free(&g_SystemHeap,pElement);

      pMap->iCount--;
      break;
    }
    pElement = pElement->pNextListElement;
  }
  
  Mutex_UnLock();
  
  return pKey;
}

/*******************************************************************************
* Function Name  : Map_AddElement
* Description    : Add a element to the end of the map but only if it doesn't already exist  
* Input          : None.
* Return         : None.
*******************************************************************************/
void Map_AddElement(map_msg_type_t* pMap,int pData,int pKey)
{

  map_ele_msg_t  *pElement     = NULL;
  map_ele_msg_t  *pTempElement = NULL;
  map_ele_msg_t  *pSrchElement = NULL;
  int i;
  
  Map_RemoveElement(pMap,pKey);

  //create list entry based on data
  pElement = (map_ele_msg_t*)Malloc(&g_SystemHeap, sizeof(map_ele_msg_t));

  pElement->DataElement = pData;
  pElement->KeyElement = pKey;

  Mutex_Lock();

  //put element in list
  if(pMap->iCount == 0)
  {
    //special case - empty list
    pMap->pHead = pMap->pTail = pElement;
  }
  else
  {
   
    if(pMap->iSort)
    {
      pTempElement= pMap->pTail;
      pMap->pTail = pElement;
      pTempElement->pNextListElement = pElement;
      pElement->pPreviousListElement = pTempElement;
    }
    else
    {
      pSrchElement = pMap->pHead;
    
      for (i=0; i<pMap->iCount; i++)
      {
        if (pSrchElement == 0) //check incase something has gone wrong
        {
          break;
        }
  
        if (pSrchElement->DataElement > pElement->DataElement ) //if existed data is bigger than new one,it would be changed
        {
          pTempElement = pSrchElement;
          
          pElement->pPreviousListElement = pTempElement->pPreviousListElement;
          pElement->pNextListElement = pTempElement;
          
          if( pTempElement->pPreviousListElement != NULL)
          {
          pTempElement->pPreviousListElement->pNextListElement = pElement;
          }
          pTempElement->pPreviousListElement = pElement;
          
          if( pSrchElement == pMap->pHead)
          {
            pMap->pHead = pElement;
          }
          break;
        }
        else
        {
          pSrchElement = pSrchElement->pNextListElement;
        }
      }
  
      /* if add element is longest data, m_pTail would be changed */
      if( i ==  pMap->iCount )
      {
        pTempElement= pMap->pTail;
        pMap->pTail = pElement;
        pTempElement->pNextListElement = pElement;
        pElement->pPreviousListElement = pTempElement;
      }
    }
  }
  pMap->iCount++;
  
  Mutex_UnLock();

}

/*******************************************************************************
* Function Name  : Map_LookAtFirstElement
* Description    : 
* Input          : None.
* Return         : None.
*******************************************************************************/
int Map_LookAtFirstElement(map_msg_type_t* pMap, int* Reference)
{
  int pData = 0; 
  *Reference = 0; 
  
  Mutex_Lock();
  
  //Do I have anything in the queue
  if (pMap->pHead)
  {
    *Reference = (int)pMap->pHead->pNextListElement;
    pData = pMap->pHead->DataElement;
  }
  
  Mutex_UnLock();
  
  return pData;
}

/*******************************************************************************
* Function Name  : Map_LookAtNextElement
* Description    : Return the Data element stored in the Reference and set reference to the next element in the map - if this is 
*                      the final element then Reference will be NULL
* Input          : None.
* Return         : None.
*******************************************************************************/
int Map_LookAtNextElement(map_msg_type_t* pMap, int* Reference)
{
  int pData = 0;
  map_ele_msg_t* pEntry;
  
  Mutex_Lock();
  
  if (*Reference)
  {		 
    pEntry = (map_ele_msg_t*)*Reference;
    
    *Reference = (int)pEntry->pNextListElement;
    pData = pEntry->DataElement;
  }
  Mutex_UnLock();
  
  return pData;
}


/*******************************************************************************
* Function Name  : Map_Empty
* Description    : Is the map empty
* Input          : None.
* Return         : None.
*******************************************************************************/
int Map_Empty(map_msg_type_t* pMap)
{
  int Result = FALSE;
  
  Mutex_Lock();
 
  if (pMap->iCount == 0)
  {
    Result = TRUE;
  }
  Mutex_UnLock();
  
  return Result;
}


/*******************************************************************************
* Function Name  : Map_GetLength
* Description    : find the number of elements in the map
* Input          : None.
* Return         : None.
*******************************************************************************/
int Map_GetLength(map_msg_type_t* pMap)
{
  int Count;
  
  Mutex_Lock();
  Count = pMap->iCount;
  Mutex_UnLock();
  
  return Count;
}


