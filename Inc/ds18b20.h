/*
 * ds18b29.h
 *
 *  Created on: 23 сентября 2018 г.
 *      Author: vasilek
 */

#ifndef DS18B20_H_
#define DS18B20_H_
#include "stm32f4xx_hal.h"


#define K_ONE_SENSOR_ON_LINE 0 /*skip ROM*/
#define K_MORE_THAN_ONE_SENSOR_ON_LINE 1 /no skip ROM*/

void ds18b20_RequestMeasureTemperature(uint8_t par_mode, uint8_t par_sensor_index);
uint8_t ds18b20_ReadScratchpad(uint8_t par_mode, uint8_t *par_data, uint8_t par_sensor_index);
uint8_t ds18b20_Init(GPIO_TypeDef* par_GPIOx, uint16_t par_GPIO_Pin, uint8_t par_mode);
uint8_t ds18b20_CheckCRC8(const uint8_t *par_scratchpad, unsigned int par_length);
float ds18b20_DecodeTemperature(const uint8_t *par_scratchpad);

#endif /* DS18B20_H_ */
