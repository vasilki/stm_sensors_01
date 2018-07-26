#include "stm32f4xx_hal.h"
#define ADC_TIMEOUT 100 /*ms*/
#define ADC_HAL_DELAY 50 /*ms*/


unsigned int adc_GetValue(ADC_HandleTypeDef *par_hadc)
{
  HAL_StatusTypeDef loc_status;
  unsigned int loc_return = 0;

  loc_status = HAL_ADC_Start(par_hadc);
  if(loc_status == HAL_OK)
  {
    loc_status = HAL_ADC_PollForConversion(par_hadc,ADC_TIMEOUT);
  }
  else
  {
    /*nothing to do*/
  }


  if(loc_status != HAL_ERROR)
  {
    loc_return = HAL_ADC_GetValue(par_hadc);
    loc_status = HAL_ADC_Stop(par_hadc);
  }
  else
  {
    /*nothing to do*/
  }


  HAL_Delay(ADC_HAL_DELAY);

  return loc_return;
}
