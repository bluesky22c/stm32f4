/******************** (C) COPYRIGHT 2008 Osung LST *****************************
* File Name          : keyproc_handler.c
* Author             : Advanced Research Team
* Version            : V2.2.1
* Date               : 09/22/2008
* Description        : key processing handler.
********************************************************************************
*
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "key_proc_handler.h"
#include "hostcom_proc.h"

#include "mem_heap.h"
#include "queue.h"
#include "hostcom_type.h"
#include "calltrace.h"
#include "hostif.h"

#include "key_proc.h"
#include "timer_proc.h"
#include "gpio.h"

#include "AutomataKR.h"
#include "Matrix_proc.h"

/* Private typedef -----------------------------------------------------------*/
enum		
{
	CHAR_STATE_HAN = 0,	    
	CHAR_STATE_ENG,					
	CHAR_STATE_eng,			
	CHAR_STATE_NUM	
};

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

u8 m_nKeyKind = CHAR_STATE_HAN;
u8 m_nCharStatus[2]={0,};
s8 m_nSelPort = 0;

u8 text[40];   // Flash read size = 40 bytes
u8 colr[40];   // Flash read size = 40 bytes

u8 m_aryTxtIdx    = 0;

bool m_KeyEnd     = TRUE;
bool m_bKEY_ATTRI = FALSE;
bool m_bKEY_OK    = FALSE;
bool m_bKEY_F1    = FALSE;
bool m_bKEY_F2    = FALSE;

/* Private define ------------------------------------------------------------*/
#define BUZZER         GpioC->Bit.b6
#define LED_KEYINPUT   GpioC->Bit.b9
#define RELAY_POWER    GpioB->Bit.b3

#define START_POS      16 // 8열 = 16, 16열 = 0

/* system heap memory */
extern mem_heap_type           g_SystemHeap;

extern u8 g_CursorPos;
extern bool g_bCursor2Byte;
extern u8 g_CursorFlag[4];
extern u8 g_FrameText[4][33];
extern u8 g_FrameColr[4][33];
extern int g_xMove[4];
extern DotEffect_t g_DotEffect[4];

extern u8 g_fontIdx;
extern u8 g_Brightness;  // Control Brightness  (0~9)

/*******************************************************************************
* Function Name  : TimerKeyDelay_CallBack
* Description    : put the data into detect proc's msg Q.
* Input          : None.
* Return         : None.
*******************************************************************************/
void TimerKeyDelay_CallBack(void* pArgs)
{
  /* LED,Buzzer Off */
  LED_KEYINPUT = 1;
  BUZZER       = 0;
  
  m_KeyEnd=TRUE;
}


