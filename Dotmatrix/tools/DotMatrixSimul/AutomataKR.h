/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AUTOMATAKR_H
#define __AUTOMATAKR_H

enum // �ܾ����ջ���
{
	HS_FIRST = 0,	    // �ʼ�
	HS_FIRST_V,			// ���� + ���� 
	HS_FIRST_C,			// ���� + ����
	HS_MIDDLE_STATE,	// �ʼ� + ���� + ����
	HS_END,				// �ʼ� + �߼� + ����
	HS_END_STATE,		// �ʼ� + �߼� + ���� + ����
	HS_END_EXCEPTION	// �ʼ� + �߼� + ����(������)
};

u16 CombineKey(int nKeyCode);

u16 GetCompleteHanCode();
u16 GetIngHanCode();
u8 GetHanStatus();
void SetHanStatus(u8 nStatus);
#endif
