#include "globals.h"

const unsigned short TCNT_16BIT_TBL[OSC_MAX][INTERVAL_MAX] = 
{
//  100us  500us   1ms
	{65136, 63536, 61536}, // 4MHz (No prescaling)
	{64736, 61536, 57536}, // 8MHz (No prescaling)
	{64431, 60007, 54477}, // 11.0592MHz (No prescaling)
	{63936, 57536, 49536}  // 16MHz (No prescaling)
};

static unsigned char g_TcntHigh, g_TcntLow;
static volatile unsigned long g_TimeTick;

//**********************************************************
// 
// 16bit Timer1을 Normal 모드로 초기화
// 
//***********************************************************
int InitTimer1(unsigned long f_osc, unsigned long interval) 
{
	e_OSC_Freq osc_idx;
	e_Interval interval_idx;
	
	///////////////////////////////////////////////
	// TCNT table 접근을 위해 해당 인덱스 산출 
	osc_idx = GetSystemFreqIndex(f_osc);
	interval_idx = GetIntervalIndex(interval);
	
	//////////////////////////////////////////////
	// 지원 불가능한 f_osc, interval 선택시
	if(osc_idx == OSC_MAX || interval_idx == INTERVAL_MAX)
		return -1;
	
	//////////////////////////////////////////////
	// TCNT table의 값을 전역변수에 저장	
	g_TcntHigh = (unsigned char)((TCNT_16BIT_TBL[osc_idx][interval_idx] & 0xff00) >> 8);
	g_TcntLow = (unsigned char)(TCNT_16BIT_TBL[osc_idx][interval_idx] & 0xff);

	//////////////////////////////////////////////
	// Timer1 초기화	
	TCCR1B = 1<<CS10; // No prescaling, Normal Mode, 
	TCNT1H = g_TcntHigh;
	TCNT1L = g_TcntLow;
	TIMSK |= 1<<TOIE1;
	
	return 1;		
}

//**********************************************************
// 
// 외부에서 tick 값 참조
// 
//***********************************************************
unsigned long GetTimeTick(void)
{
	return (g_TimeTick);	
}

//**********************************************************
// 
// ms 단위 시간지연
// 
//***********************************************************
void DelayMs(unsigned long ms)
{
	unsigned long tick;
	
	tick = g_TimeTick;
	while(g_TimeTick - tick < ms);
}

//**********************************************************
// 
// Timer1 overflow interrupt service routine
// 
//***********************************************************
#pragma vector=TIMER1_OVF_vect
__interrupt void Timer1OVFIsr(void)
{
	g_TimeTick++;
	
	OutputFrameBuffer();
	
	TCNT1H = g_TcntHigh;
	TCNT1L = g_TcntLow;	
}
