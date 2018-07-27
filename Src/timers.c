/*
 * timers.c
 *
 *  Created on: 27 июля 2018 г.
 *      Author: vasilek
 */

#include "stm32f4xx_hal.h"

static unsigned int gl_time = 0;
static unsigned int gl_time_ms = 0;

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
  //gl_time_ms += (par_htim->Init.Period + 1);
  gl_time_ms += 50;
  return;
}

unsigned int tim_GetTimeFromStartMS(void)
{
  return gl_time_ms;
}

