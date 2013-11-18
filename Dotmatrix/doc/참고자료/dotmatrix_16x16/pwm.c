#include "globals.h"

//**********************************************************
// 
// 8bit Timer0을 Fast PWM 모드로 초기화
// 
//***********************************************************
void InitPwmOut(unsigned long f_osc)
{
	e_OSC_Freq osc_idx;
	
	osc_idx = GetSystemFreqIndex(f_osc);
	
	//////////////////////////////////////////////
	// Timer0 초기화
	TCCR0 |= (1<<WGM00) | (1<<WGM01); // Fast PWM 모드
	TCCR0 |= (1<<COM00) | (1<<COM01); // Set OC0 on compare match
	
	/////////////////////////////////////////////
	// 시스템 클럭 속도에 따라 최적 분주비 설정
	if(osc_idx == OSC_4MHz)
		TCCR0 |= 1<<CS01; // 8분주 -> PWM period = 1/(4MHz/8) x 256 = 500us
	else if(osc_idx == OSC_8MHz)
		TCCR0 |= 1<<CS01; // 8분주 -> PWM period = 1/(8MHz/8) x 256 = 256us
	else if(osc_idx == OSC_11MHz)
		TCCR0 |= 1<<CS01 | 1<<CS00; // 32분주 -> PWM period = 1/(11.0592MHz/32) x 256 = 739.84us
	else if(osc_idx == OSC_16MHz)
		TCCR0 |= 1<<CS01 | 1<<CS00; // 32분주 -> PWM period = 1/(16MHz/32) x 256 =  500us	
		
	OCR0 = 0; // Duty 0%를 위해 OCR 레지스터에 0값 채움
}

//**********************************************************
// 
// PWM Duty 설정
// 
//***********************************************************
void SetPwmDuty(unsigned char percent)
{
	OCR0 = (percent*255)/100;
}