/*******************************************************************************
* Function Name  : KeyProc_Init_Handler
* Description    : 
* Input             : nMode   0=Display, 1=Edit  2=Select Left 3=Select Right.
* Return           : None.
*******************************************************************************/
void KeyProc_Init_Handler(u8 nMode)
{
#ifdef ROWCOL_1_8
    // Display
	if(nMode==0){
	
        if(LoadFromFlash(24,text,colr)){
			memset(g_CursorFlag,0,sizeof(g_CursorFlag));
        }
	}

    // Change Edit
    if(nMode==1){
        if(LoadFromFlash(m_aryTxtIdx,text,colr)){
			MatrixSetFrame(m_nSelPort, text, colr, 32);
        }
		
		SPRINTF(text, "                 No [%02d]", m_aryTxtIdx+1);

		for(u8 i=24;i<32;i++){
			text[i] = g_FrameText[m_nSelPort][i];
			colr[i] = g_FrameColr[m_nSelPort][i];
		}

		g_CursorPos  = START_POS+8;
		g_CursorFlag[m_nSelPort]=1;		
		
		m_nKeyKind=CHAR_STATE_HAN; 
    }

    // Change Left
    if(nMode==2){
        if(LoadFromFlash(m_aryTxtIdx,text,colr)){

            // set left data
			for(u8 i=24;i<32;i++){
				text[i-8] = text[i]; 
				colr[i-8] = colr[i];
			}

            // set right data
			for(u8 i=24;i<32;i++){
				text[i] = g_FrameText[m_nSelPort][i];
				colr[i] = g_FrameColr[m_nSelPort][i];
			}		
        }
    }
	
    // Change Right
    if(nMode==3){
        if(LoadFromFlash(m_aryTxtIdx,text,colr)){

            // set left data
			for(u8 i=16;i<24;i++){
				text[i] = g_FrameText[m_nSelPort][i];
				colr[i] = g_FrameColr[m_nSelPort][i];
			}		
        }
    }	

	// Exchange left right
    if(nMode==4){

        // set left data
		for(u8 i=16;i<24;i++){
			text[i+8] = g_FrameText[m_nSelPort][i];
			colr[i+8] = g_FrameColr[m_nSelPort][i];
		}		

		// set right data
		for(u8 i=24;i<32;i++){
			text[i-8] = g_FrameText[m_nSelPort][i];
			colr[i-8] = g_FrameColr[m_nSelPort][i];
		}		
	 
    }
	
	MatrixSetFrame(m_nSelPort, text,colr,32);	

    //---- port0 data -> port2 data
    //---- left <-> right
    u8 textCnv[40];
	u8 colrCnv[40];

	//left:16~23  right:24~31
    memcpy(textCnv+16,text+24,16); memcpy(colrCnv+16,colr+24,16); // right -> left
    memcpy(textCnv+24,text+16,16); memcpy(colrCnv+24,colr+16,16); // left -> right    
	MatrixSetFrame(2, textCnv,colrCnv,32);	
	
#else
	memset(g_CursorFlag,0,sizeof(g_CursorFlag));

	memcpy(text, g_FrameText[m_nSelPort], sizeof(text));
	memcpy(colr, g_FrameColr[m_nSelPort], sizeof(colr));

	g_CursorPos=START_POS;
#endif

	g_bCursor2Byte = (text[g_CursorPos]<0x80) ? FALSE : TRUE;
	
	SetHanStatus(HS_FIRST);
	m_nCharStatus[0] = 0;
	m_nCharStatus[1] = 0;

	if(nMode==0){
		m_bKEY_ATTRI = FALSE;
		m_bKEY_OK	 = FALSE;

		m_bKEY_F1    = FALSE;
		m_bKEY_F2    = FALSE;
	}
	
}

bool isValidKey(int nKey)
{
	if(nKey==KEY_POWER)return TRUE; 
	if(nKey==KEY_MENU )return TRUE;  
	if(nKey==KEY_COLOR )return TRUE; 
	if(nKey==KEY_ATTRI)return TRUE;  
	if(nKey==KEY_WIDTH)return TRUE;  
	if(nKey==KEY_FONT)return TRUE;   
	
	if(nKey==KEY_NUM0)return TRUE;  
	if(nKey==KEY_NUM1)return TRUE;   
	if(nKey==KEY_NUM2)return TRUE;   
	if(nKey==KEY_NUM3)return TRUE;   
	if(nKey==KEY_NUM4)return TRUE;   
	if(nKey==KEY_NUM5)return TRUE;   
	if(nKey==KEY_NUM6)return TRUE;   
	if(nKey==KEY_NUM7)return TRUE;   
	if(nKey==KEY_NUM8)return TRUE;   
	if(nKey==KEY_NUM9)return TRUE;   
	
	if(nKey==KEY_STAR)return TRUE;   
	if(nKey==KEY_HESI)return TRUE;	 
	
	if(nKey==KEY_LEFT)return TRUE;	 
	if(nKey==KEY_RIGHT)return TRUE;  
	if(nKey==KEY_UP)return TRUE;	  
	if(nKey==KEY_DOWN)return TRUE;   
	
	if(nKey==KEY_OK)return TRUE;	  
	
	if(nKey==KEY_STATE_HAN)return TRUE; 
	if(nKey==KEY_STATE_ENG)return TRUE; 
	if(nKey==KEY_STATE_eng)return TRUE; 
	if(nKey==KEY_STATE_NUM)return TRUE; 
	
	if(nKey==KEY_DEL)return TRUE;	 
	if(nKey==KEY_SPACE)return TRUE; 
	
	if(nKey==KEY_COPY)return TRUE;  
	if(nKey==KEY_ESC)return TRUE;	 
	
//	if(nKey==KEY_MOVE)return TRUE;  
	
	if(nKey==KEY_F1)return TRUE;	 
	if(nKey==KEY_F2)return TRUE;	 

	if(nKey==KEY_BRIGTHNESS)return TRUE;	 

    return FALSE;
}

