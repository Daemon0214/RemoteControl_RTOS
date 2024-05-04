#include "MOT.h"

void Motor_Init(void){
	GPIO_InitTypeDef  GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能A端口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOC, &GPIO_InitStructure);	  //初始化GPIOA
	
	GPIO_ResetBits(GPIOC,GPIO_Pin_6);						 //PA.15 输出低
	GPIO_ResetBits(GPIOC,GPIO_Pin_7);						 //PA.15 输出低
}

void MOT_ON(void){
	MOT1 = 1;
	MOT2 = 1;
}

void MOT_OFF(void){
	MOT1 = 0;
	MOT2 = 0;
}

