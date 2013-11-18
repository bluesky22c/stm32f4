/******************** (C) COPYRIGHT 2012 OP ***********************************
* File Name         : Matrix_proc.c
* Author             : bluesky22c
* Version            : V1.1.1
* Date                : 01/15/2012
* Description       : Matrix procedure which is DotMatrix Proc.
*******************************************************************************
*
*******************************************************************************/

/* Includes --------------------------------------------------------------------*/

#include "matrix_proc.h"
#include "mem_heap.h"
#include "gpio.h"
#include "common.h"
#include "Calltrace.h"

#include "font_ascii.h"
#include "font_num.h"
#include "font_etc.h"
#include "font_kor.h"

/********************************/
/*         DOT MATRIX                         */
/********************************/
#define KfontLen   32
#define EfontLen   16

#define CODE_NOT    0
#define CODE_NUM    1
#define CODE_ETC    2
#define CODE_KOR    3
#define CODE_ASCII  4

#define CLR_NONE    0
#define CLR_RED     1
#define CLR_GREEN   2
#define CLR_YELLOW  3
#define CLR_NOTUSE  9 

#define DOT_MAX_PORT 2
// PORT Enable
#define DOT_PORT_EN0  GpioC->Bit.b0
#define DOT_PORT_EN1  GpioC->Bit.b1
#define DOT_PORT_EN2  GpioC->Bit.b2
#define DOT_PORT_EN3  GpioC->Bit.b3

// PORT1
#define DOT_ADDR_P1   GPIOA->ODR        // A0=P4, A1=P5, A2=P6, A3=P7
#define DOT_LATCH_P1  GpioA->Bit.b15
#define DOT_DISP_P1   GpioA->Bit.b1
#define DOT_OE_P1     GpioA->Bit.b8

#define DOT_ROE_P1    GpioA->Bit.b7
#define DOT_GOE_P1    GpioA->Bit.b8

// PORT2
#define DOT_ADDR_P2   GPIOB->ODR        // A0=P4, A1=P5, A2=P6, A3=P7
#define DOT_LATCH_P2  GpioB->Bit.b15
#define DOT_DISP_P2   GpioB->Bit.b1
#define DOT_OE_P2     GpioB->Bit.b10

#define DOT_ROE_P2    GpioA->Bit.b7
#define DOT_GOE_P2    GpioA->Bit.b10

//-----------------------------
#ifndef DUTY_4

#ifdef OLD_TYPE
// old version
#define DOT_RD_11 GPIO_Pin_11
#define DOT_RD_12 GPIO_Pin_12
#define DOT_GD_12 GPIO_Pin_14
#define DOT_GD_11 GPIO_Pin_13

#define DOT_RD_21 GPIO_Pin_11
#define DOT_RD_22 GPIO_Pin_12
#define DOT_GD_22 GPIO_Pin_14
#define DOT_GD_21 GPIO_Pin_13

#else
// new version
#define DOT_RD_12 GPIO_Pin_11
#define DOT_RD_11 GPIO_Pin_12
#define DOT_GD_11 GPIO_Pin_14
#define DOT_GD_12 GPIO_Pin_13

#define DOT_RD_22 GPIO_Pin_11
#define DOT_RD_21 GPIO_Pin_12
#define DOT_GD_21 GPIO_Pin_14
#define DOT_GD_22 GPIO_Pin_13

#endif

#endif

#ifdef DUTY_4
#define DOT_RD_11 GPIO_Pin_11
#define DOT_RD_12 GPIO_Pin_12
#define DOT_RD_13 GPIO_Pin_13
#define DOT_RD_14 GPIO_Pin_14

#define DOT_RD_21 GPIO_Pin_11
#define DOT_RD_22 GPIO_Pin_12
#define DOT_RD_23 GPIO_Pin_13
#define DOT_RD_24 GPIO_Pin_14

#define DOT_GD_11 GPIO_Pin_15
#define DOT_GD_12 GPIO_Pin_14
#define DOT_GD_13 GPIO_Pin_13
#define DOT_GD_14 GPIO_Pin_12

#define DOT_GD_21 GPIO_Pin_5
#define DOT_GD_22 GPIO_Pin_4
#define DOT_GD_23 GPIO_Pin_3
#define DOT_GD_24 GPIO_Pin_2
#endif


#define DOT_CLK  GPIO_Pin_0

/* Private define --------------------------------------*/

u8* g_FrameBuffer[4];
u8 g_FrameText[4][36];
u8 g_FrameColr[4][36];
static int g_Matrixline[4];
int g_xMove[4];
DotEffect_t g_DotEffect[4];
bool g_bEffectFlag[4]; // In=True, Out=False
u8 g_CursorFlag[4];	 // 0 = cursor off, 1 = cursor on

u8 g_CursorPos  = 0; // 8bit
bool g_bCursor2Byte = FALSE; 

u8 g_PortFlag[2];
u8 g_fontIdx;
u8 g_Buzzer;
u8 g_Brightness;
u8 g_PrevCmd;

bool g_bTimeDotDsp = TRUE;

/* extern --------------------------------------------*/
extern mem_heap_type    g_SystemHeap;

int NMtoIndex(u16 code)
{
	u8 high,low;
	u16 idx;
	
	high = (code>>8)&0xff;
	low  =  code    &0xff;
	if ((code>=0xa3b0)&&(code<=0xa3b9)){
		idx=(high-0xa3)+low-0xb0;
		return (idx*KfontLen);
	} else return -1;
}

int ETCtoIndex(u16 code)
{
	u8 high,low;
	u16 idx=0xffff;
	
	high = (code>>8)&0xff;
	low  =  code    &0xff;
	if ((code>=0xa1dc)&&(code<=0xa2ba)){
		if(code==0xa1dc)idx=0;
		if(code==0xa1e1)idx=1;
		if(code==0xa1e6)idx=2;
		if(code==0xa1e7)idx=3;
		if(code==0xa1dc)idx=4;
		if(code==0xa2b8)idx=5;
		if(code==0xa2ba)idx=6;

		if(idx!=0xffff)
		    return (idx*KfontLen);
		else
			return -1;


	} else return -1;
}

int KStoIndex(u16 code)
{
	u8 high,low;
	u16 idx;
	
	high = (code>>8)&0xff;
	low  =  code    &0xff;
	if ((code>=0xb0a1)&&(low<=0xfe)){
		idx=(high-0xb0)*94+low-0xa1;
		
		return (idx*KfontLen+10*KfontLen);
	} else return -1;
}

int KSSMtoIndex(u16 code)
{
	u8 high,low;
	u16 index;
	
	high = (code>>8)&0xff;
	low  =  code    &0xff;
	if ((code>=0x8861)&&(low<=0xd3bd)&&low!=0x20&&low!=0x00){
		index = (high - 0xd3) + low - 0x61;
	
		return index;
	} else return -1;
}

