/******************** (C) COPYRIGHT 2008 Osung LST *****************************
* File Name          : hostcom_handler.c
* Author             : Advanced Research Team
* Version            : V2.2.1
* Date               : 09/22/2008
* Description        : hostcom cmd handler.
********************************************************************************
*
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"
#include "hostcom_proc.h"

#include "mem_heap.h"
#include "queue.h"
#include "hostcom_type.h"

#include "calltrace.h"
#include "common.h"

#include "Matrix_proc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private macro ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* system heap memory */

extern mem_heap_type g_SystemHeap;

extern bool g_bTimeDotDsp;
extern u8 g_Buzzer;
extern u8 g_fontIdx;
extern u8 g_PrevCmd;
extern bool g_bResetSystem;

extern u8 g_FrameColr[4][36];
extern DotEffect_t g_DotEffect[4];

extern u8* g_FrameBuffer[4];

/*******************************************************************************
* Function Name  : Convert Functions
* Description    : 
* Input          : None.
* Return         : None.
*******************************************************************************/

int CharTo2Str(u8 cValue, char* buf)
{
	int ret=1;
	
	*buf = cValue;
	
	switch(cValue)
	{
	case ' ': memcpy(buf,"  ",2); ret=2; break;
	case '0': memcpy(buf,"０",2); ret=2; break;
	case '1': memcpy(buf,"１",2); ret=2; break;
	case '2': memcpy(buf,"２",2); ret=2; break;
	case '3': memcpy(buf,"３",2); ret=2; break;
	case '4': memcpy(buf,"４",2); ret=2; break;
	case '5': memcpy(buf,"５",2); ret=2; break;
	case '6': memcpy(buf,"６",2); ret=2; break;
	case '7': memcpy(buf,"７",2); ret=2; break;
	case '8': memcpy(buf,"８",2); ret=2; break;
	case '9': memcpy(buf,"９",2); ret=2; break;
	}
	
	return ret;
}

int IntTo2Str(u8 cValue, char* buf)
{
	int ret=1;
	
	*buf = cValue;
	
	switch(cValue)
	{
	case 0: memcpy(buf,"０",2); ret=2; break;
	case 1: memcpy(buf,"１",2); ret=2; break;
	case 2: memcpy(buf,"２",2); ret=2; break;
	case 3: memcpy(buf,"３",2); ret=2; break;
	case 4: memcpy(buf,"４",2); ret=2; break;
	case 5: memcpy(buf,"５",2); ret=2; break;
	case 6: memcpy(buf,"６",2); ret=2; break;
	case 7: memcpy(buf,"７",2); ret=2; break;
	case 8: memcpy(buf,"８",2); ret=2; break;
	case 9: memcpy(buf,"９",2); ret=2; break;
	}
	
	return ret;
}

int IntToArrow(u8 cValue, char* buf)
{
	int ret=2;
	
	*buf = cValue;
	
	switch(cValue)
	{
	case 0: memcpy(buf,"  ",2); ret=2; break;
	case 1: memcpy(buf,"▶",2); ret=2; break;
	case 2: memcpy(buf,"◀",2); ret=2; break;
	}
	
	return ret;
}

int IntToCircle(u8 cValue, char* buf)
{
	int ret=2;
	
	*buf = cValue;
	
	switch(cValue)
	{
	case 0: memcpy(buf,"  ",2); ret=2; break;
	default: memcpy(buf,"●",2); ret=2; break;
	}
	
	return ret;
}

int IntToRect(u8 cValue, char* buf)
{
	int ret=2;
	
	*buf = cValue;
	
	switch(cValue)
	{
	case 0: memcpy(buf,"  ",2); ret=2; break;
	default: memcpy(buf,"■",2); ret=2; break;
	}
	
	return ret;
}

/*******************************************************************************
* Function Name  : HostRecv_GPS
* Description    :
* Input          : 'HHmmss.mse'  // ex '120524.291'
* Return         : None.
*******************************************************************************/
void HostRecv_GPS(u8* buf)
{
//	CallTrace(TRACE_APP_LOW, "HostRecv_GPS\n\r");

	// hh:mm
	u8 text[16*2+2];
	u8 colr[16*2+2];
	
	u8 temp[5]; temp[2]=0;
	u8 HH, mm;
	
	memset(text, 0x20, sizeof(text) );
	memset(colr, (1<<6|0<<4), sizeof(colr) );

    // port1 clear 
	MatrixSetFrame(2, text,colr,32); // 
	
	memcpy(temp,buf,2); 
	HH = atoi((char*)temp)+9; // korea +9

	if(HH>=24)HH-=24;
	
	memcpy(temp,buf+2,2); 
	mm = atoi((char*)temp);

	SPRINTF((char*)temp,"%02d:%02d",HH,mm);
	
	CharTo2Str(temp[0], (char*)(text+4*2) );
	CharTo2Str(temp[1], (char*)(text+5*2) );	
	text[6*2]=temp[2];
	CharTo2Str(temp[3], (char*)(text+6*2+1) );	
	CharTo2Str(temp[4], (char*)(text+7*2+1) );	

    g_bTimeDotDsp=!g_bTimeDotDsp;
		
	MatrixSetFrame(0, text,colr,34);
}


