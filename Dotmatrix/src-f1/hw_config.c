/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : hw_config.c
* Author             : MCD Application Team
* Version            : V2.2.1
* Date               : 09/22/2008
* Description        : Hardware Configuration & Setup
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"
#include "stm32f10x_it.h"
#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "usb_pwr.h"

#include "gpio.h"
#include "calltrace.h"

#include "common.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USART_InitTypeDef  USART_InitStructure;
ErrorStatus        HSEStartUpStatus;
GPIO_InitTypeDef   GPIO_InitStructure;

u16    g_tim2_freq;
u16    g_tim3_freq;

/* Extern variables ----------------------------------------------------------*/

extern u8 g_Brightness;  // Control Brightness  (0~9)


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : Hw_Initialize
* Description    : Configures All of the peripheral.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Hw_Initialize(void)
{
	/* PLL clock system configuration */
	Rcc_Configuration();

	/* Interrupt system configuration */
	Nvic_Configuration();

	/* 1ms System tick configuration */
	SysTick_Configuration();

	/* GPIO configuration */
	Gpio_Configuration();

	/* MatrixProc init */
	MatrixProc_Init();

	/* USART configuration */
	Usart_Configuration();


/* PWM */
		
#ifdef USE_TIM4
	
#ifdef DUTY_8
	
#ifdef OLD_TYPE
		P_DIMM_Control(2000,95,0);	// duty 1/8 old
#else
		P_DIMM_Control(2000,5,0);	// duty 1/8  new
#endif
	
#else	  
	//	P_DIMM_Control(2000,95,0);	// duty 1/16 
    P_DIMM_Control(2000,g_Brightness*10+9,0);	

//    CallTrace(TRACE_APP_LOW, "PWM Load=%d \n\r",g_Brightness);

#endif 
	
#endif


#ifdef USE_TIM2
	Tim2_Configuration(10000); // 100us
#endif

#ifdef USE_TIM3
	Tim3_Configuration(10000); // 100us
#endif


#ifdef USE_IWDG
  /* IWDG configuration */
  Hw_WatchDogEnable();
#endif

}

/*******************************************************************************
* Function Name  : Hw_WatchDogEnable
* Description    : enable watch dog function.
* Input          : None.
* Return         : None.
*******************************************************************************/
void Hw_WatchDogEnable()
{
  /* Enable write access to IWDG_PR and IWDG_RLR registers */
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  
  /* IWDG counter clock: 40KHz(LSI) / 32 = 1.25 KHz  1250/s=0.8ms*/
  IWDG_SetPrescaler(IWDG_Prescaler_32);
  
  /* Set counter reload value to 349 */
  IWDG_SetReload(349); /* 0.8ms / 349 = 280ms*/

  /* Reload IWDG counter */
  IWDG_ReloadCounter();
  
  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  IWDG_Enable();
}

/*******************************************************************************
* Function Name  : Hw_KickWatchDog
* Description    : Reset watch dog timer
* Input          : None.
* Return         : None.
*******************************************************************************/
void Hw_KickWatchDog()
{
  IWDG_ReloadCounter();
}

/*******************************************************************************
* Function Name  : Usart_Configuration
* Description    : Configures the USART function.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Usart_Configuration(void)
{
/*******************************************************************/
/*		                       USART 1 Configuration				                          */
/*******************************************************************/
#ifdef USE_USART1
  /* Configure the USART1_Tx as Alternate function Push-Pull */
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure the USART1_Rx as input floating */
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10 ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* USART1 configuration
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_StructInit(&USART_InitStructure); 
  USART_InitStructure.USART_BaudRate            = UART1_BAUDRATE; 
  USART_InitStructure.USART_WordLength          = USART_WordLength_8b; 
  USART_InitStructure.USART_StopBits            = USART_StopBits_1; 
  USART_InitStructure.USART_Parity              = USART_Parity_No; 
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
  USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx; 
  USART_Init(USART1, &USART_InitStructure); 
  
  /* rx interrupt enable */
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 

  /* tx interrupt disable */
  USART_ITConfig(USART1, USART_IT_TXE, DISABLE);

  /* Enable the USART1 */
  USART_Cmd(USART1, ENABLE);
#endif

