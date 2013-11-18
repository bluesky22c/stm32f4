#include "globals.h"

//////////////////////////////////////////////////////////////
//
// Port 정의
//
#define OE_PORT		PORTE
#define LATCH_PORT	PORTE
#define RED_PORT	PORTE
#define GREEN_PORT	PORTB
#define CLOCK_PORT	PORTB
#define	ADDR_PORT	PORTB

/////////////////////////////////////////////////////////////
//
// Bit mask 정의
//
#define OE_BIT		Hw5
#define LATCH_BIT	Hw6
#define RED_BIT		Hw7
#define CLOCK_BIT	Hw5
#define GREEN_BIT	Hw4
#define A3_BIT		Hw3
#define A2_BIT		Hw2
#define A1_BIT		Hw1
#define A0_BIT		Hw0

/////////////////////////////////////////////////////////////
//
// Color 정의
//
#define	NO_COLOR		0
#define RED_COLOR		1
#define GREEN_COLOR		2
#define ORANGE_COLOR	3

/////////////////////////////////////////////////////////////
//
// Dot matrix 모듈 width, height 정의
//
#define MATRIX_WIDTH	16
#define MATRIX_HEIGHT	16

/////////////////////////////////////////////////////////////
//
// 프레임 버퍼 및 기타 변수
//
static unsigned char g_FrameBuffer[MATRIX_WIDTH * MATRIX_HEIGHT];
static unsigned long g_Line;

/////////////////////////////////////////////////////////////
//
// Dot matrix interface 초기화
//
void InitDotMatixPort(void)
{
	DDRB |= (GREEN_BIT | CLOCK_BIT | A3_BIT | A2_BIT | A1_BIT | A0_BIT);
	DDRE |= (OE_BIT	| LATCH_BIT | RED_BIT);
	
	CLOCK_PORT &= ~CLOCK_BIT;
	LATCH_PORT &= ~LATCH_BIT;
	OE_PORT &= ~OE_BIT;
	ADDR_PORT &= ~(A3_BIT | A2_BIT | A1_BIT | A0_BIT);
}

/////////////////////////////////////////////////////////////
//
// 프레임 버퍼 초기화
//
void InitDotMatrixFrameBuffer(unsigned char init_data)
{
	memset(g_FrameBuffer, init_data, sizeof(g_FrameBuffer));	
}

/////////////////////////////////////////////////////////////
//
// Y 라인 지정
//
static void SetDotMatrixLine(unsigned char line)
{
	ADDR_PORT &= ~(A3_BIT | A2_BIT | A1_BIT | A0_BIT);
	ADDR_PORT |= line;
}

/////////////////////////////////////////////////////////////
//
//  Color 지정
//
static void SetDotMatrixColor(unsigned char data)
{
	if(data == NO_COLOR)
	{
		RED_PORT &= ~RED_BIT;
		GREEN_PORT &= ~GREEN_BIT; 	
	}	
	else if(data == RED_COLOR)
	{
		RED_PORT |= RED_BIT;
		GREEN_PORT &= ~GREEN_BIT; 	
	}
	else if(data == GREEN_COLOR)
	{
		RED_PORT &= ~RED_BIT;
		GREEN_PORT |= GREEN_BIT; 	
	}
	else if(data == ORANGE_COLOR)
	{
		RED_PORT |= RED_BIT;
		GREEN_PORT |= GREEN_BIT; 	
	}	
}

/////////////////////////////////////////////////////////////
//
// MATRIX_WIDTH 해당하는 프레임 버퍼 데이터 출력
// Datasheeet의 Timing chart 부분 참고 바랍니다.
static void OutputFrameBufferLine(unsigned char line)
{
	int i;

	OE_PORT &= ~OE_BIT;
	
	SetDotMatrixLine(line);
	
	if(++line >= MATRIX_HEIGHT) // 이 부분은 timing chart를 유심히 볼것!!
		line = 0;				 
		
	for(i=0; i<MATRIX_WIDTH; i++)
	{
		SetDotMatrixColor(g_FrameBuffer[(line*MATRIX_WIDTH)+i]);	
		
		CLOCK_PORT |= CLOCK_BIT;
		CLOCK_PORT &= ~CLOCK_BIT;	
	}
	
	LATCH_PORT |= LATCH_BIT;
	LATCH_PORT &= ~LATCH_BIT;
	
	OE_PORT |= OE_BIT;
}

/////////////////////////////////////////////////////////////
//
// 프레임 레이트 60Hz를 맞추기 위해 1ms 타이머 핸들러에서 호출
// 1라인 당 1ms 주기로 출력 -> 1프레임 출력시 16ms -> 대략 60Hz 
void OutputFrameBuffer(void)
{
	OutputFrameBufferLine(g_Line);
	if(++g_Line >= MATRIX_HEIGHT)
		g_Line = 0;	
}

///////////////////////////////////////////////////////////
//
// 테스트
//
void SetPixel(unsigned char x, unsigned char y, unsigned char color)
{
	g_FrameBuffer[(y*MATRIX_WIDTH)+x] = color;	
}

unsigned long g_TestTimer, g_X, g_Y;
unsigned char g_Color=RED_COLOR;

void TestDotMatrix(void)
{
	if(GetTimeTick() - g_TestTimer >= 62)
	{
		SetPixel(g_X, g_Y, g_Color);
			
		if(++g_X >= MATRIX_WIDTH)
		{
			g_X = 0;
			
			if(++g_Y >= MATRIX_HEIGHT)
			{
				g_Y = 0;
					
				if(g_Color == RED_COLOR)
					g_Color = GREEN_COLOR;
				else if(g_Color == GREEN_COLOR)
					g_Color = RED_COLOR;		
			}
		}
		g_TestTimer = GetTimeTick();	
	}
}
