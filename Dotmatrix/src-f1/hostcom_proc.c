/******************** (C) COPYRIGHT 2008 Osung LST *****************************
* File Name          : hostcom_proc.c
* Author             : Advanced Research Team
* Version            : V2.2.1
* Date               : 09/22/2008
* Description        : hostcom procedure which is performing to parse command from host PC.
********************************************************************************
*
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"
#include "hostcom_proc.h"

#include "mem_heap.h"
#include "queue.h"
#include "hostcom_type.h"
#include "hostcom_handler.h"

#include "calltrace.h"
#include "common.h"


/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
queue_type_t            g_hostcom_q;
queue_type_t            g_hostgps_q;

int g_nGPSDsp=1;   // GPS Display

/* Private define ------------------------------------------------------------*/
/* system heap memory */
extern mem_heap_type    g_SystemHeap;
extern bool g_bResetSystem;
extern u8 g_PrevCmd;

/*******************************************************************************
* Function Name  : HostComProc_MsgQInit
* Description    : Initialize host communication procedure variables and something.
* Input          : None.
* Return         : None.
*******************************************************************************/
void HostComProc_MsgQInit(void)
{
  /* Queue configuration */
  Q_Init(&g_hostcom_q,"HostComProc",HOSTCOM_MAX_Q_SIZE);

}

/*******************************************************************************
* Function Name  : HostComProc_QPut
* Description    : put the data into hostcomproc'q msg Q.
* Input          : None.
* Return         : None.
*******************************************************************************/
void HostComProc_QPut(u8* str)
{
    //CallTrace(TRACE_DRV_ERROR, "HostComProc_QPut\n\r ");
	Q_PutElement(&g_hostcom_q,(int)str);
}

/*******************************************************************************
* Function Name  : HostComProc_ParsePkt
* Description    : parsing packet from Host.
* Input          : None.
* Return         : None.
*******************************************************************************/
void HostComProc_ParsePkt(u8* str)
{
//	u8 lenH = *(str+1);
	u8 lenL = *(str+2);
	u8 Cmd  = *(str+3);

//	CallTrace(TRACE_APP_LOW, "HostComProc_ParsePkt ==0x%x,%d\r\n", Cmd, lenL);

    // 0x30 : Normal
    // 0x31 : GPS
    // 0x32 : BANNER
    // 0x33 : brightness
    // 0x34 : use 24Sec
    // 0x35 : Font
    // 0x36 : ¹èµå¹ÎÅÏ¿ë 
    // 0x38 : reset
    // 0x39 : TestMode

    if(Cmd==0x30 || Cmd==0x31 || Cmd==0x34 || Cmd==0x36){
		if(g_PrevCmd!=0 && g_PrevCmd!=Cmd){
			g_bResetSystem=TRUE;
		}

		g_PrevCmd = Cmd;
    }

		
	if(Cmd==0x30 || Cmd==0x34 || Cmd==0x36){
		g_nGPSDsp=0;

        if(lenL==48)
		HostRecv_48Team4_2012(str);	

		if(lenL==46)
		HostRecv_46Char20(str);	

		if(lenL==106)
		HostRecv_106Volleyball(str);	
	}

    // Reset
    if(Cmd==0x38){
		g_bResetSystem=TRUE;
    }

    // TEST MODE
    if(Cmd==0x39){
		g_nGPSDsp=0;

		HostRecv_TestMode(str);
    }

    // GPS
	if(Cmd==0x31 ){
		g_nGPSDsp=1;

		// hh:mm
		u8 text[16*2+2];
		u8 colr[16*2+2];
		memset(text, 0x20, sizeof(text) );
		memset(colr, (1<<6|0<<4), sizeof(colr) );

	    MatrixSetFrame(0, text,colr,32); // 		
	    MatrixSetFrame(2, text,colr,32); // 		
	}

    // banner
    if(Cmd==0x32){
		g_nGPSDsp=0;

		HostRecv_266Banner(str);
    }

    // brightness
    if(Cmd==0x33){
		
		HostRecv_BrightCtrl(str);		
    }

	// font
	if(Cmd==0x35){
		
		HostRecv_FontCtrl(str);
	}
	
}


/*******************************************************************************
* Function Name  : HostComProc
* Description    : Host communication procedure handler
* Input          : None.
* Return         : None.
*******************************************************************************/
void HostComProc(void)
{

  u8* p_str;

  while(1)
  {
    if (Q_IsEmpty(&g_hostcom_q))
    {
      break;
    }
    else
    {
      p_str = (u8*)Q_GetElement(&g_hostcom_q);

      /* parsing command */
      HostComProc_ParsePkt(p_str);

      Free(&g_SystemHeap,p_str);
    }
  }
}


/*******************************************************************************
* Function Name  : HostGPSProc_MsgQInit
* Description    : Initialize host communication procedure variables and something.
* Input          : None.
* Return         : None.
*******************************************************************************/
void HostGPSProc_MsgQInit(void)
{
  /* Queue configuration */
  Q_Init(&g_hostgps_q,"HostGPSProc",HOSTGPS_MAX_Q_SIZE);
}

/*******************************************************************************
* Function Name  : HostGPSProc_QPut
* Description    : put the data into hostcomproc'q msg Q.
* Input          : None.
* Return         : None.
*******************************************************************************/
void HostGPSProc_QPut(u8* str)
{
//    CallTrace(TRACE_DRV_ERROR, "HostGPSProc_QPut\n\r ");
	Q_PutElement(&g_hostgps_q,(int)str);
}

/*******************************************************************************
* Function Name  : HostGPSProc_ParsePkt
* Description    : parsing packet from Host.
* Input          : None.
* Return         : None.
*******************************************************************************/
void HostGPSProc_ParsePkt(u8* str)
{
  if(g_nGPSDsp==0)
  	return;

//   CallTrace(TRACE_APP_LOW, "HostGPSProc_ParsePkt ==%x\r\n", g_nGPSDsp);

   HostRecv_GPS(str);
}

/*******************************************************************************
* Function Name  : HostGPSProc
* Description    : GPS communication procedure handler
* Input          : None.
* Return         : None.
*******************************************************************************/
void HostGPSProc(void)
{

  u8* p_str;

  while(1)
  {
    if (Q_IsEmpty(&g_hostgps_q))
    {
      break;
    }
    else
    {
      p_str = (u8*)Q_GetElement(&g_hostgps_q);

      /* parsing command */
      HostGPSProc_ParsePkt(p_str);

      Free(&g_SystemHeap,p_str);
    }
  }
}