int ASCIItoIndex(u8 code)
{
	u16 idx;
	
	if ((code<0xb0)){
		idx=code-0x20;

		if(idx>95)
			return -1;
		else
		    return (idx*EfontLen);
	} else return -1;

}

/*******************************************************************************
* Function Name  : MatrixConvKssm.
* Description    : Set data to g_FrameBuffer.
* Input          : KS -> KSSM
* Output         : None.
* Return         : None.
*******************************************************************************/
void MatrixConvKssm(u8* text, int len)
{
	int wCode;
	for(u8 i=0; i<len;i++){
		
		if(text[i]<0x80)
  			continue;

        wCode = ( *(text+i)<<8) + ( *(text+i+1)&0xff );	
		
		// Check etc Code
		if(0xa1dc<=wCode && wCode<=0xa2ba){
			i++;
			continue;
		}
		
		wCode=ConvKSSMCode(wCode);
		
		if(wCode!=-1){
			*(text+i+0) = wCode>>8;
			*(text+i+1) = wCode;
		}
		i++;
	}
}

int GetIntToStrEx(u8 cValue, char* buf)
{
	int ret=1;

	*buf = cValue;

	switch(cValue)
	{
	case '0': sprintf(buf,"０"); ret=2; break;
	case '1': sprintf(buf,"１"); ret=2; break;
	case '2': sprintf(buf,"２"); ret=2; break;
	case '3': sprintf(buf,"３"); ret=2; break;
	case '4': sprintf(buf,"４"); ret=2; break;
	case '5': sprintf(buf,"５"); ret=2; break;
	case '6': sprintf(buf,"６"); ret=2; break;
	case '7': sprintf(buf,"７"); ret=2; break;
	case '8': sprintf(buf,"８"); ret=2; break;
	case '9': sprintf(buf,"９"); ret=2; break;
	}
	
	return ret;
}

/*******************************************************************************
* Function Name  : GetColor.
* Description    : 
* Input          : Data.
* Output         : None.
* Return         : None.
*******************************************************************************/
u8 GetColor(u8 data, u8* pclr, u8 pos)
{
	u8 clrText = ( *(pclr+pos) >>6 & 0x3);
	u8 clrBack = ( *(pclr+pos) >>4 & 0x3);

	if(data!=0) 	
		return clrText;
	else
		return clrBack;
}

/*******************************************************************************
* Function Name  : SetDotColorPort1. 
* Description    : 
* Input          : Data.
* Output         : None.
* Return         : None.
*******************************************************************************/
void SetDotColorPort1(u8 data_1, u8 data_2, u8 data_3, u8 data_4)
{	
    if(data_1 == CLR_NONE)
   	{
		GPIOA->BRR = DOT_GD_11;
		GPIOA->BRR = DOT_RD_11;
   	}	
    else if(data_1 == CLR_RED)
	{
		GPIOA->BRR  = DOT_GD_11;
		GPIOA->BSRR = DOT_RD_11;
   	}
    else if(data_1 == CLR_GREEN)
	{
		GPIOA->BSRR = DOT_GD_11;
		GPIOA->BRR  = DOT_RD_11;
   	}	
    else if(data_1 == CLR_YELLOW)
	{
		GPIOA->BSRR = DOT_GD_11;
		GPIOA->BSRR = DOT_RD_11;
   	}	

	
	if(data_2==CLR_NOTUSE)
		return;
	
	if(data_2 == CLR_NONE)
	{
		GPIOA->BRR = DOT_GD_12;
		GPIOA->BRR = DOT_RD_12;
	}
	else if(data_2 == CLR_RED)
	{
		GPIOA->BRR  = DOT_GD_12;
		GPIOA->BSRR = DOT_RD_12;
	}
	else if(data_2 == CLR_GREEN)
	{
		GPIOA->BSRR = DOT_GD_12;
		GPIOA->BRR  = DOT_RD_12;
	}	
	else if(data_2 == CLR_YELLOW)
	{
		GPIOA->BSRR = DOT_GD_12;
		GPIOA->BSRR = DOT_RD_12;	
	}

#ifdef DUTY_4

	if(data_3==CLR_NOTUSE)
		return;

	if(data_3 == CLR_NONE)
	{
		GPIOA->BRR = DOT_GD_13;
		GPIOA->BRR = DOT_RD_13;
	}
	else if(data_3 == CLR_RED)
	{
		GPIOA->BRR	= DOT_GD_13;
		GPIOA->BSRR = DOT_RD_13;
	}
	else if(data_3 == CLR_GREEN)
	{
		GPIOA->BSRR = DOT_GD_13;
		GPIOA->BRR	= DOT_RD_13;
	}	
	else if(data_3 == CLR_YELLOW)
	{
		GPIOA->BSRR = DOT_GD_13;
		GPIOA->BSRR = DOT_RD_13;	
	}
	
	if(data_4==CLR_NOTUSE)
		return;

	if(data_4 == CLR_NONE)
	{
		GPIOA->BRR = DOT_GD_14;
		GPIOA->BRR = DOT_RD_14;
	}
	else if(data_4 == CLR_RED)
	{
		GPIOA->BRR	= DOT_GD_14;
		GPIOA->BSRR = DOT_RD_14;
	}
	else if(data_4 == CLR_GREEN)
	{
		GPIOA->BSRR = DOT_GD_14;
		GPIOA->BRR	= DOT_RD_14;
	}	
	else if(data_4 == CLR_YELLOW)
	{
		GPIOA->BSRR = DOT_GD_14;
		GPIOA->BSRR = DOT_RD_14;	
	}
	
#endif
	
}

