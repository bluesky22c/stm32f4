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

int	m_nKeyStatus;	// �ܾ����ջ���
int	m_nPhonemez[5]; // ����[��,��,��,������1,������2]
u16 m_completeHanCode;
u16 m_ingHanCode;

// �ʼ� �ռ� ���̺�
// ���� : table_cho[] index 
u8 MIXED_CHO_CONSON[19][3] =
{
	{ 2, 2,17}, // ��,��,��  
	{17, 2, 3}, // ��,��,��   
	{ 3, 2, 2}, // ��,��,��
	
    { 4, 4, 7}, // ��,��,��
    { 7, 4, 4}, // ��,��,��

	{ 5, 5,18}, // ��,��,��
	{18, 5, 6}, // ��,��,��
	{ 6, 5, 5}, // ��,��,��
	
	{ 9, 9,19}, // ��,��,��
	{19, 9,10}, // ��,��,��
	{10, 9, 9}, // ��,��,��
	
	{11,11,20}, // ��,��,��
	{20,11,12}, // ��,��,��
	{12,11,11}, // ��,��,��
	
	{14,14,16}, // ��,��,��
	{16,14,15}, // ��,��,��
	{15,14,14}, // ��,��,��
 
    {13,13, 8}, // ��,��
    { 8,13,13}  // ��,��

};

// �ʼ�,�߼� ���� �ռ� ���̺�
// ���� : table_joong[] index+20
int MIXED_VOWEL[22][3] = 
{
	{23,23,25},	// ��,��,��
	{25,23,23},	// ��,��,��
	
	{23,49,24},	// ��,��,��
	{25,49,26},	// ��,��,��
	
	{27,27,31},	// ��,��,��
	{31,27,27},	// ��,��,��
	
	{27,49,30},	// ��,��,��
	{31,49,32},	// ��,��,��

	{33,33,47},	// ��,��,��
	{47,33,39},	// ��,��,��	
	{39,33,33},	// ��,��,��
	
	{33,23,34},	// ��,��,��
	{34,49,35},	// ��,��,��
	
	{33,49,38},	// ��,��,��
	
	{40,40,46},	// ��,��,��
	{46,40,40},	// ��,��,��
	
	{40,27,41},	// ��,��,��
	{41,49,42},	// ��,��,��
	
	{40,49,43},	// ��,��,��
	
	{49,49,47},	// ��,��,��
	{47,49,48},	// ��,��,��
	{48,49,49}	// ��,��,��
};

// ���� �ռ� ���̺�
// ���� : table_jong[] index+50
int MIXED_JONG_CONSON[27][3] = 
{
	{52,52,76}, // ��,��,��
	{76,52,53}, // ��,��,��
	{53,52,52}, // ��,��,��
	
	{52,71,54}, // ��,��,��
	
	{55,74,56}, // ��,��,��
	{55,79,57}, // ��,��,��
	
	{55,55,59}, // ��,��,��
	{59,55,55}, // ��,��,��

	{58,58,77}, // ��,��,��
	{77,58,58}, // ��,��,��
	
	{59,52,60}, // ��,��,��
	{59,67,61}, // ��,��,��
	
	{59,69,62}, // ��,��,��
	{62,69,65}, // ��,��,��
	
	{59,71,63}, // ��,��,��
	{59,58,64}, // ��,��,��	
	{59,79,66}, // ��,��,��
	
	{69,69,78}, // ��,��,��
	{78,69,69}, // ��,��,��
	
	{69,71,70}, // ��,��,��
	
	{71,71,79}, // ��,��,��
	{79,71,72}, // ��,��,��
	{72,71,71}, // ��,��,��
	
	{74,74,75}, // ��,��,��
	{75,74,74},  // ��,��,��

	{73,73,67},	// ��,��,��
	{67,73,73}	// ��,��,��	
};

// ���� ���� ���̺�
// ���� : table_jong[] index+50
int DIVIDE_JONG_CONSON[13][3] = 
{
	{52,52,53}, // ��,��,��
	{52,71,54}, // ��,��,��
	{55,74,56}, // ��,��,��
	{55,79,57}, // ��,��,��
	{59,52,60}, // ��,��,��
	{59,67,61}, // ��,��,��
	{59,69,62}, // ��,��,��
	{59,78,65}, // ��,��,��
	{59,71,63}, // ��,��,��
	{59,77,64}, // ��,��,��	
	{59,79,66}, // ��,��,��
	{69,71,70}, // ��,��,��
	{71,71,72}  // ��,��,��
};

//--------------------------------
u16 CombineHangle(int status);

// �ʼ����� ��ȯ
int ToInitial(int nKeyCode)
{
	switch(nKeyCode)
	{
	case 52: return  2;	// ��
	case 53: return  3;	// ��
	case 55: return  4;	// ��
	case 58: return  5;	// ��
	case 59: return  7;	// ��
	case 67: return  8;	// ��
	case 69: return  9;	// ��
	case 71: return 11;	// ��
	case 72: return 12;	// ��
	case 73: return 13;	// ��
	case 74: return 14;	// ��
	case 75: return 16;	// ��
	case 76: return 17;	// ��
	case 77: return 18;	// ��
	case 78: return 19;	// ��
	case 79: return 20;	// ��
	}
	return -1;
}

