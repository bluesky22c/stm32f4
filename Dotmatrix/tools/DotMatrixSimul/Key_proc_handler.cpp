#include "StdAfx.h"
#include "Key_proc_handler.h"
#include "AutomataKR.h"
#include "Matrix_proc.h"
#include "font_kor.h"

enum CHAR_STATE		
{
	CHAR_STATE_HAN = 0,	    
	CHAR_STATE_ENG,					
	CHAR_STATE_eng,			
	CHAR_STATE_NUM	
};

u8 m_nKeyKind = CHAR_STATE_HAN;
u8 m_nCharStatus[2]={HS_FIRST,};
int m_nSelPort = 0;

u8 text[16*2+1];
u8 colr[16*2+1];

extern u8 g_CursorPos;
extern bool g_bCursor2Byte;
extern u8 g_CursorFlag[4];
extern u8 g_FrameText[4][33];
extern u8 g_FrameColr[4][33];

void KeyProc_Init_Handler()
{
	memcpy(text, g_FrameText[m_nSelPort], sizeof(text));
	memcpy(colr, g_FrameColr[m_nSelPort], sizeof(colr));

	g_CursorPos=0;
	SetHanStatus(HS_FIRST);
	m_nCharStatus[0] = 0;
	m_nCharStatus[1] = 0;

    g_bCursor2Byte = (text[g_CursorPos]<0x80) ? FALSE : TRUE;  
	  
	g_CursorFlag[m_nSelPort] = !g_CursorFlag[m_nSelPort];
}

void KeyProc_Key_Handler(int nKey)
{
	if(nKey==KEY_POWER){
		
		return;
	}

	if(nKey==KEY_MENU){
		KeyProc_Init_Handler();
		return;
	}

	if(nKey==KEY_STATE_HAN){
		m_nKeyKind=CHAR_STATE_HAN;
		return;
	}
	if(nKey==KEY_STATE_ENG){
		m_nKeyKind=CHAR_STATE_ENG;
		return;
	}
	if(nKey==KEY_STATE_eng){
		m_nKeyKind=CHAR_STATE_eng;
		return;
	}
	if(nKey==KEY_STATE_NUM){
		m_nKeyKind=CHAR_STATE_NUM;
		return;
	}

	if(g_CursorFlag[m_nSelPort]==0)
		return;

	if(nKey==KEY_LEFT){
		byte high=0x0, low=0x0;

		if(g_CursorPos>0) low  = text[g_CursorPos-1];		
		if(g_CursorPos>1) high = text[g_CursorPos-2];		
        
		if(g_CursorPos>1 && KSSMtoIndex((high<<8)+(low&0xff))!=-1){ // previous char is 2byte char
            g_CursorPos-=2;
		}else if(g_CursorPos>0){
			g_CursorPos-=1;
		}

		SetHanStatus(HS_FIRST);
		m_nCharStatus[0] = 0;
		m_nCharStatus[1] = 0;

		g_bCursor2Byte = (text[g_CursorPos]<0x80) ? FALSE : TRUE;

		return;
	}

	if(nKey==KEY_RIGHT){
		if(text[g_CursorPos]>=0x80){ // previous char is 2byte char
			if(g_CursorPos< (DOT_MAX_MOD*2-2) )
				g_CursorPos+=2;
		}else{
			if(g_CursorPos< (DOT_MAX_MOD*2-1) )
				g_CursorPos+=1;
		}
	    
		SetHanStatus(HS_FIRST);
		m_nCharStatus[0] = 0;
		m_nCharStatus[1] = 0;

		g_bCursor2Byte = (text[g_CursorPos]<0x80) ? FALSE : TRUE;

		return;
	}

	if(nKey==KEY_UP){
		memset(g_CursorFlag,0,sizeof(g_CursorFlag));

		m_nSelPort--;
		if(m_nSelPort<0)
			m_nSelPort=3;

        KeyProc_Init_Handler();
		
		return;
	}

	if(nKey==KEY_DOWN){
		memset(g_CursorFlag,0,sizeof(g_CursorFlag));

		m_nSelPort++;
		if(m_nSelPort>3)
			m_nSelPort=0;

		KeyProc_Init_Handler();
		
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
		return;
	}

	// 이전글씨삭제
	if(nKey==KEY_DEL){
		if(m_nKeyKind==CHAR_STATE_HAN &&GetHanStatus()!=HS_FIRST){
		
		}else{
			byte high=0x0, low=0x0;
			
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
		if( completeCode && (m_nCharStatus[0]==HS_END_STATE||m_nCharStatus[0]==HS_END_EXCEPTION)&&
			(m_nCharStatus[1]==HS_FIRST||m_nCharStatus[1]==HS_FIRST_V||m_nCharStatus[1]==HS_MIDDLE_STATE) ){
			
			text[g_CursorPos+0] = (u8)(completeCode>>8);
			text[g_CursorPos+1] = (u8)completeCode;
			
			if(g_CursorPos<(DOT_MAX_MOD*2-2))
			  g_CursorPos+=2;			

			g_bCursor2Byte = true;
		}		
			
		text[g_CursorPos+0] = (u8)(ingCode>>8);
		text[g_CursorPos+1] = (u8)ingCode;
		
	    MatrixSetFrame(m_nSelPort, text,colr,32);

		return;
	}
	
}