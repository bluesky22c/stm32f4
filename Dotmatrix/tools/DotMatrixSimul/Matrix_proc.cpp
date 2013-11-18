/******************** (C) COPYRIGHT 2012 OP ***********************************
* File Name          : Matrix_proc.c
* Author             : bluesky22c
* Version            : V1.1.1
* Date               : 11/11/2011
* Description        : Matrix procedure which is DotMatrix Proc.
*******************************************************************************
*
*******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "stdafx.h"

#include "matrix_proc.h"

#include "font_ascii.h"
#include "font_num.h"
#include "font_etc.h"
#include "font_kor.h"

/********************************/
/*         16 DOT MATRIX        */
/********************************/
#define KfontLen   32
#define EfontLen   16 

#define CODE_NOT  0
#define CODE_NUM  1
#define CODE_ETC  2
#define CODE_KOR  3
#define CODE_ASCII  4

#define DOT_RD_L   temp 
#define DOT_RD_H   temp 
#define DOT_GD_H   temp
#define DOT_GD_L   temp

#define DOT_CLK    temp 
#define DOT_OE     temp 
#define DOT_LATCH  temp
#define DOT_DISP   temp
#define DOT_ADDR   temp

#define HIGH       1
#define LOW        0

// A0 = PA4
// A1 = PA5
// A2 = PA6
// A3 = PA7

#define CLR_NONE    0
#define CLR_RED     1
#define CLR_GREEN   2
#define CLR_YELLOW  3
//----------------------
u8* g_FrameBuffer[4];
int g_Matrixline[4];
int g_xMove[4];
DotEffect_t g_DotEffect[4];
u8 g_CursorFlag[4];	 // 0 = cursor off, 1 = cursor on
u8 g_CursorPos  = 0; // 8bit
bool g_bCursor2Byte = false; 

u8 g_FrameText[4][33];
u8 g_FrameColr[4][33];

int temp;
 
int NMtoIndex(u16 code)
{
	byte high,low;
	u16 index;
	
	high = (code>>8)&0xff;
	low  =  code    &0xff;
	if ((code>=0xa3b0)&&(code<=0xa3b9)){
		index=(high-0xa3)+low-0xb0;
		return (index*KfontLen);
	} else return -1;
}

int ETCtoIndex(u16 code)
{
	u16 idx=0xffff;
	
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
	byte high,low;
	u16 index;
	
	high = (code>>8)&0xff;
	low  =  code    &0xff;
	if ((code>=0xb0a1)&&(low<=0xfe)){
		index=(high-0xb0)*94+low-0xa1;
		
		return (index*KfontLen+10*KfontLen);
	} else return -1;
}

int KSSMtoIndex(u16 code)
{
	byte high,low;
	u16 index;
	
	high = (code>>8)&0xff;
	low  =  code    &0xff;
	if ((code>=0x8861)&&(low<=0xd3bd)&&low!=0x20&&low!=0x00){
		index = (high - 0xd3) + low - 0x61;
		
		return index;
	} else return -1;
}

int ASCIItoIndex(byte code)
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
//--------------------------------------

u8 GetColor(u8 data, u8* pclr, u8 pos)
{
	u8 clrText = ( *(pclr+pos) >>6 & 0x3);
	u8 clrBack = ( *(pclr+pos) >>4 & 0x3);

	if(data!=0)		
		return clrText;
	else
		return clrBack;
}

