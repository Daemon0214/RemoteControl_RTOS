/********************************************************************************
* @File name   : sys.h
* @Author      : Daemon.Chen
* @Version     : Released
* @Date        : 2024-5-4
* @Description : 系统硬件驱动初始化以及NVIC中断配置头文件，包含所有头文件
********************************************************************************/

#ifndef __SYS_H
#define __SYS_H	

/*系统头文件*/
#include <stdio.h>                             /*标准库系统文件*/
#include <stdint.h>                            /*标准库系统文件*/
#include <stdlib.h>                            /*标准库系统文件*/
#include <string.h>                            /*字符串系统文件*/
#include <math.h>                              /*数学系统文件*/

/*自定义头文件*/
#include "stm32f10x.h"                         /*STM32F10X头文件*/
#include "delay.h"                             /*延时函数头文件*/
#include "usart.h"                             /*串口头文件*/
#include "led.h"                               /*LED头文件*/
#include "PWR.h"                               /*电源控制头文件*/
#include "HANDLE.h"                            /*手柄ADC头文件*/
#include "lcd.h"                               /*LCD头文件*/
#include "lcd_init.h"                          /*LCD初始化头文件*/
#include "CHARGER.h"                           /*电量检测头文件*/
#include "key.h"                               /*按键头文件*/
#include "I2C.h"                               /*I2C头文件*/
#include "I2C_MPU6050.h"                       /*MPU6050头文件*/
#include "MPU6050.h"                           /*MPU6050头文件*/
#include "inv_mpu.h"                           /*MPU6050头文件*/
#include "inv_mpu_dmp_motion_driver.h"         /*MPU6050头文件*/
#include "MOT.h"                               /*Z轴振动马达头文件*/
#include "DAC.h"                               /*DAC头文件*/
#include "Timer.h"                             /*定时器头文件*/
#include "yinpin.h"                            /*音频头文件*/
#include "Picture.h"                           /*UI图片头文件*/
#include "Photo.h"                             /*相册头文件*/

//0,不支持os
//1,支持os
#define SYSTEM_SUPPORT_OS		1		               /*定义系统文件夹是否支持OS*/
	 
//位带操作,实现51类似的GPIO控制功能
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) 		 /*0x4001080C*/
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) 		 /*0x40010C0C*/ 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) 		 /*0x4001100C*/ 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) 		 /*0x4001140C*/ 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) 		 /*0x4001180C*/ 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) 		 /*0x40011A0C*/    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) 		 /*0x40011E0C*/    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) 			 /*0x40010808*/ 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) 			 /*0x40010C08*/ 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) 			 /*0x40011008*/ 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) 			 /*0x40011408*/ 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) 			 /*0x40011808*/ 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) 			 /*0x40011A08*/ 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8)       /*0x40011E08*/ 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  /*输出*/ 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  /*输入*/ 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  /*输出*/ 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  /*输入*/ 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  /*输出*/ 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  /*输入*/ 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  /*输出*/ 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  /*输入*/ 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  /*输出*/ 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  /*输入*/

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  /*输出*/ 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  /*输入*/

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  /*输出*/ 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  /*输入*/

void SystemHardwareDriverInit(void);           /*硬件初始化*/
void NVIC_Configuration(void);                 /*中断优先级配置*/

#endif
