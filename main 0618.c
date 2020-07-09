#include "user_comm.h"

Color_t color = {0, 0, 0};
extern uint32_t counter;
AxesRaw_t axesBuff = {0, 0, 0};
extern uint8_t catchTime;
extern Pose_t pose;
extern PWMDuty_t pwmDuty;
uint8_t ifStartCountSleep;
uint8_t ifSleeping;
uint8_t runningState = 0;
extern int8_t Acc_Data[3];
uint16_t Conversion_Value = 0;
extern unsigned char LedsArray[];
extern int8_t AccData8Bit[];

extern signed short int SL_ACCEL_X,SL_ACCEL_Y,SL_ACCEL_Z;

// 定时器配置
static void TIM4_Config(void)
{
  /* TIM4 configuration:
   - TIM4CLK is set to 16 MHz, the TIM4 Prescaler is equal to 128 so the TIM1 counter
   clock used is 16 MHz / 128 = 125 000 Hz
  - With 125 000 Hz we can generate time base:
      max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
      min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
  - In this example we need to generate a time base equal to 1 ms
   so TIM4_PERIOD = (0.001 * 125000 - 1) = 124 */

  /* Time base configuration */
  //16 000 000
  //TIM4_TimeBaseInit(TIM4_PRESCALER_2, TIM4_PERIOD);
  TIM4_TimeBaseInit(TIM4_PRESCALER_128, TIM4_PERIOD);
  /* Clear TIM4 update flag */
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  /* Enable update interrupt */
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);

  /* enable interrupts */
  enableInterrupts();

  /* Enable TIM4 */
  TIM4_Cmd(ENABLE);
}

// led的pwm配置
static void TIM2Config(void)
{
  /* Time base configuration */
  TIM2_TimeBaseInit(TIM2_PRESCALER_16, MAX_DUTY);

  // green
  /* PWM1 Mode configuration: Channel1 */
  TIM2_OC1Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, 0, TIM2_OCPOLARITY_HIGH);
  TIM2_OC1PreloadConfig(ENABLE);

  // red
  /* PWM1 Mode configuration: Channel2 */
  TIM2_OC2Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, 0, TIM2_OCPOLARITY_HIGH);
  TIM2_OC2PreloadConfig(ENABLE);

  // blue
  /* PWM1 Mode configuration: Channel3 */
  TIM2_OC3Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, 0, TIM2_OCPOLARITY_HIGH);
  TIM2_OC3PreloadConfig(ENABLE);

  TIM2_ARRPreloadConfig(ENABLE);

  /* TIM2 enable counter */
  TIM2_Cmd(ENABLE);
}

// 电源管理
void PowerManage(void)
{
  static uint16_t sleepCounter = 0;
  static int16_t postOld = 0;
  // SL_ACCEL_X,SL_ACCEL_Y,SL_ACCEL_Z
  
  uint16_t poseTotalNow = (int16_t)SL_ACCEL_X + (int16_t)SL_ACCEL_Y + (int16_t)SL_ACCEL_Z;
  
  //prinfNumber(poseTotalNow);

  if (abs(postOld - poseTotalNow) < SLEEP_POSE_THRESHOLD)
  {
    sleepCounter++;
    
    if (sleepCounter > SLEEP_MODE_WAIT_SECONDS)
    {
      //disableInterrupts();

#if MEMS_LIS3DH      
      I2C_SDA_HIGH();
      I2C_SCL_HIGH();
      LIS3DH_SetMode(LIS3DH_POWER_DOWN);
      //DelayMs(1);
#endif

#if RGB_PWM      
      AllOff();
#endif
      
      // 如果长时间不动, 就闪蓝灯,然后关机
      WS2812BlueBlink();

      PowerOff();

    }
  }
  else
  {
    sleepCounter = 0;
  }
  postOld = poseTotalNow;
}

void main_charging_rainbow(void)
{
  CLK_HSICmd(ENABLE);

  CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);

  TIM4_Config();

  TIM2Config();

  while (1)
  {
    ChangeColorByRainbow();
    DelayMs(10);
  }
}

