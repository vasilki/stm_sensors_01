#include "main.h"
#include "stm32f4xx_hal.h"
#include <string.h>
/* USER CODE BEGIN Includes */
#include <stdio.h>


extern UART_HandleTypeDef huart1; /*declared in main.c*/

void main_usercode()
{
  static unsigned int loc_time = 0;
  static char loc_buff[1000];

  HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_5);
  HAL_Delay(300);
  loc_time++;

  sprintf(loc_buff,"time=%d\n\r",loc_time);

  HAL_UART_Transmit(&huart1, (uint8_t*)loc_buff, strlen(loc_buff),0xFFFF);
	
  return;
}
