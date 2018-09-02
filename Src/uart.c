/*
 * uart.c
 *
 *  Created on: 28 июля 2018 г.
 *      Author: vasilek
 */

#include "stm32f4xx_hal.h"
#include <string.h>

#define K_MAX_STRING 200
extern uint8_t GL_PROJECT_NAME[];

void uart_Init(UART_HandleTypeDef *par_uart)
{
  HAL_UART_Transmit(par_uart, (uint8_t*)"\n\r", strnlen("\n\r",K_MAX_STRING),0xFFFF);

  return;
}

void uart_Printf(UART_HandleTypeDef *par_uart,const uint8_t *par_string)
{
  /*HAL_StatusTypeDef loc_status;*/
  HAL_UART_Transmit(par_uart, (uint8_t*)par_string, strnlen(par_string,K_MAX_STRING),0xFFFF);

  return;
}

void uart_PrintfBuildVersion(UART_HandleTypeDef *par_uart)
{
  /*HAL_StatusTypeDef loc_status;*/
  uint8_t loc_buf[K_MAX_STRING];

  snprintf(loc_buf,sizeof(loc_buf),"Project:%s Date:%s Time:%s",GL_PROJECT_NAME,__DATE__,__TIME__);
  uart_Printf(par_uart,loc_buf);

  return;
}


static void uart_SetBinString(unsigned int par_value, char *par_string)
{
  int loc_i;
  unsigned int loc_value = par_value;

  par_string[sizeof(unsigned int)*8] = 0;

  for(loc_i = sizeof(unsigned int)*8 - 1;loc_i>=0;loc_i--)
  {
    par_string[loc_i] = 0x30 + (loc_value & 0x1);
    loc_value = loc_value >> 1;
  }

  return;
}



void uart_PrintfInteger(UART_HandleTypeDef *par_uart, int par_value, const char *par_base)
{
  char loc_buff[40];

  if((strcmp(par_base,"dec")==0)||(strcmp(par_base,"DEC")==0))
  {
    sprintf(loc_buff,"%d\n\r",par_value);
  }
  else if((strcmp(par_base,"hex")==0)||(strcmp(par_base,"HEX")==0))
  {
    sprintf(loc_buff,"0x%08x\n\r",par_value);
  }
  else if((strcmp(par_base,"oct")==0)||(strcmp(par_base,"OCT")==0))
  {
    sprintf(loc_buff,"%o\n\r",par_value);
  }
  else if((strcmp(par_base,"bin")==0)||(strcmp(par_base,"BIN")==0))
  {
    uart_SetBinString(par_value,loc_buff);
    strcat(loc_buff,"\n\r");
  }
  else
  {
    /*nothing to do*/
  }

  HAL_UART_Transmit(par_uart, (uint8_t*)loc_buff, strnlen(loc_buff,K_MAX_STRING),0xFFFF);

  return;
}


