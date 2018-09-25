/*
 * temperature_ds18b20.c
 *
 *  Created on: 20 сент. 2018 г.
 *      Author: vasilek
 */
#include "stm32f4xx_hal.h"
#include "dwt_stm32_delay.h"
#include "string.h"
#include "ds18b20.h"
#include "uart.h"


#define K_STEP_5us 5 /*step of checking confirmation (us)*/

enum DS18B20_COMMANDS {
  K_READ_ROM = 0x33, /*command read ROM*/
  K_CONVERT_TEMPERATURE = 0x44, /*command convert temperature*/
  K_COPY_SCRATCHPAD = 0x48, /*command copy data from RAM into ROM*/
  K_WRITE_SCRACTCHPAD = 0x4E, /*command write into ROM (non volatile memory)*/
  K_MATCH_ROM = 0x55, /*command match ROM*/
  K_READ_POWER_SUPPLY = 0xB4, /*command check type of power supply (external or "parasite")*/
  K_RECALL_E2 = 0xB8, /*command reload TH,TL and configuration data from ROM*/
  K_READ_SCRATCHPAD = 0xBE, /*command read memory*/
  K_SKIP_ROM = 0xCC, /*command skip ROM*/
  K_ALARM_SEARCH = 0xEC, /*command alarm search*/
  K_SEARCH_ROM = 0xF0 /*command search ROM*/
};

enum DS18B20_RESOLUTIONS {
  K_RESOLUTION_9BIT = 0x1F,
  K_RESOLUTION_10BIT = (0x1F | (1 << 5)),
  K_RESOLUTION_11BIT = (0x1F | (1 << 6)),
  K_RESOLUTION_12BIT = (K_RESOLUTION_10BIT | K_RESOLUTION_11BIT)
};
enum DS18B20_T_CONV {
  K_T_CONV_RESOLUTION_9BIT = (800000 / 8),
  K_T_CONV_RESOLUTION_10BIT = (800000 / 4),
  K_T_CONV_RESOLUTION_11BIT = (800000 / 2),
  K_T_CONV_RESOLUTION_12BIT = 800000, /*800000us (800 ms) base time for temperature's conversion*/
};

#define K_TH_REGISTER 100 /*High temperature limit 100 degree celsius*/
#define K_TL_REGISTER 20 /*Low temperature limit 20 degree celsius*/

#define K_CRC8_DS_INIT 0x00 /*CRC8 Dallas 0x00*/

static GPIO_TypeDef *GL_GPIOx = (GPIO_TypeDef*)0;
static uint16_t GL_GPIO_PIN = 0;
static uint8_t GL_RESOLUTION = K_RESOLUTION_12BIT;
static uint32_t GL_T_CONV = K_T_CONV_RESOLUTION_12BIT;



static void ds18b20_GPIO_Init(uint32_t par_GPIO_MODE, GPIO_TypeDef *par_GPIOx, uint16_t par_GPIO_Pin);
static uint8_t ds18b20_Reset(void);
static uint8_t ds18b20_ReadBit(void);
static uint8_t ds18b20_ReadByte(void);
static void ds18b20_WriteBit(uint8_t par_bit);
static void ds18b20_WriteByte(uint8_t par_byte);
static uint8_t ds18b20_CalculateCRC8(const uint8_t *par_data, unsigned int par_length);

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
    loc_bit = (HAL_GPIO_ReadPin(GL_GPIOx, GL_GPIO_PIN) == GPIO_PIN_RESET);
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

/*return 1 in case of ds18b20 is online, 0 otherwise*/
static uint8_t ds18b20_Reset(void)
{
  uint8_t loc_status;

  /*Initialize output GPIO*/
  ds18b20_GPIO_Init(GPIO_MODE_OUTPUT_OD,GL_GPIOx,GL_GPIO_PIN);
  /*Set low level on 1-wire line*/
  HAL_GPIO_WritePin(GL_GPIOx, GL_GPIO_PIN, GPIO_PIN_RESET);
  /*Delay >=480 us Reset Pulse from MCU. +5us spare*/
  DWT_Delay_us(500);
  /*Initialize input GPIO - release 1-wire line*/
  ds18b20_GPIO_Init(GPIO_MODE_INPUT,GL_GPIOx,GL_GPIO_PIN);
  /*Delay 15-60 us: answer from DS18B20. +5us spare*/
  DWT_Delay_us(65);
  /*Read answer during 480-60 us and count answer's confirmation (wait PRESENCE)*/
  loc_status = ds18b20_CheckBitWithConfirmation(0,60,420);
  /*Delay 20 us*/
  DWT_Delay_us(20);

  /*confirmation greater than 60us or equal 60us zero-answer from DS18B20*/
  return loc_status;
}


