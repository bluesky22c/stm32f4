/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : hw_config.c
* Author             : MCD Application Team
* Version            : V2.2.1
* Date               : 09/22/2008
* Description        : Hardware Configuration & Setup
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"
#include "stm32f10x_it.h"
#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_drv.h"
#include "hw_config.h"
#include "platform_config.h"
#include "usb_pwr.h"

#include "calltrace.h"

#include "common.h"
#include "key_proc.h"
#include "mem_heap.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define MAX_IR_KEY        27

#define lead	1 
#define code	2

#define CustomCode1 0x77
#define CustomCode2 0x88

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
typedef struct
{
  u8  ir_key;
  u8  pc_key;
}key_conv_t;

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

vu8    data[4];  // custom code : data[0],[1]  data : data[2],[3]
vu8    ir_count_100us   = 0;
vu8    bit_count=0;
vu8    state=0;
vu8    flag=0;
vu8    temp=0;

key_conv_t key_conv_table[MAX_IR_KEY] = 
{ \
  {0x02, 0x52}, /* Up arrow */
  {0x04, 0x51}, /* Down arrow */
  {0x05, 0x4F}, /* Right arrow */  
  {0x06, 0x50}, /* Left arrow */
  {0x03, 0x2C}, /*  Center key - Space bar */
  {0x10, 0x1E}, /* 1 */
  {0x11, 0x1F}, /* 2 */
  {0x12, 0x20}, /* 3 */
  {0x14, 0x21}, /* 4 */
  {0x15, 0x22}, /* 5 */
  {0x16, 0x23}, /* 6 */
  {0x18, 0x24}, /* 7 */
  {0x19, 0x25}, /* 8 */
  {0x1A, 0x26}, /* 9 */
  {0x1D, 0x27}, /* 0*/
  {0x1C, 0x43}, /* Mute - F10 */
  {0x00, 0x42}, /* PWR  - F9  */
  {0x0B, 0x28}, /* Play - Enter */
  {0x0F, 0x4E}, /* BW - Page Down */
  {0x07, 0x4B}, /* FW - Page UP */
  {0x0A, 0x57}, /* + - + */
  {0x0E, 0x56}, /* - - - */
  {0x08, 0x00}, /* Menu - NULL */
  {0x0C, 0x00}, /* Band - NULL */
  {0x0D, 0x00}, /* Menu - NULL */
  {0x01, 0x00}, /* Disp - NULL */
  {0x1E, 0x2A} /* Cancel - DEL */

};

/* system heap memory */
extern mem_heap_type           g_SystemHeap;



/*******************************************************************************
* Function Name  : Conv_Key
* Description    : Converting IR key to PC key
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u8 Conv_Key(u8 key)
{
  u8 pc_key;
  int i;

  return key;
  
  for(i = 0; i<MAX_IR_KEY; i++)
  {
    if( key == key_conv_table[i].ir_key )
    {
      pc_key = key_conv_table[i].pc_key;
      return pc_key;
    }
  }

  return 0;
}

/*******************************************************************************
* Function Name  : SendKey
* Description    : Send key to key processing proc.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SendKey(u8 key)
{
  key_proc_req_t  *p_req = NULL;

  p_req = (key_proc_req_t*)Malloc(&g_SystemHeap,8);

  if( p_req != NULL)
  {
    p_req->cmd = KEY_CMD_IR_INPUT;
    p_req->key_msg.key_modifier = 0;
    p_req->key_msg.key_val = key;

    KeyProc_QPut((u8*)p_req);
  }
  else
  {
    CallTrace(TRACE_APP_LOW,"Remocon: Memory alloc fail !\n\r");
  }

}

/*******************************************************************************
* Function Name  : IRemoCon_ISR
* Description    : process IR remocon received pulse .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void IRemoCon_ISR()
{
//	CallTrace(TRACE_DRV_HIGH, "IRemoCon_ISR %d %d\n\r",state,ir_count_100us);

  switch(state)
  { 
	case lead :   
		// between falling edge (8.5ms~9.5ms)
		// then, lead code!!!
		while((ir_count_100us >= 85) && (ir_count_100us < 95)){	
			  state = code; 								    
			  bit_count = 0;   
			  temp = 0;

			  CallTrace(TRACE_DRV_HIGH, "IRemoCon lead %d\n\r",ir_count_100us);
			  
			  break;
		}
	case code : 											
															
		 if((ir_count_100us < 15)){ // dummy time
		 	
		 }else if((ir_count_100us >= 20) && (ir_count_100us < 25)) // between falling edge (2ms~2.5ms)
			 temp |= 0x80;								      // then, code is one(1)!!!
	     else{ 
			 state = lead;
			 break;
		 }
		  
	     bit_count++;

	     if(!(bit_count % 8))						   
	     { 
			  data[(bit_count / 8) - 1] = temp; 
			  temp = 0;

			  if(bit_count >= 32)
		      { 
				  state = lead; 
				  bit_count = 0;
				  flag = 1;
                
			      //CallTrace(TRACE_DRV_HIGH, "IRemoCon custom code:0x%02x%02x data:0x%02x%02x\n\r",data[0],data[1],data[2],data[3]);

				   if(CustomCode1==data[0] && CustomCode2==data[1])
				  SendKey(Conv_Key(data[2]));				  
			  }
		  }

		  temp = temp >> 1;
		  break;

	default: 
		state = lead;
		break;
	
  }

  ir_count_100us = 0;
  
}