/*******************************************************************************
* Function Name  : SetDotColorPort2.
* Description    : 
* Input          : Data.
* Output         : None.
* Return         : None.
*******************************************************************************/
void SetDotColorPort2(u8 data_1, u8 data_2, u8 data_3, u8 data_4)
{	   
	if(data_1 == CLR_NONE)
	{
		GPIOB->BRR = DOT_GD_21;
		GPIOB->BRR = DOT_RD_21;
	}	
	else if(data_1 == CLR_RED)
	{
		GPIOB->BRR	= DOT_GD_21;
		GPIOB->BSRR = DOT_RD_21;
	}
	else if(data_1 == CLR_GREEN)
	{
		GPIOB->BSRR = DOT_GD_21;
		GPIOB->BRR	= DOT_RD_21;
	}	
	else if(data_1 == CLR_YELLOW)
	{
		GPIOB->BSRR = DOT_GD_21;
		GPIOB->BSRR = DOT_RD_21;
	}		
		
	if(data_2==CLR_NOTUSE)
		return;
	
	if(data_2 == CLR_NONE)
	{
		GPIOB->BRR = DOT_GD_22;
		GPIOB->BRR = DOT_RD_22;
	}
	else if(data_2 == CLR_RED)
	{
		GPIOB->BRR	= DOT_GD_22;
		GPIOB->BSRR = DOT_RD_22;
	}
	else if(data_2 == CLR_GREEN)
	{
		GPIOB->BSRR = DOT_GD_22;
		GPIOB->BRR	= DOT_RD_22;
	}	
	else if(data_2 == CLR_YELLOW)
	{
		GPIOB->BSRR = DOT_GD_22;
		GPIOB->BSRR = DOT_RD_22;	
	}		

#ifdef DUTY_4

	if(data_3==CLR_NOTUSE)
		return;

	if(data_3 == CLR_NONE)
	{
		GPIOA->BRR = DOT_GD_23;
		GPIOA->BRR = DOT_RD_23;
	}
	else if(data_3 == CLR_RED)
	{
		GPIOA->BRR	= DOT_GD_23;
		GPIOA->BSRR = DOT_RD_23;
	}
	else if(data_3 == CLR_GREEN)
	{
		GPIOA->BSRR = DOT_GD_23;
		GPIOA->BRR	= DOT_RD_23;
	}	
	else if(data_3 == CLR_YELLOW)
	{
		GPIOA->BSRR = DOT_GD_23;
		GPIOA->BSRR = DOT_RD_23;	
	}
	
	if(data_4==CLR_NOTUSE)
		return;

	if(data_4 == CLR_NONE)
	{
		GPIOA->BRR = DOT_GD_24;
		GPIOA->BRR = DOT_RD_24;
	}
	else if(data_4 == CLR_RED)
	{
		GPIOA->BRR	= DOT_GD_24;
		GPIOA->BSRR = DOT_RD_24;
	}
	else if(data_4 == CLR_GREEN)
	{
		GPIOA->BSRR = DOT_GD_24;
		GPIOA->BRR	= DOT_RD_24;
	}	
	else if(data_4 == CLR_YELLOW)
	{
		GPIOA->BSRR = DOT_GD_24;
		GPIOA->BSRR = DOT_RD_24;	
	}	
#endif
	
}

