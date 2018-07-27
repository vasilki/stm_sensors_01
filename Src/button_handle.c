#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f401xe.h"

typedef struct
{
  unsigned char B_IsActive;
  unsigned char B_IsPressed;
  GPIO_TypeDef *pLine;
  unsigned int Pin;
  unsigned int LastTimeChange;
}T_ButtonState;

#define K_MAX_LINES ('E'-'A'+1) /*Max I, but I use from A to E*/
#define K_MAX_PINS 16
#define K_MACRO_GET_LINE (x) ((x/K_MAX_PINS)+0x41)
#define K_MACRO_GET_PIN (x) (x%K_MAX_PINS)

static T_ButtonState loc_ButtonState[K_MAX_LINES*K_MAX_PINS]; /*[A-E] * [0..15]*/
static GPIO_TypeDef *button_GetGPIOLine(char par_line);
static unsigned int button_GetGPIOPin(char par_pin);

void button_Processing(void)
{
  int loc_i;

  for(loc_i = 0; loc_i < K_MAX_LINES*K_MAX_PINS; loc_i++)
  {
    if(loc_ButtonState[loc_i].B_IsActive != 0)
    {
      loc_ButtonState[loc_i].B_IsPressed = (unsigned char)HAL_GPIO_ReadPin(loc_ButtonState[loc_i].pLine,
          loc_ButtonState[loc_i].Pin);
    }
    else
    {
      /*nothing to do*/
    }
  }

  return;
}


void button_SetActiveButtons(unsigned char par_line,
                             unsigned char par_pin)
{
  int loc_index;
  if((par_line >= 'A' && par_line <= 'E') &&
      (par_pin < K_MAX_PINS))
  {
    loc_index = (par_line-0x41)*K_MAX_PINS+par_pin;
    loc_ButtonState[loc_index].B_IsActive = 1;
    loc_ButtonState[loc_index].pLine = button_GetGPIOLine(par_line);
    loc_ButtonState[loc_index].Pin = button_GetGPIOPin(par_pin);
  }
  else
  {
    /*nothing to do*/
  }
}

GPIO_TypeDef *button_GetGPIOLine(char par_line)
{
  GPIO_TypeDef *loc_pgpio = NULL;

  switch (par_line)
  {
    case 'A':
      loc_pgpio = GPIOA;
      break;
    case 'B':
      loc_pgpio = GPIOB;
      break;
    case 'C':
      loc_pgpio = GPIOC;
      break;
    case 'D':
      loc_pgpio = GPIOD;
      break;
    case 'E':
      loc_pgpio = GPIOE;
      break;
    default:
      /*nothing to do*/
      break;
  }

  return loc_pgpio;
}


unsigned int button_GetGPIOPin(char par_pin)
{
  return 1<<((unsigned int)par_pin);
}


unsigned char button_GetButtonState(unsigned char par_line,
                             unsigned char par_pin)
{
  int loc_index;
  unsigned char loc_state = 0;

  if((par_line >= 'A' && par_line <= 'E') &&
      (par_pin < K_MAX_PINS))
  {
    loc_index = (par_line-0x41)*K_MAX_PINS+par_pin;
    loc_state = loc_ButtonState[loc_index].B_IsPressed;
  }
  else
  {
    /*nothing to do*/
  }

  return loc_state;
}
