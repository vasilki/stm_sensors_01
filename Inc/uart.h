/*
 * uart.h
 *
 *  Created on: 28 июля 2018 г.
 *      Author: vasilek
 */

#ifndef UART_H_
#define UART_H_

#include "stm32f4xx_hal.h"

void uart_Init(UART_HandleTypeDef *par_uart);
void uart_Printf(UART_HandleTypeDef *par_uart,const uint8_t *par_string);
void uart_PrintfInteger(UART_HandleTypeDef *par_uart, int par_value, const char *par_base);
void uart_PrintfBuildVersion(UART_HandleTypeDef *par_uart);

#define UART_PRINTFINTEGER(A,B) \
{ \
extern UART_HandleTypeDef huart1; \
extern void uart_PrintfInteger(UART_HandleTypeDef *par_uart, int par_value, const char *par_base); \
uart_PrintfInteger(&huart1,A,B); \
}



#endif /* UART_H_ */