/*******************************************************************************
* Function Name  : KeyProc_Key_Handler
* Description    : 
* Input          : None.
* Return         : None.
*******************************************************************************/
void KeyProc_Key_Handler(u8* str, u32 len)
{ 
  if(!m_KeyEnd)
	  	return;
	  
  key_proc_req_t   *p_req;  
  p_req = (key_proc_req_t*)str;
  
  //CallTrace(TRACE_APP_LOW, "KEY_CMD_KEY: 0x%x \n\r",p_req->key_msg.key_val);

  u8 nKey = p_req->key_msg.key_val;

  if(!isValidKey(nKey))
  	return;

  /* LED,Buzzer On  */
  LED_KEYINPUT= 0;
  BUZZER      = 1;
  
  if(nKey==KEY_POWER){
      RELAY_POWER = !RELAY_POWER;

	  m_KeyEnd=FALSE;
	  TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );	  
	  return;
  } 
    
  if(nKey==KEY_MENU){  	
      KeyProc_Init_Handler(1);

	  m_KeyEnd=FALSE;
	  TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );	  
	  return;
  }
  if(nKey==KEY_ESC){
	  KeyProc_Init_Handler(0);

	  m_KeyEnd=FALSE;
	  TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );
	  return;
  }

  if(nKey==KEY_ATTRI){
	  KeyProc_Init_Handler(4);
	  
	  m_KeyEnd=FALSE;
	  TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );	  
	  return;	  
  }  

  if(nKey==KEY_BRIGTHNESS){

	 g_Brightness++;

	 if(g_Brightness<3)g_Brightness=3;
	 if(g_Brightness>9)g_Brightness=3;
	 
	 P_DIMM_Control(2000,g_Brightness*10+9,0);		  

     m_KeyEnd=FALSE;
     TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );  
     return;	 
  }
  
  if(nKey==KEY_F1){

	  text[0]=0;
	  text[1]=0;
	  text[2]=0;
	  g_CursorPos = 0;
	  
	  m_nKeyKind=CHAR_STATE_NUM;
	  g_CursorFlag[m_nSelPort]=1;			  
  	
	  if(!m_bKEY_F1){
		  m_bKEY_F1 = TRUE;
		  m_bKEY_F2 = FALSE;
	     
		  m_KeyEnd=FALSE;
		  TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );	  
		  return;	  
	  }else{
          nKey=KEY_UP;
	  }
	  
  }  
  if(nKey==KEY_F2){

	  text[0]=0;
	  text[1]=0;
	  text[2]=0;
	  g_CursorPos = 0;
	  
	  m_nKeyKind=CHAR_STATE_NUM;
	  g_CursorFlag[m_nSelPort]=1;			  
  	
	  if(!m_bKEY_F2){
		  m_bKEY_F1 = FALSE;
		  m_bKEY_F2 = TRUE;
		  	
		  m_KeyEnd=FALSE;
		  TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );	  
		  return;	  
	  }else{
		  nKey=KEY_UP;
	  }
  }  
  
  if(nKey==KEY_UP){
        m_aryTxtIdx++;
		if(m_aryTxtIdx>23)
			m_aryTxtIdx=0;

             if(m_bKEY_F1) KeyProc_Init_Handler(2);  // load left
        else if(m_bKEY_F2) KeyProc_Init_Handler(3);  // load right
        else if(g_CursorFlag[m_nSelPort]==1) KeyProc_Init_Handler(1); // load edit
		
		m_KeyEnd=FALSE;
		TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );		
		
		return;
  }
  if(nKey==KEY_DOWN){
	  m_aryTxtIdx--;
	  if(m_aryTxtIdx==0xff)
		  m_aryTxtIdx=23;

		     if(m_bKEY_F1) KeyProc_Init_Handler(2);  // load left
		else if(m_bKEY_F2) KeyProc_Init_Handler(3);  // load right
		else if(g_CursorFlag[m_nSelPort]==1) KeyProc_Init_Handler(1); // load edit

		m_KeyEnd=FALSE;
		TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );		
		
		return;
  } 

  if(nKey==KEY_COPY){
	// Change left or right
	if( (m_bKEY_F1 || m_bKEY_F2) ){
		if( SaveToFlash(24) ){
			m_bKEY_F1 = FALSE;
			m_bKEY_F2 = FALSE;
		}			
	}

	m_KeyEnd=FALSE;	
	TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );
	return;	
  }
  
  if(nKey==KEY_OK){

      if(m_bKEY_F1 || m_bKEY_F2){  // Load

		u8 buf[3]={0,};
		buf[0] = text[0];
		buf[1] = text[1];
		buf[2] = 0;

		u8 tmp = atoi(buf);
		//CallTrace(TRACE_APP_LOW, "Input Num: %d\n\r",tmp);

		if(0<tmp && tmp<25){
			m_aryTxtIdx=tmp-1;

	        if(m_bKEY_F1)KeyProc_Init_Handler(2);
	        if(m_bKEY_F2)KeyProc_Init_Handler(3);
	        			
			memset(g_CursorFlag,0,sizeof(g_CursorFlag));
		    m_bKEY_F1    = FALSE;
		    m_bKEY_F2    = FALSE;			
		}

		m_KeyEnd=FALSE;	  
		TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );
		return;	  
				
      }
	  
      // Editing
      if(g_CursorFlag[m_nSelPort]==1)
	  	SaveToFlash(m_aryTxtIdx);
	  	  
	  m_KeyEnd=FALSE;	  
	  TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );
	  return;	  
  }   

  if(nKey==KEY_MOVE){

	  if(g_DotEffect[0].inType==0)g_DotEffect[0].inType=1;
	  else                        g_DotEffect[0].inType=0;

	  if(g_DotEffect[1].inType==0)g_DotEffect[1].inType=1;
	  else                        g_DotEffect[1].inType=0;

	  if(g_DotEffect[2].inType==0)g_DotEffect[2].inType=1;
	  else                        g_DotEffect[2].inType=0;

	  if(g_DotEffect[3].inType==0)g_DotEffect[3].inType=1;
	  else                        g_DotEffect[3].inType=0;

	  g_xMove[0]=0;
	  g_xMove[1]=0;
	  g_xMove[2]=0;
	  g_xMove[3]=0;
	  

	  m_KeyEnd=FALSE;
	  TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );
	  
	  return;
  }

  
  // under coding is using for only Edit Mode
  if(g_CursorFlag[m_nSelPort]==0)
	 return;

 
  if(nKey==KEY_FONT){
      g_fontIdx++;
	  if(g_fontIdx>2)
	  	g_fontIdx=0;

	  memset(g_FrameText[m_nSelPort], 0x20, sizeof(g_FrameText[m_nSelPort]) );

	  MatrixSetFrame(m_nSelPort, text,colr,32); 		  

	  m_KeyEnd=FALSE;
	  TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );
	  
	  return;
  }  
  if(nKey==KEY_STATE_HAN){
	  m_nKeyKind=CHAR_STATE_HAN;

	  m_KeyEnd=FALSE;
	  TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );
	  
	  return;
  }
  if(nKey==KEY_STATE_ENG){
	  m_nKeyKind=CHAR_STATE_ENG;

	  m_KeyEnd=FALSE;
	  TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );
	  
	  return;
  }
  if(nKey==KEY_STATE_eng){
	  m_nKeyKind=CHAR_STATE_eng;

	  m_KeyEnd=FALSE;
	  TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );
	  return;
  }
  if(nKey==KEY_STATE_NUM){
	  m_nKeyKind=CHAR_STATE_NUM;

	  m_KeyEnd=FALSE;
	  TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );
	  return;
  }
  
  	
  if(nKey==KEY_LEFT){
	  u8 high=0x0, low=0x0;
	  
	  if(g_CursorPos>0) low  = text[g_CursorPos-1]; 	  
	  if(g_CursorPos>1) high = text[g_CursorPos-2]; 	  

#ifdef ROWCOL_1_8
	  if(g_CursorPos>(START_POS+8+1) && KSSMtoIndex((high<<8)+(low&0xff))!=-1){ // previous char is 2byte char
	     g_CursorPos-=2;
	  }else if(g_CursorPos>(START_POS+8) ){
	     g_CursorPos-=1;
	  }
#else
	  if(g_CursorPos>1 && KSSMtoIndex((high<<8)+(low&0xff))!=-1){ // previous char is 2byte char
		  g_CursorPos-=2;
	  }else if(g_CursorPos>0){
		  g_CursorPos-=1;
	  }
#endif

	  SetHanStatus(HS_FIRST);
	  m_nCharStatus[0] = 0;
	  m_nCharStatus[1] = 0;

	  g_bCursor2Byte = (text[g_CursorPos]<0x80) ? FALSE : TRUE;

	  m_KeyEnd=FALSE;
	  TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );
	  return;
  }
  if(nKey==KEY_RIGHT){
	  if(text[g_CursorPos]>=0x80){ // previous char is 2byte char
		  if(g_CursorPos < (DOT_MAX_MOD*2-2) )
			  g_CursorPos+=2;
	  }else{
		  if(g_CursorPos < (DOT_MAX_MOD*2-1) )
			  g_CursorPos+=1;
	  }

	  SetHanStatus(HS_FIRST);
	  m_nCharStatus[0] = 0;
	  m_nCharStatus[1] = 0;

      g_bCursor2Byte = (text[g_CursorPos]<0x80) ? FALSE : TRUE;

	  m_KeyEnd=FALSE;
	  TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );	  
	  return;
  }
  
  if(nKey==KEY_COLOR){
	  u8 tc,bc;
	  tc = (colr[g_CursorPos]&0xc0)>>6;
	  bc = (colr[g_CursorPos]&0x30)>>4;

		   if(tc==1)tc=2;
	  else if(tc==2)tc=3;
	  else if(tc==3)tc=1;

	  if(g_bCursor2Byte){
		  colr[g_CursorPos+0] = (tc<<6|bc<<4);
		  colr[g_CursorPos+1] = (tc<<6|bc<<4);
	  }else{
		  colr[g_CursorPos+0] = (tc<<6|bc<<4);
	  }

	  MatrixSetFrame(m_nSelPort, text,colr,32); 		  

	  m_KeyEnd=FALSE;
	  TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );
	  return;
  }

  if(nKey==KEY_WIDTH){
	  u8 tc,bc;
	  tc = (colr[g_CursorPos]&0xc0)>>6;
	  bc = (colr[g_CursorPos]&0x30)>>4;

	       if(bc==0)bc=1;
	  else if(bc==1)bc=2;
	  else if(bc==2)bc=3;
	  else if(bc==3)bc=0;

	  if(g_bCursor2Byte){
		  colr[g_CursorPos+0] = (tc<<6|bc<<4);
		  colr[g_CursorPos+1] = (tc<<6|bc<<4);
	  }else{
		  colr[g_CursorPos+0] = (tc<<6|bc<<4);
	  }

	  MatrixSetFrame(m_nSelPort, text,colr,32);

	  m_KeyEnd=FALSE;
	  TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );
	  return;
  }
  
  // 이전글씨삭제
  if(nKey==KEY_DEL){
	  if(m_nKeyKind==CHAR_STATE_HAN &&GetHanStatus()!=HS_FIRST){
	  
	  }else{
		  u8 high=0x0, low=0x0;
		  
		  if(g_CursorPos>0) low  = text[g_CursorPos-1]; 	  
		  if(g_CursorPos>1) high = text[g_CursorPos-2]; 	  
		  
		  if(g_CursorPos>1 && KSSMtoIndex((high<<8)+(low&0xff))!=-1){ // previous char is 2byte char
			  g_CursorPos-=2;
			  text[g_CursorPos+0] = 0x20;
			  text[g_CursorPos+1] = 0x20;
		  }else if(g_CursorPos>0){
			  g_CursorPos-=1;
			  text[g_CursorPos+0] = 0x20;
		  }

          g_bCursor2Byte = (text[g_CursorPos]<0x80) ? FALSE : TRUE;
		  m_nCharStatus[0] = 0;

		  MatrixSetFrame(m_nSelPort, text,colr,32); 		  
		  
		  m_KeyEnd=FALSE;
		  TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );
		  return;

	  }
  }

  if(nKey==KEY_SPACE){
	  g_bCursor2Byte = (text[g_CursorPos]<0x80) ? FALSE : TRUE;

	  if(g_bCursor2Byte){
		  text[g_CursorPos+0] = 0x20;
		  text[g_CursorPos+1] = 0x20;
		  g_CursorPos+=2;
	  }else{
		  text[g_CursorPos+0] = 0x20;
		  g_CursorPos+=1;
	  }
	  g_bCursor2Byte = (text[g_CursorPos]<0x80) ? FALSE : TRUE;
	  m_nCharStatus[0] = 0;

	  MatrixSetFrame(m_nSelPort, text,colr,32);

	  m_KeyEnd=FALSE;
	  TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );
	  return;
  }


  if(m_nKeyKind == CHAR_STATE_ENG){
	  int nCode=nKey;
	  
	  switch(nKey){
	  case KEY_NUM1:  // ㄱ .QZ 1
		  if(m_nCharStatus[0]==0)nCode = '.';
		  if(m_nCharStatus[0]==1)nCode = 'Q';
		  if(m_nCharStatus[0]==2)nCode = 'Z'; 
		  break; 
	  case KEY_NUM2: // ㄴ ABC 2
		  if(m_nCharStatus[0]==0)nCode = 'A'; 
		  if(m_nCharStatus[0]==1)nCode = 'B'; 
		  if(m_nCharStatus[0]==2)nCode = 'C'; 
		  break;		  
	  case KEY_NUM3: // ㄷ DEF 3 
		  if(m_nCharStatus[0]==0)nCode = 'D'; 
		  if(m_nCharStatus[0]==1)nCode = 'E'; 
		  if(m_nCharStatus[0]==2)nCode = 'F'; 
		  break; 
	  case KEY_NUM4: // ㅂ GHI 4 
		  if(m_nCharStatus[0]==0)nCode = 'G'; 
		  if(m_nCharStatus[0]==1)nCode = 'H'; 
		  if(m_nCharStatus[0]==2)nCode = 'I'; 
		  break; 
	  case KEY_NUM5: // ㅅ JKL 5 
		  if(m_nCharStatus[0]==0)nCode = 'J'; 
		  if(m_nCharStatus[0]==1)nCode = 'K'; 
		  if(m_nCharStatus[0]==2)nCode = 'L'; 
		  break; 
	  case KEY_NUM6: // ㅈ MNO 6 
		  if(m_nCharStatus[0]==0)nCode = 'M'; 
		  if(m_nCharStatus[0]==1)nCode = 'N'; 
		  if(m_nCharStatus[0]==2)nCode = 'O'; 
		  break; 
	  case KEY_NUM7: // ㅇ PRS 7 
		  if(m_nCharStatus[0]==0)nCode = 'P'; 
		  if(m_nCharStatus[0]==1)nCode = 'R'; 
		  if(m_nCharStatus[0]==2)nCode = 'S'; 
		  break; 
	  case KEY_NUM8: // ㅏ TUV 8 
		  if(m_nCharStatus[0]==0)nCode = 'T'; 
		  if(m_nCharStatus[0]==1)nCode = 'U'; 
		  if(m_nCharStatus[0]==2)nCode = 'V'; 
		  break; 
	  case KEY_NUM9: // ㅓ WXY 9 
		  if(m_nCharStatus[0]==0)nCode = 'W'; 
		  if(m_nCharStatus[0]==1)nCode = 'X'; 
		  if(m_nCharStatus[0]==2)nCode = 'Y'; 
		  break; 
	  case KEY_STAR: // ㅗ /{} * 
		  if(m_nCharStatus[0]==0)nCode = '/'; 
		  if(m_nCharStatus[0]==1)nCode = '{'; 
		  if(m_nCharStatus[0]==2)nCode = '}'; 
		  break; 
	  case KEY_NUM0: // ㅜ %^? 0 
		  if(m_nCharStatus[0]==0)nCode = '%'; 
		  if(m_nCharStatus[0]==1)nCode = '^'; 
		  if(m_nCharStatus[0]==2)nCode = '?'; 
		  break; 
	  case KEY_HESI: // ㅣ @<> # 
		  if(m_nCharStatus[0]==0)nCode = '@'; 
		  if(m_nCharStatus[0]==1)nCode = '<'; 
		  if(m_nCharStatus[0]==2)nCode = '>'; 
		  break; 
	  } 	  
	  
	       if(m_nCharStatus[0]==0)m_nCharStatus[0]=1;
	  else if(m_nCharStatus[0]==1)m_nCharStatus[0]=2;
	  else if(m_nCharStatus[0]==2)m_nCharStatus[0]=0;
	  
	  text[g_CursorPos] = (u8)nCode;
	  
	  g_bCursor2Byte = (text[g_CursorPos]<0x80) ? FALSE : TRUE;
	  
	  MatrixSetFrame(m_nSelPort, text,colr,32); 	  

	  m_KeyEnd=FALSE;
	  TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );
	  return; 
  }

  if(m_nKeyKind == CHAR_STATE_eng){
	  int nCode=nKey;
	  
	  switch(nKey){
	  case KEY_NUM1:  // ㄱ .QZ 1
		  if(m_nCharStatus[0]==0)nCode = '.';
		  if(m_nCharStatus[0]==1)nCode = 'q';
		  if(m_nCharStatus[0]==2)nCode = 'z'; 
		  break; 
	  case KEY_NUM2: // ㄴ ABC 2
		  if(m_nCharStatus[0]==0)nCode = 'a'; 
		  if(m_nCharStatus[0]==1)nCode = 'b'; 
		  if(m_nCharStatus[0]==2)nCode = 'c'; 
		  break;		  
	  case KEY_NUM3: // ㄷ DEF 3 
		  if(m_nCharStatus[0]==0)nCode = 'd'; 
		  if(m_nCharStatus[0]==1)nCode = 'e'; 
		  if(m_nCharStatus[0]==2)nCode = 'f'; 
		  break; 
	  case KEY_NUM4: // ㅂ GHI 4 
		  if(m_nCharStatus[0]==0)nCode = 'g'; 
		  if(m_nCharStatus[0]==1)nCode = 'h'; 
		  if(m_nCharStatus[0]==2)nCode = 'i'; 
		  break; 
	  case KEY_NUM5: // ㅅ JKL 5 
		  if(m_nCharStatus[0]==0)nCode = 'j'; 
		  if(m_nCharStatus[0]==1)nCode = 'k'; 
		  if(m_nCharStatus[0]==2)nCode = 'l'; 
		  break; 
	  case KEY_NUM6: // ㅈ MNO 6 
		  if(m_nCharStatus[0]==0)nCode = 'm'; 
		  if(m_nCharStatus[0]==1)nCode = 'n'; 
		  if(m_nCharStatus[0]==2)nCode = 'o'; 
		  break; 
	  case KEY_NUM7: // ㅇ PRS 7 
		  if(m_nCharStatus[0]==0)nCode = 'p'; 
		  if(m_nCharStatus[0]==1)nCode = 'r'; 
		  if(m_nCharStatus[0]==2)nCode = 's'; 
		  break; 
	  case KEY_NUM8: // ㅏ TUV 8 
		  if(m_nCharStatus[0]==0)nCode = 't'; 
		  if(m_nCharStatus[0]==1)nCode = 'u'; 
		  if(m_nCharStatus[0]==2)nCode = 'v'; 
		  break; 
	  case KEY_NUM9: // ㅓ WXY 9 
		  if(m_nCharStatus[0]==0)nCode = 'w'; 
		  if(m_nCharStatus[0]==1)nCode = 'x'; 
		  if(m_nCharStatus[0]==2)nCode = 'y'; 
		  break; 
	  case KEY_STAR: // ㅗ /{} * 
		  if(m_nCharStatus[0]==0)nCode = '/'; 
		  if(m_nCharStatus[0]==1)nCode = '{'; 
		  if(m_nCharStatus[0]==2)nCode = '}'; 
		  break; 
	  case KEY_NUM0: // ㅜ %^? 0 
		  if(m_nCharStatus[0]==0)nCode = '%'; 
		  if(m_nCharStatus[0]==1)nCode = '^'; 
		  if(m_nCharStatus[0]==2)nCode = '?'; 
		  break; 
	  case KEY_HESI: // ㅣ @<> # 
		  if(m_nCharStatus[0]==0)nCode = '@'; 
		  if(m_nCharStatus[0]==1)nCode = '<'; 
		  if(m_nCharStatus[0]==2)nCode = '>'; 
		  break; 
	  } 	  
	  
		   if(m_nCharStatus[0]==0)m_nCharStatus[0]=1;
	  else if(m_nCharStatus[0]==1)m_nCharStatus[0]=2;
	  else if(m_nCharStatus[0]==2)m_nCharStatus[0]=0;

	  text[g_CursorPos] = (u8)nCode;
	  
	  g_bCursor2Byte = (text[g_CursorPos]<0x80) ? FALSE : TRUE;
	  
	  MatrixSetFrame(m_nSelPort, text,colr,32); 	  

	  m_KeyEnd=FALSE;
	  TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );
	  return;	  
  }

  if(m_nKeyKind == CHAR_STATE_NUM){
	  int nCode=nKey;
	  
	  switch(nKey){
	  case KEY_NUM1: nCode = 0x31; break; // ㄱ .QZ 1
	  case KEY_NUM2: nCode = 0x32; break; // ㄴ ABC 2
	  case KEY_NUM3: nCode = 0x33; break; // ㄷ DEF 3
	  case KEY_NUM4: nCode = 0x34; break; // ㅂ GHI 4
	  case KEY_NUM5: nCode = 0x35; break; // ㅅ JKL 5
	  case KEY_NUM6: nCode = 0x36; break; // ㅈ MNO 6
	  case KEY_NUM7: nCode = 0x37; break; // ㅇ PRS 7
	  case KEY_NUM8: nCode = 0x38; break; // ㅏ TUV 8
	  case KEY_NUM9: nCode = 0x39; break; // ㅓ WXY 9
	  case KEY_STAR: nCode = 0x2a; break; // ㅗ /{} *
	  case KEY_NUM0: nCode = 0x30; break; // ㅜ %^? 0
	  case KEY_HESI: nCode = 0x23; break; // ㅣ @<> #
	  } 	  

	  text[g_CursorPos] = (u8)nCode;
	  if(g_CursorPos<(DOT_MAX_MOD*2-1))
	  g_CursorPos+=1;
	  
	  g_bCursor2Byte = (text[g_CursorPos]<0x80) ? FALSE : TRUE;

	  MatrixSetFrame(m_nSelPort, text,colr,32); 	  

	  m_KeyEnd=FALSE;
	  TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );
	  return;
  }

  if(m_nKeyKind == CHAR_STATE_HAN){
	  int nCode=nKey;

	  switch(nKey){
	  case KEY_NUM1: nCode =  2; break; // ㄱ .QZ 1
	  case KEY_NUM2: nCode =  4; break; // ㄴ ABC 2
	  case KEY_NUM3: nCode =  5; break; // ㄷ DEF 3
	  case KEY_NUM4: nCode =  9; break; // ㅂ GHI 4
	  case KEY_NUM5: nCode = 11; break; // ㅅ JKL 5
	  case KEY_NUM6: nCode = 14; break; // ㅈ MNO 6
	  case KEY_NUM7: nCode = 13; break; // ㅇ PRS 7
	  case KEY_NUM8: nCode = 23; break; // ㅏ TUV 8
	  case KEY_NUM9: nCode = 27; break; // ㅓ WXY 9
	  case KEY_STAR: nCode = 33; break; // ㅗ /{} *
	  case KEY_NUM0: nCode = 40; break; // ㅜ %^? 0
	  case KEY_HESI: nCode = 49; break; // ㅣ @<> #
	  }

	  if(nKey==KEY_SPACE || nKey==KEY_RIGHT)
		  m_nCharStatus[1] = HS_END_STATE;

	  u16 ingCode = CombineKey(nCode);

	  m_nCharStatus[0] = m_nCharStatus[1];
	  m_nCharStatus[1] = GetHanStatus();
	  u16 completeCode = GetCompleteHanCode();

	  // 다음 글씨인가 검사
	  if(completeCode && (m_nCharStatus[0]==HS_END_STATE||m_nCharStatus[0]==HS_END_EXCEPTION)&&
		  (m_nCharStatus[1]==HS_FIRST||m_nCharStatus[1]==HS_FIRST_V||m_nCharStatus[1]==HS_MIDDLE_STATE) ){
		  
		  text[g_CursorPos+0] = (u8)(completeCode>>8);
		  text[g_CursorPos+1] = (u8)completeCode;
		  
		  if(g_CursorPos<(DOT_MAX_MOD*2-2))
			g_CursorPos+=2; 		  

		  g_bCursor2Byte = TRUE;
	  } 	  
		  
	  text[g_CursorPos+0] = (u8)(ingCode>>8);
	  text[g_CursorPos+1] = (u8)ingCode;
	  
	  MatrixSetFrame(m_nSelPort, text,colr,32);

	  m_KeyEnd=FALSE;
	  TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );
	  return;
  }

  m_KeyEnd=FALSE;
  TimerSet(TIMER_KEY_DELAY, TIMER_KEY_DELAY_TIME ,TIMER_TYPE_ONESHOT, &TimerKeyDelay_CallBack );
}

  

