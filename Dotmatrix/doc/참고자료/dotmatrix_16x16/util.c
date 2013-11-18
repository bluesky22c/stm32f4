#include "globals.h"

e_OSC_Freq GetSystemFreqIndex(unsigned long f_osc)
{
	if(f_osc == 4000000)
		return OSC_4MHz;
	else if(f_osc == 8000000)
		return OSC_8MHz;
	else if(f_osc == 11059200)
		return OSC_11MHz;
	else if(f_osc == 16000000)
		return OSC_16MHz;
		
	return OSC_MAX;	
}

e_Bps GetBpsIndex(unsigned long bps)
{
	if(bps == 2400)
		return BPS_2400;
	else if(bps == 4800)
		return BPS_4800;
	else if(bps == 9600)
		return BPS_9600;
	else if(bps == 14400)
		return BPS_14K;
	else if(bps == 19200)
		return BPS_19K;
	else if(bps == 28800)
		return BPS_28K;
	else if(bps == 38400)
		return BPS_38K;
	else if(bps == 57600)
		return BPS_57K;
	else if(bps == 76800)
		return BPS_76K;
	else if(bps == 115200)
		return BPS_115K;
	else if(bps == 230400)
		return BPS_230K;
	else if(bps == 250000)
		return BPS_250K;
		
	return BPS_MAX;	
}

e_Interval GetIntervalIndex(unsigned long interval)
{
	if(interval == 100) // 100us
		return INTERVAL_100US;
	else if(interval == 500) // 500us
		return INTERVAL_500US;
	else if(interval == 1000) // 1ms
		return INTERVAL_1MS;
		
	return INTERVAL_MAX;	
}
