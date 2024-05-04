#include "PWR.h"

void Power_Init(void){
	GPIO_InitTypeDef  GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD, ENABLE);	 //使能A端口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  //初始化GPIOA
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	
	GPIO_Init(GPIOD, &GPIO_InitStructure);	  //初始化GPIOD 
}

void Power_On(void){
	GPIO_SetBits(GPIOA,GPIO_Pin_5);						 //PA.5 输出高
}

void Power_Off(void){
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);						 //PA.5 输出低
}

void TFT_Power_On(void){
	GPIO_SetBits(GPIOD,GPIO_Pin_2);						 //PD.2 输出高
}

void TFT_Power_Off(void){
	GPIO_ResetBits(GPIOD,GPIO_Pin_2);						 //PD.2 输出低
}
