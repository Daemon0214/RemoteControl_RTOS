/********************************************************************************
* @File name   : TaskManage.c
* @Author      : Daemon.Chen
* @Version     : Released
* @Date        : 2024-5-4
* @Description : 操作系统下负责所有任务管理的头文件
********************************************************************************/

#include "TaskManage.h"                     /*任务管理头文件*/
#include "sys.h"                            /*系统头文件*/
#include "FreeRTOS.h"                       /*FreeRTOS头文件*/
#include "task.h"                           /*任务头文件*/
#include "queue.h"                          /*队列头文件*/
#include "Picture.h"                        /*UI图片头文件*/
#include "semphr.h"                         /*互斥量头文件*/
#include "event_groups.h"                   /*事件标志组头文件*/
#include "string.h"                         /*字符串头文件*/

/**********任务宏定义*********/

/*开始任务*/
#define START_TASK_PRIO		    1             /*任务优先级*/
#define START_STK_SIZE 		    128           /*任务堆栈大小*/  
TaskHandle_t StartTask_Handler;             /*任务句柄*/
void start_task(void *pvParameters);        /*任务函数*/

/*LED显示任务*/
#define LED_TASK_PRIO		      2             /*任务优先级*/
#define LED_STK_SIZE 		      256           /*任务堆栈大小*/  
TaskHandle_t LEDTask_Handler;               /*任务句柄*/
void led_task(void *pvParameters);          /*任务函数*/

/*无线串口发送任务*/
#define UART_TASK_PRIO		    2             /*任务优先级*/
#define UART_STK_SIZE 		    256           /*任务堆栈大小*/  
TaskHandle_t UARTTask_Handler;              /*任务句柄*/
void uart_task(void *pvParameters);         /*任务函数*/

/*手柄检测任务*/
#define HANDLE_TASK_PRIO	    2             /*任务优先级*/
#define HANDLE_STK_SIZE 	    256           /*任务堆栈大小*/  
TaskHandle_t HANDLETask_Handler;            /*任务句柄*/
void handle_task(void *pvParameters);       /*任务函数*/

/*摇杆模式任务*/
#define LCD_HANDLE_TASK_PRIO	1             /*任务优先级*/
#define LCD_HANDLE_STK_SIZE 	512           /*任务堆栈大小*/  
TaskHandle_t LCDHandleTask_Handler;         /*任务句柄*/
void lcd_handle_task(void *pvParameters);   /*任务函数*/

/*体感模式任务*/
#define LCD_IMU_TASK_PRIO	    1             /*任务优先级*/
#define LCD_IMU_STK_SIZE 	    1024          /*任务堆栈大小*/
TaskHandle_t LCDIMUTask_Handler;            /*任务句柄*/
void lcd_imu_task(void *pvParameters);      /*任务函数*/

/*电量检测任务*/
#define CHARGER_TASK_PRIO	    2             /*任务优先级*/
#define CHARGER_STK_SIZE 	    256           /*任务堆栈大小*/  
TaskHandle_t CHARGERTask_Handler;           /*任务句柄*/
void charger_task(void *pvParameters);      /*任务函数*/

/*按键检测任务*/
#define KEY_TASK_PRIO	        2             /*任务优先级*/
#define KEY_STK_SIZE 	        1024          /*任务堆栈大小*/ 
TaskHandle_t KEYTask_Handler;               /*任务句柄*/
void key_task(void *pvParameters);          /*任务函数*/

/*IMU检测任务*/
#define IMU_TASK_PRIO	        2             /*任务优先级*/	
#define IMU_STK_SIZE 	        256           /*任务堆栈大小*/  
TaskHandle_t IMUTask_Handler;               /*任务句柄*/
void imu_task(void *pvParameters);          /*任务函数*/

/*相册播放任务*/
#define PHOTO_TASK_PRIO	      1             /*任务优先级*/
#define PHOTO_STK_SIZE 	      1024          /*任务堆栈大小*/ 
TaskHandle_t PHOTOTask_Handler;             /*任务句柄*/
void photo_task(void *pvParameters);        /*任务函数*/

