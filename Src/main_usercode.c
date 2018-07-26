#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdio.h>
#include "adc.h"

extern UART_HandleTypeDef huart1; /*declared in main.c*/
extern ADC_HandleTypeDef hadc1; /*declared in main.c*/

static void mu_TimeTransmit(UART_HandleTypeDef *par_huart,
                            unsigned int par_time);
static void mu_UpdatePeriod(void);

static unsigned int gl_time = 0;

void main_usercode(void)
{
  unsigned int loc_adc_val;
  char loc_buff[40];

  mu_UpdatePeriod();
  HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_5);
  HAL_Delay(300);
  mu_TimeTransmit(&huart1,gl_time);

  loc_adc_val = adc_GetValue(&hadc1);
  sprintf(loc_buff,"ADC VALUE=%d\n\r",loc_adc_val);
  HAL_UART_Transmit(&huart1, (uint8_t*)loc_buff, strlen(loc_buff),0xFFFF);

  return;
}


void mu_TimeTransmit(UART_HandleTypeDef *par_huart,
                     unsigned int par_time)
{
  char loc_buff[40];
  HAL_StatusTypeDef loc_status;

  sprintf(loc_buff,"time=%d\n\r",par_time);
  loc_status = HAL_UART_Transmit(par_huart, (uint8_t*)loc_buff, strlen(loc_buff),0xFFFF);

  return;
}

void mu_UpdatePeriod(void)
{
  gl_time++;
  return;
}


