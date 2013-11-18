#include "globals.h"

static t_LedTask g_sLedTask[4];

void LedTaskPB0(void)
{
	switch(g_sLedTask[0].task_state)
	{
		case INIT_STATE:
			InitLedPort(0);
			
			g_sLedTask[0].task_state = DELAY_STATE;
			g_sLedTask[0].tick = GetTimeTick();
			g_sLedTask[0].delay_ms = 500;
			g_sLedTask[0].led_state = LED_OFF_STATE;
			break;
		case DELAY_STATE:
			if(GetTimeTick() - g_sLedTask[0].tick >= g_sLedTask[0].delay_ms)
			{
				if(g_sLedTask[0].led_state == LED_ON_STATE)
					g_sLedTask[0].task_state = LED_OFF_STATE;
				else
					g_sLedTask[0].task_state = LED_ON_STATE;
			}
			break;
		case LED_ON_STATE:
			SetLed(0, TRUE);
			
			g_sLedTask[0].task_state = DELAY_STATE;
			g_sLedTask[0].tick = GetTimeTick();
			g_sLedTask[0].led_state = LED_ON_STATE;
			break;
		case LED_OFF_STATE:
			SetLed(0, FALSE);
			
			g_sLedTask[0].task_state = DELAY_STATE;
			g_sLedTask[0].tick = GetTimeTick();
			g_sLedTask[0].led_state = LED_OFF_STATE;
			break;	
	}
}

void LedTaskPB1(void)
{
	switch(g_sLedTask[1].task_state)
	{
		case INIT_STATE:
			InitLedPort(1);
			
			g_sLedTask[1].task_state = DELAY_STATE;
			g_sLedTask[1].tick = GetTimeTick();
			g_sLedTask[1].delay_ms = 250;
			g_sLedTask[1].led_state = LED_OFF_STATE;
			break;
		case DELAY_STATE:
			if(GetTimeTick() - g_sLedTask[1].tick >= g_sLedTask[1].delay_ms)
			{
				if(g_sLedTask[1].led_state == LED_ON_STATE)
					g_sLedTask[1].task_state = LED_OFF_STATE;
				else
					g_sLedTask[1].task_state = LED_ON_STATE;
			}
			break;
		case LED_ON_STATE:
			SetLed(1, TRUE);
			
			g_sLedTask[1].task_state = DELAY_STATE;
			g_sLedTask[1].tick = GetTimeTick();
			g_sLedTask[1].led_state = LED_ON_STATE;
			break;
		case LED_OFF_STATE:
			SetLed(1, FALSE);
			
			g_sLedTask[1].task_state = DELAY_STATE;
			g_sLedTask[1].tick = GetTimeTick();
			g_sLedTask[1].led_state = LED_OFF_STATE;
			break;	
	}
}

void LedTaskPB2(void)
{
	switch(g_sLedTask[2].task_state)
	{
		case INIT_STATE:
			InitLedPort(2);
			
			g_sLedTask[2].task_state = DELAY_STATE;
			g_sLedTask[2].tick = GetTimeTick();
			g_sLedTask[2].delay_ms = 100;
			g_sLedTask[2].led_state = LED_OFF_STATE;
			break;
		case DELAY_STATE:
			if(GetTimeTick() - g_sLedTask[2].tick >= g_sLedTask[2].delay_ms)
			{
				if(g_sLedTask[2].led_state == LED_ON_STATE)
					g_sLedTask[2].task_state = LED_OFF_STATE;
				else
					g_sLedTask[2].task_state = LED_ON_STATE;
			}
			break;
		case LED_ON_STATE:
			SetLed(2, TRUE);
			
			g_sLedTask[2].task_state = DELAY_STATE;
			g_sLedTask[2].tick = GetTimeTick();
			g_sLedTask[2].led_state = LED_ON_STATE;
			break;
		case LED_OFF_STATE:
			SetLed(2, FALSE);
			
			g_sLedTask[2].task_state = DELAY_STATE;
			g_sLedTask[2].tick = GetTimeTick();
			g_sLedTask[2].led_state = LED_OFF_STATE;
			break;	
	}
}

void LedFadeInOutTask(void)
{
	switch(g_sLedTask[3].task_state)
	{
		case INIT_STATE:
			InitLedPort(4);
			InitPwmOut(SYSTEM_HZ);
			
			g_sLedTask[3].delay_ms = 10;
			g_sLedTask[3].fade_percent = 0;
			g_sLedTask[3].task_state = LED_FADE_IN_STATE;
			break;
		case DELAY_STATE:
			if(GetTimeTick() - g_sLedTask[3].tick >= g_sLedTask[3].delay_ms)
			{
				if(g_sLedTask[3].led_state == LED_FADE_IN_STATE)
					g_sLedTask[3].task_state = LED_FADE_IN_STATE;
				else
					g_sLedTask[3].task_state = LED_FADE_OUT_STATE;	
			}	
			break;
		case LED_FADE_IN_STATE:
			if(++g_sLedTask[3].fade_percent >= 100)
				g_sLedTask[3].task_state = LED_FADE_OUT_STATE;
			else
			{
				g_sLedTask[3].led_state = LED_FADE_IN_STATE;
				g_sLedTask[3].tick = GetTimeTick();
				g_sLedTask[3].task_state = DELAY_STATE;
			}
			
			SetPwmDuty(g_sLedTask[3].fade_percent);
			break;
		case LED_FADE_OUT_STATE:
			if(--g_sLedTask[3].fade_percent == 0)
				g_sLedTask[3].task_state = LED_FADE_IN_STATE;
			else
			{
				g_sLedTask[3].led_state = LED_FADE_OUT_STATE;
				g_sLedTask[3].tick = GetTimeTick();
				g_sLedTask[3].task_state = DELAY_STATE;
			}
			
			SetPwmDuty(g_sLedTask[3].fade_percent);
			break;
	}
}
