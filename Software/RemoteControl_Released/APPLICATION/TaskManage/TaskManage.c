/********************************************************************************
* @File name   : TaskManage.c
* @Author      : Daemon.Chen
* @Version     : Released
* @Date        : 2024-5-4
* @Description : ����ϵͳ�¸���������������ͷ�ļ�
********************************************************************************/

#include "TaskManage.h"                     /*�������ͷ�ļ�*/
#include "sys.h"                            /*ϵͳͷ�ļ�*/
#include "FreeRTOS.h"                       /*FreeRTOSͷ�ļ�*/
#include "task.h"                           /*����ͷ�ļ�*/
#include "queue.h"                          /*����ͷ�ļ�*/
#include "Picture.h"                        /*UIͼƬͷ�ļ�*/
#include "semphr.h"                         /*������ͷ�ļ�*/
#include "event_groups.h"                   /*�¼���־��ͷ�ļ�*/
#include "string.h"                         /*�ַ���ͷ�ļ�*/

/**********����궨��*********/

/*��ʼ����*/
#define START_TASK_PRIO		    1             /*�������ȼ�*/
#define START_STK_SIZE 		    128           /*�����ջ��С*/  
TaskHandle_t StartTask_Handler;             /*������*/
void start_task(void *pvParameters);        /*������*/

/*LED��ʾ����*/
#define LED_TASK_PRIO		      2             /*�������ȼ�*/
#define LED_STK_SIZE 		      256           /*�����ջ��С*/  
TaskHandle_t LEDTask_Handler;               /*������*/
void led_task(void *pvParameters);          /*������*/

/*���ߴ��ڷ�������*/
#define UART_TASK_PRIO		    2             /*�������ȼ�*/
#define UART_STK_SIZE 		    256           /*�����ջ��С*/  
TaskHandle_t UARTTask_Handler;              /*������*/
void uart_task(void *pvParameters);         /*������*/

/*�ֱ��������*/
#define HANDLE_TASK_PRIO	    2             /*�������ȼ�*/
#define HANDLE_STK_SIZE 	    256           /*�����ջ��С*/  
TaskHandle_t HANDLETask_Handler;            /*������*/
void handle_task(void *pvParameters);       /*������*/

/*ҡ��ģʽ����*/
#define LCD_HANDLE_TASK_PRIO	1             /*�������ȼ�*/
#define LCD_HANDLE_STK_SIZE 	512           /*�����ջ��С*/  
TaskHandle_t LCDHandleTask_Handler;         /*������*/
void lcd_handle_task(void *pvParameters);   /*������*/

/*���ģʽ����*/
#define LCD_IMU_TASK_PRIO	    1             /*�������ȼ�*/
#define LCD_IMU_STK_SIZE 	    1024          /*�����ջ��С*/
TaskHandle_t LCDIMUTask_Handler;            /*������*/
void lcd_imu_task(void *pvParameters);      /*������*/

/*�����������*/
#define CHARGER_TASK_PRIO	    2             /*�������ȼ�*/
#define CHARGER_STK_SIZE 	    256           /*�����ջ��С*/  
TaskHandle_t CHARGERTask_Handler;           /*������*/
void charger_task(void *pvParameters);      /*������*/

/*�����������*/
#define KEY_TASK_PRIO	        2             /*�������ȼ�*/
#define KEY_STK_SIZE 	        1024          /*�����ջ��С*/ 
TaskHandle_t KEYTask_Handler;               /*������*/
void key_task(void *pvParameters);          /*������*/

/*IMU�������*/
#define IMU_TASK_PRIO	        2             /*�������ȼ�*/	
#define IMU_STK_SIZE 	        256           /*�����ջ��С*/  
TaskHandle_t IMUTask_Handler;               /*������*/
void imu_task(void *pvParameters);          /*������*/

/*��Ქ������*/
#define PHOTO_TASK_PRIO	      1             /*�������ȼ�*/
#define PHOTO_STK_SIZE 	      1024          /*�����ջ��С*/ 
TaskHandle_t PHOTOTask_Handler;             /*������*/
void photo_task(void *pvParameters);        /*������*/

/*����״̬ͳ������*/
#define QUERY_TASK_PRIO	      1             /*�������ȼ�*/
#define QUERY_STK_SIZE 	      1024          /*�����ջ��С*/
TaskHandle_t QUERYTask_Handler;             /*������*/
void query_task(void *pvParameters);        /*������*/

