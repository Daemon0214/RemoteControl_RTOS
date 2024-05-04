#ifndef __CHARGER_H
#define __CHARGER_H

#include "sys.h"

u16 Get_Adc_CHG(u8 ch);
u16 Get_Adc_Average_CHG(u8 ch,u8 times);

void Charger_Init(void);

#endif
