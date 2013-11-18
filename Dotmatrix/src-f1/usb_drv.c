/******************** (C) COPYRIGHT 2008 Osung LST *****************************
* File Name          : usb_drv.c
* Author             : MCD Application Team
* Version            : V2.2.1
* Date               : 09/22/2008
* Description        : usb device driver
********************************************************************************
* 
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"
#include "stm32f10x_it.h"
#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "platform_config.h"
#include "usb_pwr.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

extern ErrorStatus        HSEStartUpStatus;

/*******************************************************************************
* Function Name  : UsbClk_Configruation
* Description    : Configures USB Clock input (48MHz)
* Input          : None.
* Return         : None.
*******************************************************************************/
void UsbClk_Configruation(void)
{
  /* USBCLK = PLLCLK / 1.5 */
  RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
  /* Enable USB clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);

}

/*******************************************************************************
* Function Name  : Enter_LowPowerMode
* Description    : Power-off system clocks and power while entering suspend mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void Enter_LowPowerMode(void)
{
  /* Set the device state to suspend */
  bDeviceState = SUSPENDED;

  /* Clear EXTI Line18 pending bit */
  //EXTI_ClearITPendingBit(GPIO_KEY_EXTI_Line);

  /* Request to enter STOP mode with regulator in low power mode */
  //PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
}

/*******************************************************************************
* Function Name  : Leave_LowPowerMode
* Description    : Restores system clocks and power while exiting suspend mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{
  DEVICE_INFO *pInfo = &Device_Info;

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  /* Enable PLL */
  RCC_PLLCmd(ENABLE);

  /* Wait till PLL is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
  {}

  /* Select PLL as system clock source */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

  /* Wait till PLL is used as system clock source */
  while (RCC_GetSYSCLKSource() != 0x08)
  {}

  /* Set the device state to the correct state */
  if (pInfo->Current_Configuration != 0)
  {
    /* Device configured */
    bDeviceState = CONFIGURED;
  }
  else
  {
    bDeviceState = ATTACHED;
  }
}

/*******************************************************************************
* Function Name  : USB_Cable_Config
* Description    : Software Connection/Disconnection of USB Cable
* Input          : None.
* Return         : Status
*******************************************************************************/
void USB_Cable_Config (FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    GPIO_ResetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
  }
  else
  {
    GPIO_SetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
  }
}

/*******************************************************************************
* Function Name  : Get_SerialNum.
* Description    : Create the serial number string descriptor.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Get_SerialNum(void)
{
  vu32 Device_Serial0, Device_Serial1, Device_Serial2;

  Device_Serial0 = *(vu32*)(0x1FFFF7E8);
  Device_Serial1 = *(vu32*)(0x1FFFF7EC);
  Device_Serial2 = *(vu32*)(0x1FFFF7F0);

  if (Device_Serial0 != 0)
  {
    Joystick_StringSerial[2] = (vu8)(Device_Serial0 & 0x000000FF);
    Joystick_StringSerial[4] = (vu8)((Device_Serial0 & 0x0000FF00) >> 8);
    Joystick_StringSerial[6] = (vu8)((Device_Serial0 & 0x00FF0000) >> 16);
    Joystick_StringSerial[8] = (vu8)((Device_Serial0 & 0xFF000000) >> 24);

    Joystick_StringSerial[10] = (vu8)(Device_Serial1 & 0x000000FF);
    Joystick_StringSerial[12] = (vu8)((Device_Serial1 & 0x0000FF00) >> 8);
    Joystick_StringSerial[14] = (vu8)((Device_Serial1 & 0x00FF0000) >> 16);
    Joystick_StringSerial[16] = (vu8)((Device_Serial1 & 0xFF000000) >> 24);

    Joystick_StringSerial[18] = (vu8)(Device_Serial2 & 0x000000FF);
    Joystick_StringSerial[20] = (vu8)((Device_Serial2 & 0x0000FF00) >> 8);
    Joystick_StringSerial[22] = (vu8)((Device_Serial2 & 0x00FF0000) >> 16);
    Joystick_StringSerial[24] = (vu8)((Device_Serial2 & 0xFF000000) >> 24);
  }
}

/*******************************************************************************
* Function Name : Joystick_Send.
* Description   : prepares buffer to be sent containing Joystick event infos.
* Input         : Keys: keys received from terminal.
* Output        : None.
* Return value  : None.
*******************************************************************************/
void Joystick_Send(u8 Keys)
{
  u8 Mouse_Buffer[8] = {0,0,0,0,0,0,0,0};

  
  /* prepare buffer to send */
  Mouse_Buffer[2] = Keys;

  /*copy mouse position info in ENDP1 Tx Packet Memory Area*/
  UserToPMABufferCopy(Mouse_Buffer, GetEPTxAddr(ENDP1), 8);
  /* enable endpoint for transmission */
  SetEPTxValid(ENDP1);
}

/*******************************************************************************
* Function Name : JoyState.
* Description   : Decodes the Joystick direction.
* Input         : None.
* Output        : None.
* Return value  : The direction value.
*******************************************************************************/
u8 JoyState(void)
{
#if 0
  /* "right" key is pressed */
  if (!GPIO_ReadInputDataBit(GPIO_RIGHT, GPIO_Pin_RIGHT))
  {
    return RIGHT;
  }
  /* "left" key is pressed */
  if (!GPIO_ReadInputDataBit(GPIO_LEFT, GPIO_Pin_LEFT))
  {
    return LEFT;
  }
  /* "up" key is pressed */
  if (!GPIO_ReadInputDataBit(GPIO_UP, GPIO_Pin_UP))
  {
    return UP;
  }
  /* "down" key is pressed */
  if (!GPIO_ReadInputDataBit(GPIO_DOWN, GPIO_Pin_DOWN))
  {
    return DOWN;
  }
  /* No key is pressed */
  else
  {
    return 0;
  }
#else

  return 0;
#endif

}




