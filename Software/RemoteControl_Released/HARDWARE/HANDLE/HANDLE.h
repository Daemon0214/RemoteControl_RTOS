#ifndef __HANDLE_H
#define __HANDLE_H

#include "sys.h"

void Handle_Init(void);
u16 Get_Adc(u8 ch);
u16 Get_Adc_Average(u8 ch,u8 times);

#endif
