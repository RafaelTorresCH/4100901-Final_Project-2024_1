/* USER CODE BEGIN Header */
/**
  **************************
  * @file           : main.c
  * @brief          : Main program body
  **************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  **************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
//
//#include "ssd1306.h"
//#include "ssd1306_fonts.h"

#include "ring_buffer.h"
#include "keypad.h"
#include "luces.h"
#include <ctype.h>  // for isdigit
#include <string.h>  // for strcmp
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
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

#define USART2_BUFFER_SIZE 8
uint8_t usart2_buffer[USART2_BUFFER_SIZE];
ring_buffer_t usart2_rb;


uint8_t usart2_rx;
uint8_t contador;
uint8_t status = 0;
uint8_t block_ = 0;

uint8_t recibido_3 =0 ;




//DIEGO
#define MAX_DIGITS 5 // Ajusta según la cantidad máxima de dígitos
char digits[MAX_DIGITS+1] = {0}; // Arreglo para almacenar los dígitos
int digit_count = 0;


uint8_t desbloqueado=-1;



#define MAX_PASSWORD_LENGTH 2 // Ajusta según la longitud de tu clave
char password[] = "11"; // Reemplaza con tu clave deseada
char input_password[MAX_PASSWORD_LENGTH + 1]; // +1 para el carácter nulo
int current_index = 0;

void process_keypad_input(uint8_t key_pressed);

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int _write(int file, char *ptr, int len)
{
  HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, 10);
  return len;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Data received in USART2 */
  if (huart->Instance == USART3) {
	  usart2_rx = USART3->RDR; // leyendo el byte recibido de USART2
	  //ring_buffer_write(&usart2_rb, usart2_rx); // put the data received in buffer
	  //HAL_UART_Transmit(&huart2, &recibido_3, 1,10);
	  //HAL_UART_Receive_IT(&huart3, &recibido_3, 1); // enable interrupt for USART2 Rx

	  //HAL_UART_Receive_IT(&huart2, &usart2_rx, 1); // enable interrupt to continue receiving
	  //ATOMIC_SET_BIT(USART2->CR1, USART_CR1_RXNEIE); // usando un funcion mas liviana para reducir memoria
  }
}

// Esta función se llamará cuando se presione una tecla
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {


    // Leer la tecla presionada
	char key = keypad_scan(GPIO_Pin);  // Convertir uint8_t a unsigned char

    //control bloqueo
    //desbloquea el sistema
    if(key == 'A'){
    	printf("desbloqueado\r\n");
    	Ublock();
    	block_ = 1;//DESBLOQUEA
    	status = 0;
    }
    //bloquea el sistema
    if(key == 'B'){
    	printf("bloquedo\r\n");
    	HAL_GPIO_WritePin(derecho_GPIO_Port,derecho_Pin, 0);
    	HAL_GPIO_WritePin(izquierdo_GPIO_Port,izquierdo_Pin, 0);
        	block_ = 2;//BLOQUEA
        	status = 0;
     }

    	//control luces
    //enciende señal izquierda
    if(key == '1'){
    	SET_(6,0,0);
    	HAL_GPIO_WritePin(derecho_GPIO_Port,derecho_Pin, 0);
    	status = 1;

    }
    //enciende parqueo
    if(key == '2'){
    	SET_(0,0xFF,0);
    	HAL_GPIO_WritePin(derecho_GPIO_Port,derecho_Pin, 0);
    	    	HAL_GPIO_WritePin(izquierdo_GPIO_Port,izquierdo_Pin, 0);
    	 status = 2;


   	}
    //enciende señal derecha
    if(key == '3'){
    	SET_(0,0,6);
    	HAL_GPIO_WritePin(izquierdo_GPIO_Port,izquierdo_Pin, 0);
    	status = 3;

	}
    //apaga el sistema
    if(key == '*'){
    	HAL_GPIO_WritePin(derecho_GPIO_Port,derecho_Pin, 0);
    	HAL_GPIO_WritePin(izquierdo_GPIO_Port,izquierdo_Pin, 0);

    	SET_(0,0,0);
    	status = 4;

    }



}

//  seguridad del codigo, a traves de lectura por teclado


void process_keypad_input(uint8_t key_pressed) {
    if (isdigit(key_pressed)) {
        // Agregar dígito a la clave
        input_password[current_index++] = key_pressed;
        input_password[current_index] = '\0'; // Agrega el terminador nulo al final
        printf("%c",input_password[current_index++]);
    } else if (key_pressed == '#') {
        // Validar la clave solo al presionar '#'
        if (strcmp(input_password, password) == 0) {
            printf("Clave correcta!\n");
            desbloqueado=1;





            // Aquí puedes hacer otras acciones como activar un LED o abrir una puerta
        } else {

        	desbloqueado=0;
            printf("Clave incorrecta!\n");

        }
        // Resetea para una nueva entrada de clave
        memset(input_password, 0, sizeof(input_password));
        current_index = 0;
    } else if (key_pressed == '*') {
        // Resetea la entrada cuando se presiona '*'
        printf("Reseteando clave...\n");
        memset(input_password, 0, sizeof(input_password));
        current_index = 0;
        ssd1306_Fill(Black); // Limpiar la pantalla
        ssd1306_UpdateScreen();
    }
}