void MatrixProc_Init(void)
{
	for(int m=0;m<4;m++){
		g_FrameBuffer[m] = (u8*)malloc(DOT_MAX_X*DOT_MAX_Y);
		g_Matrixline[m]=0;		
		g_xMove[m] = 0;

		g_DotEffect[m].inType=0;
		g_DotEffect[m].outType=0;
		g_DotEffect[m].delay=0;
		g_DotEffect[m].speed=1;

		g_CursorFlag[m] = 0;
		g_CursorPos     = 0; // 8bit 기준
	}

    memset(g_FrameText, 0, sizeof(g_FrameText) );
    memset(g_FrameColr, 0, sizeof(g_FrameColr) );
	
	u8 txt[40]={0,};
	u8 clr[40]={0,};
	
	sprintf((char*)txt, "가나다라바사아자차카타파하０１２"); // 32 byte
//	sprintf((char*)txt, "０:１가나다라바사아자차카타파００"); // 33 byte
//	sprintf((char*)txt, "ABCDEFGHIJKLMNOPQRSTUVWXYZ１１１"); // 32 byte
//	sprintf((char*)txt, "■→←●◀▶1234567890abcdefgh２"); // 32 byte
//	sprintf((char*)txt, "０１２３４５６７８９０１２３４５"); // 32 byte
	
	memset(clr, (CLR_YELLOW<<6|CLR_NONE<<4), sizeof(clr));

	MatrixConvKssm(txt, sizeof(txt) );

    MatrixSetFrame(0, txt, clr, 32);
    MatrixSetFrame(1, txt, clr, 32);
    MatrixSetFrame(2, txt, clr, 32);
    MatrixSetFrame(3, txt, clr, 32);
}

/*******************************************************************************
* Function Name  : SetFrameBuffer.
* Description    : Set data to g_FrameBuffer.
* Input          : startPos, endPos -> byte count
* Output         : None.
* Return         : None.
*******************************************************************************/
void MatrixSetFrame(int m, u8* text, u8* colr, int len)
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
	int skip=0;

    u8 KoreanBuffer[33] = {0,};		// 32 byte Korean font buffer
    
	//----------------------------------------------
	//  8x16 기준으로 한다.
	//----------------------------------------------
	while(TRUE){

		if(ntextPos+1 > len)
			break; 

		buf[1]= *(text+ntextPos+0); 
		buf[0]= *(text+ntextPos+1);

		if(buf[1] < 0x80){  // ascii
 		   wData = buf[1];
		}else{
          wData = (buf[1]<<8) + (buf[0]&0xff);
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
				GetKSSMData(wData, KoreanBuffer);
			}

			if(nFontStartIdx!=-1)
				nCode=CODE_KOR;
		}

		// KSSM 미완성글씨
		if(nCode==CODE_NOT){
			wKSSMCode = wData;

			if(0x8442<=wKSSMCode && wKSSMCode<=0xd041){
				GetKSSMData(wKSSMCode, KoreanBuffer);
				nCode=CODE_KOR;
			}			
		}

		// ASCII
		if(nCode==CODE_NOT && wData!=0x8441){
			nFontStartIdx = ASCIItoIndex(wData);
			if(nFontStartIdx!=-1)
				nCode=CODE_ASCII;
		}

		if(buf[1]==':'){
			g_FrameText[m][ntextPos]=text[ntextPos]; 
			nSemiPos=ntextPos;
			ntextPos+=1;
			nOffset-=8;
			continue;
		}

		// Check Data Change
		if( 
			g_FrameText[m][ntextPos]  ==text[ntextPos]   && 
			g_FrameColr[m][ntextPos]  ==colr[ntextPos]   && 
			g_FrameText[m][ntextPos+1]==text[ntextPos+1] && 
			g_FrameColr[m][ntextPos+1]==colr[ntextPos+1] && 
			text[ntextPos]!=':'
			)
		{
			if(nCode==CODE_ASCII)
				ntextPos+=1;
			else
				ntextPos+=2;
			
			continue;
		}		

		//TRACE("MatrixSetFrame\n");

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
			for(int nFontIdx=nFontStartIdx;nFontIdx<(nFontStartIdx+KfontLen);nFontIdx++){			
				byData = NUM_fontTable[nFontIdx];
				
				for( x=0;x<8;x++){
					*(g_FrameBuffer[m]+nLine*DOT_MAX_X+(xStart+x)) = GetColor((byData & 0x80),colr,ntextPos);
					byData = byData << 1;
				}
				g_FrameText[m][ntextPos]=text[ntextPos];
				g_FrameColr[m][ntextPos]=colr[ntextPos];
				
				nFontIdx++;					
				byData = NUM_fontTable[nFontIdx];
				
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

            for( x=0;x<16;x++){
				byData = KoreanBuffer[x+16];
				for(nLine=8;nLine<16;nLine++){
					*(g_FrameBuffer[m]+nLine*DOT_MAX_X+(xStart+x)) = GetColor((byData & 0x01),colr,ntextPos);
					byData = byData >> 1;
				}
			}

			g_FrameText[m][ntextPos]  =text[ntextPos];
			g_FrameColr[m][ntextPos]  =colr[ntextPos];
			g_FrameText[m][ntextPos+1]=text[ntextPos+1];
			g_FrameColr[m][ntextPos+1]=colr[ntextPos+1];

			ntextPos+=2;
		}		

	} // while

	// ':'
	if(nSemiPos!=0){
		xStart=nSemiPos*8-1;
		nSemiPos=0;

		nLine = 5; 
		x=0;
		*(g_FrameBuffer[m]+(nLine+0)*DOT_MAX_X+(xStart+x)) = CLR_RED;
		*(g_FrameBuffer[m]+(nLine+1)*DOT_MAX_X+(xStart+x)) = CLR_RED;
		x=1;
		*(g_FrameBuffer[m]+(nLine+0)*DOT_MAX_X+(xStart+x)) = CLR_RED;
		*(g_FrameBuffer[m]+(nLine+1)*DOT_MAX_X+(xStart+x)) = CLR_RED;

		nLine = 10; 
		x=0;
		*(g_FrameBuffer[m]+(nLine+0)*DOT_MAX_X+(xStart+x)) = CLR_RED;
		*(g_FrameBuffer[m]+(nLine+1)*DOT_MAX_X+(xStart+x)) = CLR_RED;
		x=1;
		*(g_FrameBuffer[m]+(nLine+0)*DOT_MAX_X+(xStart+x)) = CLR_RED;
		*(g_FrameBuffer[m]+(nLine+1)*DOT_MAX_X+(xStart+x)) = CLR_RED;
	}

}