/*******************************************************************/
/*                                  USART 2 Configuration                                               */
/*******************************************************************/
#ifdef USE_USART2

  /* Disable the USART2 Pins Partial Software Remapping */
  GPIO_PinRemapConfig(GPIO_Remap_USART2, DISABLE);  
    
  /* Configure USART2 Tx (PA2) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USART2 Rx (PA3) as input floating */
  GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

   /* USART2 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - one Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the second edge 
        - USART LastBit: The clock pulse of the last data bit is not output to 
                         the SCLK pin
  */
  USART_InitStructure.USART_BaudRate            = UART2_BAUDRATE;
  USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits            = USART_StopBits_1;
  USART_InitStructure.USART_Parity              = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;

  /* Configure the USART2 */
  USART_Init(USART2, &USART_InitStructure);

  
  /* rx line -- > interrupt(interrupt check)
       tx line -- > polling(flag check)  */

  /* Enable the USART Receive interrupt: this interrupt is generated when the 
       USART2 receive data register is not empty */
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

  /* tx interrupt disable */
  USART_ITConfig(USART2, USART_IT_TXE, DISABLE);

  /* Enable USART2 */
  USART_Cmd(USART2, ENABLE);

#endif /* USE_USART2 */

/*******************************************************************/
/*                                  USART 3 Configuration                                               */
/*******************************************************************/
#ifdef USE_USART3

  /* Disable the USART3 Pins Partial Software Remapping */
  GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);  
  GPIO_PinRemapConfig(GPIO_FullRemap_USART3, DISABLE);  
    
  /* Configure USART3 Tx (PB10) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Configure USART3 Rx (PB11) as input floating */
  GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

   /* USART3 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - one Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the second edge 
        - USART LastBit: The clock pulse of the last data bit is not output to 
                         the SCLK pin
  */
  USART_InitStructure.USART_BaudRate            = UART3_BAUDRATE;
  USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits            = USART_StopBits_1;
  USART_InitStructure.USART_Parity              = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;

  /* Configure the USART3 */
  USART_Init(USART3, &USART_InitStructure);

  
  /* rx line -- > interrupt(interrupt check)
       tx line -- > polling(flag check)  */

  /* Enable the USART Receive interrupt: this interrupt is generated when the 
       USART2 receive data register is not empty */
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

  /* tx interrupt disable */
  USART_ITConfig(USART3, USART_IT_TXE, DISABLE);

  /* Enable USART3 */
  USART_Cmd(USART3, ENABLE);

#endif /* USE_USART3 */

/*******************************************************************/
/*                                  UART 4 Configuration                                               */
/*******************************************************************/
#ifdef USE_UART4
    
  /* Configure UART4 Tx (PC10) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Configure UART4 Rx (PC11) as input floating */
  GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

   /* UART4 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - one Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the second edge 
        - USART LastBit: The clock pulse of the last data bit is not output to 
                         the SCLK pin
  */
  USART_InitStructure.USART_BaudRate            = UART4_BAUDRATE;
  USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits            = USART_StopBits_1;
  USART_InitStructure.USART_Parity              = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;

  /* Configure the UART4 */
  USART_Init(UART4, &USART_InitStructure);

  
  /* rx line -- > interrupt(interrupt check)
       tx line -- > polling(flag check)  */

  /* Enable the USART Receive interrupt: this interrupt is generated when the 
       USART2 receive data register is not empty */
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);

  /* tx interrupt disable */
  USART_ITConfig(UART4, USART_IT_TXE, DISABLE);

  /* Enable USART4 */
  USART_Cmd(UART4, ENABLE);

#endif /* USE_UART4 */

/*******************************************************************/
/*                                  UART 5 Configuration                                               */
/*******************************************************************/
#ifdef USE_UART5
    
  /* Configure UART5 Tx (PC12) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Configure UART5 Rx (PD2) as input floating */
  GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

   /* UART5 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - one Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the second edge 
        - USART LastBit: The clock pulse of the last data bit is not output to 
                         the SCLK pin
  */
  USART_InitStructure.USART_BaudRate            = UART5_BAUDRATE;
  USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits            = USART_StopBits_1;
  USART_InitStructure.USART_Parity              = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;

  /* Configure the USART5 */
  USART_Init(UART5, &USART_InitStructure);

  
  /* rx line -- > interrupt(interrupt check)
       tx line -- > polling(flag check)  */

  /* Enable the USART Receive interrupt: this interrupt is generated when the 
       USART2 receive data register is not empty */
  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);

  /* tx interrupt disable */
  USART_ITConfig(UART5, USART_IT_TXE, DISABLE);

  /* Enable UART5 */
  USART_Cmd(UART5, ENABLE);

#endif /* USE_UART5 */

}



