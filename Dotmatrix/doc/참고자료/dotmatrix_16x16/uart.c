#include "globals.h"

const unsigned short UBRR_TBL[OSC_MAX][BPS_MAX] = 
{
	{103, 51, 25, 16, 12, 8, 6, 3, 2, 1, 0, 0}, // OSC = 4MHz
	{207, 103, 51, 34, 25, 16, 12, 8, 6, 3, 1, 1},// OSC = 8MHz
	{287, 143, 71, 47, 35, 23, 17, 11, 8, 5, 2, 2}, // OSC = 11.0592MHz
	{416, 207, 103, 68, 51, 34, 25, 16, 12, 8, 3, 3} // OSC = 16MHz	 
};
 
//**********************************************************
// USART CH0�� Ȱ��ȭ ��ŵ�ϴ�.
// �̵�ϵ� f_osc �Ǵ� bps �Է½� atmel���� �����ϴ� 
// ���Ŀ� ���� UBRR �� ���  
//***********************************************************
void InitUsartCh0(unsigned long f_osc, unsigned long bps)
{
	int equation_on=0;
	e_OSC_Freq osc_idx;
	e_Bps bps_idx;
	
	//////////////////////////////////////////
	// UBRR table ������ ���� �ش� �ε��� ����
	osc_idx = GetSystemFreqIndex(f_osc);
	bps_idx = GetBpsIndex(bps);

	//////////////////////////////////////////
	// �̵�ϵ� f_osc �Ǵ� bps ��� ���Ŀ� ����
	// UBRR �� ���
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
* printf() �Լ� ������
*
***********************************************************/
int putchar(int ch)
{
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = (char)ch;
	return ch;
}