uint8_t ds18b20_Init(GPIO_TypeDef* par_GPIOx, uint16_t par_GPIO_Pin, uint8_t par_mode)
{
  uint8_t loc_status;
  
  DWT_Delay_Init();
  GL_GPIOx = par_GPIOx;
  GL_GPIO_PIN = par_GPIO_Pin;
  GL_RESOLUTION = K_RESOLUTION_12BIT;

  switch(GL_RESOLUTION)
  {
    case K_RESOLUTION_9BIT:
      GL_T_CONV = K_T_CONV_RESOLUTION_9BIT;
      break;
    case K_RESOLUTION_10BIT:
      GL_T_CONV = K_T_CONV_RESOLUTION_10BIT;
      break;
    case K_RESOLUTION_11BIT:
      GL_T_CONV = K_T_CONV_RESOLUTION_11BIT;
      break;
    case K_RESOLUTION_12BIT:
      GL_T_CONV = K_T_CONV_RESOLUTION_12BIT;
      break;
    default:
      GL_T_CONV = K_T_CONV_RESOLUTION_12BIT;
      break;
  }

  ds18b20_GPIO_Init(GPIO_MODE_OUTPUT_OD,GL_GPIOx,GL_GPIO_PIN);
  
  loc_status = ds18b20_Reset();
  if(loc_status != 0)
  {
  
    if(par_mode == K_ONE_SENSOR_ON_LINE)
    {
      ds18b20_WriteByte(K_SKIP_ROM);
      ds18b20_WriteByte(K_TH_REGISTER);  
      ds18b20_WriteByte(K_TL_REGISTER); 
      ds18b20_WriteByte(GL_RESOLUTION);
    }
    else
    {
    }
  }
  else
  {
    /*nothing to do - ds18b20 is offline */
  }
     
  return loc_status;
}

static uint8_t ds18b20_ReadBit(void)
{
  uint8_t loc_bit = 0;

  /*Initialize output GPIO*/
  ds18b20_GPIO_Init(GPIO_MODE_OUTPUT_OD,GL_GPIOx,GL_GPIO_PIN);
  /*Set low level on 1-wire line*/
  HAL_GPIO_WritePin(GL_GPIOx, GL_GPIO_PIN, GPIO_PIN_RESET);
  /*Delay 1 us Master read from MCU. +2us spare*/
  DWT_Delay_us(3);
  /*Initialize input GPIO*/
  ds18b20_GPIO_Init(GPIO_MODE_INPUT,GL_GPIOx,GL_GPIO_PIN);
  /*Delay 15 us Master read from DS18B20. +3us spare*/
  DWT_Delay_us(18);
  /*Read bit*/
  loc_bit = (HAL_GPIO_ReadPin(GL_GPIOx, GL_GPIO_PIN) == GPIO_PIN_SET);
  /*Delay for next bit*/
  DWT_Delay_us(45);

  return loc_bit;
}

static uint8_t ds18b20_ReadByte(void)
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
  /*Set low level on 1-wire line*/
  HAL_GPIO_WritePin(GL_GPIOx, GL_GPIO_PIN, GPIO_PIN_RESET);
  if(par_bit == 0)
  {
    /*Delay 60 < delay < 120 us Master write 0 from MCU*/
    DWT_Delay_us(70);
    /*Initialize input GPIO - release 1-wire line*/
    ds18b20_GPIO_Init(GPIO_MODE_INPUT,GL_GPIOx,GL_GPIO_PIN);
  }
  else
  {
    /*Delay < 15 us Master write 0 from MCU*/
    DWT_Delay_us(10);
    /*Initialize input GPIO - release 1-wire line*/
    ds18b20_GPIO_Init(GPIO_MODE_INPUT,GL_GPIOx,GL_GPIO_PIN);
    /*Delay 15/2 + 60 for processing "1" by DS18B20. +5us spare*/
    DWT_Delay_us(10 + 65);
  }
  /*Relocation Delay 1. +4us spare*/
  DWT_Delay_us(5);

  return;
}

static void ds18b20_WriteByte(uint8_t par_byte)
{
  uint32_t loc_i;

  for (loc_i = 0; loc_i < sizeof(uint8_t); loc_i++)
  {
    ds18b20_WriteBit((par_byte >> loc_i) & 0x1);
  }

  return;
}


void ds18b20_RequestMeasureTemperature(uint8_t par_mode, uint8_t par_sensor_index)
{
  uint8_t loc_status;
  
  loc_status = ds18b20_Reset();
  if(loc_status != 0)
  {
    if(par_sensor_index == 0)
    {
      if(par_mode == K_ONE_SENSOR_ON_LINE)
      {
        ds18b20_WriteByte(K_SKIP_ROM);
      }
      else
      {
        /*nothing to do*/
      }
    }
    else
    {
      /*for more sensors on 1-wire line*/
    }
    ds18b20_WriteByte(K_CONVERT_TEMPERATURE);
    /*Delay for processing temperature by ds18b20*/
    DWT_Delay_us(GL_T_CONV);
  }
  else
  {
    /*nothing to do*/
  }
  
  return;
}

