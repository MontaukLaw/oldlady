#include "user_comm.h"

static void ADC_Config()
{
  /*  Init GPIO for ADC1 */
  GPIO_Init(GPIOD, GPIO_PIN_6, GPIO_MODE_IN_FL_NO_IT);

  /* De-Init ADC peripheral*/
  ADC1_DeInit();

  /* Init ADC1 peripheral */
  ADC1_Init(ADC1_CONVERSIONMODE_CONTINUOUS, ADC1_CHANNEL_6,
            ADC1_PRESSEL_FCPU_D2, ADC1_EXTTRIG_TIM, DISABLE,
            ADC1_ALIGN_RIGHT, ADC1_SCHMITTTRIG_CHANNEL6, DISABLE);

  /* Enable EOC interrupt */
  //ADC1_ITConfig(ADC1_IT_AWS6,ENABLE);

  /*Start Conversion */
  ADC1_StartConversion();
}

void ADC_Init(void)
{
  // 设置ADC
  ADC_Config();  
  
}

uint16_t MX_ADC1_Get_Data(void)
{
  uint16_t iADC1_Value;

  //读取转换结果
  iADC1_Value = ADC1_GetConversionValue();

  return iADC1_Value;
}

uint16_t MX_ADC1_Get_Average_Data(void)
{
  uint8_t i;
  uint16_t average_VCC = 0;

  for (i = 0; i < 10; i++)
  {
    average_VCC += MX_ADC1_Get_Data();
  }
  average_VCC /= 10;

  return average_VCC;
}

// 检查电池电压
void CheckBatVol(void)
{
  uint16_t vBat = 0;
  static uint8_t lowVolCounter = 0;
  // 获取电压
  vBat = MX_ADC1_Get_Average_Data();
  prinfNumber(vBat);
  // 3.3伏的一半时, 是459
  if (vBat < MIN_BAT_ADC)
  {
    lowVolCounter++;
    // 期待获取一个持续值
    if (lowVolCounter > LOW_VOL_MAX)
    {      
      DelayMs(1);
#if RGB_PWM      
      RedBlink();      
#endif
      
#if WS2812    
      WS2812RedBlink();
#endif  
      
      PowerOff();
    }
  }
  else
  {
    lowVolCounter = 0;
  }
}
