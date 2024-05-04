#include "PWR.h"

void Power_Init(void){
	GPIO_InitTypeDef  GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD, ENABLE);	 //ʹ��A�˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  //��ʼ��GPIOA
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	
	GPIO_Init(GPIOD, &GPIO_InitStructure);	  //��ʼ��GPIOD 
}

void Power_On(void){
	GPIO_SetBits(GPIOA,GPIO_Pin_5);						 //PA.5 �����
}

void Power_Off(void){
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);						 //PA.5 �����
}

void TFT_Power_On(void){
	GPIO_SetBits(GPIOD,GPIO_Pin_2);						 //PD.2 �����
}

void TFT_Power_Off(void){
	GPIO_ResetBits(GPIOD,GPIO_Pin_2);						 //PD.2 �����
}
