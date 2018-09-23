#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdio.h>
#include "button_handle.h"
#include "timers.h"
#include "uart.h"
#include "ds18b20.h"

extern UART_HandleTypeDef huart1; /*declared in main.c*/
extern TIM_HandleTypeDef htim9;
extern TIM_HandleTypeDef htim10;

static void main_Init();
static void main_sensors_init(void);
static void main_experiments_with_ds18b20(void);

void main_usercode(void)
{
  unsigned int loc_adc_val=0;
  unsigned char loc_B_button_state = 0;
  uint8_t loc_buff[200];
  unsigned int loc_time;
  unsigned int loc_time_ms;
  unsigned int loc_time_sec;
  static unsigned int loc_prev_time_sec = 0;
  static unsigned int loc_prev_time_ms=0;
  static unsigned int loc_prev_time_measure=0;

  main_Init();
  
  tim_UpdatePeriod();
  loc_time = tim_GetPeriod();
  loc_time_ms = tim_GetTimeFromStartMS();
  loc_time_sec = tim_GetTimeFromStartSEC();


  if((loc_prev_time_measure + 5) == loc_time_sec)
  {
    main_experiments_with_ds18b20();
    loc_prev_time_measure = loc_time_sec;
  }
  else
  {
    /*nothing to do*/
  }


  if(loc_prev_time_sec != loc_time_sec)
  {
    /*
    sprintf((char*)loc_buff,"sec=%d\n\r",loc_time_sec);
    uart_Printf(&huart1,loc_buff);
    */
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

  loc_prev_time_sec = loc_time_sec;

  return;
}


static void main_Init()
{
  static uint8_t loc_B_IsFirstTime = 0;
  
  if(loc_B_IsFirstTime == 0)
  {
    /*BUTTON init*/
    button_SetActiveButtons('C',13);
    button_SetActiveButtons('B',6);
    
    /*TIM init*/
    tim_StartTimer(&htim9);
    tim_StartTimer(&htim10);
    
    /*UART init*/
    uart_Init(&huart1);
    uart_PrintfBuildVersion(&huart1);
   
    /*sensors init*/
    main_sensors_init();
    
    loc_B_IsFirstTime = 1;
  }
  else
  {
    /*nothing to do*/
  }
  
  return;
}


static void main_sensors_init(void)
{
  /*DS18B20 init*/
  ds18b20_Init(GPIOB, GPIO_PIN_5, K_ONE_SENSOR_ON_LINE);
      
  return;
}


static void main_experiments_with_ds18b20(void)
{
  uint8_t loc_scratchpad[9]={0};
  uint8_t loc_status;
  float loc_temperature = 0.0;
  uint8_t loc_buff[200];
  
  ds18b20_RequestMeasureTemperature(K_ONE_SENSOR_ON_LINE, 0);
  loc_status = ds18b20_ReadScratchpad(K_ONE_SENSOR_ON_LINE, loc_scratchpad, sizeof(loc_scratchpad));
  if(loc_status != 0)
  {
    loc_status = ds18b20_CheckCRC8(loc_scratchpad, sizeof(loc_scratchpad));
    if(loc_status != 0)
    {
      loc_temperature = ds18b20_DecodeTemperature(loc_scratchpad);
    }
    else
    {
      /*nothing to do*/
    }
    sprintf((char*)loc_buff,"crc is ok= %d temperature = %f\n\r", loc_status, loc_temperature);
    uart_Printf(&huart1,loc_buff);
  }
  else
  {
    /*nothing to do*/
  }
  
  
  return;
}





