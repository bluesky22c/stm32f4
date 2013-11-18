// ==========================================================================
//	�ѱ� 24*24 test
// OK_TFT32X ���忡 ū��¥(24*24)�߰�(*OK-128TFT_large.h)
// MCU = ATmega128 
//  DIDR0 = 0x0F;
//  DDRG  = 0x3F;					// PORTG output
//  PORTG = 0x00;
// ========================================================================== 
#include <stdint.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <math.h>
#include <inttypes.h>

#include "OK-TFT_font.h"
#include "OK-TFT32X.h"
#include "OK-128TFT_large_C.h"

int main(void)
{

Initialize_MCU();
Initialize_TFT_LCD();	// initialize TFT-LCD module
TFT_clear_screen();
//
font_type=1;
   TFT_string( 2, 6,White, Magenta," �ѱ� 24*24 Test Program "); 


font_type=0;
  TFT_string_large( 3,13,Green, Black,  "����ü");
font_type=1;
  TFT_string_large( 3,18,Green, Black,  "���ü");
font_type=2;
  TFT_string_large( 3,23,Green, Black,  "�ʱ�ü");

  while(1)
    {
	font_type++;if(font_type>2)font_type=0;
	TFT_string_large( 3,28,Yellow, Black,  "�����ٶ󸶹ٻ�");
	TFT_string_large( 3,33,Yellow, Black,  "   2011/11/06"); 
	Delay_ms(500);
	}
}
