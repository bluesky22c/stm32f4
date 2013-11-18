#include "globals.h"

//**********************************************************
// 
// 8bit Timer0�� Fast PWM ���� �ʱ�ȭ
// 
//***********************************************************
void InitPwmOut(unsigned long f_osc)
{
	e_OSC_Freq osc_idx;
	
	osc_idx = GetSystemFreqIndex(f_osc);
	
	//////////////////////////////////////////////
	// Timer0 �ʱ�ȭ
	TCCR0 |= (1<<WGM00) | (1<<WGM01); // Fast PWM ���
	TCCR0 |= (1<<COM00) | (1<<COM01); // Set OC0 on compare match
	
	/////////////////////////////////////////////
	// �ý��� Ŭ�� �ӵ��� ���� ���� ���ֺ� ����
	if(osc_idx == OSC_4MHz)
		TCCR0 |= 1<<CS01; // 8���� -> PWM period = 1/(4MHz/8) x 256 = 500us
	else if(osc_idx == OSC_8MHz)
		TCCR0 |= 1<<CS01; // 8���� -> PWM period = 1/(8MHz/8) x 256 = 256us
	else if(osc_idx == OSC_11MHz)
		TCCR0 |= 1<<CS01 | 1<<CS00; // 32���� -> PWM period = 1/(11.0592MHz/32) x 256 = 739.84us
	else if(osc_idx == OSC_16MHz)
		TCCR0 |= 1<<CS01 | 1<<CS00; // 32���� -> PWM period = 1/(16MHz/32) x 256 =  500us	
		
	OCR0 = 0; // Duty 0%�� ���� OCR �������Ϳ� 0�� ä��
}

//**********************************************************
// 
// PWM Duty ����
// 
//***********************************************************
void SetPwmDuty(unsigned char percent)
{
	OCR0 = (percent*255)/100;
}