/*任务状态统计任务*/
#define QUERY_TASK_PRIO	      1             /*任务优先级*/
#define QUERY_STK_SIZE 	      1024          /*任务堆栈大小*/
TaskHandle_t QUERYTask_Handler;             /*任务句柄*/
void query_task(void *pvParameters);        /*任务函数*/

/********************************/

/**********消息队列宏定义*********/

/*左手柄X轴消息队列*/
#define Handle_LX_NUM     5                 /*消息队列数量*/
QueueHandle_t Handle_LX_Queue;              /*消息队列句柄*/

/*左手柄Y轴消息队列*/
#define Handle_LY_NUM     5                 /*消息队列数量*/
QueueHandle_t Handle_LY_Queue;              /*消息队列句柄*/

/*右手柄X轴消息队列*/
#define Handle_RX_NUM     5                 /*消息队列数量*/
QueueHandle_t Handle_RX_Queue;              /*消息队列句柄*/

/*右手柄Y轴消息队列*/
#define Handle_RY_NUM     5                 /*消息队列数量*/
QueueHandle_t Handle_RY_Queue;              /*消息队列句柄*/

/*按键1消息队列*/
#define KEY1_NUM          1                 /*消息队列数量*/
QueueHandle_t KEY1_Queue;                   /*消息队列句柄*/

/*俯仰角消息队列*/
#define PITCH_NUM         5                 /*消息队列数量*/
QueueHandle_t PITCH_Queue;                  /*消息队列句柄*/

/*横滚角消息队列*/
#define ROLL_NUM          5                 /*消息队列数量*/
QueueHandle_t ROLL_Queue;                   /*消息队列句柄*/

/*偏航角消息队列*/
#define YAW_NUM           5                 /*消息队列数量*/
QueueHandle_t YAW_Queue;                    /*消息队列句柄*/

/*相册翻页消息队列*/
#define PHOTO_NUM         1                 /*消息队列数量*/
QueueHandle_t PHOTO_Queue;                  /*消息队列句柄*/

/********************************/

/**********互斥信号量定义*********/

SemaphoreHandle_t xLCDMutex;                /*LCD临界资源互斥访问信号量*/
SemaphoreHandle_t xUARTMutex;               /*UART临界资源互斥访问信号量*/

/********************************/

/*********任务运行信息缓存********/

char InfoBuffer[1000];                      /*任务运行信息缓存*/

/********************************/

/*******************************************************************************
* Function Name  : TaskCreateFunction
* Description    : 创建任务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TaskCreateFunction(void)
{   
	xTaskCreate((TaskFunction_t )start_task,      /*任务函数*/
				(const char*    )"start_task",          /*任务名称*/
				(uint16_t       )START_STK_SIZE,        /*任务堆栈大小*/
				(void*          )NULL,                  /*传递给任务函数的参数*/
				(UBaseType_t    )START_TASK_PRIO,       /*任务优先级*/
				(TaskHandle_t*  )&StartTask_Handler);   /*任务句柄*/
}

