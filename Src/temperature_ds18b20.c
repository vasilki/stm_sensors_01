/*
 * temperature_ds18b20.c
 *
 *  Created on: 20 сент. 2018 г.
 *      Author: vasilek
 */
#include "stm32f4xx_hal.h"
#include "dwt_stm32_delay.h"
#include "string.h"

static GPIO_TypeDef *GL_GPIOx = (GPIO_TypeDef*)0;
static uint16_t GL_GPIO_PIN = 0;
#define K_STEP_5us 5 /*step of checking confirmation (us)*/

static void ds18b20_GPIO_Init(uint32_t par_GPIO_MODE, GPIO_TypeDef *par_GPIOx, uint16_t par_GPIO_Pin)
{
  static GPIO_InitTypeDef loc_GPIO_InitStruct = {0,0xFFFFFFFF,0,0,0};
  
  if(par_GPIO_MODE != loc_GPIO_InitStruct.Mode)
  {

    if(par_GPIO_MODE == GPIO_MODE_OUTPUT_OD)
    {
      loc_GPIO_InitStruct.Pin = par_GPIO_Pin;
      loc_GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
      loc_GPIO_InitStruct.Pull = GPIO_NOPULL;
      loc_GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    }
    else if(par_GPIO_MODE == GPIO_MODE_INPUT)
    {
      /*Configure GPIO pin Output Level */
      HAL_GPIO_WritePin(par_GPIOx, par_GPIO_Pin, GPIO_PIN_RESET);
      loc_GPIO_InitStruct.Pin = par_GPIO_Pin;
      loc_GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
      loc_GPIO_InitStruct.Pull = GPIO_NOPULL;
    }
    else
    {
      /*nothing to do - wrong parameters for gpio connected to DS18B20*/
    }
    HAL_GPIO_Init(par_GPIOx, &loc_GPIO_InitStruct);
  }
  else
  {
    /*nothing to do*/
  }

  return;
}


void ds18b20_Init(GPIO_TypeDef* par_GPIOx, uint16_t par_GPIO_Pin)
{
  DWT_Delay_Init();
  GL_GPIOx = par_GPIOx;
  GL_GPIO_PIN = par_GPIO_Pin;
  ds18b20_GPIO_Init(GPIO_MODE_OUTPUT_OD,GL_GPIOx,GL_GPIO_PIN);

  return;
}

static uint8_t ds18b20_CheckBitWithConfirmation(uint8_t par_bit,
                                                uint32_t par_confirmation_time_us,
                                                uint32_t par_time_slot_limit_us)
{
  uint8_t loc_bit;
  uint32_t loc_i;
  uint32_t loc_confirmation_us = 0;
  uint32_t loc_confirmation_max_us = 0;

  /*Initialize input GPIO*/
  ds18b20_GPIO_Init(GPIO_MODE_INPUT,GL_GPIOx,GL_GPIO_PIN);
  /*Read answer during confirmation time us and count answer's confirmation*/
  for(loc_i = 0; loc_i < par_time_slot_limit_us; loc_i += K_STEP_5us)
  {
    loc_bit = (HAL_GPIO_ReadPin(GL_GPIOx, GL_GPIO_PIN) == GPIO_PIN_SET);
    if(loc_bit == par_bit)
    {
      loc_confirmation_us += K_STEP_5us;
      if(loc_confirmation_max_us < loc_confirmation_us)
      {
        loc_confirmation_max_us = loc_confirmation_us;
      }
      else
      {
        /*nothing to do*/
      }
    }
    else
    {
      loc_confirmation_us = 0;
    }
    DWT_Delay_us(K_STEP_5us);
  }

  return (loc_confirmation_max_us >= par_confirmation_time_us);
}