void main_charging_test(void)
{

  uint8_t ifCharging = 0;
  CLK_HSICmd(ENABLE);

  CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);

  TIM4_Config();

  GPIO_Init(GPIOD, GPIO_PIN_2, GPIO_MODE_IN_PU_NO_IT);

  GPIO_Init(GPIOD, GPIO_PIN_3, GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_WriteLow(GPIOD, GPIO_PIN_3);

  GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_WriteLow(GPIOD, GPIO_PIN_4);

  GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_WriteLow(GPIOA, GPIO_PIN_3);

  while (1)
  {
    ifCharging = GPIO_ReadInputPin(GPIOD, GPIO_PIN_2) && GPIO_PIN_2;
    if (ifCharging == RESET)
    {
      GPIO_WriteHigh(GPIOD, GPIO_PIN_3);
      DelayMs(1000);
    }
    else
    {
      GPIO_WriteLow(GPIOD, GPIO_PIN_3);
    }
    DelayMs(1000);
  }
}

void main_sc7a20(void)
{
  CLK_HSICmd(ENABLE);

  CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);
  // B4 SCL
  GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_FAST);

  // B5 SDA
  GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);

  G_Sensor_SC7A20_Init();

  UART1_DeInit();

  UART1_Init((uint32_t)115200,
             UART1_WORDLENGTH_8D,
             UART1_STOPBITS_1,
             UART1_PARITY_NO,
             UART1_SYNCMODE_CLOCK_DISABLE,
             UART1_MODE_TXRX_ENABLE);

  UART1_Cmd(ENABLE);

  while (1)
  {
    ReadXYZdata();
    //read_acceler_data();
    Debug('x');
    PrintInt8('l', AccData8Bit[0]);
    PrintInt8('h', AccData8Bit[1]);
#if 0    
    Debug('y');
    PrintInt8('l', AccData8Bit[2]);
    PrintInt8('h', AccData8Bit[3]);
    Debug('z');
    PrintInt8('l', AccData8Bit[4]);
    PrintInt8('h', AccData8Bit[5]);
#endif
    //DelayMs(1000);
  }
}

void main_rt(void)
{
  //uint8_t i;
  __IO RGBColor_t color = {255, 0, 0};

  GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_WriteHigh(GPIOC, GPIO_PIN_5);

  CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);
  GPIO_Init(GPIOD, GPIO_PIN_3, GPIO_MODE_OUT_OD_LOW_FAST);
  GPIOD->DDR = 0x08;
  GPIOD->CR1 = 0x08;

  TIM4_Config();
  while (1)
  {
    rainbowCycle(5);
  }
  //while(1){
  //GPIOC->ODR ^= 0x20;
  //__disable_interrupt();
  //asm("BCPL $500A,#5 \n"
  // "nop         \n"
  // "nop         \n"
  // "nop         \n"
  //"nop         \n"
  // );
  //__enable_interrupt();
  //}
  while (1)
  {
    color.R = 0;
    color.B = 0;
    color.G = 0;
    RGBSetColor(0, color);
    //RGBSetColor();
    rgb_SendArray();

    DelayMs(1000);

    LedsArray[0] = 255;
    LedsArray[1] = 255;
    LedsArray[2] = 255;
    //LedsArray[3] = 0;
    //LedsArray[4] = 0;
    //LedsArray[5] = 255;

    rgb_SendArray();

    DelayMs(1000);
  }
}

static void IWDG_Config(void)
{
  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  IWDG_Enable();
  
  /* IWDG timeout equal to 250 ms (the timeout may varies due to LSI frequency
     dispersion) */
  /* Enable write access to IWDG_PR and IWDG_RLR registers */
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  
  /* IWDG counter clock: LSI/128 */
  IWDG_SetPrescaler(IWDG_Prescaler_256);
  
  /* Set counter reload value to obtain 250ms IWDG Timeout.
    Counter Reload Value = 250ms/IWDG counter clock period
                         = 250ms / (LSI/128)
                         = 0.25s / (LsiFreq/128)
                         = LsiFreq/(128 * 4)
                         = LsiFreq/512
   */
  IWDG_SetReload(0xff);
  
  /* Reload IWDG counter */
  IWDG_ReloadCounter();
}

// 主程序
void main(void)
{

  uint16_t sleepModeCounter = 0;
  uint8_t ifUSBPlugIn = 0;
  uint8_t ifUSBPlugOutCounter = 0;

  //uint8_t oldRunningState = RUNNING_NORMAL;
  
  runningState = RUNNING_NORMAL;
  //runningState = RUNNING_TESTING;

  // 启用内置时钟
  CLK_HSICmd(ENABLE);

  // 设置时钟频率不分频
  CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);

  // 开启led与adc的开关
  // C5 ADC_EN
  GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);
  
  GPIO_WriteHigh(GPIOC, GPIO_PIN_5);
   
  ADC_Init();

  // 设置定时器tim4, 用于做delay定时
  TIM4_Config();

