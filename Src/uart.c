/*
 * uart.c
 *
 *  Created on: 28 июля 2018 г.
 *      Author: vasilek
 */

#include "stm32f4xx_hal.h"
#include <string.h>

#define K_MAX_STRING 200

void uart_Printf(UART_HandleTypeDef *par_uart,const uint8_t *par_string)
{
  /*HAL_StatusTypeDef loc_status;*/
  HAL_UART_Transmit(par_uart, (uint8_t*)par_string, strnlen(par_string,K_MAX_STRING),0xFFFF);

  return;
}


