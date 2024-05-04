/********************************************************************************
* @File name   : sys.c
* @Author      : Daemon.Chen
* @Version     : Released
* @Date        : 2024-5-4
* @Description : 系统硬件驱动初始化以及NVIC中断配置
********************************************************************************/

#include "sys.h"   /*系统头文件*/

/*******************************************************************************
* Function Name  : SystemHardwareDriverInit
* Description    : 硬件初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SystemHardwareDriverInit(void)
{	
	Power_Init();																 /*电源控制初始化*/
	Power_On();																	 /*打开总电源*/
	NVIC_Configuration();												 /*NVIC中断初始化*/
	delay_init();																 /*延时函数初始化*/
	TIM3_Int_Init(71,155); 											 /*音频中断初始化*/
	Dac1_Init();																 /*音频DAC初始化*/
	Music_Flag = 0;															 /*设置开始播放音乐标志位*/
	Motor_Init();																 /*Z轴振动马达初始化*/
	MOT_ON();																		 /*Z轴振动马达开启*/
	delay_ms(50);																 /*延时50ms*/
	MOT_OFF();																	 /*Z轴振动马达关闭*/
	if(Music_Flag == 0){												 /*如果开始播放开机音乐*/
		TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); /*使能定时器3*/
		TIM_Cmd(TIM3, ENABLE);										 /*使能定时器3*/
		DAC_Cmd(DAC_Channel_1, ENABLE);						 /*使能DAC通道1*/
	}
	delay_ms(1000);															 /*延时1000ms*/
	LED_Init();                                  /*LED初始化*/
	Handle_Init();                               /*手柄ADC初始化*/
	Charger_Init();                              /*电量监测初始化*/
	KEY_Init();                                  /*按键初始化*/
	i2cInit();							                     /*IIC初始化 用于挂靠在总线上的设备使用*/
	delay_ms(10);						                     /*延时10ms*/
	MPU6050_Init();						                   /*MPU6050 DMP陀螺仪初始化*/
	TFT_Power_On();                              /*TFTLCD屏幕和无线串口电源打开*/
	UART4_Config(115200);					               /*初始化无线串口4，波特率115200*/
	LCD_Init();                                  /*LCD初始化*/
	LCD_Fill(0,0,LCD_W,LCD_H,WHITE);             /*LCD屏幕刷白*/
	LCD_ShowPicture(24,0,80,80,gImage_Picture);  /*LCD显示球形机器人LOGO*/
	LCD_ShowPicture(6,84,70,18,gImage_Mode);     /*LCD显示遥控模式图片*/
	LCD_ShowPicture(6,106,70,18,gImage_Order);   /*LCD显示遥控模式命令*/
	LCD_ShowPicture(82,84,40,18,gImage_Handle);  /*LCD显示摇杆模式命令*/
	LCD_ShowPicture(82,106,40,18,gImage_Stop);   /*LCD显示停止命令*/
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : 中断优先级配置
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	/*中断优先级配置*/
}
