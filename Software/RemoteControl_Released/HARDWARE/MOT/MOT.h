#ifndef __MOT_H
#define __MOT_H

#include "sys.h"

#define MOT1 PCout(6)
#define MOT2 PCout(7)

void Motor_Init(void); //��Դ��ʼ��
void MOT_ON(void);
void MOT_OFF(void);

#endif