/********************************/

/**********��Ϣ���к궨��*********/

/*���ֱ�X����Ϣ����*/
#define Handle_LX_NUM     5                 /*��Ϣ��������*/
QueueHandle_t Handle_LX_Queue;              /*��Ϣ���о��*/

/*���ֱ�Y����Ϣ����*/
#define Handle_LY_NUM     5                 /*��Ϣ��������*/
QueueHandle_t Handle_LY_Queue;              /*��Ϣ���о��*/

/*���ֱ�X����Ϣ����*/
#define Handle_RX_NUM     5                 /*��Ϣ��������*/
QueueHandle_t Handle_RX_Queue;              /*��Ϣ���о��*/

/*���ֱ�Y����Ϣ����*/
#define Handle_RY_NUM     5                 /*��Ϣ��������*/
QueueHandle_t Handle_RY_Queue;              /*��Ϣ���о��*/

/*����1��Ϣ����*/
#define KEY1_NUM          1                 /*��Ϣ��������*/
QueueHandle_t KEY1_Queue;                   /*��Ϣ���о��*/

/*��������Ϣ����*/
#define PITCH_NUM         5                 /*��Ϣ��������*/
QueueHandle_t PITCH_Queue;                  /*��Ϣ���о��*/

/*�������Ϣ����*/
#define ROLL_NUM          5                 /*��Ϣ��������*/
QueueHandle_t ROLL_Queue;                   /*��Ϣ���о��*/

/*ƫ������Ϣ����*/
#define YAW_NUM           5                 /*��Ϣ��������*/
QueueHandle_t YAW_Queue;                    /*��Ϣ���о��*/

/*��ᷭҳ��Ϣ����*/
#define PHOTO_NUM         1                 /*��Ϣ��������*/
QueueHandle_t PHOTO_Queue;                  /*��Ϣ���о��*/

/********************************/

/**********�����ź�������*********/

SemaphoreHandle_t xLCDMutex;                /*LCD�ٽ���Դ��������ź���*/
SemaphoreHandle_t xUARTMutex;               /*UART�ٽ���Դ��������ź���*/

/********************************/

/*********����������Ϣ����********/

char InfoBuffer[1000];                      /*����������Ϣ����*/

/********************************/

/*******************************************************************************
* Function Name  : TaskCreateFunction
* Description    : ����������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TaskCreateFunction(void)
{   
	xTaskCreate((TaskFunction_t )start_task,      /*������*/
				(const char*    )"start_task",          /*��������*/
				(uint16_t       )START_STK_SIZE,        /*�����ջ��С*/
				(void*          )NULL,                  /*���ݸ��������Ĳ���*/
				(UBaseType_t    )START_TASK_PRIO,       /*�������ȼ�*/
				(TaskHandle_t*  )&StartTask_Handler);   /*������*/
}

