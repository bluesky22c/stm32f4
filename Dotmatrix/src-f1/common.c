/******************** (C) COPYRIGHT 2012 OP *****************************
* File Name          : common.c
* Author             : bluesky22c
* Version            : V2.2.1
* Date               : 01/01/2012
* Description        : common driver
********************************************************************************
*
********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "common.h"
#include "calltrace.h"
#include "usart.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : delay_us
* Description    : delay function per micro sec
* Input          : None.
* Return         : None.
*******************************************************************************/
void delay_us(u8 time_us)
{ 
  register int i,j;
  
  for(i = 0; i < time_us; i++)
  { 
    for(j= 10; j > 0 ;j--)
    {
      __NOP();
    }
  }
}


/*******************************************************************************
* Function Name  : delay_ms
* Description    : delay function per mili sec
* Input          : None.
* Return         : None.
*******************************************************************************/
void delay_ms(u16 time_ms)
{ 

  register unsigned int i;
  
  for(i = 0; i < time_ms; i++)
  {
    delay_us(250);
    delay_us(250);
    delay_us(250);
    delay_us(250);
  }
}


/*******************************************************************************
* Function Name  : Delay
* Description    : Just delay function per 24 uSec
* Input          : None.
* Return         : None.
*******************************************************************************/
void Delay( u32 time)
{
  int i,j;

  for(i=1 ;i<=time ;i++)
  {
    for(j= 0xFF; j > 0 ;j--)
    {
      __NOP();
    }
  }
}

/*******************************************************************************
* Function Name  : MakeUpper
* Description    : Change small character to Large character.
* Input          : - str: The string
*                  - intnum: The intger to be converted
* Output         : None
* Return         : None
*******************************************************************************/
u8 MakeUpper(u8 ch)
{
  if( (ch >= 'a') && (ch <= 'z'))
  {
    return( ch - 0x20);
  }
  else
  {
    return ch;
  }
}


/*******************************************************************************
* Function Name  : Int2Str
* Description    : Convert an Integer to a string
* Input          : - str: The string
*                  - intnum: The intger to be converted
* Output         : None
* Return         : None
*******************************************************************************/
void Int2Str(u8* str, u32 intnum)
{
  u32 i, Div = 1000000000, j = 0, Status = 0;

  for (i = 0; i < 10; i++)
  {
    str[j++] = (intnum / Div) + '0';

    intnum = intnum % Div;
    Div /= 10;
    if ((str[j-1] == '0') && (Status == 0))
    {
      j = 0;
    }
    else
    {
      Status++;
    }
  }
  str[j] = '\0';
}


/*******************************************************************************
* Function Name  : Str2Int
* Description    : Convert a string to an integer
* Input 1        : - inputstr: The string to be converted
*                  - intnum: The intger value
* Output         : None
* Return         : 1: Correct
*                  0: Error
*******************************************************************************/
u32 Str2Int(u8 *inputstr, s32 *intnum)
{
  u32 i = 0, res = 0;
  u32 val = 0;

  if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X'))
  {
    if (inputstr[2] == '\0')
    {
      return 0;
    }
    for (i = 2; i < 11; i++)
    {
      if (inputstr[i] == '\0')
      {
        *intnum = val;
        /* return 1; */
        res = 1;
        break;
      }
      if (ISVALIDHEX(inputstr[i]))
      {
        val = (val << 4) + CONVERTHEX(inputstr[i]);
      }
      else
      {
        /* return 0, Invalid input */
        res = 0;
        break;
      }
    }
    /* over 8 digit hex --invalid */
    if (i >= 11)
    {
      res = 0;
    }
  }
  else /* max 10-digit decimal input */
  {
    for (i = 0;i < 11;i++)
    {
      if (inputstr[i] == '\0')
      {
        *intnum = val;
        /* return 1 */
        res = 1;
        break;
      }
      else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0))
      {
        val = val << 10;
        *intnum = val;
        res = 1;
        break;
      }
      else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0))
      {
        val = val << 20;
        *intnum = val;
        res = 1;
        break;
      }
      else if (ISVALIDDEC(inputstr[i]))
      {
        val = val * 10 + CONVERTDEC(inputstr[i]);
      }
      else
      {
        /* return 0, Invalid input */
        res = 0;
        break;
      }
    }
    /* Over 10 digit decimal --invalid */
    if (i >= 11)
    {
      res = 0;
    }
  }

  return res;
}


/*******************************************************************************
* Function Name  : HexToAsc
* Description    : Convert a hex data to an ASCII
* Output         : None
* Return         : None
*******************************************************************************/
void HexToAsc(u8 d, u8 *cp)
{
  int		n;
  
  d &= 0xff;
  n = d >> 4;
  if(n > 9)
    *cp = n + 'A' - 10;
  else
    *cp = n + '0';
  
  
  n = d & 0x0f;
  ++cp;
  if(n > 9)
    *cp = n + 'A' - 10;
  else
    *cp = n + '0';
  *++cp = 0;
}

