/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);


/* Function processing DMA Rx data. Counts how many capital and small letters are in sentence.
 * Result is supposed to be stored in global variable of type "letter_count_" that is defined in "main.h"
 *
 * @param1 - received sign
 */
void proccesDmaData(uint8_t sign);


/* Space for your global variables. */

	// type your global variables here:
#define MAX_CHARS_AFTER_START 35

letter_count_ letter_count;
char valid_string[MAX_CHARS_AFTER_START + 1];
volatile uint8_t valid_string_ready = 0;

#include <stdio.h>

uint8_t tx_status[100];
uint8_t tx_result[100];

static void send_string(uint8_t *buf, int len)
{
	if(len <= 0) return;
	if(len > 255) len = 255;
	while(!USART2_IsTxFree());
	USART2_PutBuffer(buf, (uint8_t)len);
}


int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* Configure the system clock */
  SystemClock_Config();
  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();

  /* Space for your local variables, callback registration ...*/

  	  //type your code here:
  USART2_RegisterCallback(proccesDmaData);

  uint16_t tick_10ms = 0;

  while (1)
  {
	  /* Periodic transmission of information about DMA Rx buffer state.
	   * Transmission frequency - 0.5Hz.
	   * Message format - "Buffer capacity: %d bytes, occupied memory: %d bytes, load [in %]: %f%"
	   * Example message (what I wish to see in terminal) - Buffer capacity: 1000 bytes, occupied memory: 231 bytes, load [in %]: 23.1%
	   */

	  /* Valid text string information transmission.
	   * Transmission frequency - when new valid string is received.
	   * Message format - "Valid string: %s, lower-case: %d, upper-case: %d"
	   * Example message (what I wish to see in terminal) - Valid string: Platn15uborZnakov, lower-case: 13, upper-case: 2
	   */

  	  	  	  //type your code here:
	  if(valid_string_ready)
	  {
		  int len = snprintf((char*)tx_result, sizeof(tx_result),
				  "Valid string: %s, lower-case: %d, upper-case: %d\r\n",
				  valid_string, letter_count.small_letter, letter_count.capital_letter);
		  valid_string_ready = 0;
		  send_string(tx_result, len);
	  }

	  if(tick_10ms >= 200)
	  {
		  tick_10ms = 0;

		  uint16_t occupied = USART2_GetRxBufferOccupied();
		  uint32_t load_x10 = (occupied * 1000UL) / DMA_USART2_BUFFER_SIZE;

		  int len = snprintf((char*)tx_status, sizeof(tx_status),
				  "Buffer capacity: %d bytes, occupied memory: %d bytes, load [in %%]: %u.%u%%\r\n",
				  DMA_USART2_BUFFER_SIZE, occupied,
				  (unsigned)(load_x10 / 10), (unsigned)(load_x10 % 10));
		  send_string(tx_status, len);
	  }

	  LL_mDelay(10);
	  tick_10ms++;
  }
  /* USER CODE END 3 */
}


void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);

  if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0)
  {
  Error_Handler();  
  }
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {
    
  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {
  
  }
  LL_Init1msTick(8000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SetSystemCoreClock(8000000);
}

/*
 * Implementation of function processing data received via USART.
 */
void proccesDmaData(uint8_t sign)
{
	/* Process received data */

		// type your algorithm here:
	static uint8_t receiving = 0;
	static uint8_t count = 0;
	static char buf[MAX_CHARS_AFTER_START + 1];
	static letter_count_ tmp;

	if(sign == '#')
	{
		receiving = 1;
		count = 0;
		tmp.small_letter = 0;
		tmp.capital_letter = 0;
		return;
	}

	if(!receiving) return;

	count++;

	if(sign == '$')
	{
		buf[count - 1] = '\0';
		for(uint8_t i = 0; i < count; i++) valid_string[i] = buf[i];
		letter_count = tmp;
		valid_string_ready = 1;
		receiving = 0;
		return;
	}

	if(count >= MAX_CHARS_AFTER_START)
	{
		receiving = 0;
		return;
	}

	buf[count - 1] = sign;
	if(sign >= 'a' && sign <= 'z')      tmp.small_letter++;
	else if(sign >= 'A' && sign <= 'Z') tmp.capital_letter++;
}


void Error_Handler(void)
{

}

#ifdef  USE_FULL_ASSERT

void assert_failed(char *file, uint32_t line)
{ 

}

#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