/*******************************************************************************
* Function Name  : HostRecv_48Team4   
* Description    :
* Input          : None.
* Return         : None.
*******************************************************************************/
void HostRecv_48Team4(u8* buf)
{ 
	u8 txt[2][16*2+1];
	u8 clr[2][16*2+1];
	u8 c;
	
	char tmp[4];

	memset(txt,0x20,sizeof(txt));
	memset(clr, (1<<6 | 0<<4), sizeof(clr) );  // default RED

	// time  hh:mm
	CharTo2Str(buf[4], (char*)(txt[0]+4*2) );
	CharTo2Str(buf[5], (char*)(txt[0]+5*2) );
	*(txt[0]+6*2) =buf[6];
	CharTo2Str(buf[7], (char*)(txt[0]+6*2+1) );
	CharTo2Str(buf[8], (char*)(txt[0]+7*2+1) );
	
	// left team Name
	c = *(buf+9);
	if(c==0)c=(1<<6); // red
	if(c==1)c=(2<<6); // green
	if(c==2)c=(3<<6); // yellow
	
	memset(clr[0],c,8);
	memcpy(txt[0],buf+10,8);
	
	// right team Name
	c = *(buf+18);
	if(c==0)c=(1<<6); // red
	if(c==1)c=(2<<6); // green
	if(c==2)c=(3<<6); // yellow
	
	memset(clr[0]+8*2+1,c,8);
	memcpy(txt[0]+8*2+1,buf+19,8);	
		
	// set
	memcpy(txt[1]+10*2,buf+27,2);		
	
    // left count
	SPRINTF(tmp,"%3d", (buf[29]<<8) + (u8)(buf[30]) );
	CharTo2Str(tmp[0], (char*)(txt[0]+13*2+1) );
	CharTo2Str(tmp[1], (char*)(txt[0]+14*2+1) );
	CharTo2Str(tmp[2], (char*)(txt[0]+15*2+1) );

	
    // right count
	SPRINTF(tmp,"%3d", (buf[31]<<8) + (u8)(buf[32]) );
	CharTo2Str(tmp[0], (char*)(txt[1]+0*2) );
	CharTo2Str(tmp[1], (char*)(txt[1]+1*2) );
	CharTo2Str(tmp[2], (char*)(txt[1]+2*2) );

	// left arrow
	IntToArrow(buf[33], (char*)(txt[1]+7*2) );
	// right arrow
	if(buf[34]==1)
	IntToArrow(2, (char*)(txt[0]+12*2+1) );
	else
	IntToArrow(0, (char*)(txt[0]+12*2+1) );

	// circle
	IntToCircle(buf[35], (char*)(txt[1]+3*2) );
	IntToCircle(buf[36], (char*)(txt[1]+4*2) );
	IntToCircle(buf[37], (char*)(txt[1]+5*2) );
	IntToCircle(buf[38], (char*)(txt[1]+12*2) );
	IntToCircle(buf[39], (char*)(txt[1]+13*2) );
	IntToCircle(buf[40], (char*)(txt[1]+14*2) );

	// left team foul
	IntTo2Str(buf[41], (char*)(txt[1]+6*2) );
	// player no 
	SPRINTF(tmp,"%2d", buf[42]);
	CharTo2Str(tmp[0], (char*)(txt[1]+8*2) );
	CharTo2Str(tmp[1], (char*)(txt[1]+9*2) );
	// player foul 
	IntTo2Str(buf[43], (char*)(txt[1]+11*2) );
	// right team foul 
	IntTo2Str(buf[44], (char*)(txt[1]+15*2) );

	//g_Buzzer = buf[45];
	
	MatrixConvKssm(txt[0], sizeof(txt[0]) );
	MatrixConvKssm(txt[1], sizeof(txt[1]) );

	MatrixSetFrame(0, txt[0], clr[0],33);
	MatrixSetFrame(2, txt[1], clr[1],32);
	
}

