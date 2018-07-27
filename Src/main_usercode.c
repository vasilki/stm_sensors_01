#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdio.h>
#include "adc.h"
#include "button_handle.h"
#include "timers.h"


extern UART_HandleTypeDef huart1; /*declared in main.c*/
extern ADC_HandleTypeDef hadc1; /*declared in main.c*/
extern TIM_HandleTypeDef htim10;

static void mu_TimeTransmit(UART_HandleTypeDef *par_huart,
                            unsigned int par_time);


void main_usercode(void)
{
  unsigned int loc_adc_val=0;
  char loc_buff[40];
  unsigned int loc_time;
  unsigned int loc_time_ms;

  tim_UpdatePeriod();
  loc_time = tim_GetPeriod();
  loc_time_ms = tim_GetTimeFromStartMS();

  if(loc_time == 1)
  {
    button_SetActiveButtons('C',13);
    button_SetActiveButtons('B',6);
    tim_StartTimer(&htim10);
  }
  else
  {
    /*nothing to do*/
  }


  /*HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_5);
  HAL_Delay(300);*/
  mu_TimeTransmit(&huart1,loc_time_ms);

  //loc_adc_val = adc_GetValue(&hadc1);
  button_Processing();
  loc_adc_val = button_GetButtonState('B',6);
  if(loc_adc_val != 0)
  {
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET);
  }
  else
  {
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET);
  }



 // sprintf(loc_buff,"ADC VALUE=%d\n\r",loc_adc_val);
 // HAL_UART_Transmit(&huart1, (uint8_t*)loc_buff, strlen(loc_buff),0xFFFF);

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



