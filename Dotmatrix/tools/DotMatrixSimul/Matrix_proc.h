/******************** (C) COPYRIGHT 2011 OP ***********************************
* File Name          : Matrix_proc.c
* Author             : Advanced Research Team
* Version            : V1.1.1
* Date               : 11/11/2011
******************************************************************************/
#include "StdAfx.h"

#define DOT_MAX_MOD 16
#define DOT_MAX_X  DOT_MAX_MOD*16 
#define DOT_MAX_Y  16 

#define __PACKED__

typedef struct  
{
	u8 inType;  // (0~6)
	u8 delay;   // (0~10)
	u8 outType; // (0~6)
	u8 speed;   // (0~9)
}__PACKED__ DotEffect_t;

//----------------------------------
//  Function
int NMtoIndex(u16 code);
int ETCtoIndex(u16 code);
int KStoIndex(u16 code);
int KSSMtoIndex(u16 code);
int ASCIItoIndex(byte code);
u8 GetColor(u8 data, u8* pclr, u8 pos);

void MatrixConvKssm(u8* text, int len);
void MatrixSetFrame(int m, u8* text, u8* colr, int len);
void MatrixProc_Init(void);
void MatrixSetEffect(int m);