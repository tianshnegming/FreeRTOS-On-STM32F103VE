/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
 #include <stdio.h>
 
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

	xQueueHandle xQueue;
	TaskHandle_t TaskAboutPrintf;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

int fputc(int ch,FILE *f)
{
	 while(!(USART3->SR & (1<<7)));
	 USART3->DR = ch;
	 return ch;
}

void my_Task_LED3(void * pvParameters){
	long lValueToSend = (long)pvParameters;
	portBASE_TYPE xStatus;
	while(1){
		HAL_GPIO_TogglePin(GPIOD, LED3_Pin);
		printf("I am alive 100 \r\n"); 
//		taskYIELD();
		xStatus = xQueueSendToBack( xQueue, &lValueToSend, 0 );
		if( xStatus != pdPASS )
		{
				/* ���Ͳ������ڶ��������޷���� �C ���Ȼ���ڴ�����Ϊ�����еĶ��в��������� */
				printf( "ERROR   Could not send to the queue.\r\n" );
		}
		
		
		vTaskDelay(200);
	}
}


void my_Task_LED4(void * pvParameters){
	long lValueToSend = (long)pvParameters;
	portBASE_TYPE xStatus;
	while(1){
		HAL_GPIO_TogglePin(GPIOD, LED4_Pin);
		printf("I am alive 200 \r\n"); 
//		taskYIELD();
		xStatus = xQueueSendToBack( xQueue, &lValueToSend, 0 );
		if( xStatus != pdPASS )
		{
				/* ���Ͳ������ڶ��������޷���� �C ���Ȼ���ڴ�����Ϊ�����еĶ��в��������� */
				printf( "ERROR   Could not send to the queue.\r\n" );
		}
		
		
		vTaskDelay(1000);
	}
}



static void vReceiverTask( void *pvParameters )
{
/* �������������ڱ���Ӷ����н��յ������ݡ� */
long lReceivedValue;
portBASE_TYPE xStatus;
const portTickType xTicksToWait = 100 / portTICK_RATE_MS;
/* ��������Ȼ������ѭ���С� */
	for( ;; )
	{
		/* �˵��ûᷢ�ֶ���һֱΪ�գ���Ϊ����������ɾ����д����е����ݵ�Ԫ�� */
		if( uxQueueMessagesWaiting( xQueue ) != 0 )
		{
			printf( "Queue should have been empty!\r\n" );
		}
		/* �Ӷ����н�������
		��һ�������Ǳ���ȡ�Ķ��С������ڵ���������֮ǰ�ͱ������ˣ��������ڴ�����ִ�С�
		�ڶ��������Ǳ�����յ������ݵĻ�������ַ�������м�����lReceivedValue�ĵ�ַ���˱���������
		�������ݵ�Ԫ������ͬ���������㹻�Ĵ�С���洢���յ������ݡ�
		������������������ʱʱ�� �C �����п�ʱ������ת������״̬�Եȴ�����������Ч�������г���
		portTICK_RATE_MS������100�������ʱ��ת��Ϊ��ϵͳ����Ϊ��λ��ʱ��ֵ��
		*/
		xStatus = xQueueReceive( xQueue, &lReceivedValue, portMAX_DELAY );
			if( xStatus == pdPASS )
			{
				/* �ɹ��������ݣ���ӡ������ */
				printf( "Received = %ld \r\n " ,lReceivedValue);
			}
			else
			{
				/* �ȴ�100msҲû���յ��κ����ݡ�
				��Ȼ���ڴ�����Ϊ���������ڲ�ͣ����������д������ */
				printf( "ERROR Could not receive from the queue.\r\n" );
			}
	}
}



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_Init();
  /* USER CODE BEGIN 2 */

	/* �����Ķ������ڱ������5��ֵ��ÿ�����ݵ�Ԫ�����㹻�Ŀռ����洢һ��long�ͱ��� */
	xQueue = xQueueCreate( 5, sizeof( long ) );
		if( xQueue != NULL )
		{
				/* ��������д��������ʵ����������ڲ������ڴ��ݷ��͵����е�ֵ������һ��ʵ����ͣ�������з���
				100������һ������ʵ����ͣ�������з���200��������������ȼ�����Ϊ1�� */
				xTaskCreate( my_Task_LED3, "Sender1", 1000, ( void * ) 100, 1, NULL );
				xTaskCreate( my_Task_LED4, "Sender1", 1000, ( void * ) 200, 1, NULL );
				/* ����һ������������ʵ���������ȼ���Ϊ2������д�������ȼ� */
				xTaskCreate( vReceiverTask, "Receiver", 1000, NULL, 2, NULL );
				/* ����������������ʼִ�� */
				vTaskStartScheduler();
		}
		else
		{
				/* ���д���ʧ��*/
				while(1);
		}


		xTaskCreate(my_Task_LED3, (const char *)"LED3", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
		vTaskStartScheduler();
		
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//		HAL_GPIO_TogglePin(GPIOD, LED3_Pin|LED4_Pin);
//		HAL_Delay(1000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