/*******************************************************************************
* Function Name  : start_task
* Description    : 开始任务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void start_task(void *pvParameters)
{ 
	BaseType_t xReturn = pdPASS;                                      /*定义返回类型*/
	taskENTER_CRITICAL();                                             /*进入临界区*/
	
	
	/*创建消息队列*/
  Handle_LX_Queue = xQueueCreate(Handle_LX_NUM,sizeof(u16));        /*创建消息Handle_LX_Queue*/
	Handle_LY_Queue = xQueueCreate(Handle_LY_NUM,sizeof(u16));        /*创建消息Handle_LY_Queue*/
	Handle_RX_Queue = xQueueCreate(Handle_RX_NUM,sizeof(u16));        /*创建消息Handle_RX_Queue*/
	Handle_RY_Queue = xQueueCreate(Handle_RY_NUM,sizeof(u16));        /*创建消息Handle_RY_Queue*/
	KEY1_Queue      = xQueueCreate(KEY1_NUM     ,sizeof(u16));        /*创建消息KEY1_Queue*/
	PITCH_Queue     = xQueueCreate(PITCH_NUM    ,sizeof(float));      /*创建消息PITCH_Queue*/
	ROLL_Queue      = xQueueCreate(ROLL_NUM     ,sizeof(float));      /*创建消息ROLL_Queue*/
	YAW_Queue       = xQueueCreate(YAW_NUM      ,sizeof(float));      /*创建消息YAW_Queue*/
	PHOTO_Queue     = xQueueCreate(PHOTO_NUM    ,sizeof(u8));         /*创建消息PHOTO_Queue*/
	
	/*创建互斥型信号量*/
	xLCDMutex  = xSemaphoreCreateMutex();                             /*创建LCD访问互斥信号量*/
	xUARTMutex = xSemaphoreCreateMutex();                             /*创建UART访问互斥信号量*/
	
	/*创建LED任务*/
	xReturn = xTaskCreate((TaskFunction_t )led_task,           /*任务函数*/    	
							(const char*    )"led_task",   	               /*任务名称*/
							(uint16_t       )LED_STK_SIZE,                 /*任务堆栈大小*/
							(void*          )NULL,                         /*传递给任务函数的参数*/				
							(UBaseType_t    )LED_TASK_PRIO,                /*任务优先级*/	
							(TaskHandle_t*  )&LEDTask_Handler);            /*任务句柄*/
	if(xReturn==pdPASS){
//		printf("LED TASK SUCCEED!\r\n");                       /*判断任务是否创建成功*/
	}
	
	/*创建UART任务*/
	xReturn = xTaskCreate((TaskFunction_t )uart_task,          /*任务函数*/  	
							(const char*    )"uart_task",   	             /*任务名称*/
							(uint16_t       )UART_STK_SIZE,                /*任务堆栈大小*/
							(void*          )NULL,                         /*传递给任务函数的参数*/				
							(UBaseType_t    )UART_TASK_PRIO,               /*任务优先级*/	
							(TaskHandle_t*  )&UARTTask_Handler);           /*任务句柄*/
	if(xReturn==pdPASS){
//		printf("UART TASK SUCCEED!\r\n");                      /*判断任务是否创建成功*/
	} 
	
	/*创建HANDLE任务*/
	xReturn = xTaskCreate((TaskFunction_t )handle_task,        /*任务函数*/   	
							(const char*    )"handle_task",   	           /*任务名称*/
							(uint16_t       )HANDLE_STK_SIZE,              /*任务堆栈大小*/
							(void*          )NULL,                         /*传递给任务函数的参数*/				
							(UBaseType_t    )HANDLE_TASK_PRIO,             /*任务优先级*/	
							(TaskHandle_t*  )&HANDLETask_Handler);         /*任务句柄*/
	if(xReturn==pdPASS){
//		printf("HANDLE TASK SUCCEED!\r\n");                    /*判断任务是否创建成功*/
	} 
	
	/*创建LCD HANDLE任务*/
	xReturn = xTaskCreate((TaskFunction_t )lcd_handle_task,    /*任务函数*/   	
							(const char*    )"lcd_handle_task",   	       /*任务名称*/
							(uint16_t       )LCD_HANDLE_STK_SIZE,          /*任务堆栈大小*/
							(void*          )NULL,                         /*传递给任务函数的参数*/				
							(UBaseType_t    )LCD_HANDLE_TASK_PRIO,         /*任务优先级*/	
							(TaskHandle_t*  )&LCDHandleTask_Handler);      /*任务句柄*/
	if(xReturn==pdPASS){
//		printf("LCD HANDLE TASK SUCCEED!\r\n");                /*判断任务是否创建成功*/
	} 
	
	/*创建LCD IMU任务*/
	xReturn = xTaskCreate((TaskFunction_t )lcd_imu_task,       /*任务函数*/  	
							(const char*    )"lcd_imu_task",   	           /*任务名称*/
							(uint16_t       )LCD_IMU_STK_SIZE,             /*任务堆栈大小*/
							(void*          )NULL,                         /*传递给任务函数的参数*/				
							(UBaseType_t    )LCD_IMU_TASK_PRIO,            /*任务优先级*/	
							(TaskHandle_t*  )&LCDIMUTask_Handler);         /*任务句柄*/
	if(xReturn==pdPASS){
//		printf("LCD IMU TASK SUCCEED!\r\n");                   /*判断任务是否创建成功*/
	} 
	
	/*创建CHARGER任务*/
	xReturn = xTaskCreate((TaskFunction_t )charger_task,       /*任务函数*/  	
							(const char*    )"charger_task",   	           /*任务名称*/
							(uint16_t       )CHARGER_STK_SIZE,             /*任务堆栈大小*/
							(void*          )NULL,                         /*传递给任务函数的参数*/				
							(UBaseType_t    )CHARGER_TASK_PRIO,            /*任务优先级*/	
							(TaskHandle_t*  )&CHARGERTask_Handler);        /*任务句柄*/
	if(xReturn==pdPASS){
//		printf("CHARGER TASK SUCCEED!\r\n");                   /*判断任务是否创建成功*/
	} 
	
	/*创建KEY任务*/
	xReturn = xTaskCreate((TaskFunction_t )key_task,     	     /*任务函数*/
							(const char*    )"key_task",   	               /*任务名称*/
							(uint16_t       )KEY_STK_SIZE,                 /*任务堆栈大小*/
							(void*          )NULL,                         /*传递给任务函数的参数*/				
							(UBaseType_t    )KEY_TASK_PRIO,                /*任务优先级*/	
							(TaskHandle_t*  )&KEYTask_Handler);            /*任务句柄*/
	if(xReturn==pdPASS){
//		printf("KEY TASK SUCCEED!\r\n");                       /*判断任务是否创建成功*/
	} 
	
	/*创建IMU任务*/
	xReturn = xTaskCreate((TaskFunction_t )imu_task,           /*任务函数*/  	
							(const char*    )"imu_task",   	               /*任务名称*/
							(uint16_t       )IMU_STK_SIZE,                 /*任务堆栈大小*/
							(void*          )NULL,                         /*传递给任务函数的参数*/				
							(UBaseType_t    )IMU_TASK_PRIO,                /*任务优先级*/	
							(TaskHandle_t*  )&IMUTask_Handler);            /*任务句柄*/
	if(xReturn==pdPASS){
//		printf("IMU TASK SUCCEED!\r\n");                       /*判断任务是否创建成功*/
	} 
	
	/*创建Photo任务*/
	xReturn = xTaskCreate((TaskFunction_t )photo_task,         /*任务函数*/  	
							(const char*    )"photo_task",   	             /*任务名称*/
							(uint16_t       )PHOTO_STK_SIZE,               /*任务堆栈大小*/
							(void*          )NULL,                         /*传递给任务函数的参数*/				
							(UBaseType_t    )PHOTO_TASK_PRIO,              /*任务优先级*/	
							(TaskHandle_t*  )&PHOTOTask_Handler);          /*任务句柄*/
	if(xReturn==pdPASS){
//		printf("PHOTO TASK SUCCEED!\r\n");                     /*判断任务是否创建成功*/
	}

	/*创建Query任务*/
	xReturn = xTaskCreate((TaskFunction_t )query_task,         /*任务函数*/ 	
							(const char*    )"query_task",   	             /*任务名称*/
							(uint16_t       )QUERY_STK_SIZE,               /*任务堆栈大小*/
							(void*          )NULL,	                       /*传递给任务函数的参数*/			
							(UBaseType_t    )QUERY_TASK_PRIO,              /*任务优先级*/	
							(TaskHandle_t*  )&QUERYTask_Handler);          /*任务句柄*/
	if(xReturn==pdPASS){
//		printf("QUERY TASK SUCCEED!\r\n");                     /*判断任务是否创建成功*/
	}
	
	vTaskSuspend(PHOTOTask_Handler);                           /*挂起相册任务*/
	vTaskSuspend(LCDIMUTask_Handler);                          /*挂起体感模式任务*/
	vTaskSuspend(QUERYTask_Handler);                           /*挂起任务状态查询任务*/
	
	vTaskDelete(StartTask_Handler);                            /*删除开始任务*/
	taskEXIT_CRITICAL();                                       /*退出临界区*/
} 

