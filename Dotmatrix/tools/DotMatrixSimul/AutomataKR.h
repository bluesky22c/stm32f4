/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AUTOMATAKR_H
#define __AUTOMATAKR_H

enum // 단어조합상태
{
	HS_FIRST = 0,	    // 초성
	HS_FIRST_V,			// 자음 + 자음 
	HS_FIRST_C,			// 모음 + 모음
	HS_MIDDLE_STATE,	// 초성 + 모음 + 모음
	HS_END,				// 초성 + 중성 + 종성
	HS_END_STATE,		// 초성 + 중성 + 자음 + 자음
	HS_END_EXCEPTION	// 초성 + 중성 + 종성(곁자음)
};

u16 CombineKey(int nKeyCode);

u16 GetCompleteHanCode();
u16 GetIngHanCode();
u8 GetHanStatus();
void SetHanStatus(u8 nStatus);
#endif
