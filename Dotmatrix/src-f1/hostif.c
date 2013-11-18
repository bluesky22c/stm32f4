/******************** (C) COPYRIGHT 2008 Osung LST *****************************
* File Name          : hostif.c
* Author             : Advanced Research Team
* Version            : V2.2.1
* Date               : 09/22/2008
* Description        : parsing raw data from usart and handover it to hostcom procedure.
********************************************************************************
*
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"
#include "hostif.h"
#include "queue.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "mem_heap.h"

#include "hostcom_type.h"
#include "hostcom_proc.h"

#include "usart.h"
#include "common.h"
#include "calltrace.h"
#include "gpio.h"
#include "usb_drv.h"
#include "sys_info.h"


/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
queue_type_t            g_usb_q;

const host_if_fun_t     g_hostIfFun[MAX_HOST_INTERFACE] = {\
                          { UartHostPutChar, UartHostPutNChars}
                        };

u8                      g_uartBufStr[PACKET_MAX_LEN];
u8                      g_uartBufGPS[PACKET_MAX_LEN];

/* Private define ------------------------------------------------------------*/
extern mem_heap_type    g_SystemHeap;
extern system_info_t    g_systemInfo;


/*******************************************************************************
* Function Name  : HostIf_Init
* Description    : Initialize hostif block such as variables, ... 
* Input          : None.
* Return         : None.
*******************************************************************************/
void HostIf_Init(void)
{

}


/*******************************************************************************
* Function Name  : ChkFCS1
* Description    : 1Byte Checking Frame Check Sum.
* Input          : None.
* Return         : None.
*******************************************************************************/
bool ChkFCS1(u8* str, u32 len, u8* chksum  )
{
  u32 i;
  u8  fcs   = 0;

  for(i = 0; i < len; i++)
  {
    fcs += *(str + i);
  }
  
  if(chksum[0] == fcs)
  {
    return TRUE;
  }
  else
  {
//    CallTrace(TRACE_DRV_ERROR, "UartBuild_Msg: FCS Error %d %x %x \n\r ", len, chksum[0], fcs);
  
    return FALSE;
  }
}