/*******************************************************************************
* Function Name  : HostRecv_106Volleyball 
* Description    :
* Input          : None.
* Return         : None.
*******************************************************************************/
void HostRecv_106Volleyball(u8* buf)
{ 
	u8 txt[2][16*2+1];
	u8 clr[2][16*2+1];

	u8 Cmd  = buf[3];

	memset(txt,0x20,sizeof(txt));
	memset(clr, (1<<6 | 0<<4), sizeof(clr) );  // default RED

//	memcpy(clr[0],buf+54,50);

	memcpy(txt[0]+ 4,buf+ 4, 6); // 이긴세트3
	
	memcpy(txt[0]+10,buf+10, 8); // 팀명    
	memcpy(txt[0]+18,buf+40, 8); // 팀명	

//	memcpy(txt[0]+26,buf+48, 6); // 이긴세트

	memcpy(txt[0]+30,buf+48, 2); // 이긴세트1
	memcpy(txt[0]+28,buf+50, 2); // 이긴세트2
	memcpy(txt[0]+26,buf+52, 2); // 이긴세트3

	memcpy(txt[1]+10,buf+18, 4); // 점수
	memcpy(txt[1]+14,buf+22, 4); // 작전-
	memcpy(txt[1]+18,buf+26, 2); // 공격
	memcpy(txt[1]+20,buf+28, 2); // 쿼터
	memcpy(txt[1]+22,buf+30, 2); // 공격
	memcpy(txt[1]+24,buf+32, 4); // 작전-
	memcpy(txt[1]+28,buf+36, 4); // 점수

    // color  
//	memcpy(clr[0]+ 4,buf+ 4+50, 6); // 이긴세트		
	memcpy(clr[0]+10,(buf+10+50), 8); // 팀명    
	memcpy(clr[0]+18,(buf+40+50), 8); // 팀명	
//	memcpy(clr[0]+26,buf+48+50, 6); // 이긴세트
	
//	memcpy(clr[1]+10,buf+18+50, 4); // 점수
//	memcpy(clr[1]+14,buf+22+50, 4); // 작전-
//	memcpy(clr[1]+18,buf+26+50, 2); // 공격
	memcpy(clr[1]+20,(buf+28+50), 2); // 쿼터
//	memcpy(txt[1]+22,buf+30+50, 2); // 공격
//	memcpy(txt[1]+24,buf+32+50, 4); // 작전-
//	memcpy(clr[1]+28,buf+36+50, 4); // 점수

	MatrixConvKssm(txt[0], 32);	
	MatrixConvKssm(txt[1], 32);

	MatrixSetFrame(0, txt[0], clr[0], 32);
	MatrixSetFrame(2, txt[1], clr[1], 32);

}

