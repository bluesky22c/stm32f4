#include "globals.h"

//////////////////////////////////////////////////////////////
//
// Port ����
//
#define OE_PORT		PORTE
#define LATCH_PORT	PORTE
#define RED_PORT	PORTE
#define GREEN_PORT	PORTB
#define CLOCK_PORT	PORTB
#define	ADDR_PORT	PORTB

/////////////////////////////////////////////////////////////
//
// Bit mask ����
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
// Color ����
//
#define	NO_COLOR		0
#define RED_COLOR		1
#define GREEN_COLOR		2
#define ORANGE_COLOR	3

/////////////////////////////////////////////////////////////
//
// Dot matrix ��� width, height ����
//
#define MATRIX_WIDTH	16
#define MATRIX_HEIGHT	16

/////////////////////////////////////////////////////////////
//
// ������ ���� �� ��Ÿ ����
//
static unsigned char g_FrameBuffer[MATRIX_WIDTH * MATRIX_HEIGHT];
static unsigned long g_Line;

/////////////////////////////////////////////////////////////
//
// Dot matrix interface �ʱ�ȭ
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
// ������ ���� �ʱ�ȭ
//
void InitDotMatrixFrameBuffer(unsigned char init_data)
{
	memset(g_FrameBuffer, init_data, sizeof(g_FrameBuffer));	
}

/////////////////////////////////////////////////////////////
//
// Y ���� ����
//
static void SetDotMatrixLine(unsigned char line)
{
	ADDR_PORT &= ~(A3_BIT | A2_BIT | A1_BIT | A0_BIT);
	ADDR_PORT |= line;
}

/////////////////////////////////////////////////////////////
//
//  Color ����
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
// MATRIX_WIDTH �ش��ϴ� ������ ���� ������ ���
// Datasheeet�� Timing chart �κ� ���� �ٶ��ϴ�.
static void OutputFrameBufferLine(unsigned char line)
{
	int i;

	OE_PORT &= ~OE_BIT;
	
	SetDotMatrixLine(line);
	
	if(++line >= MATRIX_HEIGHT) // �� �κ��� timing chart�� ������ ����!!
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
// ������ ����Ʈ 60Hz�� ���߱� ���� 1ms Ÿ�̸� �ڵ鷯���� ȣ��
// 1���� �� 1ms �ֱ�� ��� -> 1������ ��½� 16ms -> �뷫 60Hz 
void OutputFrameBuffer(void)
{
	OutputFrameBufferLine(g_Line);
	if(++g_Line >= MATRIX_HEIGHT)
		g_Line = 0;	
}

///////////////////////////////////////////////////////////
//
// �׽�Ʈ
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
