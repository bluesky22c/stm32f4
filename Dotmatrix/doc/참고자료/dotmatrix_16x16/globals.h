/***********************************************************
*
* 전체 소스에 걸쳐 사용되는 헤더파일 및 상수들을 정의 합니다.
*
*************************************************************/
#include <iom128.h>
#include <ina90.h>
#include <stdio.h>
#include "typedef.h"

/////////////////////////////////////////////////////////
// 시스템 설정 관련 옵션
#define SYSTEM_HZ		16000000 // 16MHz
#define USART_BPS		57600	 // 57.6Kbps
#define TIMER_INTERVAL	1000	 // 1ms

////////////////////////////////////////////////////////
// 테스트 옵션
//#define TEST_CPU_USAGE

////////////////////////////////////////////////////////
// 기타
#define FALSE	0
#define TRUE	1

#define Hw0		0x01
#define Hw1		0x02
#define Hw2		0x04
#define Hw3		0x08
#define Hw4		0x10
#define Hw5		0x20
#define Hw6		0x40
#define Hw7		0x80

////////////////////////////////////////////////////////
// 
extern e_OSC_Freq GetSystemFreqIndex(unsigned long f_osc);
extern e_Bps GetBpsIndex(unsigned long bps);
extern e_Interval GetIntervalIndex(unsigned long interval);
extern void InitUsartCh0(unsigned long fosc, unsigned long baud);
extern int InitTimer1(unsigned long f_osc, unsigned long interval);
extern unsigned long GetTimeTick(void);
extern void DelayMs(unsigned long ms);
extern void InitLedPort(unsigned char pin_num);
extern void SetLed(unsigned char pin_num, unsigned char on);
extern void InitCpuUsage(void);
extern int CalcCpuUsage(int src);
extern void LedTaskPB0(void);
extern void LedTaskPB1(void);
extern void LedTaskPB2(void);
extern void InitPwmOut(unsigned long f_osc);
extern void SetPwmDuty(unsigned char percent);
extern void LedFadeInOutTask(void);

extern void InitDotMatixPort(void);
extern void InitDotMatrixFrameBuffer(unsigned char init_data);
extern void OutputFrameBuffer(void);
extern void SetPixel(unsigned char x, unsigned char y, unsigned char color);
extern void TestDotMatrix(void);
