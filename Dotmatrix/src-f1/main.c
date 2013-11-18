/******************** (C) COPYRIGHT 2011 bluesky22c*******************************
* File Name         : main.c
* Author             : bluesky22c
* Version            : V1.0.0
* Date                : 11/12/2011
* Description       : Main program body
* MCU                : STM32F103RET6 (flash=512Kbytes, RAM=64Kbytes)
********************************************************************************
* 
*******************************************************************************/
/*
 Platform_config.h -> DUTY define  
 Matrix_proc.h  -> #define ROWCOL_1_8 
 로 버전 종류 결정

  Version format
     1/8   1Row  8Col  remocon ver00  => matrix_rom_b08_1r8c_rc_v00
     1/16 1Row 16Col  remocon ver00  => matrix_rom_b16_1r16c_rc_v00    


   History

   ver : matrix_rom_b08_1r8c_rc_v01 	 (2012.01.30)
	  add protocol	HostRecv_48Team4_2012 

   ver : matrix_rom_b08_1r8c_rc_v03 	(2012.02.09)
         PLL  72M -> 128M

   ver : matrix_rom_b08_1r8c_rc_v04 	(2012.02.09)
         PLL  128M -> 72M

   ver : matrix_rom_b08_1r8c_rc_v05 	(2012.02.11)
         old type module suppert
         number font changed
         gps port changed to uart5 -> usart2
         host port add to usart1, uart5

   ver : matrix_rom_b08_1r8c_rc_v06 	(2012.02.11)
         fixed bug
         
   ver : matrix_rom_b08_1r8c_rc_v07 	(2012.02.11)
         time area 1 dot space

   ver : matrix_rom_b08_1r8c_rc_v08 	(2012.02.11)
        add  buzzer enable

   ver : matrix_rom_b08_1r8c_rc_v14 	(2012.02.26)
        add  HostRecv_FontCtrl

   ver : matrix_rom_b08_1r8c_rc_v16 	(2012.03.05)
        bug fixed  GPS displayl

   ver : matrix_rom_b08_1r8c_rc_v21 	(2012.11.04)
        HostRecv_48Team4_2012() Protocol cmd x36 add

   ver : matrix_rom_b08_1r8c_rc_v22 	(2012.11.06)
	 Change Ascii font data
        
   History

    ver : matrix_rom_b16_1r8c_rc_v03   (2012.01.18)
            add function select text array by up, down key
            
    ver : matrix_rom_b16_1r8c_rc_v04	  (2012.01.19)
		add function select text array by number key
		
    ver : matrix_rom_b16_1r8c_rc_v05	  (2012.01.19)
	       v04 bug fixed

    ver : matrix_rom_b16_1r8c_rc_v06	  (2012.01.19)
	       add  function for remocon copy key 

    ver : matrix_rom_b16_1r8c_rc_v07	  (2012.01.26)
	       1/16 change output port 2-> 1, 4->3 

    ver : matrix_rom_b16_1r8c_rc_v08	  (2012.01.27)
	       1/16 change output port 2-> 1, 4->3 
	       1,3 port data is equl
	       change GPS Port 2 -> 5 
	       
    ver : matrix_rom_b16_1r8c_rc_v09	  (2012.03.20)
             팀명저장시에 한글우선키
             동작버튼정지후 원위치로 돌아오기
             팀명 저장시에 다른키 동작됨

    ver : matrix_rom_b16_1r8c_rc_v10	  (2012.03.23)    
             1,3 출력 좌우 반대로 되게
             속성키로 좌우 교체

    ver : matrix_rom_b16_1r8c_rc_v11	  (2012.04.10)    
             부저 소리 활성화 

    ver : matrix_rom_b16_1r8c_rc_v12	  (2012.07.10)	  
	      remocon 으로 밝기 조정	      
*/


/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"
#include "hw_config.h"


#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "usb_drv.h"

#include "common.h"
#include "gpio.h"
#include "calltrace.h"

#include "mem_heap.h"
#include "queue.h"
#include "timer.h"
#include "hostif.h"

#include "hostcom_type.h"
#include "sys_info.h"