/*******************************************************************************
* Function Name  : StrToHex
* Description    : Convert a string to an Hex
* Output         : None
* Return         : None
*******************************************************************************/
u8 StrToHex(u8 *cp)
{
  u8   result = 0;
  u8   temp[2];


  temp[0] = MakeUpper(*cp);
  temp[1] = MakeUpper(*(cp+1));

  if( (temp[0] >= 'A') && (temp[0] <= 'F'))
  {
    result = temp[0] - 0x37;
  }
  else if ((temp[0] >= '0') && (temp[0] <= '9'))
  {
    result = temp[0] - 0x30;
  }

  result <<= 4;
  
  if( (temp[1] >= 'A') && (temp[1] <= 'F'))
  {
    result |= temp[1] - 0x37;
  }
  else if ((temp[1] >= '0') && (temp[1] <= '9'))
  {
    result |= temp[1] - 0x30;
  }

  return result;
}

/*******************************************************************************
* Function Name  : AscToHex
* Description    : Convert a char  data to an Hex
* Output         : None
* Return         : None
*******************************************************************************/
u8 AscToHex(u8 cp)
{
  u8   result = 0;
  
  if( (cp >= 'A') && (cp <= 'F'))
  {
    result = cp - 0x37;
  }
  else if ((cp >= '0') && (cp <= '9'))
  {
    result = cp - 0x30;
  }
  else if( (cp >= 'a') && (cp <= 'f'))
  {
    result = cp - 0x57;
  }

  return result;
}


static void printchar(char **str, int c)   
{   
  if (str) 
  {   
   **str = c;   
   ++(*str);   
  }   
  else
  { 
    DebugPutChar(c);   
  }
}

static int prints(char **out, const char *string, int width, int pad)
{   
  register int pc = 0, padchar = ' ';

  if (width > 0) 
  {
    register int len = 0;
    register const char *ptr;

    for (ptr = string; *ptr; ++ptr) ++len;   
    if (len >= width) width = 0;   
    else width -= len;   
    if (pad & PAD_ZERO) padchar = '0';   
  }   

  if (!(pad & PAD_RIGHT)) 
  {   
    for ( ; width > 0; --width) 
    {
      printchar (out, padchar);   
      ++pc;   
    }   
  }

  for ( ; *string ; ++string) 
  {
    printchar (out, *string);   
    ++pc;   
  }

  for ( ; width > 0; --width) 
  {   
    printchar (out, padchar);   
    ++pc;   
  }   
  return pc;   
}
   

static int printi(char **out, int i, int b, int sg, int width, int pad, int letbase)
{   
  char print_buf[PRINT_BUF_LEN];   
  register char *s;
  register int t, neg = 0, pc = 0;
  register unsigned int u = i;

  if (i == 0) 
  {   
    print_buf[0] = '0';   
    print_buf[1] = '\0';   
    return prints (out, print_buf, width, pad);   
  }

  if (sg && b == 10 && i < 0) 
  {   
    neg = 1;   
    u = -i;   
  }

  s = print_buf + PRINT_BUF_LEN-1;   
  *s = '\0';   
  while (u) 
  {   
    t = u % b;   
    if( t >= 10 )   
      t += letbase - '0' - 10;   
    *--s = t + '0';   
    u /= b;   
  }

  if (neg) 
  {   
    if( width && (pad & PAD_ZERO) ) 
    {   
      printchar (out, '-');   
      ++pc;   
      --width;   
    }   
    else 
    {   
      *--s = '-';   
    }   
  }   
  return pc + prints (out, s, width, pad);   
}   


int PRINT( char **out, const char *format, va_list args )
{   
  register int width, pad;
  register int pc = 0;
  char scr[2];

  for (; *format != 0; ++format) 
  {   
    if (*format == '%')
    {   
      ++format;   
      width = pad = 0;   
      if (*format == '\0') break;   
      if (*format == '%') goto out;   
      if (*format == '-') 
      {   
        ++format;   
        pad = PAD_RIGHT;   
      }

      while (*format == '0') 
      {   
        ++format;   
        pad |= PAD_ZERO;   
      }


      for ( ; *format >= '0' && *format <= '9'; ++format)
      {   
        width *= 10;   
        width += *format - '0';   
      }

      if( *format == 's' ) 
      {   
        register char *s = (char *)va_arg( args, int );
        pc += prints (out, s?s:"(null)", width, pad);   
        continue;   
      }


      if( *format == 'd' ) 
      {   
        pc += printi (out, va_arg( args, int ), 10, 1, width, pad, 'a');
        continue;   
      }   
      if( *format == 'x' )
      {   
        pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'a');
        continue;   
      }   
      if( *format == 'X' ) 
      {
        pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'A');
        continue;   
      }   

      if( *format == 'u' ) 
      {
        pc += printi (out, va_arg( args, int ), 10, 0, width, pad, 'a');
        continue;   
      }   

      if( *format == 'c' )
      {   
        /* char are converted to int then pushed on the stack */  
        scr[0] = (char)va_arg( args, int );
        scr[1] = '\0';   
        pc += prints (out, scr, width, pad);   
        continue;   
      }   
    }   
    else 
    {   
out:   
      printchar (out, *format);   
      ++pc;   
    }   
  }   

  if (out) **out = '\0';   

  va_end( args );
  return pc;   
}


int SPRINTF(char *out, const char *format, ...)
{
  va_list args;
  
  va_start( args, format );
  return PRINT( &out, format, args );
}


int SNPRINTF( char *buf, unsigned int count, const char *format, ... )
{
  va_list args;
  
  ( void ) count;
  
  va_start( args, format );
  return PRINT( &buf, format, args );
}


#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{
  CallTrace(TRACE_DIAG_ERROR, "*** ASSERT Failed %s %d *****\r\n", file, line );
  /* Infinite loop */
  while (1)
  {
    
  }
}
#endif