uint8_t ds18b20_ReadScratchpad(uint8_t par_mode, uint8_t *par_data, uint8_t par_sensor_index)
{
  uint32_t loc_i;
  uint8_t loc_status;
  
  loc_status = ds18b20_Reset();
  if((loc_status != 0) && (par_data != 0))
  {
    if(par_sensor_index == 0)
    {
      if(par_mode == K_ONE_SENSOR_ON_LINE)
      {
        ds18b20_WriteByte(K_SKIP_ROM);
        ds18b20_WriteByte(K_READ_SCRATCHPAD);
        /*8 byte data and 1 byte CRC8*/
        for(loc_i = 0;loc_i < K_SCRATCHPAD_LENGTH; loc_i++)
        {
          par_data[loc_i] = ds18b20_ReadByte();
        }
      }
      else
      {
        /*nothing to do*/
      }
    }
    else
    {
      /*for more sensors on 1-wire line*/
    }
  }
  else
  {
    /*nothing to do*/
  }
    
  return loc_status;
}

/*  Poly  : 0x8C    x^8 + x^5 + x^4 + 1 */
static uint8_t ds18b20_CalculateCRC8(const uint8_t *par_data, unsigned int par_length)
{
  uint8_t loc_crc8 = K_CRC8_DS_INIT;
  uint32_t loc_j = 0;
  uint32_t loc_i;
  uint8_t loc_data;
  uint8_t loc_byte;
  
  while ((loc_j < par_length) && (par_data != 0))
  {
    loc_data = par_data[loc_j];

    for (loc_i = 0; loc_i < sizeof(uint8_t); loc_i++)
    {
      loc_byte = (loc_crc8 ^ loc_data) & 0x1;
      loc_crc8 = loc_crc8 >> 0x1;
      loc_data = loc_data >> 0x1;
      if(loc_byte == 1)
      {
        loc_crc8 = loc_crc8 & 0x8C;
      }
      else
      {
        /*nothing to do*/
      }
    }
    loc_j++;
  }
  
  return loc_crc8;
}

uint8_t ds18b20_CheckCRC8(const uint8_t *par_scratchpad, unsigned int par_length)
{
  uint8_t loc_ok = 0;

  /*8 bytes of data from scracthpad + 1 byte of crc8 from scratchpad*/
  if((par_scratchpad != 0) && (par_length == K_SCRATCHPAD_LENGTH))
  {
    loc_ok = ds18b20_CalculateCRC8(par_scratchpad, K_SCRATCHPAD_LENGTH - 1);
    loc_ok = (loc_ok == par_scratchpad[K_SCRATCHPAD_LENGTH - 1]);
  }
  else
  {
    /*nothing to do*/
  }

  return loc_ok;
}

float ds18b20_DecodeTemperature(const uint8_t *par_scratchpad)
{
  float loc_temperature;
  int32_t loc_int;
  uint8_t loc_sign;
  
  if(par_scratchpad != 0)
  {
    switch(GL_RESOLUTION)
    {
      case K_RESOLUTION_9BIT:
        /*for 9bit resolutions bits 0-2 are not defined*/
        loc_temperature = ((par_scratchpad[0] >> 3) & 0x1) * 0.5;
        break;
      case K_RESOLUTION_10BIT:
        /*for 10bit resolutions bits 0-1 are not defined*/
        loc_temperature = ((par_scratchpad[0] >> 2) & 0x3) * 0.25;
        break;
      case K_RESOLUTION_11BIT:
        /*for 11bit resolutions bit 0 is not defined*/
        loc_temperature = ((par_scratchpad[0] >> 1) & 0x7) * 0.125;
        break;
      case K_RESOLUTION_12BIT:
        /*for 12bit resolutions*/
        loc_temperature = (par_scratchpad[0] & 0xF) * 0.0625;
        break;
      default:
        /*for 12bit resolutions*/
        loc_temperature = (par_scratchpad[0] & 0xF) * 0.0625;
        break;
    }
    loc_int = ((par_scratchpad[0] >> 4) & 0xF) | ((par_scratchpad[1] & 0x7) << 4);
    loc_temperature = loc_temperature + loc_int;
    
    loc_sign = (((par_scratchpad[1] >> 3) & 0x1F) != 0);
    if(loc_sign != 0)
    {
      loc_temperature *= (-1);
    }
    else
    {
      /*nothing to do*/
    }
  }
  else
  {
    loc_temperature = 0.0;
  }
  
  return loc_temperature;
}

