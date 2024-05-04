#ifndef __MPU6050_H__
#define __MPU6050_H__
//#include "sys.h"

#define q30  1073741824.0f

void MPU6050_Init(void);
void MPU6050_Pose(float *Pitch, float *Roll, float *Yaw);

#endif