/*******************************************************************************
* Function Name  : HostRecv_48Team4_2012 
* Description    :
* Input          : None.
* Return         : None.
*******************************************************************************/
void HostRecv_48Team4_2012(u8* buf)
{ 
	u8 txt[2][16*2+1];
	u8 clr[2][16*2+1];
	u8 c, xStart;

	u8 Cmd  = buf[3];
	
	char tmp[4];

	g_DotEffect[0].inType = 0;
	g_DotEffect[0].delay  = 0;
	g_DotEffect[0].outType= 0;
	g_DotEffect[0].speed  = 1; 

	memset(txt,0x20,sizeof(txt));
	memset(clr, (1<<6 | 0<<4), sizeof(clr) );  // default RED
	
	if(Cmd!=0x36){
		
		// time  hh:mm
		CharTo2Str(buf[4], (char*)(txt[0]+4*2) );
		CharTo2Str(buf[5], (char*)(txt[0]+5*2) );
		*(txt[0]+6*2) =buf[6];
		CharTo2Str(buf[7], (char*)(txt[0]+6*2+1) );
		CharTo2Str(buf[8], (char*)(txt[0]+7*2+1) );
		
		// left team Name
		c = *(buf+9);
		if(c==0)c=(1<<6); // red
		if(c==1)c=(2<<6); // green
		if(c==2)c=(3<<6); // yellow
		
		memset(clr[0],c,8);
		memcpy(txt[0],buf+10,8);
		
		// right team Name
		c = *(buf+18);
		if(c==0)c=(1<<6); // red
		if(c==1)c=(2<<6); // green
		if(c==2)c=(3<<6); // yellow
		
		memset(clr[0]+8*2+1,c,8);
		memcpy(txt[0]+8*2+1,buf+19,8);	

		// dot area clear 
		xStart=11*8+6;
		for(c=0;c<16;c++){
			if(c!=5 && c!=6 && c!=10 && c!=11){
			*(g_FrameBuffer[0]+c*DOT_MAX_X+(xStart+0)) = 0;
			*(g_FrameBuffer[0]+c*DOT_MAX_X+(xStart+1)) = 0;
			*(g_FrameBuffer[0]+c*DOT_MAX_X+(xStart+2)) = 0;
			*(g_FrameBuffer[0]+c*DOT_MAX_X+(xStart+3)) = 0;
			}
		}
		
	}
	else{		
		// left team Name
		c = *(buf+4);
		if(c==0)c=(1<<6); // red
		if(c==1)c=(2<<6); // green
		if(c==2)c=(3<<6); // yellow
		
		memset(clr[0],c,10);
		memcpy(txt[0],buf+5,10); 
		
		if(buf[38]==0x20){
			*(txt[0]+10) ='_';
			CharTo2Str(buf[39], (char*)(txt[0]+12) );
		}else{
			*(txt[0]+10) ='_';
			CharTo2Str(buf[38], (char*)(txt[0]+11) );
			CharTo2Str(buf[39], (char*)(txt[0]+13) );

			// clear 
			xStart=11*8+6;
			for(c=0;c<16;c++){
			*(g_FrameBuffer[0]+c*DOT_MAX_X+(xStart+0)) = 0;
			*(g_FrameBuffer[0]+c*DOT_MAX_X+(xStart+1)) = 0;
			*(g_FrameBuffer[0]+c*DOT_MAX_X+(xStart+2)) = 0;
			*(g_FrameBuffer[0]+c*DOT_MAX_X+(xStart+3)) = 0;
			}
			
		}
		
		// right team Name
		c = *(buf+16);
		if(c==0)c=(1<<6); // red
		if(c==1)c=(2<<6); // green
		if(c==2)c=(3<<6); // yellow
		
		memset(clr[0]+15,c,10);
		memcpy(txt[0]+15,buf+17,10);			
	}
			
	// set
	c=(2<<6);
	memset(clr[1]+10*2,c,2); // green
	memcpy(txt[1]+10*2,buf+27,2);
	
    // left count
	CharTo2Str(buf[29], (char*)(txt[1]+0*2) );
	CharTo2Str(buf[30], (char*)(txt[1]+1*2) );
	CharTo2Str(buf[31], (char*)(txt[1]+2*2) );
	
    // right count
	CharTo2Str(buf[32], (char*)(txt[0]+13*2+1) );
	CharTo2Str(buf[33], (char*)(txt[0]+14*2+1) );
	CharTo2Str(buf[34], (char*)(txt[0]+15*2+1) );

	// arrow
	if(buf[35]==0x30){
	IntToArrow(0, (char*)(txt[1]+7*2) );
	IntToArrow(0, (char*)(txt[0]+12*2+1) );
	}
	
	if(buf[35]==0x31){
	IntToArrow(1, (char*)(txt[1]+7*2) );
	IntToArrow(0, (char*)(txt[0]+12*2+1) );
	}

	if(buf[35]==0x32){
	IntToArrow(0, (char*)(txt[1]+7*2) );
	IntToArrow(2, (char*)(txt[0]+12*2+1) );
	}
	
	// circle		
	c=0;
	if( (buf[36]-0x30)==1 )c|=0x1;
	if( (buf[36]-0x30)==2 )c|=0x3;
	if( (buf[36]-0x30)==3 )c|=0x7;
	
	IntToCircle( (c&0x1), (char*)(txt[1]+3*2) );
	IntToCircle( (c&0x2), (char*)(txt[1]+4*2) );
	IntToCircle( (c&0x4), (char*)(txt[1]+5*2) );
	
	c=0;
	if( (buf[37]-0x30)==1 )c|=0x1;
	if( (buf[37]-0x30)==2 )c|=0x3;
	if( (buf[37]-0x30)==3 )c|=0x7;
	
	IntToCircle( (c&0x1), (char*)(txt[1]+12*2) );
	IntToCircle( (c&0x2), (char*)(txt[1]+13*2) );
	IntToCircle( (c&0x4), (char*)(txt[1]+14*2) );

	// left team foul	
	CharTo2Str(buf[40], (char*)(txt[1]+6*2) );

	// right team foul 
	CharTo2Str(buf[41], (char*)(txt[1]+15*2) );
	
	// player no 
	CharTo2Str(buf[42], (char*)(txt[1]+8*2) );
	CharTo2Str(buf[43], (char*)(txt[1]+9*2) );
	c=(2<<6); // green
	memset(clr[1]+8*2,c,4);
	
	// player foul 
	CharTo2Str(buf[44], (char*)(txt[1]+11*2) );
	c=(3<<6); // yellow
	memset(clr[1]+11*2,c,2);

	g_Buzzer = buf[45];
	g_bTimeDotDsp = TRUE;

	
	MatrixConvKssm(txt[0], 33 );	
	MatrixConvKssm(txt[1], 32 );

	MatrixSetFrame(0, txt[0], clr[0],33);
	MatrixSetFrame(2, txt[1], clr[1],32);
	
}