/*******************************************************************************
* Function Name  : start_task
* Description    : ��ʼ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void start_task(void *pvParameters)
{ 
	BaseType_t xReturn = pdPASS;                                      /*���巵������*/
	taskENTER_CRITICAL();                                             /*�����ٽ���*/
	
	
	/*������Ϣ����*/
  Handle_LX_Queue = xQueueCreate(Handle_LX_NUM,sizeof(u16));        /*������ϢHandle_LX_Queue*/
	Handle_LY_Queue = xQueueCreate(Handle_LY_NUM,sizeof(u16));        /*������ϢHandle_LY_Queue*/
	Handle_RX_Queue = xQueueCreate(Handle_RX_NUM,sizeof(u16));        /*������ϢHandle_RX_Queue*/
	Handle_RY_Queue = xQueueCreate(Handle_RY_NUM,sizeof(u16));        /*������ϢHandle_RY_Queue*/
	KEY1_Queue      = xQueueCreate(KEY1_NUM     ,sizeof(u16));        /*������ϢKEY1_Queue*/
	PITCH_Queue     = xQueueCreate(PITCH_NUM    ,sizeof(float));      /*������ϢPITCH_Queue*/
	ROLL_Queue      = xQueueCreate(ROLL_NUM     ,sizeof(float));      /*������ϢROLL_Queue*/
	YAW_Queue       = xQueueCreate(YAW_NUM      ,sizeof(float));      /*������ϢYAW_Queue*/
	PHOTO_Queue     = xQueueCreate(PHOTO_NUM    ,sizeof(u8));         /*������ϢPHOTO_Queue*/
	
	/*�����������ź���*/
	xLCDMutex  = xSemaphoreCreateMutex();                             /*����LCD���ʻ����ź���*/
	xUARTMutex = xSemaphoreCreateMutex();                             /*����UART���ʻ����ź���*/
	
	/*����LED����*/
	xReturn = xTaskCreate((TaskFunction_t )led_task,           /*������*/    	
							(const char*    )"led_task",   	               /*��������*/
							(uint16_t       )LED_STK_SIZE,                 /*�����ջ��С*/
							(void*          )NULL,                         /*���ݸ��������Ĳ���*/				
							(UBaseType_t    )LED_TASK_PRIO,                /*�������ȼ�*/	
							(TaskHandle_t*  )&LEDTask_Handler);            /*������*/
	if(xReturn==pdPASS){
//		printf("LED TASK SUCCEED!\r\n");                       /*�ж������Ƿ񴴽��ɹ�*/
	}
	
	/*����UART����*/
	xReturn = xTaskCreate((TaskFunction_t )uart_task,          /*������*/  	
							(const char*    )"uart_task",   	             /*��������*/
							(uint16_t       )UART_STK_SIZE,                /*�����ջ��С*/
							(void*          )NULL,                         /*���ݸ��������Ĳ���*/				
							(UBaseType_t    )UART_TASK_PRIO,               /*�������ȼ�*/	
							(TaskHandle_t*  )&UARTTask_Handler);           /*������*/
	if(xReturn==pdPASS){
//		printf("UART TASK SUCCEED!\r\n");                      /*�ж������Ƿ񴴽��ɹ�*/
	} 
	
	/*����HANDLE����*/
	xReturn = xTaskCreate((TaskFunction_t )handle_task,        /*������*/   	
							(const char*    )"handle_task",   	           /*��������*/
							(uint16_t       )HANDLE_STK_SIZE,              /*�����ջ��С*/
							(void*          )NULL,                         /*���ݸ��������Ĳ���*/				
							(UBaseType_t    )HANDLE_TASK_PRIO,             /*�������ȼ�*/	
							(TaskHandle_t*  )&HANDLETask_Handler);         /*������*/
	if(xReturn==pdPASS){
//		printf("HANDLE TASK SUCCEED!\r\n");                    /*�ж������Ƿ񴴽��ɹ�*/
	} 
	
	/*����LCD HANDLE����*/
	xReturn = xTaskCreate((TaskFunction_t )lcd_handle_task,    /*������*/   	
							(const char*    )"lcd_handle_task",   	       /*��������*/
							(uint16_t       )LCD_HANDLE_STK_SIZE,          /*�����ջ��С*/
							(void*          )NULL,                         /*���ݸ��������Ĳ���*/				
							(UBaseType_t    )LCD_HANDLE_TASK_PRIO,         /*�������ȼ�*/	
							(TaskHandle_t*  )&LCDHandleTask_Handler);      /*������*/
	if(xReturn==pdPASS){
//		printf("LCD HANDLE TASK SUCCEED!\r\n");                /*�ж������Ƿ񴴽��ɹ�*/
	} 
	
	/*����LCD IMU����*/
	xReturn = xTaskCreate((TaskFunction_t )lcd_imu_task,       /*������*/  	
							(const char*    )"lcd_imu_task",   	           /*��������*/
							(uint16_t       )LCD_IMU_STK_SIZE,             /*�����ջ��С*/
							(void*          )NULL,                         /*���ݸ��������Ĳ���*/				
							(UBaseType_t    )LCD_IMU_TASK_PRIO,            /*�������ȼ�*/	
							(TaskHandle_t*  )&LCDIMUTask_Handler);         /*������*/
	if(xReturn==pdPASS){
//		printf("LCD IMU TASK SUCCEED!\r\n");                   /*�ж������Ƿ񴴽��ɹ�*/
	} 
	
	/*����CHARGER����*/
	xReturn = xTaskCreate((TaskFunction_t )charger_task,       /*������*/  	
							(const char*    )"charger_task",   	           /*��������*/
							(uint16_t       )CHARGER_STK_SIZE,             /*�����ջ��С*/
							(void*          )NULL,                         /*���ݸ��������Ĳ���*/				
							(UBaseType_t    )CHARGER_TASK_PRIO,            /*�������ȼ�*/	
							(TaskHandle_t*  )&CHARGERTask_Handler);        /*������*/
	if(xReturn==pdPASS){
//		printf("CHARGER TASK SUCCEED!\r\n");                   /*�ж������Ƿ񴴽��ɹ�*/
	} 
	
	/*����KEY����*/
	xReturn = xTaskCreate((TaskFunction_t )key_task,     	     /*������*/
							(const char*    )"key_task",   	               /*��������*/
							(uint16_t       )KEY_STK_SIZE,                 /*�����ջ��С*/
							(void*          )NULL,                         /*���ݸ��������Ĳ���*/				
							(UBaseType_t    )KEY_TASK_PRIO,                /*�������ȼ�*/	
							(TaskHandle_t*  )&KEYTask_Handler);            /*������*/
	if(xReturn==pdPASS){
//		printf("KEY TASK SUCCEED!\r\n");                       /*�ж������Ƿ񴴽��ɹ�*/
	} 
	
	/*����IMU����*/
	xReturn = xTaskCreate((TaskFunction_t )imu_task,           /*������*/  	
							(const char*    )"imu_task",   	               /*��������*/
							(uint16_t       )IMU_STK_SIZE,                 /*�����ջ��С*/
							(void*          )NULL,                         /*���ݸ��������Ĳ���*/				
							(UBaseType_t    )IMU_TASK_PRIO,                /*�������ȼ�*/	
							(TaskHandle_t*  )&IMUTask_Handler);            /*������*/
	if(xReturn==pdPASS){
//		printf("IMU TASK SUCCEED!\r\n");                       /*�ж������Ƿ񴴽��ɹ�*/
	} 
	
	/*����Photo����*/
	xReturn = xTaskCreate((TaskFunction_t )photo_task,         /*������*/  	
							(const char*    )"photo_task",   	             /*��������*/
							(uint16_t       )PHOTO_STK_SIZE,               /*�����ջ��С*/
							(void*          )NULL,                         /*���ݸ��������Ĳ���*/				
							(UBaseType_t    )PHOTO_TASK_PRIO,              /*�������ȼ�*/	
							(TaskHandle_t*  )&PHOTOTask_Handler);          /*������*/
	if(xReturn==pdPASS){
//		printf("PHOTO TASK SUCCEED!\r\n");                     /*�ж������Ƿ񴴽��ɹ�*/
	}

	/*����Query����*/
	xReturn = xTaskCreate((TaskFunction_t )query_task,         /*������*/ 	
							(const char*    )"query_task",   	             /*��������*/
							(uint16_t       )QUERY_STK_SIZE,               /*�����ջ��С*/
							(void*          )NULL,	                       /*���ݸ��������Ĳ���*/			
							(UBaseType_t    )QUERY_TASK_PRIO,              /*�������ȼ�*/	
							(TaskHandle_t*  )&QUERYTask_Handler);          /*������*/
	if(xReturn==pdPASS){
//		printf("QUERY TASK SUCCEED!\r\n");                     /*�ж������Ƿ񴴽��ɹ�*/
	}
	
	vTaskSuspend(PHOTOTask_Handler);                           /*�����������*/
	vTaskSuspend(LCDIMUTask_Handler);                          /*�������ģʽ����*/
	vTaskSuspend(QUERYTask_Handler);                           /*��������״̬��ѯ����*/
	
	vTaskDelete(StartTask_Handler);                            /*ɾ����ʼ����*/
	taskEXIT_CRITICAL();                                       /*�˳��ٽ���*/
} 