/*******************************************************************************
* Function Name  : Rcc_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Rcc_Configuration(void)
{
	ErrorStatus 	   HSEStartUpStatus;
	
	/* RCC system reset(for debug purpose) */
	RCC_DeInit();
	
	/* Enable HSE */
	RCC_HSEConfig(RCC_HSE_ON);
	
	/* Wait till HSE is ready */
	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	
	if (HSEStartUpStatus == SUCCESS)
	{
	  /* Enable Prefetch Buffer */
	  FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
	
	  /* Flash 2 wait state */
	  FLASH_SetLatency(FLASH_Latency_2);
	
	  /* HCLK = SYSCLK */
	  RCC_HCLKConfig(RCC_SYSCLK_Div1);
	
	  /* PCLK2 = HCLK */
	  RCC_PCLK2Config(RCC_HCLK_Div1);
	
	  /* PCLK1 = HCLK/2 */
	  RCC_PCLK1Config(RCC_HCLK_Div2);
	
	  /* PLLCLK = 8MHz * 9 = 72 MHz */
	  //RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

	  /* PLLCLK = 8MHz * 16 = 128 MHz (OVER CLICKING) */
	  RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_16);
	
	  /* Enable PLL */
	  RCC_PLLCmd(ENABLE);
	
	  /* Wait till PLL is ready */
	  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
	  {}
	
	  /* Select PLL as system clock source */
	  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	
	  /* Wait till PLL is used as system clock source */
	  while (RCC_GetSYSCLKSource() != 0x08)
	  {}
	}
	
	/* Enable GPIOA, GPIOB, GPIOC, AFIO, and USART1 clock  */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |RCC_APB2Periph_GPIOD
						   | RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1, ENABLE);


#ifdef USE_USART2
  /* Enable USART2 clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
#endif

#ifdef USE_USART3
  /* Enable USART3 clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
#endif

#ifdef USE_UART4
  /* Enable UART4 clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
#endif

#ifdef USE_UART5
  /* Enable UART5 clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
#endif

#ifdef USE_I2C_CH1
  /* Enable I2C1 clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
#endif

#ifdef USE_I2C_CH2
  /* Enable I2C2 clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
#endif

#ifdef USE_TIM4
  /* TIM4 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
#endif

#ifdef USE_TIM3
  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
#endif

#ifdef USE_TIM2
  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
#endif

#ifdef USE_ADC
  /* Enable ADC1 and GPIOC clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
#endif

#ifdef USE_RTC
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP | RCC_APB1Periph_PWR,ENABLE);
#endif


#ifdef USE_IWDG
  /* Enable LSI clock */  
  RCC_LSICmd(ENABLE);

  /* Check if the LSI clock is ready or not */
  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {}

  /* Check if the system has resumed from IWDG reset */
  if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == SET)
  {
    /* Clear reset flags */
    RCC_ClearFlag();
  }
  else if (RCC_GetFlagStatus(RCC_FLAG_PORRST) == SET)
  {
    /* Clear reset flags */
    RCC_ClearFlag();
  }
  else
  {
    //CallTrace(TRACE_APP_ERROR, "No Reset Event... ");
  }
#endif


}

/*******************************************************************************
* Function Name  : Nvic_Configuration
* Description    : Configures each deviees interrupts
* Input          : None.
* Return         : None.
*******************************************************************************/

void Nvic_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;

  // Set the Vector Table base location at 0x08000000
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000);

  /* 3 bits pre-emption, 5bits of sub priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);


  /* Enable USART1 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&NVIC_InitStructure);


#ifdef USE_USART2
  /* Enable the USART2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel                   = USART2_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif /* USE_USART2 */

#ifdef USE_USART3
  /* Enable the USART3 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel                   = USART3_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif /* USE_USART3 */

#ifdef USE_UART4
  /* Enable the UART4 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel                   = UART4_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif /* USE_UART4 */

#ifdef USE_UART5
  /* Enable the UART5 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel                   = UART5_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif /* USE_UART5 */

#ifdef USE_TIM2
  /* Enable Timer2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel                   = TIM2_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd 			   = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif

#ifdef USE_TIM3
  /* Enable Timer3 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel                   = TIM3_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd 			   = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif

#ifdef USE_I2C_CH1
  /* Enable the I2C1 Event Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel                   = I2C1_EV_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the I2C1 Error Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel                   = I2C1_ER_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority	       = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd	               = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif 

#ifdef  USE_I2C_CH2
  /* Enable the I2C2 Event Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel                   = I2C2_EV_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the I2C2 Error Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel                   = I2C2_ER_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif

  
#ifdef USE_RTC
	/* Configure one bit for preemption priority */
	/* Enable the RTC Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel					 = RTC_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority		 = 0;
	NVIC_Init(&NVIC_InitStructure);
#endif

  /* Enable the EXTI9_5 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel                   = EXTI9_5_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 2;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the EXTI Line 7  Interrupt */
  EXTI_InitStructure.EXTI_Mode                         = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Line                         = EXTI_Line7;
  EXTI_InitStructure.EXTI_Trigger                      = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd                      = ENABLE;
  EXTI_Init(&EXTI_InitStructure);   

}

