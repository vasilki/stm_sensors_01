/*
 * uart.h
 *
 *  Created on: 28 июля 2018 г.
 *      Author: vasilek
 */

#ifndef UART_H_
#define UART_H_

#include "stm32f4xx_hal.h"

void uart_Printf(UART_HandleTypeDef *par_uart,const uint8_t *par_string);


#endif /* UART_H_ */