/*******************************************************************************
* Function Name  : led_task
* Description    : LED��ʾ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void led_task(void *pvParameters)
{
	u32 Received_Charger = 0;                                  /*�����������֪ͨ�ĵ�������*/
	
    for(;;)
    {
				xTaskNotifyWait(0, 0, &Received_Charger, 0);         /*��charger task��������֪ͨ�ĵ���*/
				Charger_Disp(Received_Charger);                      /*LED����ʾ����*/
        vTaskDelay(500);                                     /*��ʱ500ms*/
    }
} 

/*******************************************************************************
* Function Name  : uart_task
* Description    : UART��ӡ������
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
						printf("ҡ��ģʽ����ת\r\n");
					}
					else if(Received_Handle_LX>3000){
						printf("ҡ��ģʽ����ת\r\n");
					}
					
					if(Received_Handle_RY<1000){
						printf("ҡ��ģʽ������\r\n");
					}
					else if(Received_Handle_RY>3000){
						printf("ҡ��ģʽ��ǰ��\r\n");
					}
					
					if((Received_Handle_LX>=1000)&&(Received_Handle_LX<=3000)&&(Received_Handle_RY>=1000)&&(Received_Handle_RY<=3000)){
						printf("ҡ��ģʽ��ֹͣ\r\n");
					}
				}
				else{
					xQueueReceive(PITCH_Queue, &Received_PITCH, 0);
					xQueueReceive(ROLL_Queue , &Received_ROLL , 0);
					
					if((Received_PITCH < -15)&&(Received_PITCH > -90)){
						printf("���ģʽ��ǰ��\r\n");
					}
					else if((Received_PITCH > 15)&&(Received_PITCH < 90)){
						printf("���ģʽ������\r\n");
					}
					
					if((Received_ROLL > 15)&&(Received_ROLL < 90)){
						printf("���ģʽ����ת\r\n");
					}
					else if((Received_ROLL < -15)&&(Received_ROLL > -90)){
						printf("���ģʽ����ת\r\n");
					}
					
					if((Received_PITCH >= -15)&&(Received_PITCH <= 15)&&(Received_ROLL >= -15)&&(Received_ROLL <= 15)){
						printf("���ģʽ��ֹͣ\r\n");
					}
				}
				xSemaphoreGive(xUARTMutex);
			}
        vTaskDelayUntil(&preTime, 10);
    }
}

/*******************************************************************************
* Function Name  : handle_task
* Description    : �ֱ�ɨ��������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void handle_task(void *pvParameters){
	
	u16 Handle_LX = 2048;                              /*����Ҫ���͵���ҡ��X�����*/
	u16 Handle_LY = 2048;                              /*����Ҫ���͵���ҡ��Y�����*/      
	u16 Handle_RX = 2048;                              /*����Ҫ���͵���ҡ��X�����*/
	u16 Handle_RY = 2048;                              /*����Ҫ���͵���ҡ��Y�����*/
	
	for(;;){
		
		Handle_LX = Get_Adc_Average(ADC_Channel_1,1);    /*�����ҡ��X��ADC����*/
		Handle_LY = Get_Adc_Average(ADC_Channel_2,1);    /*�����ҡ��Y��ADC����*/
		Handle_RX = Get_Adc_Average(ADC_Channel_14,1);   /*�����ҡ��X��ADC����*/
		Handle_RY = Get_Adc_Average(ADC_Channel_15,1);   /*�����ҡ��Y��ADC����*/
		
		xQueueSend(Handle_LX_Queue, &Handle_LX, 0);      /*������ҡ��X��ADC���ݵ���Ϣ����*/
		xQueueSend(Handle_LY_Queue, &Handle_LY, 0);      /*������ҡ��Y��ADC���ݵ���Ϣ����*/
		xQueueSend(Handle_RX_Queue, &Handle_RX, 0);      /*������ҡ��X��ADC���ݵ���Ϣ����*/
		xQueueSend(Handle_RY_Queue, &Handle_RY, 0);      /*������ҡ��Y��ADC���ݵ���Ϣ����*/
		
		vTaskDelay(5);                                   /*��ʱ5ms*/
	}
}

