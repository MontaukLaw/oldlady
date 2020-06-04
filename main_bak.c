/**
  ******************************************************************************
  * @file     TIM4_TimeBase\main.c
  * @author   MCD Application Team
  * @version V2.0.4
  * @date     26-April-2018
  * @brief    This file contains the main function for TIM4 Time Base Configuration example.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
//#include "stm8s.h"
#include <iostm8s103f3.h>
//#include "stm8s_eval.h"
#include "stm8s_gpio.h"
#include "ws2812B_fx.h"

#define LED PE_ODR_ODR5 //LED���ź궨��

static u32 uptime = 0;
static u32 delay_time = 0;
uint16_t z = 0; //������

void uptime_routine(void)
{
  uptime++;
  if (uptime == 0xFFFFFFFF)
  {
    delay_time = 0;
    uptime = 0;
  }
}

void _delay_ms(u16 wait)
{
  delay_time = uptime + wait;
  while (delay_time > uptime)
  {
  }
}

#pragma vector = TIM4_OVR_UIF_vector // 定时器中断
__interrupt void timer4()
{
  TIM4_SR = 0; //��־λ����
  uptime_routine();
  //GPIO_WriteReverse(GPIOC, GPIO_PIN_4);
}

void init(void)
{
  CLK_CKDIVR = 0x00; // 不分频
#if 0 
  PC_DDR_DDR4 = 1; // PC3输出
  PC_CR1_C14 = 1;  // 推挽输出
  PC_CR2_C24 = 1;  // 高速
#endif
#if 1
  TIM4_PSCR = 0x07; // 125KHz
  TIM4_ARR = 0x7d;  // 1ms
  TIM4_IER = 0x01;  // 使能中断
  TIM4_EGR = 0x01;  // 触发事件更新
  TIM4_CR1 = 0x01;  // 开始计数
#endif
}

int main(void)
{
  uint16_t i;
  init();
  // deinit tim1
  TIM1_DeInit();
  
  GPIO_Init(GPIOC, GPIO_PIN_5,GPIO_MODE_OUT_OD_LOW_FAST);
  
  while(1){
    _delay_ms(1);
    GPIO_WriteReverse(GPIOC, GPIO_PIN_5);
  }
}
int mainOld(void)
{
  uint16_t i, j;
  init();     // 
  asm("rim"); // 
  //rgb_SetColor();
  
  for(i=0; i < NB_LEDS; i++) 
  {
    rgb_SetColor(i,Wheel(((i * 256 / 10) + j) & 255));
  }
  
  rgb_SendArray();
  while (1)
  {
    rgb_SendArray();
    //rainbowCycle(5);
  }
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
