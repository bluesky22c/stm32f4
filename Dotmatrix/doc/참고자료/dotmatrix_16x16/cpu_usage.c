#include "globals.h"

t_CpuUsage g_sCpuUsage;

void InitCpuUsage(void)
{
	g_sCpuUsage.init_count = 0;
	g_sCpuUsage.tick = GetTimeTick();
	
	while(1)
	{
		if(CalcCpuUsage(0) == 1)
			break;
	}
}

int CalcCpuUsage(int src)
{
	unsigned long cpu_usage_percent;
	
	if(GetTimeTick() - g_sCpuUsage.tick >= 1000)
	{
		if(src == 1)
		{
			if(g_sCpuUsage.run_count > g_sCpuUsage.init_count)
				g_sCpuUsage.run_count = g_sCpuUsage.init_count;
				
			cpu_usage_percent= 100 - ((g_sCpuUsage.run_count*100)/g_sCpuUsage.init_count);
			
			printf("CPU Usage : %d%%\r\n", cpu_usage_percent);
		}
	
		g_sCpuUsage.run_count = 0; 
		g_sCpuUsage.tick = GetTimeTick();
		
		return 1;
	}
	else
	{
		if(src == 0) // init count
			g_sCpuUsage.init_count++;
		else
			g_sCpuUsage.run_count++;
	}
	
	return -1;
}