void MakeClock()
{	
	DOT_CLK = LOW;	
	DOT_CLK = HIGH;
}

void SetDotColor(byte data_H, byte data_L)
{	
    if(data_H == CLR_NONE)
   	{
		DOT_GD_H  = LOW;		 
		DOT_RD_H  = LOW;
   	}	
    else if(data_H == CLR_RED)
	{
		DOT_GD_H  = LOW;		 
		DOT_RD_H  = HIGH;
   	}
    else if(data_H == CLR_GREEN)
	{
		DOT_GD_H  = HIGH;		 
		DOT_RD_H  = LOW;
   	}	
    else if(data_H == CLR_YELLOW)
	{
		DOT_GD_H  = HIGH;		 
		DOT_RD_H  = HIGH;
   	}	
	
	if(data_L == CLR_NONE)
	{
		DOT_GD_L  = LOW;		 
		DOT_RD_L  = LOW;
	}
	else if(data_L == CLR_RED)
	{
		DOT_GD_L  = LOW;		 
		DOT_RD_L  = HIGH;
	}
	else if(data_L == CLR_GREEN)
	{
		DOT_GD_L  = HIGH;		 
		DOT_RD_L  = LOW;
	}	
	else if(data_L == CLR_YELLOW)
	{
		DOT_GD_L  = HIGH;		 
		DOT_RD_L  = HIGH;
	}
	
}

/*******************************************************************************
* Function Name  : MatrixSetEffect.
* Description       : Calltime 100ms
*                         Driver B/D ver 2.6 Duty 1/8
*
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void MatrixSetEffect(int m)
{
    // in none
	if(g_DotEffect[m].inType==0){
		g_xMove[m]=0;
	}
	
    // in from right
	if(g_DotEffect[m].inType==1){
		g_xMove[m]+=g_DotEffect[m].speed;
	}
	
    // in from left
	if(g_DotEffect[m].inType==2){
		g_xMove[m]-=g_DotEffect[m].speed;
	}
	
	if(g_DotEffect[m].inType==3){
		g_xMove[m]=0;
	}
	
	if(g_DotEffect[m].inType==4){
		g_xMove[m]=0;
	}
	
	if(g_DotEffect[m].inType==5){
		g_xMove[m]=0;
	}
	
	if(g_DotEffect[m].inType==6){
		g_xMove[m]=0;
	}
	
	if(g_xMove[m]<=0)
		g_xMove[m]=DOT_MAX_X-1;
	
	if(g_xMove[m]>=DOT_MAX_X)
		g_xMove[m]=0;	
	
}