/*******************************************************************************
* Function Name  : led_task
* Description    : LED显示任务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void led_task(void *pvParameters)
{
	u32 Received_Charger = 0;                                  /*定义接收任务通知的电量变量*/
	
    for(;;)
    {
				xTaskNotifyWait(0, 0, &Received_Charger, 0);         /*从charger task接收任务通知的电量*/
				Charger_Disp(Received_Charger);                      /*LED灯显示电量*/
        vTaskDelay(500);                                     /*延时500ms*/
    }
} 

/*******************************************************************************
* Function Name  : uart_task
* Description    : UART打印任务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void uart_task(void *pvParameters){
	
		TickType_t preTime;
		u16 Received_KEY1 = 0;
		u16 Received_Handle_LX = 0;
		u16 Received_Handle_LY = 0;
		u16 Received_Handle_RX = 0;
		u16 Received_Handle_RY = 0;
		float Received_PITCH  = 0.0;
		float Received_ROLL   = 0.0;
		preTime = xTaskGetTickCount();
		
		for(;;)
    {
			if (xSemaphoreTake(xUARTMutex, portMAX_DELAY) == pdTRUE) {
				xQueueReceive(KEY1_Queue, &Received_KEY1, 0);
				
				if(Received_KEY1%2 == 0){
					xQueueReceive(Handle_LX_Queue, &Received_Handle_LX, 0);
					xQueueReceive(Handle_LY_Queue, &Received_Handle_LY, 0);
					xQueueReceive(Handle_RX_Queue, &Received_Handle_RX, 0);
					xQueueReceive(Handle_RY_Queue, &Received_Handle_RY, 0);
					
					if(Received_Handle_LX<1000){
						printf("摇杆模式：左转\r\n");
					}
					else if(Received_Handle_LX>3000){
						printf("摇杆模式：右转\r\n");
					}
					
					if(Received_Handle_RY<1000){
						printf("摇杆模式：后退\r\n");
					}
					else if(Received_Handle_RY>3000){
						printf("摇杆模式：前进\r\n");
					}
					
					if((Received_Handle_LX>=1000)&&(Received_Handle_LX<=3000)&&(Received_Handle_RY>=1000)&&(Received_Handle_RY<=3000)){
						printf("摇杆模式：停止\r\n");
					}
				}
				else{
					xQueueReceive(PITCH_Queue, &Received_PITCH, 0);
					xQueueReceive(ROLL_Queue , &Received_ROLL , 0);
					
					if((Received_PITCH < -15)&&(Received_PITCH > -90)){
						printf("体感模式：前进\r\n");
					}
					else if((Received_PITCH > 15)&&(Received_PITCH < 90)){
						printf("体感模式：后退\r\n");
					}
					
					if((Received_ROLL > 15)&&(Received_ROLL < 90)){
						printf("体感模式：左转\r\n");
					}
					else if((Received_ROLL < -15)&&(Received_ROLL > -90)){
						printf("体感模式：右转\r\n");
					}
					
					if((Received_PITCH >= -15)&&(Received_PITCH <= 15)&&(Received_ROLL >= -15)&&(Received_ROLL <= 15)){
						printf("体感模式：停止\r\n");
					}
				}
				xSemaphoreGive(xUARTMutex);
			}
        vTaskDelayUntil(&preTime, 10);
    }
}

/*******************************************************************************
* Function Name  : handle_task
* Description    : 手柄扫描任务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void handle_task(void *pvParameters){
	
	u16 Handle_LX = 2048;                              /*定义要发送的左摇杆X轴变量*/
	u16 Handle_LY = 2048;                              /*定义要发送的左摇杆Y轴变量*/      
	u16 Handle_RX = 2048;                              /*定义要发送的右摇杆X轴变量*/
	u16 Handle_RY = 2048;                              /*定义要发送的右摇杆Y轴变量*/
	
	for(;;){
		
		Handle_LX = Get_Adc_Average(ADC_Channel_1,1);    /*获得左摇杆X轴ADC数据*/
		Handle_LY = Get_Adc_Average(ADC_Channel_2,1);    /*获得左摇杆Y轴ADC数据*/
		Handle_RX = Get_Adc_Average(ADC_Channel_14,1);   /*获得右摇杆X轴ADC数据*/
		Handle_RY = Get_Adc_Average(ADC_Channel_15,1);   /*获得右摇杆Y轴ADC数据*/
		
		xQueueSend(Handle_LX_Queue, &Handle_LX, 0);      /*发送左摇杆X轴ADC数据到消息队列*/
		xQueueSend(Handle_LY_Queue, &Handle_LY, 0);      /*发送左摇杆Y轴ADC数据到消息队列*/
		xQueueSend(Handle_RX_Queue, &Handle_RX, 0);      /*发送右摇杆X轴ADC数据到消息队列*/
		xQueueSend(Handle_RY_Queue, &Handle_RY, 0);      /*发送右摇杆Y轴ADC数据到消息队列*/
		
		vTaskDelay(5);                                   /*延时5ms*/
	}
}