/*******************************************************************************
* Function Name  : lcd_handle_task
* Description    : lcdˢ��ҡ��ģʽ������
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
* Description    : lcdˢ�����ģʽ������
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
* Description    : �������������
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
* Description    : ����ɨ��������
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
		vTaskDelay(10);			//��ʱ10ms 
	}
}

/*******************************************************************************
* Function Name  : imu_task
* Description    : imu���������
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
		
		vTaskDelay(10);			//��ʱ1ms 
	}
}

/*******************************************************************************
* Function Name  : photo_task
* Description    : �����ʾ������
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
* Description    : ����״̬ͳ�ƺ���
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
		printf("|                       ��������ͳ��                      |\r\n");
		printf("|                                                         |\r\n");
		printf("-----------------------------------------------------------\r\n");
		printf("|           ������           |����״̬|���ȼ�|����ջ|�����|\r\n");
		printf("-----------------------------------------------------------\r\n");
		
		vTaskList(InfoBuffer);							//��ȡ�����������Ϣ
		printf("%s\r\n",InfoBuffer);					//ͨ�����ڴ�ӡ�����������Ϣ
		printf("-----------------------------------------------------------\r\n");
		printf("\r\n");
		xSemaphoreGive(xUARTMutex);
		vTaskSuspend(NULL);
		vTaskDelay(1);
	}
}
