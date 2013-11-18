/******************** (C) COPYRIGHT 2008 Osung LST *****************************
* File Name          : usart.c
* Author             : Advanced Research Team
* Version            : V2.2.1
* Date               : 09/22/2008
* Description        : usart driver
********************************************************************************
*
********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : SerialPutChar
* Description    : Print a character on the HyperTerminal
* Input          : - c: The character to be printed
* Output         : None
* Return         : None
*******************************************************************************/
void SerialPutChar(USART_TypeDef* USARTx, u8 c)
{
  USART_SendData(USARTx, c);
  while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
}

/*******************************************************************************
* Function Name  : SerialPutString
* Description    : Print a string on the HyperTerminal
* Input          : - s: The string to be printed
* Output         : None
* Return         : None
*******************************************************************************/
void SerialPutString(USART_TypeDef* USARTx, u8 *s)
{
  while (*s != '\0')
  {
    SerialPutChar(USARTx, *s);
    s ++;
  }
}

/*******************************************************************************
* Function Name  : DebugPutChar
* Description    : Print a character on the debug port
* Input          : - c: The character to be printed
* Output         : None
* Return         : None
*******************************************************************************/
void DebugPutChar(u8 c)
{
#if(DEBUG_PORT == 1)
  SerialPutChar(USART1, c);
#elif (DEBUG_PORT == 2 )
  SerialPutChar(USART2, c);
#elif ( DEBUG_PORT == 3 )
  USART_To_USB_Send_Data(c);
#endif

}

/*******************************************************************************
* Function Name  : UartHostPutChar
* Description    : Print a character on the host interface port.
* Input          : - c, The string to be printed.
* Output         : None
* Return         : None
*******************************************************************************/
void UartHostPutChar(u8 c)
{
#if(HOST_PORT == 1)
  SerialPutChar(USART1, c);
#elif (HOST_PORT ==2 )
  SerialPutChar(USART2, c);
#endif
}



/*******************************************************************************
* Function Name  : UartHostPutNChars
* Description    : Print  characters on the host interface port.
* Input          : - s: The string to be printed, len : length of string.
* Output         : None
* Return         : None
*******************************************************************************/
void UartHostPutNChars(u8 *s, u32 len)
{
  u32 i;

  for(i =0; i < len; i++)
  {
  #if(HOST_PORT == 1)
    SerialPutChar(USART1, *(s+i));
  #elif (HOST_PORT ==2 )
    SerialPutChar(USART2, *(s+i));
  #endif
  }
}

