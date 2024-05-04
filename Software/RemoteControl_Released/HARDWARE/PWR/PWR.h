#ifndef __PWR_H
#define __PWR_H

#include "sys.h"

void Power_Init(void); //电源初始化
void Power_On(void); //开机
void Power_Off(void); //关机
void TFT_Power_On(void); //TFT电源开机
void TFT_Power_Off(void); //TFT电源关机

#endif
