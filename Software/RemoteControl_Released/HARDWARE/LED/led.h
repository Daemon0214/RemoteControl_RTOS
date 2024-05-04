#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

#define LEDG PAout(11)
#define LEDB PAout(12)
#define LED1 PBout(12)
#define LED2 PBout(13)
#define LED3 PBout(14)
#define LED4 PBout(15)

void LED_Init(void);//LED≥ı ºªØ
void Charger_Disp(u32 Charger_Value);
		 				    
#endif
