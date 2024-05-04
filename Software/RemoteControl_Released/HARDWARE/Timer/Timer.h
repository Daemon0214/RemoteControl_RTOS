#ifndef __TIMER_H
#define __TIMER_H

#include "sys.h"
#include "KEY.h"

extern u8 Music_Flag; 

void TIM3_Int_Init(u16 arr,u16 psc);

#endif