void low_power_mode()
{
#define AWAKE_TIME (10 * 1000) // 10 segundos
	static uint32_t sleep_tick = AWAKE_TIME;

	if (sleep_tick > HAL_GetTick()) {
		return;
	}
	printf("Sleeping\r\n");
	sleep_tick = HAL_GetTick() + AWAKE_TIME;

	RCC->AHB1SMENR  = 0x0;
	RCC->AHB2SMENR  = 0x0;
	RCC->AHB3SMENR  = 0x0;

	RCC->APB1SMENR1 = 0x0;
	RCC->APB1SMENR2 = 0x0;
	RCC->APB2SMENR  = 0x0;

	/*Suspend Tick increment to prevent wakeup by Systick interrupt.
	Otherwise the Systick interrupt will wake up the device within 1ms (HAL time base)*/
	HAL_SuspendTick();

	/* Enter Sleep Mode , wake up is done once User push-button is pressed */
	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

	/* Resume Tick interrupt if disabled prior to SLEEP mode entry */
	HAL_ResumeTick();

	printf("Awake\r\n");
}

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
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  ssd1306_Init();
  ssd1306_SetCursor(25, 10);  // Baja el cursor 10 píxeles más abajo para la segunda línea
   ssd1306_WriteString("   LOCKED", Font_7x10, White);  // Escribe el segundo mensaje

  ssd1306_SetCursor(25, 30);  // Establece el cursor en la posición (25, 30)
  ssd1306_WriteString("WELCOME", Font_11x18, White);

  ssd1306_UpdateScreen();

  ring_buffer_init(&usart2_rb, usart2_buffer, USART2_BUFFER_SIZE);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  printf("Starting...\r\n");
  //HAL_UART_Receive_IT(&huart3, &usart2_rx, 1); // enable interrupt for USART2 Rx
  //HAL_UART_Receive_IT(&huart3, &recibido_3, 1); // enable interrupt for USART2 Rx
  //ATOMIC_SET_BIT(USART2->CR1, USART_CR1_RXNEIE); // usando un funcion mas liviana para reducir memoria
  while (1) {

//	   	   printf("played...\r\n");
//

	  //bucle principal
	  //block en 1 inidica que el sistema esta desbloqueado, se procede a las ejecuciones que indique el usuario
	  if(block_ == 1){
		  if(status == 1){
			  	  	  //direccional izquierda
		  	  	   	turn_signal_left();

		  	  }
		  	  	   if(status == 2){
		  	  		//parqueo
		  	  	   park_signal();
		  	 }
		  	 if(status == 3){
		  		//direccional derecha
		  	  	 turn_signal_right();
		  	  }
		  	 if(status == 4){
		  		 //sistema apagado
		  			 park_signal();
		  			 turn_signal_left();
		  			 turn_signal_right();
		  			 SYSTEM_0FF();

		  			 status = -1;
		  	 }
	  }
	  if(block_ == 2){
		  block();
	  }

	 HAL_Delay(1);  // Pausa para no saturar el sistema

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
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

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x10D19CE4;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED_HEARTBEAT_Pin|LED_LEFT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, izquierdo_Pin|derecho_Pin|LED_RIGHT_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ROW_1_GPIO_Port, ROW_1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, ROW_2_Pin|ROW_4_Pin|ROW_3_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : BUTTON_LEFT_Pin BUTTON_RIGHT_Pin */
  GPIO_InitStruct.Pin = BUTTON_LEFT_Pin|BUTTON_RIGHT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_HEARTBEAT_Pin LED_LEFT_Pin */
  GPIO_InitStruct.Pin = LED_HEARTBEAT_Pin|LED_LEFT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : izquierdo_Pin derecho_Pin LED_RIGHT_Pin */
  GPIO_InitStruct.Pin = izquierdo_Pin|derecho_Pin|LED_RIGHT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : COLUMN_1_Pin */
  GPIO_InitStruct.Pin = COLUMN_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(COLUMN_1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : COLUMN_4_Pin */
  GPIO_InitStruct.Pin = COLUMN_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(COLUMN_4_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : COLUMN_2_Pin COLUMN_3_Pin */
  GPIO_InitStruct.Pin = COLUMN_2_Pin|COLUMN_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : ROW_1_Pin */
  GPIO_InitStruct.Pin = ROW_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(ROW_1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : ROW_2_Pin ROW_4_Pin ROW_3_Pin */
  GPIO_InitStruct.Pin = ROW_2_Pin|ROW_4_Pin|ROW_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
