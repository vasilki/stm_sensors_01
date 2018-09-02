/*
 * LED.c
 *
 *  Created on: 2 сент. 2018 г.
 *      Author: vasilek
 */
#include "timers.h"
#include "stm32f4xx_hal.h"

static unsigned int gl_onboard_LED_blinking_period_ms = 1000;

void led_set_onboard_LED_blinking_period(unsigned int par_period_ms)
{
  if(par_period_ms < 50)
  {
    gl_onboard_LED_blinking_period_ms = 1000;
  }
  else
  {
    gl_onboard_LED_blinking_period_ms = par_period_ms;
  }

  return;
}


void led_onboard_LED_blink()
{
  unsigned char loc_B_on;

  if(gl_onboard_LED_blinking_period_ms < 50)
  {
    gl_onboard_LED_blinking_period_ms = 1000;
  }
  else
  {
    /*nothing to do*/
  }

  loc_B_on = (tim_GetTimeFromStartMS() / gl_onboard_LED_blinking_period_ms) % 2;

  if(loc_B_on != 0)
  {
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);
  }
  else
  {
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);
  }


  return;
}