/*******************************************************************************
* Function Name  : lcd_handle_task
* Description    : lcd刷新摇杆模式任务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void lcd_handle_task(void *pvParameters){
	
	u16 Received_Handle_LX = 2048;
	u16 Received_Handle_LY = 2048;
	u16 Received_Handle_RX = 2048;
	u16 Received_Handle_RY = 2048;
	
	for(;;){
		
		if (xSemaphoreTake(xLCDMutex, portMAX_DELAY) == pdTRUE) {
			
			LCD_RTOSShowPicture(82,84,40,18,gImage_Handle);
		
			xQueueReceive(Handle_LX_Queue, &Received_Handle_LX, 0);
			xQueueReceive(Handle_LY_Queue, &Received_Handle_LY, 0);
			xQueueReceive(Handle_RX_Queue, &Received_Handle_RX, 0);
			xQueueReceive(Handle_RY_Queue, &Received_Handle_RY, 0);
			
			if(Received_Handle_LX<1000){
				LCD_ShowPicture(82,106,40,18,gImage_Left);
			}
			else if(Received_Handle_LX>3000){
				LCD_ShowPicture(82,106,40,18,gImage_Right);
			}
			
			if(Received_Handle_RY<1000){
				LCD_ShowPicture(82,106,40,18,gImage_Back);
			}
			else if(Received_Handle_RY>3000){
				LCD_ShowPicture(82,106,40,18,gImage_Forward);
			}
			
			if((Received_Handle_LX>=1000)&&(Received_Handle_LX<=3000)&&(Received_Handle_RY>=1000)&&(Received_Handle_RY<=3000)){
				LCD_ShowPicture(82,106,40,18,gImage_Stop);
			}
			
			xSemaphoreGive(xLCDMutex);
		}
		
		vTaskDelay(5);
		
	}
	
}

/*******************************************************************************
* Function Name  : lcd_imu_task
* Description    : lcd刷新体感模式任务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void lcd_imu_task(void *pvParameters){
	
	float Received_PITCH  = 0.0;
	float Received_ROLL   = 0.0;
	
	for(;;){
		
		 if (xSemaphoreTake(xLCDMutex, portMAX_DELAY) == pdTRUE) {
			 
			LCD_ShowPicture(82,84,40,18,gImage_IMU);
		
			xQueueReceive(PITCH_Queue, &Received_PITCH, 0);
			xQueueReceive(ROLL_Queue , &Received_ROLL , 0);
			
			if((Received_PITCH < -15)&&(Received_PITCH > -90)){
				LCD_ShowPicture(82,106,40,18,gImage_Forward);
			}
			else if((Received_PITCH > 15)&&(Received_PITCH < 90)){
				LCD_ShowPicture(82,106,40,18,gImage_Back);
			}
			
			if((Received_ROLL > 15)&&(Received_ROLL < 90)){
				LCD_ShowPicture(82,106,40,18,gImage_Left);
			}
			else if((Received_ROLL < -15)&&(Received_ROLL > -90)){
				LCD_ShowPicture(82,106,40,18,gImage_Right);
			}
			
			if((Received_PITCH >= -15)&&(Received_PITCH <= 15)&&(Received_ROLL >= -15)&&(Received_ROLL <= 15)){
				LCD_ShowPicture(82,106,40,18,gImage_Stop);
			}
			 
       xSemaphoreGive(xLCDMutex);
		 }

		
		vTaskDelay(10);
		

	}
	
}

/*******************************************************************************
* Function Name  : charger_task
* Description    : 电量检测任务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void charger_task(void *pvParameters){
	
	u16 Charger = 0;
	
	for(;;){
		
		Charger = Get_Adc_Average_CHG(ADC_Channel_3,10);
		if(Charger>=4096) Charger = 4096;
		if(Charger<=3413) Charger = 3413;
		
		xTaskNotify(LEDTask_Handler, Charger, eSetValueWithOverwrite);
		
		vTaskDelay(500);
	}
	
}

/*******************************************************************************
* Function Name  : key_task
* Description    : 按键扫描任务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void key_task(void *pvParameters){
	u8  key = 0;
	u16 Send_KEY1 = 0;
	u16 Send_KEY5 = 0;
	u8 Send_Page = 1;
	u8 Photo_Flag = 0;
	
	for(;;)
	{
		xQueueSend(KEY1_Queue, &Send_KEY1, 0);
		
		key=KEY_Scan(0);
		
		switch(key)
		{
			case KEY1_PRES:
					while(xSemaphoreTake(xLCDMutex, portMAX_DELAY) != pdTRUE);
					if(Photo_Flag == 0){
						Send_KEY1++;
						if(Send_KEY1%2 == 0){
							vTaskSuspend(LCDIMUTask_Handler);
							vTaskResume(LCDHandleTask_Handler);
						}
						else{
							vTaskSuspend(LCDHandleTask_Handler);
							vTaskResume(LCDIMUTask_Handler);
						}
					}
					xSemaphoreGive(xLCDMutex);
				break;
			
			case KEY2_PRES:
					while(xSemaphoreTake(xLCDMutex, portMAX_DELAY) != pdTRUE);
					Photo_Flag = 1;
					Send_Page = 1;
					xQueueSend(PHOTO_Queue, &Send_Page, 0);
					vTaskSuspend(UARTTask_Handler);
					vTaskSuspend(LCDHandleTask_Handler);
					vTaskSuspend(LCDIMUTask_Handler);
					vTaskResume(PHOTOTask_Handler);
					xSemaphoreGive(xLCDMutex);
				break;
			
			case KEY3_PRES:
					while(xSemaphoreTake(xLCDMutex, portMAX_DELAY) != pdTRUE);
					Photo_Flag = 0;
					Send_KEY1 = 0;
					LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
					LCD_ShowPicture(24,0,80,80,gImage_Picture);
					LCD_ShowPicture(6,84,70,18,gImage_Mode);
					LCD_ShowPicture(6,106,70,18,gImage_Order);
					LCD_ShowPicture(82,84,40,18,gImage_Handle);
					LCD_ShowPicture(82,106,40,18,gImage_Stop);
					vTaskResume(LCDHandleTask_Handler);
					vTaskResume(UARTTask_Handler);
					vTaskSuspend(LCDIMUTask_Handler);
					vTaskSuspend(PHOTOTask_Handler);
					xSemaphoreGive(xLCDMutex);
				break;
			
			case KEY4_PRES:
					while(xSemaphoreTake(xLCDMutex, portMAX_DELAY) != pdTRUE);
					Send_Page--;
					if(Send_Page<1) Send_Page = 1;
					xQueueSend(PHOTO_Queue, &Send_Page, 0);
					xSemaphoreGive(xLCDMutex);
				break;
			
			case KEY5_PRES:
					while(xSemaphoreTake(xUARTMutex, portMAX_DELAY) != pdTRUE);
					Send_KEY5++;
					if(Send_KEY5%2 == 0){
						vTaskSuspend(QUERYTask_Handler);
						vTaskResume(UARTTask_Handler);
					}
					else{
						vTaskSuspend(UARTTask_Handler);
						vTaskResume(QUERYTask_Handler);
					}
					xSemaphoreGive(xUARTMutex);
				break;
			
			case KEY6_PRES:
					while(xSemaphoreTake(xLCDMutex, portMAX_DELAY) != pdTRUE);
					Send_Page++;
					if(Send_Page>3) Send_Page = 3;
					xQueueSend(PHOTO_Queue, &Send_Page, 0);
					xSemaphoreGive(xLCDMutex);
				break;
			
			default:
				break;
		}
		vTaskDelay(10);			//延时10ms 
	}
}

/*******************************************************************************
* Function Name  : imu_task
* Description    : imu检测任务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void imu_task(void *pvParameters){
	
	float Pitch = 0.0, Roll = 0.0, Yaw = 0.0;
	
	for(;;){
		
		MPU6050_Pose(&Pitch, &Roll, &Yaw);
		
		xQueueSend(PITCH_Queue, &Pitch, 0);
		xQueueSend(ROLL_Queue , &Roll,  0);
		xQueueSend(YAW_Queue  , &Yaw,   0);
		
		vTaskDelay(10);			//延时1ms 
	}
}

/*******************************************************************************
* Function Name  : photo_task
* Description    : 相册显示任务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void photo_task(void *pvParameters)
{
	u8 Received_Page = 1;
	
    for(;;)
    {
			if (xSemaphoreTake(xLCDMutex, portMAX_DELAY) == pdTRUE){
				if(xQueueReceive(PHOTO_Queue, &Received_Page, 0) == pdTRUE){
					switch(Received_Page){
						case 1:
							LCD_ShowPicture(0,0,128,128,gImage_Dog);
							break;
						case 2:
							LCD_ShowPicture(0,0,128,128,gImage_Xiaoxin);
							break;
						case 3:
							LCD_ShowPicture(0,0,128,128,gImage_LineDog);
							break;
						default:
							break;
					}
				}
				xSemaphoreGive(xLCDMutex);
			}
		
        vTaskDelay(10);
    }
} 

/*******************************************************************************
* Function Name  : query_task
* Description    : 任务状态统计函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void query_task(void *pvParameters){
	
	
	for(;;){
		while(xSemaphoreTake(xUARTMutex, portMAX_DELAY) != pdTRUE);
		printf("\r\n");
		
		printf("-----------------------------------------------------------\r\n");
		printf("|                                                         |\r\n");
		printf("|                       任务运行统计                      |\r\n");
		printf("|                                                         |\r\n");
		printf("-----------------------------------------------------------\r\n");
		printf("|           任务名           |任务状态|优先级|空闲栈|任务号|\r\n");
		printf("-----------------------------------------------------------\r\n");
		
		vTaskList(InfoBuffer);							//获取所有任务的信息
		printf("%s\r\n",InfoBuffer);					//通过串口打印所有任务的信息
		printf("-----------------------------------------------------------\r\n");
		printf("\r\n");
		xSemaphoreGive(xUARTMutex);
		vTaskSuspend(NULL);
		vTaskDelay(1);
	}
}
