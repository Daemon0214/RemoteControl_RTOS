/********************************************************************************
* @File name   : sys.c
* @Author      : Daemon.Chen
* @Version     : Released
* @Date        : 2024-5-4
* @Description : ϵͳӲ��������ʼ���Լ�NVIC�ж�����
********************************************************************************/

#include "sys.h"   /*ϵͳͷ�ļ�*/

/*******************************************************************************
* Function Name  : SystemHardwareDriverInit
* Description    : Ӳ����ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SystemHardwareDriverInit(void)
{	
	Power_Init();																 /*��Դ���Ƴ�ʼ��*/
	Power_On();																	 /*���ܵ�Դ*/
	NVIC_Configuration();												 /*NVIC�жϳ�ʼ��*/
	delay_init();																 /*��ʱ������ʼ��*/
	TIM3_Int_Init(71,155); 											 /*��Ƶ�жϳ�ʼ��*/
	Dac1_Init();																 /*��ƵDAC��ʼ��*/
	Music_Flag = 0;															 /*���ÿ�ʼ�������ֱ�־λ*/
	Motor_Init();																 /*Z��������ʼ��*/
	MOT_ON();																		 /*Z������￪��*/
	delay_ms(50);																 /*��ʱ50ms*/
	MOT_OFF();																	 /*Z�������ر�*/
	if(Music_Flag == 0){												 /*�����ʼ���ſ�������*/
		TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); /*ʹ�ܶ�ʱ��3*/
		TIM_Cmd(TIM3, ENABLE);										 /*ʹ�ܶ�ʱ��3*/
		DAC_Cmd(DAC_Channel_1, ENABLE);						 /*ʹ��DACͨ��1*/
	}
	delay_ms(1000);															 /*��ʱ1000ms*/
	LED_Init();                                  /*LED��ʼ��*/
	Handle_Init();                               /*�ֱ�ADC��ʼ��*/
	Charger_Init();                              /*��������ʼ��*/
	KEY_Init();                                  /*������ʼ��*/
	i2cInit();							                     /*IIC��ʼ�� ���ڹҿ��������ϵ��豸ʹ��*/
	delay_ms(10);						                     /*��ʱ10ms*/
	MPU6050_Init();						                   /*MPU6050 DMP�����ǳ�ʼ��*/
	TFT_Power_On();                              /*TFTLCD��Ļ�����ߴ��ڵ�Դ��*/
	UART4_Config(115200);					               /*��ʼ�����ߴ���4��������115200*/
	LCD_Init();                                  /*LCD��ʼ��*/
	LCD_Fill(0,0,LCD_W,LCD_H,WHITE);             /*LCD��Ļˢ��*/
	LCD_ShowPicture(24,0,80,80,gImage_Picture);  /*LCD��ʾ���λ�����LOGO*/
	LCD_ShowPicture(6,84,70,18,gImage_Mode);     /*LCD��ʾң��ģʽͼƬ*/
	LCD_ShowPicture(6,106,70,18,gImage_Order);   /*LCD��ʾң��ģʽ����*/
	LCD_ShowPicture(82,84,40,18,gImage_Handle);  /*LCD��ʾҡ��ģʽ����*/
	LCD_ShowPicture(82,106,40,18,gImage_Stop);   /*LCD��ʾֹͣ����*/
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : �ж����ȼ�����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	/*�ж����ȼ�����*/
}