/*******************************************************************************
* Function Name  : ChkFCS2
* Description    : 2Byte Checking Frame Check Sum.
* Input          : None.
* Return         : None.
*******************************************************************************/
bool ChkFCS2(u8* str, u32 len, u8* chksum  )
{
  u32 i;
  u8  fcs   = 0;
  u8  str_sum[2];

  for(i = 0; i < len; i++)
  {
    fcs ^= *(str + i);
  }
   HexToAsc(fcs, str_sum);
  
  if( ( chksum[0] == str_sum[0]) && (chksum[1] == str_sum[1]))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*******************************************************************************
* Function Name  : UartBuild_GPS
* Description    : Get Date Time Data from Receive GPS
* Input          : None.
* Return         : None.
*******************************************************************************/
void UartBuild_GPS(u8 rch )
{
	static u32	buf_index = 0;
	static u32	fcs_index = 0;
	static bool etx_flag  = FALSE;
	
	static u8	fcs_str[2];
	
	u8* 		rcv_str   = NULL;
	
	switch(rch)
	{
	case '$':
	  buf_index = 0;	  /* buffer index */
	  etx_flag	= FALSE;  /* etx flag */
	  fcs_index = 0;	  /* frame check sum index */
	  
	  break;
	
	case '*':
	  etx_flag	= TRUE;
	  fcs_index = 0;	  /* frame check sum index */	  
	  break;
	
	default:
	  if( etx_flag == FALSE )
	  {
		g_uartBufGPS[buf_index] = rch;
		buf_index++;
	  }
	  else
	  {	  

		fcs_str[fcs_index++] = rch;
		if( fcs_index == 2 )
		{								
		  if(ChkFCS2(g_uartBufGPS, buf_index, fcs_str))
		  {
		    if(
			g_uartBufGPS[0]=='G' &&
			g_uartBufGPS[1]=='P' &&
			g_uartBufGPS[2]=='G' &&
			g_uartBufGPS[3]=='G' &&
			g_uartBufGPS[4]=='A')
		    {
				rcv_str = Malloc(&g_SystemHeap,10);
				memcpy( rcv_str, g_uartBufGPS+6, 10 );
				
				HostGPSProc_QPut(rcv_str);
		    }		
		  }
		  else
		  {
			//CallTrace(TRACE_DRV_ERROR, "UartBuild_GPS: FCS Error \n\r ");
		  }
		  
		  fcs_index = 0;
		}

	  }
	  break;
	}

}


/*******************************************************************************
* Function Name  : UartBuild_Msg
* Description    : build message to convey for other procedures from USB port per char.
* Input          : None.
* Return         : None.
*******************************************************************************/
void UartBuild_Msg(u8 rch )
{
  static u32  buf_index = 0;
  static u32  fcs_index = 0;
  static u32  stx_index = 0;
  static u32  etx_index = 0;
  static u8   fcs_str[2];
  static bool etx_flag  = FALSE;
  bool        bChk      = FALSE; 
  u8*         rcv_str   = NULL;

  // Resend
#ifdef USE_UART4
//  SerialPutChar(UART4, rch);
#endif

  u32 len = 0;
	
  if(buf_index>=PACKET_MAX_LEN)
	 buf_index=0;

  switch(rch)
  {
  case STX:
    g_uartBufStr[buf_index] = rch;
	buf_index++;

    etx_flag  = FALSE;  /* etx flag */
    break;

  case ETX:	
	
    // cmd==0x31
    if( (buf_index >= 6-1)&&g_uartBufStr[buf_index-5]==STX && g_uartBufStr[buf_index-3]==6&&g_uartBufStr[buf_index-2]==0x31)
    {
		 len=6;		 
		 etx_index = buf_index;
		 stx_index = buf_index-(len-1);
    }

    // cmd==0x38
    if( (buf_index >= 6-1)&&g_uartBufStr[buf_index-5]==STX && g_uartBufStr[buf_index-3]==6&&g_uartBufStr[buf_index-2]==0x38)
    {
		 len=6;		 
		 etx_index = buf_index;
		 stx_index = buf_index-(len-1);
    }

    // cmd==0x33
    if( (buf_index >= 7-1)&&g_uartBufStr[buf_index-6]==STX && g_uartBufStr[buf_index-4]==7&&g_uartBufStr[buf_index-3]==0x33)
    {
		 len=7;		 
		 etx_index = buf_index;
		 stx_index = buf_index-(len-1);
    }

    // cmd==0x39
    if( (buf_index >= 8-1)&&g_uartBufStr[buf_index-7]==STX && g_uartBufStr[buf_index-5]==8&&g_uartBufStr[buf_index-4]==0x39)
    {
		 len=8;		 
		 etx_index = buf_index;
		 stx_index = buf_index-(len-1);
    }

    // cmd==0x30
    if( (buf_index >= 46-1)&&g_uartBufStr[buf_index-45]==STX && 
		g_uartBufStr[buf_index-43]==46&&g_uartBufStr[buf_index-42]==0x30)
    {
		 len=46;		 
		 etx_index = buf_index;
		 stx_index = buf_index-(len-1);
    }

    // cmd==0x30
    if( (buf_index >= 48-1)&&g_uartBufStr[buf_index-47]==STX && 
		g_uartBufStr[buf_index-45]==48&&g_uartBufStr[buf_index-44]==0x30)
    {
		 len=48;		 
		 etx_index = buf_index;
		 stx_index = buf_index-(len-1);
    }

    // cmd==0x30
    if( (buf_index >= 106-1)&&g_uartBufStr[buf_index-105]==STX && 
		g_uartBufStr[buf_index-103]==106&&g_uartBufStr[buf_index-102]==0x30)
    {
		 len=106;		 
		 etx_index = buf_index;
		 stx_index = buf_index-(len-1);
    }

    // cmd==0x34
    if( (buf_index >= 48-1)&&g_uartBufStr[buf_index-47]==STX && 
		g_uartBufStr[buf_index-45]==48&&g_uartBufStr[buf_index-44]==0x34)
    {
		 len=48;		 
		 etx_index = buf_index;
		 stx_index = buf_index-(len-1);
    }

    // cmd==0x36
    if( (buf_index >= 48-1)&&g_uartBufStr[buf_index-47]==STX && 
		g_uartBufStr[buf_index-45]==48&&g_uartBufStr[buf_index-44]==0x36)
    {
		 len=48;		 
		 etx_index = buf_index;
		 stx_index = buf_index-(len-1);
    }

    // cmd==0x35
    if( (buf_index >= 7-1)&&g_uartBufStr[buf_index-6]==STX && g_uartBufStr[buf_index-4]==7&&g_uartBufStr[buf_index-3]==0x35)
    {
		 len=7;		 
		 etx_index = buf_index;
		 stx_index = buf_index-(len-1);
    }

    // cmd==0x32
    if( (buf_index >= 266-1)&&g_uartBufStr[buf_index-265]==STX && 
		g_uartBufStr[buf_index-264]==1 && g_uartBufStr[buf_index-263]==10 &&
	    g_uartBufStr[buf_index-262]==0x32 )
    {
		 len=266;		 
		 etx_index = buf_index;
		 stx_index = buf_index-(len-1);
    }

    g_uartBufStr[buf_index] = rch;
	buf_index++;	

	if(len>0){
	
	    fcs_index = buf_index-2;      /* frame check sum index */
		fcs_str[0] = g_uartBufStr[fcs_index];
			
	    // checksum
		if( (g_uartBufStr[stx_index+3]==0x30||g_uartBufStr[stx_index+3]==0x34) && len==48 )  // for total panel case exist Buzzer flag
	    bChk = ChkFCS1(g_uartBufStr+stx_index, len-3, fcs_str);
		else
	    bChk = ChkFCS1(g_uartBufStr+stx_index, len-2, fcs_str);

	    if(bChk && len>0)
		{			
			rcv_str = Malloc(&g_SystemHeap,len);
			memcpy( rcv_str, g_uartBufStr+stx_index, len);

			HostComProc_QPut(rcv_str);

			etx_flag  = TRUE;
			buf_index = 0;	  /* buffer index */	  
		}
		else
		{
	//	  CallTrace(TRACE_DRV_ERROR, "UartBuild_Msg: FCS Error %d, 0x%x \n\r", len, fcs_str[0] );
		}
		fcs_index = 0;

	}	
	
    break;

  default:
    if( etx_flag == FALSE )
    {
      g_uartBufStr[buf_index] = rch;
      buf_index++;
    }

    break;
  }

  
}

/*******************************************************************************
* Function Name  : SendToHost
* Description    : Send message to Host PC.
* Input          : None.
* Return         : None.
*******************************************************************************/
void SendToHost(u8* str, u32 len )
{
  u32 i;
  u8  fcs = 0;
  u8  str_sum[2];
  u8  *tx_str = NULL;

  
  CallTrace(TRACE_APP_LOW, "SendToHost: len = %d \n\r", len);

  for(i = 0; i < len; i++)
  {
    fcs ^= *(str + i);
  }
  HexToAsc(fcs, str_sum);

  tx_str = Malloc(&g_SystemHeap,(len + 6));

  if( tx_str)
  {
    tx_str[0]     = STX;
	tx_str[1]     = (u8)(len >> 8);
	tx_str[2]     = (u8)len;
	tx_str[3]     = 0x30;   // cmd	
    memcpy(&tx_str[4], str, len);

    tx_str[len+4] = str_sum[0];
    tx_str[len+5] = ETX;

    g_hostIfFun[HOST_INTERFACE_TYPE].PutNChars(tx_str, len+6 );

    Free(&g_SystemHeap, tx_str);
  }
  else
  {
    CallTrace(TRACE_APP_LOW, "SendToHost: Allocation Error\n\r");
  }
}


