#include "globals.h"

void InitLedPort(unsigned char pin_num)
{
	DDRB |= (1<<pin_num);
	PORTB |= (1<<pin_num);	
}

void SetLed(unsigned char pin_num, unsigned char on)
{
	if(on)
		PORTB &= ~(1<<pin_num);
	else
		PORTB |= (1<<pin_num);	
}
