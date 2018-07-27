/*
 * button_handle.h
 *
 *  Created on: 27 июля 2018 г.
 *      Author: vasilek
 */

#ifndef BUTTON_HANDLE_H_
#define BUTTON_HANDLE_H_


void button_Processing(void);

unsigned char button_GetButtonState(unsigned char par_line,
                             unsigned char par_pin);

void button_SetActiveButtons(unsigned char par_line,
                             unsigned char par_pin);

#endif /* BUTTON_HANDLE_H_ */
