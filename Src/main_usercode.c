#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdio.h>
#include "adc.h"
#include "button_handle.h"
#include "timers.h"
#include "uart.h"

extern UART_HandleTypeDef huart1; /*declared in main.c*/
extern ADC_HandleTypeDef hadc1; /*declared in main.c*/
extern TIM_HandleTypeDef htim9;
extern TIM_HandleTypeDef htim10;


void main_usercode(void)
{
  unsigned int loc_adc_val=0;
  unsigned char loc_B_button_state = 0;
  uint8_t loc_buff[200];
  unsigned int loc_time;
  unsigned int loc_time_ms;
  unsigned int loc_time_sec;
  static unsigned int loc_prev_time_ms=0;

  tim_UpdatePeriod();
  loc_time = tim_GetPeriod();
  loc_time_ms = tim_GetTimeFromStartMS();
  loc_time_sec = tim_GetTimeFromStartSEC();

  if(loc_time == 1)
  {
    button_SetActiveButtons('C',13);
    button_SetActiveButtons('B',6);
    tim_StartTimer(&htim9);
    tim_StartTimer(&htim10);
  }
  else
  {
    /*nothing to do*/
  }


  /*HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_5);
  HAL_Delay(300);*/
  if(loc_prev_time_ms != loc_time_ms)
  {
    sprintf(loc_buff,"ms=%d sec=%d\n\r",loc_time_ms,loc_time_sec);
    uart_Printf(&huart1,loc_buff);
    loc_prev_time_ms = loc_time_ms;
  }
  else
  {
    /*nothing to do*/
  }

  if((loc_time_sec % 2) == 0)
  {
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);
  }
  else
  {
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);
  }

  //loc_adc_val = adc_GetValue(&hadc1);
  button_Processing();
  loc_B_button_state = button_GetButtonState('B',6);
  if(loc_B_button_state != 0)
  {
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET);
  }
  else
  {
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET);
  }


  return;
}

