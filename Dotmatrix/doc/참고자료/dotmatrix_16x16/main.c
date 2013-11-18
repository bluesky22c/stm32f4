#include "globals.h"

int main(void)
{
	//////////////////////////////////////////////////////////
	// Initialization of peripheral devices in the Atmega128
	InitUsartCh0(SYSTEM_HZ, USART_BPS);
	InitTimer1(SYSTEM_HZ, TIMER_INTERVAL);
	
	InitDotMatixPort();
	InitDotMatrixFrameBuffer(0);
		
	__enable_interrupt();

#ifdef TEST_CPU_USAGE
	InitCpuUsage(); 
#endif	
	
	printf("*********************************\r\n");
	printf("* AVR system start now.\r\n");
	printf("*********************************\r\n");
	
	while(1)
	{
		TestDotMatrix();

#ifdef TEST_CPU_USAGE
		CalcCpuUsage(1);
#endif		
	}
}