// �������� ��ȯ
int ToFinal(int nKeyCode)
{
	switch(nKeyCode)
	{
	case  2: return 52;	// ��
	case  3: return 53;	// ��
	case  4: return 55;	// ��
	case  5: return 58;	// ��
	case  7: return 59;	// ��
	case  8: return 67;	// ��
	case  9: return 69;	// ��
	case 11: return 71;	// ��
	case 12: return 72;	// ��
	case 13: return 73;	// ��
	case 14: return 74;	// ��
	case 16: return 75;	// ��
	case 17: return 76;	// ��
	case 18: return 77;	// ��
	case 19: return 78;	// ��
	case 20: return 79;	// ��
	}
	return -1;
}

// ��������
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

// �ʼ��ռ�
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

// �����ռ�
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

// �����ռ�
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
		//�ʼ�
	case 0: return GetKSSMCode(m_nPhonemez[0],0 ,0);
		//�ʼ� + �߼�
	case 1: return GetKSSMCode(m_nPhonemez[0], m_nPhonemez[1]-20, 0);
		
		//�ʼ� + �߼� + ����
	case 2: return GetKSSMCode(m_nPhonemez[0], m_nPhonemez[1]-20, m_nPhonemez[2]-50);
		
		//�ʼ� + �߼� + ������01
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

	//�۾��� ���� ��� 
	if(cho_5bit==0 && joong_5bit==0 && jong_5bit==0)
	{
		m_nPhonemez[0] = 0;
		m_nPhonemez[1] = 0;
		m_nPhonemez[2] = 0;
		m_ingHanCode = NULL;
		
		return HS_FIRST;
	}

	//�ʼ��� �ִ� ���
	if(cho_5bit>0 && joong_5bit==0 && jong_5bit==0)
	{
		m_nPhonemez[0] = 0;
		m_ingHanCode = NULL;
		
		return HS_FIRST;
	}
	
	m_nPhonemez[0] = cho_5bit+1; //�ʼ�����
	
	if(jong_5bit == 0)	//������ ���� ���
	{
		m_ingHanCode = CombineHangle(0);
		
		return HS_FIRST_V;
	}
	
	m_nPhonemez[1] = joong_5bit+20+2; //�߼�����
	
	jong_5bit += 50;
	
	for(int i = 0; i < 13; i++)
	{
		if(jong_5bit == DIVIDE_JONG_CONSON[i][2])
		{
			m_ingHanCode = CombineHangle(3);
			m_nPhonemez[2] = DIVIDE_JONG_CONSON[i][0]; // ��������
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

	// Ư��Ű �Է�
	if(nKeyCode < 0)
	{
		if(nKeyCode == -3) // ����
		{
			m_nKeyStatus = HS_FIRST;

			m_completeHanCode = m_ingHanCode;
			m_ingHanCode = 0;

	        return m_ingHanCode;
		}
		else if(nKeyCode == -2) // ��������
		{
			
			
		}
		else if(nKeyCode == -1) // �����
		{
			if(m_ingHanCode == NULL)
			{
				// �ϼ��ȹ��ڴ� ���� �����.
				m_completeHanCode=0;
			}
			else
			{
				m_nKeyStatus = DownGradeIngWordStatus(m_ingHanCode);
			}
		}
				
		// ���� ���̴� ���ڻ���
	    CombineIngWord(m_nKeyStatus);

		if(m_completeHanCode==NULL)
			return m_ingHanCode;
		else
		    return m_completeHanCode;
	}	

	switch(m_nKeyStatus)
	{
		case HS_FIRST:
			// �ʼ�
			m_nPhonemez[0]	= nKeyCode;
			m_nKeyStatus	= nKeyCode > 20 ? HS_FIRST_C : HS_FIRST_V;
			break;
		case HS_FIRST_C:
			// ���� + ����
			if(nKeyCode > 20)	// ����
			{
				if(MixVowel(&m_nPhonemez[0], nKeyCode) == FALSE)
				{
					m_completeHanCode = GetKSSMCode(m_nPhonemez[0],0,0);
					m_nPhonemez[0] = nKeyCode;
				}
			}
			else				// ����
			{
				m_completeHanCode = GetKSSMCode(m_nPhonemez[0],0,0);
				m_nPhonemez[0]	= nKeyCode;
				m_nKeyStatus	= HS_FIRST_V;
			}
			break;
	case HS_FIRST_V:
		// ���� + ����
		if(nKeyCode > 20)	// ����
		{
			m_nPhonemez[1]	= nKeyCode;
			m_nKeyStatus	= HS_MIDDLE_STATE;
		}
		else				// ����
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
		// �ʼ� + ���� + ����
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
		// �ʼ� + �߼� + ����
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
		// �ʼ� + �߼� + ����(��) + ����(��)
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
		// �ʼ� + �߼� + ����(������)
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

	// ���� ���̴� ���ڻ���
	CombineIngWord(m_nKeyStatus);
/*
	if(m_nKeyStatus==HS_FIRST)TRACE("HS_FIRST�ʼ�\n");
	if(m_nKeyStatus==HS_FIRST_V)TRACE("HS_FIRST_V���� + ����\n");
	if(m_nKeyStatus==HS_FIRST_C)TRACE("HS_FIRST_C���� + ����\n");
	if(m_nKeyStatus==HS_MIDDLE_STATE)TRACE("HS_MIDDLE_STATE�ʼ� + ���� + ����\n");
	if(m_nKeyStatus==HS_END)TRACE("HS_END�ʼ� + �߼� + ����\n");
	if(m_nKeyStatus==HS_END_STATE)TRACE("HS_END_STATE�ʼ� + �߼� + ���� + ����\n");
	if(m_nKeyStatus==HS_END_EXCEPTION)TRACE("HS_END_EXCEPTION�ʼ� + �߼� + ����(������)\n");
*/
	return m_ingHanCode;
}