/*******************************************************************************
* Function Name  : Gpio_Configuration
* Description    : Configures GPIOs
* Input          : None.
* Return         : None.
*******************************************************************************/
void Gpio_Configuration(void)
{	
	GPIO_InitTypeDef   GPIO_InitStructure;

// JTAG port to GPIO	
//	GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);

//--A
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

//--B 	
	GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOB, &GPIO_InitStructure);

#ifdef USE_TIM4
	/* GPIOB Configuration:TIM4 Channel3,4 in Output (PWM) */
	GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
  
	GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

//--C
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6; 	                              
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* IR Input */
	GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_IPU;  // Pull-Up
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	/* interrupt pin selecting */  
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource7);

	/* Configure LED1, LED2 as output push-pull */
	GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_8 | GPIO_Pin_9 ;
	GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

}



/*******************************************************************************
* Function Name  : SysTick_Configuration
* Description    : Configures System tick clock ( tick interval is 1ms)
* Input          : None.
* Return         : None.
*******************************************************************************/
void SysTick_Configuration(void) 
{ 	
  /* SysTick end of count event each 1ms with input clock equal to 9MHz (HCLK/8, default) */
  SysTick_SetReload(9000); // 1ms, 1Khz

  /* Configure the SysTick Handler Priority: Preemption priority and subpriority */
  NVIC_SystemHandlerPriorityConfig(SystemHandler_SysTick, 0, 0);  

   /* Enable SysTick interrupt */
  SysTick_ITConfig(ENABLE);

  /* Enable the SysTick Counter */
  SysTick_CounterCmd(SysTick_Counter_Enable);
  
}

#ifdef USE_TIM2
/*******************************************************************************
* Function Name  : Tim2_Configuration
* Description    : Configuration TIM 2  as general auto reload timer
* Input          : None.
* Return         : None.
*******************************************************************************/
void Tim2_Configuration(u32 freq )
{
  TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
  TIM_OCInitTypeDef         TIM_OCInitStructure;
  u16  tim2_oc2_duty;

  TIM_DeInit(TIM2);
  
  TIM_OCStructInit(&TIM_OCInitStructure);

  g_tim2_freq	= (u16)((PLL_CLOCK / freq ) - 1 );
  
  /* TIM2 used for PWM genration */
  TIM_TimeBaseStructure.TIM_Prescaler       = 0;     /* TIM2CLK = 72 MHz */
  TIM_TimeBaseStructure.TIM_Period          = g_tim2_freq ; /* frequency */
  TIM_TimeBaseStructure.TIM_ClockDivision   = 0;
  TIM_TimeBaseStructure.TIM_CounterMode     = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* auto reload mode */
  TIM_ARRPreloadConfig(TIM2, ENABLE);

  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);

  /* Start TIM2 */
  TIM_Cmd(TIM2, ENABLE);

}
#endif

#ifdef USE_TIM3
/*******************************************************************************
* Function Name  : Tim3_Configuration
* Description    : Configuration TIM 3  as general auto reload timer
* Input          : None.
* Return         : None.
*******************************************************************************/
void Tim3_Configuration(u32 freq )
{
  TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;
  TIM_OCInitTypeDef         TIM_OCInitStructure;
  u16  tim3_oc2_duty;

  TIM_DeInit(TIM3);
  
  TIM_OCStructInit(&TIM_OCInitStructure);

  g_tim3_freq	= (u16)((PLL_CLOCK / freq ) - 1 );
  
  /* TIM2 used for PWM genration */
  TIM_TimeBaseStructure.TIM_Prescaler       = 0;     /* TIM3CLK = 72 MHz */
  TIM_TimeBaseStructure.TIM_Period          = g_tim3_freq ; /* frequency */
  TIM_TimeBaseStructure.TIM_ClockDivision   = 0;
  TIM_TimeBaseStructure.TIM_CounterMode     = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* auto reload mode */
  TIM_ARRPreloadConfig(TIM3, ENABLE);

  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);

  /* Start TIM3 */
  TIM_Cmd(TIM3, ENABLE);

}
#endif



