/*
 * timers.h
 *
 *  Created on: 27 июля 2018 г.
 *      Author: vasilek
 */

#ifndef TIMERS_H_
#define TIMERS_H_
#include "stm32f4xx_hal.h"

extern void tim_UpdatePeriod(void);
extern unsigned int tim_GetPeriod(void);
extern void tim_StartTimer(TIM_HandleTypeDef *par_htim);
extern unsigned int tim_GetTimeFromStartMS(void);
extern unsigned int tim_GetTimeFromStartSEC(void);

#endif /* TIMERS_H_ */