/*******************************************************************************
* Function Name  : HostRecv_BrightCtrl 
* Description    :
* Input          : None.
* Return         : None.
*******************************************************************************/
void HostRecv_BrightCtrl(u8* buf)
{ 
	u16 val = buf[4];

//	CallTrace(TRACE_APP_LOW, "HostRecv_BrightCtrl %d\n\r", val);

#ifdef OLD_TYPE
    val = 9 - val;
#endif

    P_DIMM_Control(2000,val*10+9,0);		
}


/*******************************************************************************
* Function Name  : HostRecv_BrightCtrl 
* Description    :
* Input          : None.
* Return         : None.
*******************************************************************************/
void HostRecv_FontCtrl(u8* buf)
{ 
	g_fontIdx=buf[4];

	if(g_fontIdx>2)
	  g_fontIdx=0;

    SaveToFlash(0);	

    memset(g_FrameColr, 0, sizeof(g_FrameColr) );	
}

/*******************************************************************************
* Function Name  : HostRecv_TestMode 
* Description    :
* Input          : None.
* Return         : None.
*******************************************************************************/
void HostRecv_TestMode(u8* buf)
{ 
	u8 txt[2][16*2+1];
	u8 clr[2][16*2+1];
	u8 c,pos;

    c = buf[4];
	pos = (buf[5]*2);

	if(c==0)c=(1<<6); // red
	if(c==1)c=(2<<6); // green
	if(c==2)c=(3<<6); // yellow

	memset(clr, (c | 0<<4), sizeof(clr) );
	memset(txt, 0x20, sizeof(txt) );

    if(pos<16){
		txt[0][pos] = 0xa1; pos++;
		txt[0][pos] = 0xe1;
   	}else{
   	    pos -= 15;
		txt[1][pos] = 0xa1; pos++;
		txt[1][pos] = 0xe1;
   	}
   
	MatrixSetFrame(0, txt[0], clr[0], 32);	
	MatrixSetFrame(2, txt[1], clr[1], 32);  	
}

/*******************************************************************************
* Function Name  : HostRecv_46Char20
* Description    : display 20bytes   팀명
* Input          : None.
* Return         : None.
*******************************************************************************/
void HostRecv_46Char20(u8* buf)
{
	u8 txt[2][16*2+1];
	u8 clr[2][16*2+1];

	memset(txt,0x20,sizeof(txt));
	memset(clr, (1<<6 | 0<<4), sizeof(clr) );  // default RED

	memcpy(txt[1]+12, buf+ 4, 20);
	memcpy(clr[1]+12, buf+24, 20);

	MatrixConvKssm(txt[1], 32 );
	MatrixSetFrame(2, txt[1], clr[1],32); // front 12bytes skip
}

/*******************************************************************************
* Function Name  : HostRecv_266Banner 
* Description    :
* Input          : None.
* Return         : None.
*******************************************************************************/
void HostRecv_266Banner(u8* buf)
{ 
	u8 txt[2][16*2+1];
	u8 clr[2][16*2+1];

	memset(txt, 0x20, sizeof(txt) );
	memset(clr, (1<<6|0<<4), sizeof(clr) );

    // port1 clear 
	MatrixSetFrame(2, txt,clr,32); // 

    // effect
	g_DotEffect[0].inType = buf[4];
	g_DotEffect[0].delay  = buf[5];
	g_DotEffect[0].outType= buf[6];
	g_DotEffect[0].speed  = buf[7]; 

	memcpy(txt[0],buf+8,  24);
	memcpy(clr[0],buf+136,24);

	// left team Name 	
//	memcpy(txt[0],buf+8,  8);
//	memcpy(clr[0],buf+136,8);
	
	// right team Name 	
//	memcpy(txt[0]+8*2+1,buf+8+8,  8);		
//	memcpy(clr[0]+8*2+1,buf+136+8,8);

	MatrixConvKssm(txt[0], sizeof(txt[0]) );
	MatrixSetFrame(0, txt[0], clr[0], 32);  

}

