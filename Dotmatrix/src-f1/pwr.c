/******************** (C) COPYRIGHT 2008 Osung LST *****************************
* File Name          : pwr.c
* Author             : Advanced Research Team
* Version            : V2.2.1
* Date               : 09/22/2008
* Description        : setting the value of VDD,VBL, IDD,IBL 
********************************************************************************
*
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"
#include "pwr.h"
#include "calltrace.h"

#include "sys_info.h"
#include "gpio.h"
#include "pwm_drv.h"


/* Private define ------------------------------------------------------------*/
//#define PWR_DEBUG

/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define PWM_DIM_MAX_FREQ      10000   /* MAX frequency */


/* Private variables ---------------------------------------------------------*/


extern system_info_t           g_systemInfo;


/*******************************************************************************
* Function Name  : P_DIMM_Control
* Description    : Setting the PWM Dimming which is controled as frequency and duty.
* Input          : None.
* Return         : None.
*******************************************************************************/
void P_DIMM_Control(u16 freq, u16 duty, u8 channel)
{
#ifdef PWR_DEBUG
  CallTrace(TRACE_APP_HIGH,"PWM Dimming Setting: freq = %d duty = %d \n\r",freq, duty);
#endif

  if( (freq >= 0) && (freq <= PWM_DIM_MAX_FREQ))
  {
    PWM4_SetControlFreq( freq);
    PWM4_Duty_Control( (u16)duty);
  }
  else
  {
    CallTrace(TRACE_APP_HIGH,"PWM dimming is out of range \n\r");
  }
}