#include "timer_proc.h"
#include "hostcom_proc.h"
#include "Matrix_proc.h"

#include "stm32f10x_flash.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* system heap memory */
mem_heap_type           g_SystemHeap;
unsigned char           g_SystemHeapPool[MAX_SYS_HEAP_SIZE];

system_info_t           g_systemInfo;
bool                    g_startFlag;
bool                    g_bResetSystem;

const u8 compile_date[] = __DATE__;
const u8 compile_time[] = __TIME__;

u32 g_nHertLEDCnt;
u8 g_SW1 = 0;


/* Extern variables ----------------------------------------------------------*/
extern queue_type_t g_usb_q;
extern u8* g_FrameBuffer;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : TimerMatrixPort1_CallBack.
* Description    : timer handler of Matrix Port.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void TimerMatrixPort1_CallBack(void* pArgs)
{
    MatrixDisplayPort(0);
}

/*******************************************************************************
* Function Name  : TimerMatrixPort2_CallBack.
* Description    : timer handler of Matrix Port.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void TimerMatrixPort2_CallBack(void* pArgs)
{
    MatrixDisplayPort(1);
}

/*******************************************************************************
* Function Name  : TimerMatrixPort1_CallBack.
* Description    : timer handler of Matrix Effect.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void TimerMatrixSetEffect_CallBack(void* pArgs)
{
	MatrixSetEffect(0);
	MatrixSetEffect(1);
	MatrixSetEffect(2);
	MatrixSetEffect(3);
}

/*******************************************************************************
* Function Name  : Init_Varables.
* Description    : Initial system variables.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Init_Varables(void)
{
  g_bResetSystem= FALSE;
  g_startFlag   = FALSE;  
  g_nHertLEDCnt = 0;

  memcpy(g_systemInfo.name,"DOT MATRIX      ", 16); 
  memcpy(g_systemInfo.board_num,(u8*)"01",1);
  memcpy(g_systemInfo.ver,(u8*)"01",1);

  strcpy(g_systemInfo.date, compile_date);
  strcpy(g_systemInfo.time, compile_time);
}


/*******************************************************************************
* Function Name  : main.
* Description    : Main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int main(void)
{ 
  Init_Varables();

  /* System Heap memory creation */
  MemHeap_Create(&g_SystemHeap,g_SystemHeapPool,MAX_SYS_HEAP_SIZE, "system");

  /* timer application layer configuration */
  Timer_Configuration();

  /* Host,GPS communication proc msg Q init */
  HostComProc_MsgQInit();
  HostGPSProc_MsgQInit();

  /* Key processing proc msg Q init */
  KeyProc_MsgQInit();

  /* Debug message configuration */
  CallTrace_Init();

  // HW INIT
  Hw_Initialize();

  TimerSet(TIMER_MATRIX_DSP_PORT1, TIMER_MATRIX_DSP_PORT1_TIME ,TIMER_TYPE_PERIODIC, &TimerMatrixPort1_CallBack );  
  TimerSet(TIMER_MATRIX_DSP_PORT2, TIMER_MATRIX_DSP_PORT2_TIME ,TIMER_TYPE_PERIODIC, &TimerMatrixPort2_CallBack );
  TimerSet(TIMER_MATRIX_SET_EFFECT, TIMER_MATRIX_SET_EFFECT_TIME ,TIMER_TYPE_PERIODIC, &TimerMatrixSetEffect_CallBack );
  
  CallTrace(TRACE_APP_LOW, "Start DotMatrix ver %d.%d \n\r", 1, 1);

  /* main processing routine */
  while (1)
  {	
    if(!g_bResetSystem)
   	Hw_KickWatchDog(); /* reset watchdog timer */
	
    /* timer proc */
    TimerProc();

    /* Key processing Procedure */
    KeyProc();

    /* HostCom Interface Procedure */
	HostComProc();

	/* HostGPS Interface Procedure */
	HostGPSProc();

	g_nHertLEDCnt++;

	if(g_nHertLEDCnt>20000){
        GpioC->Bit.b8=!GpioC->Bit.b8;   // Heart LED 	
	    g_nHertLEDCnt=0;
	}

  }

}

