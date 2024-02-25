/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* Buffers to store received messages */
uint8_t aRXBuffer2[RX_BUFFER2_SIZE];
uint8_t aRXBuffer3[RX_BUFFER3_SIZE];

/* Circular buffer to store data from sensor */
uint32_t aRXBuffer1[RX_BUFFER1_SIZE];

/* Flags that indicate actions in main while loop */
bool wifiStatusFlag;
bool mqttStatusFlag;
bool pwmStatusFlag;
bool recStatusFlag;				// Status of message recived
bool pubStatusFlag;				// Status for publishing to MQTT broker
bool pubRdStatusFlag;
bool userStatusFlag;

uint32_t uwCntValue;

/* Timer counter value */
//static uint32_t uwCntValue;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
int  __io_putchar(int ch);
//static void initializeBuf(CircularBuffer *buffer);
//static void addToBuf(uint32_t *buffer, uint32_t value, topBottom_t* flag);
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
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
  MX_TIM16_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  /* Clear pins PA.0 and PA.4 to set inital value of sensor */
  LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_0 | LL_GPIO_PIN_4);
  /* Configure TIM2 in PWM output mode */
  Configure_TIMPWMOutput();

  /* Init values for flags */
  wifiStatusFlag 	= false;
  mqttStatusFlag 	= false;
  pwmStatusFlag 	= false;
  recStatusFlag		= false;
  pubStatusFlag 	= false;
  userStatusFlag	= false;
  pubRdStatusFlag	= false;


  /* Set init value for LED2 */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
  /* Prepare for receiving in IT after an IDLE event */
  if(HAL_OK != HAL_UARTEx_ReceiveToIdle_IT(&huart2, aRXBuffer2, RX_BUFFER2_SIZE))
  {
	  Error_Handler();
  }
  if(HAL_OK != HAL_UARTEx_ReceiveToIdle_IT(&huart3, aRXBuffer3, RX_BUFFER3_SIZE))
  {
	  Error_Handler();
  }

  char txDataInit[20];
  uint16_t len = sprintf(txDataInit, "ATE0\r\n");
  HAL_UART_Transmit(&huart2, (uint8_t *)txDataInit, len, 300);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  if(userStatusFlag)
	  {
		  userStatusFlag = false;
		  process_user_cmd((const char *)aRXBuffer2, &pwmStatusFlag);
		  pubRdStatusFlag = pwmStatusFlag && wifiStatusFlag && mqttStatusFlag;
	  }

	  if(recStatusFlag)
	  {
		  recStatusFlag = false;
		  process_at_cmd((const char *)aRXBuffer3, &wifiStatusFlag, &mqttStatusFlag);
		  pubRdStatusFlag = pwmStatusFlag && wifiStatusFlag && mqttStatusFlag;
	  }


	  if(pubStatusFlag && pubRdStatusFlag)
	  {
		  pubStatusFlag = false;
		  publish_data((const uint32_t *)(&uwCntValue));
	  }
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

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* Redirecting the printf() to send data to PC via UART2 */
int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart2, (uint8_t *)(&ch), 1, 10);
	return ch;
}


/* Sends and receives messages from UART2/UART3 and checks them */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart->Instance == USART2)
	{
		/* Ensure next IT receiving */
		if(HAL_OK != HAL_UARTEx_ReceiveToIdle_IT(&huart2, aRXBuffer2, RX_BUFFER2_SIZE))
		{
			Error_Handler();
		}

		/* If received message is user defined message, skip transmitting to UART3 */
		if(strncmp((const char *)aRXBuffer2, "AT", 2) == 0)
		{
			/* Transmit received data to WIFI module via UART3 */
			HAL_UART_Transmit_IT(&huart3, aRXBuffer2, Size);
		}
		else
		{
			userStatusFlag = true;
		}
	}
	else if(huart->Instance == USART3)
	{
		/* Ensure next IT receiving */
		if(HAL_OK != HAL_UARTEx_ReceiveToIdle_IT(&huart3, aRXBuffer3, RX_BUFFER3_SIZE))
		{
			Error_Handler();
		}
		/* Transmit received data to PC via UART2 */
		HAL_UART_Transmit_IT(&huart2, aRXBuffer3, Size);

		/* Set received message flag to process received message in main loop */
		recStatusFlag = true;
	}
}

/* Measure time of Echo signal */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_4)
	{
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		static bool bEdge = false;

		if(!bEdge)
		{
			/* Reset timer counter */
			LL_TIM_SetCounter(TIM16, 0);
			/* Start CNT timer */
			LL_TIM_EnableCounter(TIM16);

			/*Configure falling edge for GPIO pin : PA8 */
			GPIO_InitStruct.Pin = GPIO_PIN_4;
			GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

			bEdge = true;
		}
		else
		{
			uwCntValue = LL_TIM_GetCounter(TIM16);

			/* Stop CNT timer */
			LL_TIM_DisableCounter(TIM16);

			/*Configure rising edge for GPIO pin : PA8  */
			GPIO_InitStruct.Pin = GPIO_PIN_4;
			GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

			pubStatusFlag = true;

			bEdge = false;
		}
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
