/*
 * timers.c
 *
 *  Created on: 27 июля 2018 г.
 *      Author: vasilek
 */

#include "stm32f4xx_hal.h"

static unsigned int gl_time = 0;
static unsigned int gl_time_ms = 0;
static unsigned int gl_time_sec = 0;

static void tim_UpdateTimeFromStartMS(TIM_HandleTypeDef *par_htim);
static void tim_UpdateTimeFromStartSEC(TIM_HandleTypeDef *par_htim);



void tim_UpdatePeriod(void)
{
  gl_time++;
  return;
}

unsigned int tim_GetPeriod(void)
{
  return gl_time;
}

void tim_StartTimer(TIM_HandleTypeDef *par_htim)
{
 // HAL_TIM_Base_Start(par_htim);
  HAL_TIM_Base_Start_IT(par_htim);
  return;
}

void tim_UpdateTimeFromStartMS(TIM_HandleTypeDef *par_htim)
{
  gl_time_ms += (par_htim->Init.Period + 1);

  return;
}

void tim_UpdateTimeFromStartSEC(TIM_HandleTypeDef *par_htim)
{
  gl_time_sec += ((par_htim->Init.Period + 1)/1000);

  return;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM9)
  {
    tim_UpdateTimeFromStartSEC(htim);
  }
  else
  {
    /*nothing to do - it is not our interrupt*/
  }

  if(htim->Instance == TIM10)
  {
    tim_UpdateTimeFromStartMS(htim);
  }
  else
  {
    /*nothing to do - it is not our interrupt*/
  }

  return;
}

unsigned int tim_GetTimeFromStartMS(void)
{
  return gl_time_ms;
}

unsigned int tim_GetTimeFromStartSEC(void)
{
  return gl_time_sec;
}
