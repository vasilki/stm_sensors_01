/*
 * temperature_ds18b20.c
 *
 *  Created on: 20 сент. 2018 г.
 *      Author: vasilek
 */
#include "stm32f4xx_hal.h"
#include "dwt_stm32_delay.h"

static GPIO_TypeDef* GL_GPIOx = (GPIO_TypeDef*)0;
static uint16_t GL_GPIO_PIN = 0;

static void ds18b20_GPIO_Init(uint8_t par_B_Output, GPIO_TypeDef* par_GPIOx, uint16_t par_GPIO_Pin)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(par_GPIOx, par_GPIO_Pin, GPIO_PIN_RESET);

  if(par_B_Output == 1)
  {
    /*Configure GPIO pin : par_GPIO_Pin */
    GPIO_InitStruct.Pin = par_GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  }
  else
  {

  }
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  return;
}


void ds18b20_Init(GPIO_TypeDef* par_GPIOx, uint16_t par_GPIO_Pin)
{
  DWT_Delay_Init();
  GL_GPIOx = par_GPIOx;
  GL_GPIO_PIN = par_GPIO_Pin;

  return;
}

uint8_t ds18b20_Reset(void)
{
  uint8_t loc_bit;
  uint32_t loc_i;
  uint32_t loc_confirmation_us;

  /*Set low level on 1-wire line*/
  HAL_GPIO_WritePin(GL_GPIOx, GL_GPIO_PIN, GPIO_PIN_RESET);
  /*Delay >=480 us Reset Pulse from MCU. +5us spare*/
  DWT_Delay_us(485);
  /*Set high level on 1-wire line*/
  HAL_GPIO_WritePin(GL_GPIOx, GL_GPIO_PIN, GPIO_PIN_SET);
  /*Delay 15-60 us: answer from DS18B20. +5us spare*/
  DWT_Delay_us(65);
  /*Read answer during 480-60 us and count answer's confirmation*/
  loc_confirmation_us = 0;
  for(loc_i = 0; loc_i < 420; loc_i += 20)
  {
    loc_bit = (HAL_GPIO_ReadPin(GL_GPIOx, GL_GPIO_PIN) == GPIO_PIN_SET);
    if(loc_bit == 0)
    {
      loc_confirmation_us += 20;
    }
    else
    {
      /*nothing to do*/
    }
    DWT_Delay_us(20);
  }
  /*Delay 20 us*/
  DWT_Delay_us(20);

  /*confirmation greater than 60us or equal 60us zero-answer from DS18B20*/
  return (loc_confirmation_us >= 60);
}

uint8_t ds18b20_ReadBit(void)
{
  uint8_t loc_bit = 0;

  /*Set low level on 1-wire line*/
  HAL_GPIO_WritePin(GL_GPIOx, GL_GPIO_PIN, GPIO_PIN_RESET);
  /*Delay 1 us Master read from MCU. +1us spare*/
  DWT_Delay_us(2);


}




