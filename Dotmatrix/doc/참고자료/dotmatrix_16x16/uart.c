#include "globals.h"

const unsigned short UBRR_TBL[OSC_MAX][BPS_MAX] = 
{
	{103, 51, 25, 16, 12, 8, 6, 3, 2, 1, 0, 0}, // OSC = 4MHz
	{207, 103, 51, 34, 25, 16, 12, 8, 6, 3, 1, 1},// OSC = 8MHz
	{287, 143, 71, 47, 35, 23, 17, 11, 8, 5, 2, 2}, // OSC = 11.0592MHz
	{416, 207, 103, 68, 51, 34, 25, 16, 12, 8, 3, 3} // OSC = 16MHz	 
};
 
//**********************************************************
// USART CH0를 활성화 시킵니다.
// 미등록된 f_osc 또는 bps 입력시 atmel에서 제공하는 
// 공식에 의해 UBRR 값 계산  
//***********************************************************
void InitUsartCh0(unsigned long f_osc, unsigned long bps)
{
	int equation_on=0;
	e_OSC_Freq osc_idx;
	e_Bps bps_idx;
	
	//////////////////////////////////////////
	// UBRR table 접근을 위한 해당 인덱스 산출
	osc_idx = GetSystemFreqIndex(f_osc);
	bps_idx = GetBpsIndex(bps);

	//////////////////////////////////////////
	// 미등록된 f_osc 또는 bps 경우 공식에 의해
	// UBRR 값 계산
	if(osc_idx == OSC_MAX || bps_idx == BPS_MAX)
		equation_on = 1;
	
	if(equation_on)
	{
		UCSR0B = (1<<RXEN0) | (1<<TXEN0);
		UBRR0H = (f_osc/(bps*16)-1) >> 8;
		UBRR0L = (f_osc/(bps*16)-1);
	}
	else
	{
		UCSR0B = (1<<RXEN0) | (1<<TXEN0);	
		UBRR0H = UBRR_TBL[osc_idx][bps_idx] >> 8;
		UBRR0L = UBRR_TBL[osc_idx][bps_idx];
	}
}

/**********************************************************
*
* printf() 함수 최종단
*
***********************************************************/
int putchar(int ch)
{
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = (char)ch;
	return ch;
}

