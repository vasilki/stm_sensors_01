#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdio.h>
#include "adc.h"
#include "button_handle.h"
#include "timers.h"
#include "uart.h"
#include "LED.h"

extern UART_HandleTypeDef huart1; /*declared in main.c*/
extern ADC_HandleTypeDef hadc1; /*declared in main.c*/
extern TIM_HandleTypeDef htim9;
extern TIM_HandleTypeDef htim10;

void main_Init();

void main_usercode(void)
{
  static unsigned int loc_adc_val=0;
  static unsigned int loc_prev_adc_val = 0xFFFFFFFF;
  unsigned char loc_B_button_state = 0;
  uint8_t loc_buff[200];
  unsigned int loc_time;
  unsigned int loc_time_ms;
  unsigned int loc_time_sec;
  static unsigned int loc_prev_time_ms=0;
  static unsigned int loc_prev_time_sec=0;

  main_Init();
  
  tim_UpdatePeriod();
  loc_time = tim_GetPeriod();
  loc_time_ms = tim_GetTimeFromStartMS();
  loc_time_sec = tim_GetTimeFromStartSEC();

  if(loc_prev_time_sec != loc_time_sec)
  {
    loc_adc_val = adc_GetValue(&hadc1);
    if((loc_adc_val / 250) > 0)
    {
      led_set_onboard_LED_blinking_period(1000 / (loc_adc_val / 250));
    }
    else
    {
      led_set_onboard_LED_blinking_period(1000);
    }
  }
  else
  {
    /*nothing to do*/
  }


  if(loc_adc_val != loc_prev_adc_val)
  {
   snprintf(loc_buff,sizeof(loc_buff),"Water Level = %d | sec = %d\n\r",loc_adc_val,loc_time_sec);
   uart_Printf(&huart1,loc_buff);
   loc_prev_adc_val = loc_adc_val;
   // loc_prev_time_ms = loc_time_ms;
  }
  else
  {
    /*nothing to do*/
  }

  led_onboard_LED_blink();

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



  loc_prev_time_sec = loc_time_sec;

  return;
}



void main_Init()
{
  static uint8_t loc_B_IsFirstTime = 0;
  
  if(loc_B_IsFirstTime == 0)
  {
    button_SetActiveButtons('C',13);
    button_SetActiveButtons('B',6);
    tim_StartTimer(&htim9);
    tim_StartTimer(&htim10);
    uart_Init(&huart1);
    uart_PrintfBuildVersion(&huart1);
    led_set_onboard_LED_blinking_period(1000);
    
    loc_B_IsFirstTime = 1;
  }
  else
  {
    /*nothing to do*/
  }
  
  return;
}










