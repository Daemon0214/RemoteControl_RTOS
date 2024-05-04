#include "led.h"

void LED_Init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PA,PB�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 
	GPIO_SetBits(GPIOA,GPIO_Pin_12);						 //PA.12 ����ߣ�RGB����
	GPIO_SetBits(GPIOA,GPIO_Pin_11);						 //PA.11 ����ߣ�RGB�̵�

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
	GPIO_SetBits(GPIOB,GPIO_Pin_12); 						 //PB.12 �����
	GPIO_SetBits(GPIOB,GPIO_Pin_13); 						 //PB.13 �����
	GPIO_SetBits(GPIOB,GPIO_Pin_14); 						 //PB.14 �����
	GPIO_SetBits(GPIOB,GPIO_Pin_15); 						 //PB.15 �����
}

void Charger_Disp(u32 Charger_Value){
	
	float Percent_Charger = 0.0;
	
	Percent_Charger = ((float)(Charger_Value - 3413))/(4096 - 3413);
	if((Percent_Charger<=1)&&(Percent_Charger>0.75)){
		LED1=0;
		LED2=0;
		LED3=0;
		LED4=0;
	}
	else if((Percent_Charger<=0.75)&&(Percent_Charger>0.5)){
		LED1=0;
		LED2=0;
		LED3=0;
		LED4=1;
	}
	else if((Percent_Charger<=0.5)&&(Percent_Charger>0.25)){
		LED1=0;
		LED2=0;
		LED3=1;
		LED4=1;
	}
	else if((Percent_Charger<=0.25)&&(Percent_Charger>0)){
		LED1=0;
		LED2=1;
		LED3=1;
		LED4=1;
	}
	else{
		LED1=1;
		LED2=1;
		LED3=1;
		LED4=1;
	}
}