#if RGB_PWM
  // pwm定时器设置
  TIM2Config();
#endif

#if WS2812
  // 驱动WS2812只需要一条线

  GPIO_Init(GPIOD, GPIO_PIN_3, GPIO_MODE_OUT_OD_LOW_FAST);
  GPIOD->DDR = 0x08;
  GPIOD->CR1 = 0x08;

  InitColor();
  
#endif

  // 串口初始化
  UartInit();

  IIC_Init();

  // 充电检测引脚初始化
  // CHRG
  GPIO_Init(GPIOD, GPIO_PIN_2, GPIO_MODE_IN_PU_NO_IT);

  // CHR_OUT
  // 改成插入usb检测.
  GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_IN_FL_NO_IT);

  GPIO_WriteHigh(GPIOC, GPIO_PIN_4);

#if MEMS_SC7A20C
  // 初始化SC7A20
  G_Sensor_SC7A20_Init();
#endif

#if MEMS_LIS3DH
  GsensorInit();

#endif
  
  // 初始化看门狗
  //IWDG_Config();
  
  while (1)
  {
    //喂狗
    //IWDG_ReloadCounter(); 
    switch (runningState)
    {
    case RUNNING_CHARGING_FINISHED:
      //GPIO_WriteHigh(GPIOC, GPIO_PIN_5);
      
      ifUSBPlugIn = GPIO_ReadInputPin(GPIOC, GPIO_PIN_4) && GPIO_PIN_4;
      // 如果管子被拔
      if( ifUSBPlugIn == RESET){
        ifUSBPlugOutCounter ++;
        if(ifUSBPlugOutCounter > 100){
          GPIO_WriteHigh(GPIOC, GPIO_PIN_5);
          WS2812GreenBlink(); 
          PowerOff(); 
        }
        //WS2812BlueBlink(); 
      }else{
        
        ifUSBPlugOutCounter = 0;
      }
      
      DelayMs(10);
      
      break;
    case RUNNING_CHARNING:
      
#if WS2812
      rainbowCycle(5);
      
#endif      
      // 充电中
#if RGB_PWM
      // 彩虹色
      ChangeColorByRainbow();

      DelayMs(10);
#endif
      break;
    case RUNNING_NORMAL:

#if 0
      // 普通模式
      if (oldRunningState == RUNNING_CHARNING)
      {
#if RGB_PWM        
        // 充好电之后, 直接关机
        AllOff();
        GreenBlink();
#endif

      }
  
#endif
      // 4.1v 636
      // 3.7v 574
#if MEMS_LIS3DH
      // 读取三轴
      GetAxisXYZ();
#endif

      //GetDutyDirect();
#if MEMS_SC7A20C

      ReadXYZdata();

#endif

// 使用rgb驱动
#if RGB_PWM
      // 将三轴信息转成pwm值
      GetDutyOldStyle();
#endif

#if WS2812
      GsensorToWS2812();
#endif

      sleepModeCounter++;
      
      // 定时检查电压跟休眠
      if (sleepModeCounter > SLEEP_MODE_WAIT_COUNTERS)
      {
        // 如果长时间不动就自动关机
        //PowerManage();

        // 如果电压不够, 就关机
        CheckBatVol();
        //AllOff();
        sleepModeCounter = 0;
      }
      // if (catchTime)
      //
      // GetAxisXYZ();
      //CountPitch();

      //GetDutyByRP();
      //catchTime = 0;
      //}

      //ChangeColorSlowly();
      break;
    }
    //oldRunningState = runningState;
  }

#if 0
  // 更裸
  while (1)
  {
    GetAxisXYZ();
    //GetDutyDirect();

    GetDutyOldStyle();

    sleepModeCounter++;

    if (sleepModeCounter > SLEEP_MODE_WAIT_COUNTERS)
    {
      //PowerManage();
      //AllOff();
      sleepModeCounter = 0;
    }
  }
#endif
}

void assert_failed(uint8_t *file, uint32_t line)
{
}