uint8_t ds18b20_Reset(void)
{
  uint8_t loc_bit;

  /*Initialize output GPIO*/
  ds18b20_GPIO_Init(GPIO_MODE_OUTPUT_OD,GL_GPIOx,GL_GPIO_PIN);
  /*Set low level on 1-wire line*/
  HAL_GPIO_WritePin(GL_GPIOx, GL_GPIO_PIN, GPIO_PIN_RESET);
  /*Delay >=480 us Reset Pulse from MCU. +5us spare*/
  DWT_Delay_us(485);
  /*Set high level on 1-wire line*/
  HAL_GPIO_WritePin(GL_GPIOx, GL_GPIO_PIN, GPIO_PIN_SET);
  /*Delay 15-60 us: answer from DS18B20. +5us spare*/
  DWT_Delay_us(65);
  /*Initialize input GPIO - release 1-wire line*/
  ds18b20_GPIO_Init(GPIO_MODE_INPUT,GL_GPIOx,GL_GPIO_PIN);
  /*Read answer during 480-60 us and count answer's confirmation*/
  loc_bit = ds18b20_CheckBitWithConfirmation(0,60,420);
  /*Delay 20 us*/
  DWT_Delay_us(20);

  /*confirmation greater than 60us or equal 60us zero-answer from DS18B20*/
  return loc_bit;
}

static uint8_t ds18b20_ReadBit(void)
{
  uint8_t loc_bit = 0;

  /*Initialize output GPIO*/
  ds18b20_GPIO_Init(GPIO_MODE_OUTPUT_OD,GL_GPIOx,GL_GPIO_PIN);
  /*Set low level on 1-wire line*/
  HAL_GPIO_WritePin(GL_GPIOx, GL_GPIO_PIN, GPIO_PIN_RESET);
  /*Delay 1 us Master read from MCU. +1us spare*/
  DWT_Delay_us(2);
  /*Initialize input GPIO*/
  ds18b20_GPIO_Init(GPIO_MODE_INPUT,GL_GPIOx,GL_GPIO_PIN);
  /*Delay 15 us Master read from DS18B20*/
  DWT_Delay_us(15);
  /*Read bit*/
  loc_bit = (HAL_GPIO_ReadPin(GL_GPIOx, GL_GPIO_PIN) == GPIO_PIN_SET);
  /*Delay for next bit*/
  DWT_Delay_us(45);

  return loc_bit;
}

uint8_t ds18b20_ReadByte(void)
{
  uint8_t loc_byte = 0;
  uint32_t loc_i;

  for (loc_i = 0; loc_i < sizeof(uint8_t); loc_i++)
  {
    loc_byte = loc_byte | (ds18b20_ReadBit() << loc_i);
  }

  return loc_byte;
}

static void ds18b20_WriteBit(uint8_t par_bit)
{
  /*Initialize output GPIO*/
  ds18b20_GPIO_Init(GPIO_MODE_OUTPUT_OD,GL_GPIOx,GL_GPIO_PIN);
  if(par_bit == 0)
  {
    /*Set low level on 1-wire line*/
    HAL_GPIO_WritePin(GL_GPIOx, GL_GPIO_PIN, GPIO_PIN_RESET);
    /*Delay > 60 us Master write 0 from MCU. +10us spare*/
    DWT_Delay_us(70); 
    /*Initialize input GPIO - release 1-wire line*/
    ds18b20_GPIO_Init(GPIO_MODE_INPUT,GL_GPIOx,GL_GPIO_PIN);
  }
  else
  {
    /*Set low level on 1-wire line*/
    HAL_GPIO_WritePin(GL_GPIOx, GL_GPIO_PIN, GPIO_PIN_RESET);
    /*Delay < 15 us Master write 0 from MCU*/
    DWT_Delay_us(15/2);    
    /*Initialize input GPIO - release 1-wire line*/
    ds18b20_GPIO_Init(GPIO_MODE_INPUT,GL_GPIOx,GL_GPIO_PIN);
    /*Delay 15/2 + 60 for processing "1" by DS18B20. +5us spare*/
    DWT_Delay_us(15/2 + 65);
  }
  /*Relocation Delay 1. +2us spare*/
  DWT_Delay_us(3); 

  return;
}

void ds18b20_WriteByte(uint8_t par_byte)
{
  uint32_t loc_i;

  for (loc_i = 0; loc_i < sizeof(uint8_t); loc_i++)
  {
    ds18b20_WriteBit((par_byte >> loc_i) & 0x1);
  }

  return;
}
