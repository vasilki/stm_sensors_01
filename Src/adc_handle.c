#include "stm32f4xx_hal.h"
#include "timers.h"
#define ADC_TIMEOUT 100 /*ms*/
#define ADC_HAL_DELAY 50 /*ms*/


unsigned int adc_GetValue(ADC_HandleTypeDef *par_hadc)
{
  HAL_StatusTypeDef loc_status;
  static unsigned int loc_return = 0;
  static unsigned int loc_last_invoke_time = 0;
  unsigned int loc_time = tim_GetTimeFromStartMS();

  if((loc_time - loc_last_invoke_time) > ADC_HAL_DELAY)
  {
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

    loc_last_invoke_time = loc_time;
  }
  else
  {
    /*nothing to do*/
  }

//  HAL_Delay(ADC_HAL_DELAY);

  return loc_return;
}


unsigned int adc_GetNormalizeValue(ADC_HandleTypeDef *par_hadc, unsigned int par_coeff)
{

  return (adc_GetValue(par_hadc) / par_coeff);
}