/*******************************************************************************
* Function Name  : SaveToFlash.
* Description  : 
* Input           : aryTxtIdx = 0~23     24 = Display Text
* Output         : None.
* Return         : None.
*******************************************************************************/
bool SaveToFlash(u8 aryTxtIdx)
{
    u32 FlashTextAddr = FLASH_TEXT_S;
    u32 FlashColrAddr = FLASH_COLR_S;
	u32 EraseCounter = 0x0;
	FLASH_Status FLASHStatus = FLASH_COMPLETE;
	u32 NbrOfPage = 0;
	s32 j;
	u32 size = 36;
	u32 Data;

	u8 txt[25][40]={0,};
	u8 clr[25][40]={0,};

    // 1024/40=25
    if(aryTxtIdx>=25)
		return FALSE;

	// Read data all
	for(u8 Idx=0;Idx<25;Idx++){
		for(u8 i=0;i<40;i+=4){

			if(Idx==aryTxtIdx){
				*((u32*)txt[Idx]+i/4) = *((u32*)g_FrameText[0]+i/4);
				*((u32*)clr[Idx]+i/4) = *((u32*)g_FrameColr[0]+i/4);
			}else{
				*((u32*)txt[Idx]+i/4) = ( *(vu32*)(FLASH_TEXT_S+i+40*Idx) );
				*((u32*)clr[Idx]+i/4) = ( *(vu32*)(FLASH_COLR_S+i+40*Idx) );
			}
		}	
	}
	
    FLASH_Unlock();

	//-------- Erase
//	NbrOfPage = (FLASH_COLR_E-FLASH_TEXT_S)/PAGE_SIZE;
	NbrOfPage = 1;

	// Clear All pending flags
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	
	// Erase the FLASH pages 
	for (EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
	{
	  FLASHStatus = FLASH_ErasePage(FLASH_TEXT_S + (PAGE_SIZE * EraseCounter));

	  if (FLASHStatus!=FLASH_COMPLETE)
	  {
		  // error
		  CallTrace(TRACE_APP_LOW, "erase error d=%x\n\r", EraseCounter);			  
		  return FALSE;
	  }
	}

    //-------- write
	FLASHStatus = FLASH_COMPLETE;

	Data = (u32)g_fontIdx&0xFFFFFFFF;	
    FLASHStatus = FLASH_ProgramWord(FLASH_TEXT_S+40*0+36, Data);

	Data = (u32)aryTxtIdx&0xFFFFFFFF;	
    FLASHStatus = FLASH_ProgramWord(FLASH_COLR_S+40*0+36, Data);

	Data = (u32)g_Brightness&0xFFFFFFFF;	
    FLASHStatus = FLASH_ProgramWord(FLASH_TEXT_S+40*1+36, Data);

    for(aryTxtIdx=0;aryTxtIdx<25;aryTxtIdx++){	
		
		FlashTextAddr = FLASH_TEXT_S+40*aryTxtIdx;
		FlashColrAddr = FLASH_COLR_S+40*aryTxtIdx;
		for (j = 0;(j < size) && (FlashTextAddr <  FLASH_TEXT_S+40*aryTxtIdx+size);j += 4)
		{
		    // text
		    Data = *((u32*)txt[aryTxtIdx]+j/4);
			FLASHStatus = FLASH_ProgramWord(FlashTextAddr, Data);

			if (*(vu32*)FlashTextAddr != Data)
			{
			  // error
			  CallTrace(TRACE_APP_LOW, "text write error s=%x a=%x d=%x\n\r", FLASHStatus,FlashTextAddr,Data);   		  
			  return FALSE;
			}

	        TimerProc();		

			// color
		    Data = *((u32*)clr[aryTxtIdx]+j/4);
			FLASH_ProgramWord(FlashColrAddr, Data);
			
			if (*(vu32*)FlashColrAddr != Data)
			{
			  // error
			  CallTrace(TRACE_APP_LOW, "colr write error a=%x d=%x\n\r", FlashColrAddr,Data);   		  
			  return FALSE;
			}

	        TimerProc();		
			
			FlashTextAddr += 4;
			FlashColrAddr += 4;
			
		}	
	
    }
	
    FLASH_Lock();

//    CallTrace(TRACE_APP_LOW, "PWM Write=%d \n\r",g_Brightness);

    return TRUE;
	
}


/*******************************************************************************
* Function Name  : LoadFromFlash.
* Description  : 
* Input           : aryTxtIdx = 0 ~ 24
* Output         : txt,clr.
* Return         : result.
*******************************************************************************/
bool LoadFromFlash(u8 aryTxtIdx,u8* txt, u8* clr)
{
	// 저장된거 불러온다.
	if(aryTxtIdx==0xff){
/*		
		aryTxtIdx = ( *(vu32*)(FLASH_COLR_S+36) );
		if(aryTxtIdx>=25)
			aryTxtIdx=0;
*/		
       aryTxtIdx=24;
	}
	
//	sprintf((char*)txt, "가나다라바사아자차카타파하０１２"); // 32 byte
//	sprintf((char*)txt, "０:１가나다라바사아자차카타파００"); // 33 byte
//	sprintf((char*)txt, "ABCDEFGHIJKLMNOPQRSTUVWXYZ１１１"); // 32 byte
//	sprintf((char*)txt, "■→←●◀▶1234567890abcdefgh２"); // 32 byte
//	sprintf((char*)txt, "０１２３４５６７８９０１２３４５"); // 32 byte	

    g_fontIdx    = ( *(vu32*)(FLASH_TEXT_S+40*0+36) );
	g_Brightness = ( *(vu32*)(FLASH_TEXT_S+40*1+36) );

	if(g_Brightness>9)g_Brightness=9;
	if(g_fontIdx>2)g_fontIdx=0;
  
	for(u8 i=0;i<40;i+=4){
		*((u32*)txt+i/4) = ( *(vu32*)(FLASH_TEXT_S+i+40*aryTxtIdx) );
		*((u32*)clr+i/4) = ( *(vu32*)(FLASH_COLR_S+i+40*aryTxtIdx) );
	}	

    // if first run
	if(txt[0]==0xff&&txt[1]==0xff&&txt[2]==0xff&&txt[3]==0xff){
        if(aryTxtIdx<24)
		SPRINTF(txt, "%02dABCD가나다라바 No [%02d]가나다%02d",aryTxtIdx+1,aryTxtIdx+1,aryTxtIdx+1); // 32 byte
        else
//		SPRINTF(txt, "%02d나다라마바사아자차카파하마바사",aryTxtIdx+1); // 32 byte
  		SPRINTF(txt, "%02dABCD가나다라바가나다라가나◀▶",aryTxtIdx+1); // 32 byte

//		SPRINTF(txt, "■■■■■■■■０１２３４５６７８"); // 32 byte
//		SPRINTF(txt, "■■■■■■■■■■■■■■■■"); // 32 byte

		MatrixConvKssm(txt, 40);
		memset(clr, (CLR_YELLOW<<6|CLR_NONE<<4), 40);
	}	

    return TRUE;
	
}

/*******************************************************************************
* Function Name  : MatrixProc_Init.
* Description    : init
* Input          : Data.
* Output         : None.
* Return         : None.
*******************************************************************************/
void MatrixProc_Init(void)
{
	DOT_PORT_EN0  = LOW;
	DOT_PORT_EN1  = LOW;
	DOT_PORT_EN2  = LOW;
	DOT_PORT_EN3  = LOW;

	DOT_ADDR_P1   = 0x0;
	DOT_LATCH_P1  = HIGH;
	DOT_DISP_P1   = LOW;
	DOT_OE_P1     = LOW;

    GPIOA->BSRR	  = DOT_CLK;

    GPIOA->BRR	  = DOT_GD_11;
    GPIOA->BRR	  = DOT_GD_12;
    GPIOA->BRR	  = DOT_RD_11;
    GPIOA->BRR	  = DOT_RD_12;
	
	DOT_ADDR_P2   = 0x0;
	DOT_LATCH_P2  = HIGH;
	DOT_DISP_P2   = LOW;
	DOT_OE_P2     = LOW;

    GPIOB->BSRR	  = DOT_CLK;

    GPIOB->BRR	  = DOT_GD_11;
    GPIOB->BRR	  = DOT_GD_12;
    GPIOB->BRR	  = DOT_RD_11;
    GPIOB->BRR	  = DOT_RD_12;

	for(int m=0;m<4;m++){
		g_FrameBuffer[m] = Malloc(&g_SystemHeap, DOT_MAX_X*DOT_MAX_Y);
	 	
		if(g_FrameBuffer[m]==NULL){
	        while(1){
			  GpioC->Bit.b8=!GpioC->Bit.b8; // led 8 toggle 
	          Delay(20); // 24uSec*20
			}
		}
		
		memset(g_FrameBuffer[m],0, DOT_MAX_X*DOT_MAX_Y );
		g_Matrixline[m]= 0;		
		g_xMove[m]     = 0;
		g_bEffectFlag[m]=TRUE;

		g_DotEffect[m].inType  =0;
		g_DotEffect[m].delay   =0;
		g_DotEffect[m].outType =0;
		g_DotEffect[m].speed   =1;
		g_DotEffect[m].timecnt =0;
		g_DotEffect[m].delaycnt=0;
			
		g_CursorFlag[m] = 0;
		g_CursorPos 	= 0; // 8bit 기준
		g_fontIdx       = 0;
		
	}

	//g_xMove[0] = 7*16;  // time

	g_PortFlag[0] = 0;
	g_PortFlag[1] = 0;
	g_Buzzer  = 0;
	g_PrevCmd = 0;

    memset(g_FrameText, 0, sizeof(g_FrameText) );
    memset(g_FrameColr, 0, sizeof(g_FrameColr) );


	u8 txt[40]={0,};
	u8 clr[40]={0,};
	
	LoadFromFlash(0xff,txt,clr);

	for(u8 m=0;m<4;m++){
		MatrixSetFrame(m, txt, clr, 32);				
	}	

#ifdef ROWCOL_1_8
    //---- port0 data -> port2 data
    //---- left <-> right
    u8 textCnv[40];
	u8 colrCnv[40];

	//left:16~23  right:24~31
    memcpy(textCnv+16,txt+24,16); memcpy(colrCnv+16,clr+24,16); // right -> left
    memcpy(textCnv+24,txt+16,16); memcpy(colrCnv+24,clr+16,16); // left -> right    
	MatrixSetFrame(2, textCnv,colrCnv,32);
#endif
		
}

/*******************************************************************************
* Function Name  : MatrixSetEffect.
* Description       : Period 100ms
*                         Driver B/D ver 2.6 Duty 1/8
*
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void MatrixSetEffect(u8 m)
{    			
    // delay time
    if(g_xMove[m]==0 && (g_DotEffect[m].timecnt <= g_DotEffect[m].delay*1000) ){
	   g_DotEffect[m].timecnt += 30; // 30ms
       return; 
    }

	if(g_DotEffect[m].timecnt > g_DotEffect[m].delay*1000)
	g_bEffectFlag[m]=!g_bEffectFlag[m];

    g_DotEffect[m].timecnt = 0;

    u8 eType=0;
	
    if(g_bEffectFlag[m])eType=g_DotEffect[m].inType;
	else                eType=g_DotEffect[m].outType;

    // none
	if(eType==0){
		g_xMove[m]=0;
//		g_xMove[m]=7*16;
	}
	
    //  from right
	if(eType==1){
		if(g_DotEffect[m].delaycnt>=g_DotEffect[m].speed)
		 g_xMove[m]++;
	}

    //  from left
	if(eType==2){
		if(g_DotEffect[m].delaycnt>=g_DotEffect[m].speed)
		 g_xMove[m]--;		 
	}

	if(eType==3){
	  g_xMove[m]=0;
	}

	if(eType==4){
	  g_xMove[m]=0;
	}

	if(eType==5){
	  g_xMove[m]=0;
	}

	if(eType==6){
	  g_xMove[m]=0;
	}
	
	if(g_xMove[m]<0 && eType!=0)
		g_xMove[m]=DOT_MAX_X-1;
	
	if(g_xMove[m]>=DOT_MAX_X)
		g_xMove[m]=0;	

	if(g_DotEffect[m].delaycnt>=g_DotEffect[m].speed){
		g_DotEffect[m].delaycnt=0;
	}	

    g_DotEffect[m].delaycnt++;
	
}


/*******************************************************************************
* Function Name  : MatrixSetFrame.
* Description    : Set data to FrameBuffer.
* Input          : len -> byte count
* Output         : None.
* Return         : None.
*******************************************************************************/
void MatrixSetFrame(u8 m, u8* text, u8* colr, int len)
{	
	u8 buf[2];
	u16 wData;
	int wKSSMCode;	
	u8 byData=0;
	int nLine=0;	
	int x=0, xStart=0;
	int nCode=0;         
	int nFontStartIdx;
	int ntextPos=0;
	
    int nSemiPos=0;
	int nOffset=0;

    u8 KoreanBuffer[33] = {0,};		// 32 byte Korean font buffer

	GpioB->Bit.b3 = g_Buzzer;
	
	//----------------------------------------------
	//  8x16 기준으로 한다.
	//----------------------------------------------
	while(TRUE){
		TimerProc(); // 

		if(ntextPos+1 > len)
			break; 

		buf[1]= *(text+ntextPos+0); 
		buf[0]= *(text+ntextPos+1);

		if(buf[1] < 0x80){  // ascii
 		   wData = (u16)buf[1];
		}else{
		   wData = (u16)((buf[1]<<8) + (buf[0]&0xff));
		}

		if(wData==0)
			wData=0x20;

		nCode=CODE_NOT;
		nFontStartIdx = -1;

		// 2byte Num_fontTable
		if(nCode==CODE_NOT){
			nFontStartIdx = NMtoIndex(wData);			
			if(nFontStartIdx!=-1)
				nCode=CODE_NUM;
		}

		// ETC_fontTable 2byte
		if(nCode==CODE_NOT){
			nFontStartIdx = ETCtoIndex(wData);
			if(nFontStartIdx!=-1)
				nCode=CODE_ETC;
		}
	
		// KSSM
		if(nCode==CODE_NOT){
			nFontStartIdx = KSSMtoIndex(wData);
			if(nFontStartIdx!=-1){
				GetKSSMData(g_fontIdx, wData, KoreanBuffer);
			    nCode=CODE_KOR;
			}
		}

		// KSSM 미완성글씨
		if(nCode==CODE_NOT){
			wKSSMCode = wData;

			if(0x8442<=wKSSMCode && wKSSMCode<=0xd041){
				GetKSSMData(g_fontIdx, wKSSMCode, KoreanBuffer);
				nCode=CODE_KOR;
			}			
		}


		// ASCII
		if(nCode==CODE_NOT && wData!=0x8441){
			nFontStartIdx = ASCIItoIndex(wData);
			if(nFontStartIdx!=-1)
				nCode=CODE_ASCII;
		}

		if(buf[1]==':' || buf[1]=='_'){
	        g_FrameText[m][ntextPos]=text[ntextPos];		

			if(buf[1]==':')
			nSemiPos=ntextPos;
			else
			nSemiPos=0;

			ntextPos+=1;
			nOffset-=8;
			continue;
		}

		TimerProc();

		// Check Data Change
		if( 
			g_FrameText[m][ntextPos]  == text[ntextPos]   && 
			g_FrameColr[m][ntextPos]  == colr[ntextPos]   && 
			g_FrameText[m][ntextPos+1]== text[ntextPos+1] && 
			g_FrameColr[m][ntextPos+1]== colr[ntextPos+1] && 
			(text[ntextPos]!=':' || text[ntextPos]!='_') 
			)
		{
			if(nCode==CODE_ASCII)
				ntextPos+=1;
			else
				ntextPos+=2;
			
			continue;
		}
		
	    GpioC->Bit.b9=!GpioC->Bit.b9;	  

		// Not Found
		if(nCode==CODE_NOT){

			// fill space
			if(buf[1] < 0x80){  // ascii
				nCode=CODE_ASCII;
				nFontStartIdx=0;
			}else{
				nCode=CODE_ETC;
				nFontStartIdx=0;
			}
		}

		xStart=ntextPos*8+nOffset; // 8bit 기준
		nLine=0;

		// 8x16
		if(nCode==CODE_ASCII){ 

			// 8x16
			for(int nFontIdx=nFontStartIdx;nFontIdx<(nFontStartIdx+EfontLen);nFontIdx++){			
				
				byData = ASCII_fontTable[nFontIdx];

				for( x=0;x<8;x++){
				*(g_FrameBuffer[m]+nLine*DOT_MAX_X+(xStart+x)) = GetColor((byData & 0x80),colr,ntextPos);
				byData = byData << 1;
				}

				g_FrameText[m][ntextPos]=text[ntextPos];
				g_FrameColr[m][ntextPos]=colr[ntextPos];
				
				nLine++;
				x=0;
			}
			
			ntextPos+=1;
		}


		// 16x16
		if(nCode==CODE_NUM){
	        s8 ofs = 0;

            // Time 00:00 
            // Time font idx = 1  left shift  1 dot
            if(len==33)
            {
				if(ntextPos==10) ofs = -1;

	            // Time font idx = 2 right shift 1 dot
	            if(ntextPos==13) ofs = 1;
            }
			
			for(int nFontIdx=nFontStartIdx;nFontIdx<(nFontStartIdx+KfontLen);nFontIdx++){			

                //if(m==0 && 8<=ntextPos && ntextPos<=16 && nSemiPos>0) 
				if(m==0 && 8<=ntextPos && ntextPos<=16) 
				 byData = NUM1_fontTable[nFontIdx]; // Time font 
				else					
				 byData = NUM2_fontTable[nFontIdx]; // Count font
				
				for( x=0;x<8;x++){
					*(g_FrameBuffer[m]+nLine*DOT_MAX_X+(xStart+x+ofs)) = GetColor((byData & 0x80),colr,ntextPos);
					byData = byData << 1;
				}
				TimerProc();

				g_FrameText[m][ntextPos]=text[ntextPos];
				g_FrameColr[m][ntextPos]=colr[ntextPos];
				
				nFontIdx++;					

                //if(m==0 && 8<=ntextPos && ntextPos<=16 && nSemiPos>0) 
				if(m==0 && 8<=ntextPos && ntextPos<=16) 
				 byData = NUM1_fontTable[nFontIdx]; // Time font
				else					
				 byData = NUM2_fontTable[nFontIdx]; // Count font

				for( x=8;x<16;x++){
					*(g_FrameBuffer[m]+nLine*DOT_MAX_X+(xStart+x+ofs)) = GetColor((byData & 0x80),colr,ntextPos);
					byData = byData << 1;
				}				
				TimerProc();

				g_FrameText[m][ntextPos+1]=text[ntextPos+1];
				g_FrameColr[m][ntextPos+1]=colr[ntextPos+1];
			
				nLine++;
				x=0;
			}

			ntextPos+=2;
		}

		// 16x16
		if(nCode==CODE_ETC){ 
			
			for(int nFontIdx=nFontStartIdx;nFontIdx<(nFontStartIdx+KfontLen);nFontIdx++){			
				byData = ETC_fontTable[nFontIdx];

				for( x=0;x<8;x++){
					*(g_FrameBuffer[m]+nLine*DOT_MAX_X+(xStart+x)) = GetColor((byData & 0x80),colr,ntextPos);
					byData = byData << 1;
				}

				g_FrameText[m][ntextPos]=text[ntextPos];
				g_FrameColr[m][ntextPos]=colr[ntextPos];

				
				nFontIdx++;					
				byData = ETC_fontTable[nFontIdx];
				
				for( x=8;x<16;x++){
					*(g_FrameBuffer[m]+nLine*DOT_MAX_X+(xStart+x)) = GetColor((byData & 0x80),colr,ntextPos);
					byData = byData << 1;
				}				

				g_FrameText[m][ntextPos+1]=text[ntextPos+1];
				g_FrameColr[m][ntextPos+1]=colr[ntextPos+1];
				
				nLine++;
				x=0;
			}

			ntextPos+=2;
		}

		// 16x16
		if(nCode==CODE_KOR){ 

            for( x=0;x<16;x++){
				byData = KoreanBuffer[x];
				for(nLine=0;nLine<8;nLine++){
				  *(g_FrameBuffer[m]+nLine*DOT_MAX_X+(xStart+x)) = GetColor((byData & 0x01),colr,ntextPos);
				  byData = byData >> 1;
				}
			}

			g_FrameText[m][ntextPos]  =text[ntextPos];
			g_FrameColr[m][ntextPos]  =colr[ntextPos];
			

            for( x=0;x<16;x++){
				byData = KoreanBuffer[x+16];
				for(nLine=8;nLine<16;nLine++){
					*(g_FrameBuffer[m]+nLine*DOT_MAX_X+(xStart+x)) = GetColor((byData & 0x01),colr,ntextPos);
					byData = byData >> 1;
				}
			}			

			g_FrameText[m][ntextPos+1]=text[ntextPos+1];
			g_FrameColr[m][ntextPos+1]=colr[ntextPos+1];

			ntextPos+=2;
		}	
		
	}
		
		
	// ':'
	u8 DotClr=CLR_RED;

	if(!g_bTimeDotDsp)
		DotClr=CLR_NONE;
		
	if(nSemiPos!=0){
		xStart=nSemiPos*8-1;
		nSemiPos=0;
	
		nLine = 5; 
		x=0;
		*(g_FrameBuffer[m]+(nLine+0)*DOT_MAX_X+(xStart+x)) = DotClr;
		*(g_FrameBuffer[m]+(nLine+1)*DOT_MAX_X+(xStart+x)) = DotClr;
		x=1;
		*(g_FrameBuffer[m]+(nLine+0)*DOT_MAX_X+(xStart+x)) = DotClr;
		*(g_FrameBuffer[m]+(nLine+1)*DOT_MAX_X+(xStart+x)) = DotClr;
	
		nLine = 10; 
		x=0;
		*(g_FrameBuffer[m]+(nLine+0)*DOT_MAX_X+(xStart+x)) = DotClr;
		*(g_FrameBuffer[m]+(nLine+1)*DOT_MAX_X+(xStart+x)) = DotClr;
		x=1;
		*(g_FrameBuffer[m]+(nLine+0)*DOT_MAX_X+(xStart+x)) = DotClr;
		*(g_FrameBuffer[m]+(nLine+1)*DOT_MAX_X+(xStart+x)) = DotClr;
	}
		
	
}



/*                OLD TYPE                                        NEW TYPE
--------------------------------- -------------------------                
        _________            __________      _________          __________
CLK                  |||||||                                       ||||||

DATA                |||||||                                      |||||||
                         ______                                       ______
OE      ________|         |_________        ________|         |_________

LATCH ______________|_________        ______________|_________

           ________________________       
DISP                                                     ________________________

PWM (99%)                                               (99%)    

*/

#ifdef DUTY_4
/*******************************************************************************
* Function Name  : DisplayMatrixPort1. 
* Description       : Driver B/D ver 2.6 Duty 1/4 
*
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void MatrixDisplayPort1_D4(int m)
{
    u32 x;
	u8 Data1, Data2, Data3, Data4;
	u16 xbuf=0;
	
	DOT_ROE_P1  = HIGH;
	DOT_GOE_P1  = HIGH;
    DOT_DISP_P1 = HIGH;
	
	int xStart= g_CursorPos*8;
	
	for(x=0; x<DOT_MAX_X; x++)
	{
		xbuf = x+g_xMove[m];
		
		if(xbuf>=DOT_MAX_X)
			xbuf-=DOT_MAX_X;
	
		Data1 = *(g_FrameBuffer[m]+(g_Matrixline[m]*2+0)*DOT_MAX_X+xbuf);
		Data2 = *(g_FrameBuffer[m]+(g_Matrixline[m]*2+1)*DOT_MAX_X+xbuf);
		Data3 = *(g_FrameBuffer[m]+(g_Matrixline[m]*2+2)*DOT_MAX_X+xbuf);
		Data4 = *(g_FrameBuffer[m]+(g_Matrixline[m]*2+3)*DOT_MAX_X+xbuf);
			
        // cursor
		if(g_CursorFlag[m]==1 && g_Matrixline[m]==3)
		{
			if(g_bCursor2Byte && (xStart<=x && x<(xStart+16) ) )
			Data4 = CLR_RED;

			else if(!g_bCursor2Byte && (xStart<=x && x<(xStart+8) ) )
			Data4 = CLR_RED;
		}	

	    if(g_xMove[m]!=0 && x>191)
		SetDotColorPort1(CLR_NONE, CLR_NONE, CLR_NONE, CLR_NONE); 	
		else
		SetDotColorPort1(Data1,Data2,Data3,Data4);		

		GPIOA->BRR  = DOT_CLK;
		GPIOA->BSRR = DOT_CLK;
		
	}

	DOT_ADDR_P1 = 0xffff & ( (DOT_ADDR_P1&~0x70)|(g_Matrixline[m]<<4) );
	
	for(int c=0;c<5;c++)
	DOT_LATCH_P1 = HIGH;
	DOT_LATCH_P1 = LOW;
	
    DOT_DISP_P1 = LOW; 
	DOT_ROE_P1  = LOW;
	DOT_GOE_P1  = LOW;
	
	g_Matrixline[m]++;
	
	if(g_Matrixline[m]>3)
		g_Matrixline[m]=0;
	
}

/*******************************************************************************
* Function Name  : DisplayMatrixPort2. 
* Description       : Driver B/D ver 2.6 Duty 1/4 
*
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void MatrixDisplayPort2_D4(int m)
{
    u32 x;
	u8 Data1, Data2, Data3, Data4;
	u16 xbuf=0;
	
    DOT_DISP_P2 = HIGH;
	DOT_ROE_P2  = HIGH;
	DOT_GOE_P2  = HIGH;
	
	int xStart= g_CursorPos*8;
	
	for(x=0; x<DOT_MAX_X; x++)
	{
		xbuf = x+g_xMove[m];
		
		if(xbuf>=DOT_MAX_X)
			xbuf-=DOT_MAX_X;
	
		Data1 = *(g_FrameBuffer[m]+(g_Matrixline[m]*2+0)*DOT_MAX_X+xbuf);
		Data2 = *(g_FrameBuffer[m]+(g_Matrixline[m]*2+1)*DOT_MAX_X+xbuf);
		Data3 = *(g_FrameBuffer[m]+(g_Matrixline[m]*2+2)*DOT_MAX_X+xbuf);
		Data4 = *(g_FrameBuffer[m]+(g_Matrixline[m]*2+3)*DOT_MAX_X+xbuf);

        // cursor
		if(g_CursorFlag[m]==1 && g_Matrixline[m]==3)
		{ 		
			if(g_bCursor2Byte && (xStart<=x && x<(xStart+16) ) )
			Data4 = CLR_RED;

			else if(!g_bCursor2Byte && (xStart<=x && x<(xStart+8) ) )
			Data4 = CLR_RED;		   
		}

		SetDotColorPort2(Data1, Data2, Data3, Data4);		

		GPIOB->BRR  = DOT_CLK;
		GPIOB->BSRR = DOT_CLK; 	
	}

    DOT_ADDR_P2 = 0xffff & ( (DOT_ADDR_P2&~0x70)|(g_Matrixline[m]<<4) );
	
	for(int c=0;c<5;c++)
	DOT_LATCH_P2 = HIGH;
	DOT_LATCH_P2 = LOW;
	
    DOT_DISP_P2 = LOW; 
	DOT_ROE_P2  = LOW;
	DOT_GOE_P2  = LOW;
	
	g_Matrixline[m]++;
	
	if(g_Matrixline[m]>3)
		g_Matrixline[m]=0;

	
}

#endif

#ifdef DUTY_8
/*******************************************************************************
* Function Name  : DisplayMatrixPort1. 
* Description       : Driver B/D ver 2.6 Duty 1/8 
*
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void MatrixDisplayPort1_D8(int m)
{
    u32 x;
	u8 DataH, DataL;
	u16 xbuf=0;

	if(m==0){
	DOT_PORT_EN1 = LOW;
	DOT_PORT_EN0 = HIGH;
	}
	if(m==1){
	DOT_PORT_EN0 = LOW;
	DOT_PORT_EN1 = HIGH;
	}
	
	DOT_OE_P1   = HIGH;
    DOT_DISP_P1 = HIGH;
	
	int xStart= g_CursorPos*8;
	
	for(x=0; x<DOT_MAX_X; x++)
	{
		xbuf = x+g_xMove[m];
		
		if(xbuf>=DOT_MAX_X)
			xbuf-=DOT_MAX_X;
	
		DataH = *(g_FrameBuffer[m]+(g_Matrixline[m]*2+0)*DOT_MAX_X+xbuf);
		DataL = *(g_FrameBuffer[m]+(g_Matrixline[m]*2+1)*DOT_MAX_X+xbuf);
			
        // cursor
		if(g_CursorFlag[m]==1 && g_Matrixline[m]==7)
		{
			if(g_bCursor2Byte && (xStart<=x && x<(xStart+16) ) )
			DataL = CLR_RED;

			else if(!g_bCursor2Byte && (xStart<=x && x<(xStart+8) ) )
			DataL = CLR_RED;
		}	

	    if(g_xMove[m]!=0 && x>191)
		SetDotColorPort1(CLR_NONE, CLR_NONE, CLR_NOTUSE,CLR_NOTUSE); 	
		else
		SetDotColorPort1(DataH, DataL, CLR_NOTUSE, CLR_NOTUSE);		

		GPIOA->BRR  = DOT_CLK;
		GPIOA->BSRR = DOT_CLK;
		
	}

	DOT_ADDR_P1 = 0xffff & ( (DOT_ADDR_P1&~0xF0)|(g_Matrixline[m]<<4) );
	
	for(int c=0;c<5;c++)
	DOT_LATCH_P1 = HIGH;
	DOT_LATCH_P1 = LOW;
	
    DOT_DISP_P1 = LOW; 
	DOT_OE_P1   = LOW;
	
	g_Matrixline[m]++;
	
	if(g_Matrixline[m]>7)
		g_Matrixline[m]=0;
	
}

/*******************************************************************************
* Function Name  : DisplayMatrixPort2. 
* Description       : Driver B/D ver 2.6 Duty 1/8 
*
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void MatrixDisplayPort2_D8(int m)
{
    u32 x;
	u8 DataH, DataL;
	u16 xbuf=0;

	if(m==2){
	DOT_PORT_EN3 = LOW;
	DOT_PORT_EN2 = HIGH;
	}
	if(m==3){
	DOT_PORT_EN2 = LOW;
	DOT_PORT_EN3 = HIGH;
	}
	
    DOT_DISP_P2 = HIGH;
	DOT_OE_P2   = HIGH;
	
	int xStart= g_CursorPos*8;
	
	for(x=0; x<DOT_MAX_X; x++)
	{
		xbuf = x+g_xMove[m];
		
		if(xbuf>=DOT_MAX_X)
			xbuf-=DOT_MAX_X;
	
		DataH = *(g_FrameBuffer[m]+(g_Matrixline[m]*2+0)*DOT_MAX_X+xbuf);
		DataL = *(g_FrameBuffer[m]+(g_Matrixline[m]*2+1)*DOT_MAX_X+xbuf);

        // cursor
		if(g_CursorFlag[m]==1 && g_Matrixline[m]==7)
		{ 		
			if(g_bCursor2Byte && (xStart<=x && x<(xStart+16) ) )
			DataL = CLR_RED;

			else if(!g_bCursor2Byte && (xStart<=x && x<(xStart+8) ) )
			DataL = CLR_RED;		   
		}

		SetDotColorPort2(DataH, DataL, CLR_NOTUSE,CLR_NOTUSE);		

		GPIOB->BRR  = DOT_CLK;
		GPIOB->BSRR = DOT_CLK; 	
	}

    DOT_ADDR_P2 = 0xffff & ( (DOT_ADDR_P2&~0xF0)|(g_Matrixline[m]<<4) );
	
	for(int c=0;c<5;c++)
	DOT_LATCH_P2 = HIGH;
	DOT_LATCH_P2 = LOW;
	
    DOT_DISP_P2 = LOW; 
	DOT_OE_P2   = LOW;
	
	g_Matrixline[m]++;
	
	if(g_Matrixline[m]>7)
		g_Matrixline[m]=0;
	
}

#endif

#ifdef DUTY_16
/*******************************************************************************
* Function Name  : DisplayMatrixPort1. 
* Description       : Driver B/D ver 2.6 Duty 1/16 
*
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void MatrixDisplayPort1_D16(int m)
{
    u32 x;
	u8 Data;
	u16 xbuf=0;
		
	if(m==0){
	DOT_PORT_EN1 = LOW;
	DOT_PORT_EN0 = HIGH;
	}
	if(m==1){
	DOT_PORT_EN0 = LOW;
	DOT_PORT_EN1 = HIGH;
	}
	
    DOT_DISP_P1 = LOW;
	DOT_OE_P1   = HIGH;
	
    DOT_ADDR_P1 = 0xffff & ( (DOT_ADDR_P1&~0xF0)|(g_Matrixline[m]<<4) );
	
	for(x=0; x<DOT_MAX_X; x++)
	{
		xbuf = x+g_xMove[m];
		
		if(xbuf>=DOT_MAX_X)
			xbuf-=DOT_MAX_X;
	
		Data = *(g_FrameBuffer[m]+(g_Matrixline[m])*DOT_MAX_X+xbuf);

        // cursor        
		if(g_CursorFlag[m]==1 && g_Matrixline[m]==15)
		{ 		
			int xStart= g_CursorPos*8;

			if(g_bCursor2Byte && (xStart<=x && x<(xStart+16) ) )
			Data = CLR_RED;

			if(!g_bCursor2Byte && (xStart<=x && x<(xStart+8) ) )
			Data = CLR_RED;		   
		}

	    if(g_xMove[m]!=0 && x>191)
		SetDotColorPort1(CLR_NONE, CLR_NOTUSE, CLR_NOTUSE,CLR_NOTUSE); 	
		else
		SetDotColorPort1(Data, CLR_NOTUSE, CLR_NOTUSE, CLR_NOTUSE);			

		GPIOA->BRR  = DOT_CLK;
		GPIOA->BSRR = DOT_CLK;  	
	}
	
	for(int c=0;c<5;c++)
	DOT_LATCH_P1 = HIGH;
	
	DOT_LATCH_P1 = LOW;
	
    DOT_DISP_P1 = LOW; 
	DOT_OE_P1   = LOW;
	
	g_Matrixline[m]++;
	
	if(g_Matrixline[m]>15)
		g_Matrixline[m]=0;
		
}

/*******************************************************************************
* Function Name  : DisplayMatrixPort2. 
* Description       : Driver B/D ver 2.6 Duty 1/16  
*
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void MatrixDisplayPort2_D16(int m)
{
    u32 x;
	u8 Data;
	u16 xbuf=0;

	if(m==2){
	DOT_PORT_EN3 = LOW;
	DOT_PORT_EN2 = HIGH;
	}
	if(m==3){
	DOT_PORT_EN2 = LOW;
	DOT_PORT_EN3 = HIGH;
	}
	
    DOT_DISP_P2 = LOW;
	DOT_OE_P2   = HIGH;
	
    DOT_ADDR_P2 = 0xffff & ( (DOT_ADDR_P2&~0xF0)|(g_Matrixline[m]<<4) );
	
	for(x=0; x<DOT_MAX_X; x++)
	{
		xbuf = x+g_xMove[m];
		
		if(xbuf>=DOT_MAX_X)
			xbuf-=DOT_MAX_X;
	
		Data = *(g_FrameBuffer[m]+(g_Matrixline[m])*DOT_MAX_X+xbuf);

        // cursor
		if(g_CursorFlag[m]==1 && g_Matrixline[m]==15)
		{ 		
			int xStart=g_CursorPos*8;

			if(g_bCursor2Byte && (xStart<=x && x<(xStart+16) ) )
			Data = CLR_RED;

			if(!g_bCursor2Byte && (xStart<=x && x<(xStart+8) ) )
			Data = CLR_RED;		   
		}

		SetDotColorPort2(Data,CLR_NOTUSE,CLR_NOTUSE,CLR_NOTUSE);		

		GPIOB->BRR  = DOT_CLK;
		GPIOB->BSRR = DOT_CLK; 	
	}
	
	for(int c=0;c<5;c++)
	DOT_LATCH_P2 = HIGH;

	DOT_LATCH_P2 = LOW;
	
    DOT_DISP_P2 = LOW; 
	DOT_OE_P2   = LOW;
	
	g_Matrixline[m]++;
	
	if(g_Matrixline[m]>15)
		g_Matrixline[m]=0;
	
}

#endif

/*******************************************************************************
* Function Name  : MatrixDisplayPort. 
* Description       : Driver B/D ver 2.6 
*
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void MatrixDisplayPort(u8 port)
{
#ifdef DUTY_4			
	
	   if(port==0){
			if(g_PortFlag[0]==0){
				g_PortFlag[0]=1;
				MatrixDisplayPort1_D4(0);
			}else{
				g_PortFlag[0]=0;
				MatrixDisplayPort1_D4(0);
			}
		}
	
		if(port==1){
			if(g_PortFlag[1]==0){
				g_PortFlag[1]=1;
				//MatrixDisplayPort2_D4(2);
			}else{
				g_PortFlag[1]=0;
				//MatrixDisplayPort2_D4(2);
			}
		}	
#endif

#ifdef DUTY_8			

   if(port==0){
		if(g_PortFlag[0]==0){
			g_PortFlag[0]=1;
			MatrixDisplayPort1_D8(0);
		}else{
			g_PortFlag[0]=0;
			MatrixDisplayPort1_D8(0);
		}
    }

    if(port==1){
		if(g_PortFlag[1]==0){
			g_PortFlag[1]=1;
			MatrixDisplayPort2_D8(2);
		}else{
			g_PortFlag[1]=0;
  		    MatrixDisplayPort2_D8(2);
		}
    }	
#endif

#ifdef DUTY_16
	if(port==0){
		 if(g_PortFlag[0]==0){
			 g_PortFlag[0]=1;
			 MatrixDisplayPort1_D16(0);
		 }else{
			 g_PortFlag[0]=0;
			 MatrixDisplayPort1_D16(0);
		 }
	 }

	 if(port==1){
		 if(g_PortFlag[1]==0){
			 g_PortFlag[1]=1;
			 MatrixDisplayPort2_D16(2);
		 }else{
			 g_PortFlag[1]=0;
			 MatrixDisplayPort2_D16(2);
		 }
	 }	 
#endif

}


