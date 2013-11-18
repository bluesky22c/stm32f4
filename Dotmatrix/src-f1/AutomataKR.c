/******************** (C) COPYRIGHT 2012 OP ***********************************
* File Name          : AutomataKR.c
* Author             : bluesky22c
* Version            : V1.1.1
* Date               : 01/03/2012
* Description        : KSSM Automata Program.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "AutomataKR.h"
#include "font_kor.h"

int	m_nKeyStatus;	// 단어조합상태
int	m_nPhonemez[5]; // 음소[초,중,종,곁자음1,곁자음2]
u16 m_completeHanCode;
u16 m_ingHanCode;

// 초성 합성 테이블
// 숫자 : table_cho[] index 
u8 MIXED_CHO_CONSON[19][3] =
{
	{ 2, 2,17}, // ㄱ,ㄱ,ㅋ  
	{17, 2, 3}, // ㅋ,ㄱ,ㄲ   
	{ 3, 2, 2}, // ㄲ,ㄱ,ㄱ
	
    { 4, 4, 7}, // ㄴ,ㄴ,ㄹ
    { 7, 4, 4}, // ㄹ,ㄴ,ㄴ

	{ 5, 5,18}, // ㄷ,ㄷ,ㅌ
	{18, 5, 6}, // ㅌ,ㄷ,ㄸ
	{ 6, 5, 5}, // ㄸ,ㄷ,ㄷ
	
	{ 9, 9,19}, // ㅂ,ㅂ,ㅍ
	{19, 9,10}, // ㅍ,ㅂ,ㅃ
	{10, 9, 9}, // ㅃ,ㅂ,ㅂ
	
	{11,11,20}, // ㅅ,ㅅ,ㅎ
	{20,11,12}, // ㅎ,ㅅ,ㅆ
	{12,11,11}, // ㅆ,ㅅ,ㅅ
	
	{14,14,16}, // ㅈ,ㅈ,ㅊ
	{16,14,15}, // ㅊ,ㅈ,ㅉ
	{15,14,14}, // ㅉ,ㅈ,ㅈ
 
    {13,13, 8}, // ㅇ,ㅁ
    { 8,13,13}  // ㅁ,ㅇ

};

// 초성,중성 모음 합성 테이블
// 숫자 : table_joong[] index+20
int MIXED_VOWEL[22][3] = 
{
	{23,23,25},	// ㅏ,ㅏ,ㅑ
	{25,23,23},	// ㅑ,ㅏ,ㅏ
	
	{23,49,24},	// ㅏ,ㅣ,ㅐ
	{25,49,26},	// ㅑ,ㅣ,ㅒ
	
	{27,27,31},	// ㅓ,ㅓ,ㅕ
	{31,27,27},	// ㅕ,ㅓ,ㅓ
	
	{27,49,30},	// ㅓ,ㅣ,ㅔ
	{31,49,32},	// ㅕ,ㅣ,ㅖ

	{33,33,47},	// ㅗ,ㅗ,ㅡ
	{47,33,39},	// ㅡ,ㅗ,ㅛ	
	{39,33,33},	// ㅛ,ㅗ,ㅗ
	
	{33,23,34},	// ㅗ,ㅏ,ㅘ
	{34,49,35},	// ㅘ,ㅣ,ㅙ
	
	{33,49,38},	// ㅗ,ㅣ,ㅚ
	
	{40,40,46},	// ㅜ,ㅜ,ㅠ
	{46,40,40},	// ㅠ,ㅜ,ㅜ
	
	{40,27,41},	// ㅜ,ㅓ,ㅝ
	{41,49,42},	// ㅝ,ㅣ,ㅞ
	
	{40,49,43},	// ㅜ,ㅣ,ㅟ
	
	{49,49,47},	// ㅣ,ㅣ,ㅡ
	{47,49,48},	// ㅡ,ㅣ,ㅢ
	{48,49,49}	// ㅢ,ㅣ,ㅣ
};

// 종성 합성 테이블
// 숫자 : table_jong[] index+50
int MIXED_JONG_CONSON[27][3] = 
{
	{52,52,76}, // ㄱ,ㄱ,ㅋ
	{76,52,53}, // ㅋ,ㄱ,ㄲ
	{53,52,52}, // ㄲ,ㄱ,ㄱ
	
	{52,71,54}, // ㄱ,ㅅ,ㄳ
	
	{55,74,56}, // ㄴ,ㅈ,ㄵ
	{55,79,57}, // ㄴ,ㅎ,ㄶ
	
	{55,55,59}, // ㄴ,ㄴ,ㄹ
	{59,55,55}, // ㄹ,ㄴ,ㄴ

	{58,58,77}, // ㄷ,ㄷ,ㅌ
	{77,58,58}, // ㅌ,ㄷ,ㄷ
	
	{59,52,60}, // ㄹ,ㄱ,ㄺ
	{59,67,61}, // ㄹ,ㅁ,ㄻ
	
	{59,69,62}, // ㄹ,ㅂ,ㄼ
	{62,69,65}, // ㄼ,ㅂ,ㄿ
	
	{59,71,63}, // ㄹ,ㅅ,ㄽ
	{59,58,64}, // ㄹ,ㄷ,ㄾ	
	{59,79,66}, // ㄹ,ㅎ,ㅀ
	
	{69,69,78}, // ㅂ,ㅂ,ㅍ
	{78,69,69}, // ㅍ,ㅂ,ㅂ
	
	{69,71,70}, // ㅂ,ㅅ,ㅄ
	
	{71,71,79}, // ㅅ,ㅅ,ㅎ
	{79,71,72}, // ㅎ,ㅅ,ㅆ
	{72,71,71}, // ㅆ,ㅅ,ㅅ
	
	{74,74,75}, // ㅈ,ㅈ,ㅊ
	{75,74,74},  // ㅊ,ㅈ,ㅈ

	{73,73,67},	// ㅇ,ㅇ,ㅁ
	{67,73,73}	// ㅁ,ㅇ,ㅇ	
};

// 종성 분해 테이블
// 숫자 : table_jong[] index+50
int DIVIDE_JONG_CONSON[13][3] = 
{
	{52,52,53}, // ㄱ,ㄱ,ㄲ
	{52,71,54}, // ㄱ,ㅅ,ㄳ
	{55,74,56}, // ㄴ,ㅈ,ㄵ
	{55,79,57}, // ㄴ,ㅎ,ㄶ
	{59,52,60}, // ㄹ,ㄱ,ㄺ
	{59,67,61}, // ㄹ,ㅁ,ㄻ
	{59,69,62}, // ㄹ,ㅂ,ㄼ
	{59,78,65}, // ㄹ,ㅍ,ㄿ
	{59,71,63}, // ㄹ,ㅅ,ㄽ
	{59,77,64}, // ㄹ,ㅌ,ㄾ	
	{59,79,66}, // ㄹ,ㅎ,ㅀ
	{69,71,70}, // ㅂ,ㅅ,ㅄ
	{71,71,72}  // ㅅ,ㅅ,ㅆ
};

//--------------------------------
u16 CombineHangle(int status);

// 초성으로 변환
int ToInitial(int nKeyCode)
{
	switch(nKeyCode)
	{
	case 52: return  2;	// ㄱ
	case 53: return  3;	// ㄲ
	case 55: return  4;	// ㄴ
	case 58: return  5;	// ㄷ
	case 59: return  7;	// ㄹ
	case 67: return  8;	// ㅁ
	case 69: return  9;	// ㅂ
	case 71: return 11;	// ㅅ
	case 72: return 12;	// ㅆ
	case 73: return 13;	// ㅇ
	case 74: return 14;	// ㅈ
	case 75: return 16;	// ㅊ
	case 76: return 17;	// ㅋ
	case 77: return 18;	// ㅌ
	case 78: return 19;	// ㅍ
	case 79: return 20;	// ㅎ
	}
	return -1;
}

// 종성으로 변환
int ToFinal(int nKeyCode)
{
	switch(nKeyCode)
	{
	case  2: return 52;	// ㄱ
	case  3: return 53;	// ㄲ
	case  4: return 55;	// ㄴ
	case  5: return 58;	// ㄷ
	case  7: return 59;	// ㄹ
	case  8: return 67;	// ㅁ
	case  9: return 69;	// ㅂ
	case 11: return 71;	// ㅅ
	case 12: return 72;	// ㅆ
	case 13: return 73;	// ㅇ
	case 14: return 74;	// ㅈ
	case 16: return 75;	// ㅊ
	case 17: return 76;	// ㅋ
	case 18: return 77;	// ㅌ
	case 19: return 78;	// ㅍ
	case 20: return 79;	// ㅎ
	}
	return -1;
}

// 자음분해
void DecomposeConsonant()
{
	int i = 0;
	if(m_nPhonemez[3] > 50 || m_nPhonemez[4] > 50)
	{
		do
		{
			if(DIVIDE_JONG_CONSON[i][2] == m_nPhonemez[2])
			{
				m_nPhonemez[3] = DIVIDE_JONG_CONSON[i][0];
				m_nPhonemez[4] = DIVIDE_JONG_CONSON[i][1];
				
				m_completeHanCode = CombineHangle(3);
				m_nPhonemez[0]	 = ToInitial(m_nPhonemez[4]);
				return;
			}
		}
		while(++i< 13);
	}
	
	m_completeHanCode = CombineHangle(1);
	m_nPhonemez[0]	 = ToInitial(m_nPhonemez[2]);
}

// 초성합성
bool MixInitial(int nKeyCode)
{
	if(nKeyCode >= 21)
		return FALSE;
	
	int i = 0;
	do
	{
		if(MIXED_CHO_CONSON[i][0] == m_nPhonemez[0] && MIXED_CHO_CONSON[i][1] == nKeyCode)
		{
			m_nPhonemez[0] = MIXED_CHO_CONSON[i][2];
			return TRUE;
		}
	}
	while(++i < 19);
	
	return FALSE;
}

// 종성합성
bool MixFinal(int nKeyCode)
{
	if(nKeyCode <= 50) return FALSE;
	
	int i = 0;
	do
	{
		if(MIXED_JONG_CONSON[i][0] == m_nPhonemez[2] && MIXED_JONG_CONSON[i][1] == nKeyCode)
		{
			m_nPhonemez[3] = m_nPhonemez[2];
			m_nPhonemez[4] = nKeyCode;
			m_nPhonemez[2] = MIXED_JONG_CONSON[i][2];
			
			return TRUE;
		}
	}
	while(++i < 27);
	
	return FALSE;
}

// 모음합성
bool MixVowel(int * currentCode, int inputCode)
{
	int i = 0;
	do
	{
		if(MIXED_VOWEL[i][0] == * currentCode && MIXED_VOWEL[i][1] == inputCode)
		{
			* currentCode = MIXED_VOWEL[i][2];
			return TRUE;
		}
	}
	while(++i< 22);
	
	return FALSE;
}

u16 CombineHangle(int status)
{
	switch(status)
	{
		//초성
	case 0: return GetKSSMCode(m_nPhonemez[0],0 ,0);
		//초성 + 중성
	case 1: return GetKSSMCode(m_nPhonemez[0], m_nPhonemez[1]-20, 0);
		
		//초성 + 중성 + 종성
	case 2: return GetKSSMCode(m_nPhonemez[0], m_nPhonemez[1]-20, m_nPhonemez[2]-50);
		
		//초성 + 중성 + 곁자음01
	case 3: return GetKSSMCode(m_nPhonemez[0], m_nPhonemez[1]-20, m_nPhonemez[3]-50);
	}
	return 0;
}

void CombineIngWord(int status)
{
	switch(status)
	{
	case HS_FIRST:
	case HS_FIRST_V:
	case HS_FIRST_C:
		m_ingHanCode = CombineHangle(0);
		break;
		
	case HS_MIDDLE_STATE:
	case HS_END:
		m_ingHanCode = CombineHangle(1);
		break;
		
	case HS_END_STATE:
	case HS_END_EXCEPTION:
		m_ingHanCode = CombineHangle(2);
		break;
	}
}

int DownGradeIngWordStatus(u16 code)
{	
	u8 cho_5bit, joong_5bit, jong_5bit;
	
	cho_5bit   = (table_cho[(code >> 10) & 0x001F]);  // get 5bit(14-10) of chosung
	joong_5bit = (table_joong[(code >> 5) & 0x001F]); // get 5bit(09-05) of joongsung
    jong_5bit  = (table_jong[code & 0x001F]);	      // get 5bit(04-00) of jongsung

	//글씨가 없는 경우 
	if(cho_5bit==0 && joong_5bit==0 && jong_5bit==0)
	{
		m_nPhonemez[0] = 0;
		m_nPhonemez[1] = 0;
		m_nPhonemez[2] = 0;
		m_ingHanCode = NULL;
		
		return HS_FIRST;
	}

	//초성만 있는 경우
	if(cho_5bit>0 && joong_5bit==0 && jong_5bit==0)
	{
		m_nPhonemez[0] = 0;
		m_ingHanCode = NULL;
		
		return HS_FIRST;
	}
	
	m_nPhonemez[0] = cho_5bit+1; //초성저장
	
	if(jong_5bit == 0)	//종성이 없는 경우
	{
		m_ingHanCode = CombineHangle(0);
		
		return HS_FIRST_V;
	}
	
	m_nPhonemez[1] = joong_5bit+20+2; //중성저장
	
	jong_5bit += 50;
	
	for(int i = 0; i < 13; i++)
	{
		if(jong_5bit == DIVIDE_JONG_CONSON[i][2])
		{
			m_ingHanCode = CombineHangle(3);
			m_nPhonemez[2] = DIVIDE_JONG_CONSON[i][0]; // 종성저장
			return HS_END_EXCEPTION;
		}
	}
	
	m_ingHanCode = CombineHangle(1);
	
	return HS_MIDDLE_STATE;
}

//------------------------------
// return KeyStatus
//------------------------------
u8 GetHanStatus()
{
	return m_nKeyStatus;
}

//------------------------------
// Set KeyStatus
//------------------------------
void SetHanStatus(u8 nStatus)
{
	m_nKeyStatus = nStatus;
}

//------------------------------
// return m_ingHanCode
//------------------------------
u16 GetIngHanCode()
{
	return m_ingHanCode;
}

//------------------------------
// return m_completeHanCode
//------------------------------
u16 GetCompleteHanCode()
{
	return m_completeHanCode;
}

//------------------------------
// return KSSM Code
//------------------------------
u16 CombineKey(int nKeyCode)
{
	m_completeHanCode = NULL;

	// 특수키 입력
	if(nKeyCode < 0)
	{
		if(nKeyCode == -3) // 띄어쓰기
		{
			m_nKeyStatus = HS_FIRST;

			m_completeHanCode = m_ingHanCode;
			m_ingHanCode = 0;

	        return m_ingHanCode;
		}
		else if(nKeyCode == -2) // 내려쓰기
		{
			
			
		}
		else if(nKeyCode == -1) // 지우기
		{
			if(m_ingHanCode == NULL)
			{
				// 완성된문자는 전부 지운다.
				m_completeHanCode=0;
			}
			else
			{
				m_nKeyStatus = DownGradeIngWordStatus(m_ingHanCode);
			}
		}
				
		// 현재 보이는 글자상태
	    CombineIngWord(m_nKeyStatus);

		if(m_completeHanCode==NULL)
			return m_ingHanCode;
		else
		    return m_completeHanCode;
	}	

	switch(m_nKeyStatus)
	{
		case HS_FIRST:
			// 초성
			m_nPhonemez[0]	= nKeyCode;
			m_nKeyStatus	= nKeyCode > 20 ? HS_FIRST_C : HS_FIRST_V;
			break;
		case HS_FIRST_C:
			// 모음 + 모음
			if(nKeyCode > 20)	// 모음
			{
				if(MixVowel(&m_nPhonemez[0], nKeyCode) == FALSE)
				{
					m_completeHanCode = GetKSSMCode(m_nPhonemez[0],0,0);
					m_nPhonemez[0] = nKeyCode;
				}
			}
			else				// 자음
			{
				m_completeHanCode = GetKSSMCode(m_nPhonemez[0],0,0);
				m_nPhonemez[0]	= nKeyCode;
				m_nKeyStatus	= HS_FIRST_V;
			}
			break;
	case HS_FIRST_V:
		// 자음 + 자음
		if(nKeyCode > 20)	// 모음
		{
			m_nPhonemez[1]	= nKeyCode;
			m_nKeyStatus	= HS_MIDDLE_STATE;
		}
		else				// 자음
		{
			if(!MixInitial(nKeyCode))
			{
				m_completeHanCode	= GetKSSMCode(m_nPhonemez[0],0,0);
				m_nPhonemez[0]	= nKeyCode;
				m_nKeyStatus	= HS_FIRST_V;
			}
		}
		break;

	case HS_MIDDLE_STATE:
		// 초성 + 모음 + 모음
		if(nKeyCode > 20)
		{
			if(MixVowel(&m_nPhonemez[1], nKeyCode) == FALSE)
			{
				m_completeHanCode	= CombineHangle(1);
				m_nPhonemez[0]	= nKeyCode;
				m_nKeyStatus	= HS_FIRST_C;
			}
		}
		else
		{
			int jungCode = ToFinal(nKeyCode);

			if(jungCode > 0)
			{
				m_nPhonemez[2]	= jungCode;
				m_nKeyStatus	= HS_END_STATE;
			}
			else
			{
				m_completeHanCode	= CombineHangle(1);
				m_nPhonemez[0]	= nKeyCode;
				m_nKeyStatus	= HS_FIRST;
			}
		}
		break;

	case HS_END:
		// 초성 + 중성 + 종성
		if(nKeyCode > 20)  
		{
			m_completeHanCode	= CombineHangle(1);
			m_nPhonemez[0]	= nKeyCode;
			m_nKeyStatus	= HS_FIRST;
		}
		else
		{
			int jungCode = ToFinal(nKeyCode);
			if(jungCode > 0)
			{
				m_nPhonemez[2]	= jungCode;
				m_nKeyStatus		= HS_END_STATE;
			}
			else
			{
				m_completeHanCode	= CombineHangle(1);
				m_nPhonemez[0]	= nKeyCode;
				m_nKeyStatus	= HS_FIRST;
			}  
		}
		break;

	case HS_END_STATE:
		// 초성 + 중성 + 자음(종) + 자음(종)
		if(nKeyCode > 20)
		{
			m_completeHanCode = CombineHangle(1);

			m_nPhonemez[0]	= ToInitial(m_nPhonemez[2]);
			m_nPhonemez[1]	= nKeyCode;
			m_nKeyStatus	= HS_MIDDLE_STATE;
		}
		else
		{
			int jungCode = ToFinal(nKeyCode);
			if(jungCode > 0)
			{
				m_nKeyStatus = HS_END_EXCEPTION;

				if(!MixFinal(jungCode))
				{
					m_completeHanCode	= CombineHangle(2);
					m_nPhonemez[0]	= nKeyCode;
					m_nKeyStatus	= HS_FIRST_V;
				}
			}
			else
			{
				m_completeHanCode	= CombineHangle(2);
				m_nPhonemez[0]	= nKeyCode;
				m_nKeyStatus	= HS_FIRST_V;
			}  
		}
		break;

	case HS_END_EXCEPTION:
		// 초성 + 중성 + 종성(곁자음)
		if(nKeyCode > 20)  
		{
			DecomposeConsonant();
			m_nPhonemez[1]	= nKeyCode;
			m_nKeyStatus		= HS_MIDDLE_STATE;
		}
		else
		{
			int jungCode = ToFinal(nKeyCode);
			if(jungCode > 0)
			{
				m_nKeyStatus = HS_END_EXCEPTION;

				if(!MixFinal(jungCode))
				{
					m_completeHanCode	= CombineHangle(2);
					m_nPhonemez[0]	= nKeyCode;
					m_nKeyStatus		= HS_FIRST_V;
				}
			}
			else
			{
				m_completeHanCode	= CombineHangle(2);
				m_nPhonemez[0]	= nKeyCode;
				m_nKeyStatus		= HS_FIRST_V; 
			}
		}
		break;
	}

	// 현재 보이는 글자상태
	CombineIngWord(m_nKeyStatus);
/*
	if(m_nKeyStatus==HS_FIRST)TRACE("HS_FIRST초성\n");
	if(m_nKeyStatus==HS_FIRST_V)TRACE("HS_FIRST_V자음 + 자음\n");
	if(m_nKeyStatus==HS_FIRST_C)TRACE("HS_FIRST_C모음 + 모음\n");
	if(m_nKeyStatus==HS_MIDDLE_STATE)TRACE("HS_MIDDLE_STATE초성 + 모음 + 모음\n");
	if(m_nKeyStatus==HS_END)TRACE("HS_END초성 + 중성 + 종성\n");
	if(m_nKeyStatus==HS_END_STATE)TRACE("HS_END_STATE초성 + 중성 + 자음 + 자음\n");
	if(m_nKeyStatus==HS_END_EXCEPTION)TRACE("HS_END_EXCEPTION초성 + 중성 + 종성(곁자음)\n");
*/
	return m_ingHanCode;
}
