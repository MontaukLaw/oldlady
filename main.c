#include "user_comm.h"


Color_t color = {0, 0, 0};
extern uint32_t counter;
AxesRaw_t axesBuff = {0, 0, 0};
extern uint8_t catchTime;
extern Pose_t pose;
extern PWMDuty_t pwmDuty;

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

static void TIM2Config(void)
{
  /* Time base configuration */
  TIM2_TimeBaseInit(TIM2_PRESCALER_16, MAX_DUTY);

  // green
  /* PWM1 Mode configuration: Channel1 */
  TIM2_OC1Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE,0, TIM2_OCPOLARITY_HIGH);
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

static void TIM1_Config(void)
{

  TIM1_TimeBaseInit(15, TIM1_COUNTERMODE_UP, 1000, 0);
  
  TIM1_ARRPreloadConfig(ENABLE);         //使能自动重装
  
  TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE); //数据更新中断
  
  TIM1_Cmd(ENABLE);                      //开定时器

#if 0  
  TIM1_DeInit();

  TIM1_TimeBaseInit(2, TIM1_COUNTERMODE_UP, 16, 0);
  
  /* Update Interrupt Enable */
  TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE); 
  
  TIM1_ARRPreloadConfig(ENABLE);
  /* Enable TIM1 */
  TIM1_Cmd(ENABLE);

  enableInterrupts();
#endif

#if 0
  TIM1_DeInit();
  
  TIM1->PSCRH = 0x00; // 16M系统时钟经预分频f=fck/(PSCR+1)
  TIM1->PSCRL = 0x10; // PSCR=0x1F3F，f=16M/16=1000 000Hz，每个计数周期1ms
  TIM1->ARRH = 0x01; // 自动重载寄存器ARR=0x01F4=500
  TIM1->ARRL = 0xF4; // 每记数500次产生一次中断，即500ms
  TIM1->IER = 0x01; // 允许更新中断
  TIM1->CR1 = 0x01; // 计数器使能，开始计数

#endif
}

void main_003(void)
{
  uint16_t duty = 0;
  uint8_t dir = 1;
  uint16_t counter = 0;
  TIM2Config();
  //TIM2_SetCompare1(MAX_DUTY);

  while (1)
  {
    //duty++;

    if (dir)
    {
      counter++;
      if (counter > MAX_DUTY * 10)
      {
        dir = 0;
      }
    }
    else
    {
      counter--;
      if (counter == 0)
      {
        dir = 1;
      }
    }
#if 0
    if(duty>5000){
      TIM2_SetCompare1(0);
    }else{
      TIM2_SetCompare1(MAX_DUTY);
    }   
    if(duty > 10000){
      duty = 0;
    }
#endif

    TIM2_SetCompare1(counter / 10);
  }
}

void TransGtoColorOld(void)
{
  static uint16_t colorRNew = 0;
  
  static uint16_t colorGNew = 0;
  
  static uint16_t colorBNew = 0;  
  
  __IO uint16_t colorTarget = color.g;
  __IO uint16_t tmp = 0;

  if (colorGNew < colorTarget)
  {
    tmp = colorTarget - colorGNew;
    if (tmp > MIN_GAP)
    {
      colorGNew++;
    }
  }
  else
  {
    tmp = colorGNew - colorTarget;
    if (tmp > MIN_GAP)
    {
      colorGNew--;
    }
  }
  
  colorTarget = color.r;
  if (colorRNew < colorTarget)
  {
    tmp = colorTarget - colorRNew;
    if (tmp > MIN_GAP)
    {
      colorRNew++;
    }
  }
  else
  {
    tmp = colorRNew - colorTarget;
    if (tmp > MIN_GAP)
    {
      colorRNew--;
    }
  }
  colorTarget = color.b;
  if (colorBNew < colorTarget)
  {
    tmp = colorTarget - colorBNew;
    if (tmp > MIN_GAP)
    {
      colorBNew++;
    }
  }
  else
  {
    tmp = colorBNew - colorTarget;
    if (tmp > MIN_GAP)
    {
      colorBNew--;
    }
  }  
  TIM2_SetCompare1(colorGNew);
  TIM2_SetCompare2(colorRNew);  
  TIM2_SetCompare3(colorBNew);
  
  //((uint16_t) axesBuff.AXIS_X)*10;
}

void TIM1_init(uint8_t prescalerHigh, uint8_t prescalerLow, long setNum)
{
  TIM1->PSCRH = prescalerHigh;
  TIM1->PSCRL = prescalerLow;  
  
  TIM1->IER = 0x01;
  
  TIM1->ARRH = (uint8_t)(setNum >> 8);
  TIM1->ARRL = (uint8_t)(setNum && 0x00FF);
  
  TIM1->CNTRH = (uint8_t)(setNum >> 8);
  TIM1->CNTRL = (uint8_t)(setNum && 0x00FF);  

}

void TransColorNew(void)
{
  uint8_t tempX = pose.x;
  uint8_t tempZ = pose.z;
  
  // 将x轴分开, 分别缝上接头
  if(tempX < 0x6A){    
    pwmDuty.b = ((uint16_t) tempX ) * 20 + 1250;
    
  }else if(tempX > 0x6A){    
    pwmDuty.b = ((uint16_t)(tempX - 0xBD)) * 20;    
  }
  
  if(tempZ < 0x6A){
    
    pwmDuty.b = ((uint16_t) tempZ ) * 20 + 1250;
    
    
  }else if(tempZ > 0x6A){
    
    pwmDuty.b = ((uint16_t)(tempZ - 0xBD)) * 20;    
  }


}
void TransGtoColor(void)
{
  static uint16_t colorRNew = 0;
  
  // 首先是红色
  // 红色的值由X轴决定, x轴的G值为4F到00, 其中从35到4F可定为为极值
  // 负值从C0到FF
  __IO uint16_t colorTarget = color.r;
  
#if 0 
  if(colorRNew > colorTarget){
    if((colorRNew - colorTarget) > MIN_GAP)
    {
      colorRNew --;
    }
    
  }else{
    if((colorTarget - colorRNew) > MIN_GAP)
    {
      colorRNew ++;
    }
  }
  
#endif
  
  if(colorTarget > MIN_DUTY)
  {
    
    TIM2_SetCompare2(colorTarget);
    //TIM2_SetCompare2(colorRNew); 
  }else{
    TIM2_SetCompare2(MIN_DUTY); 
  }
  
  colorTarget = color.b;
  
  if(colorTarget > MIN_DUTY)
  {
    
    TIM2_SetCompare3(colorTarget);
    //TIM2_SetCompare2(colorRNew); 
  }else{
    TIM2_SetCompare3(MIN_DUTY); 
  }
  
  //TIM2_SetCompare2(colorTarget);
}

void PowerManage(void)
{
  static uint16_t sleepCounter = 0;
  static uint16_t postOld = 0; 
  uint16_t poseTotalNow = pose.x + pose.y + pose.z;
  //prinfNumber(poseTotalNow);
   
  if(abs(postOld - poseTotalNow) < SLEEP_POSE_THRESHOLD){
    sleepCounter ++;
    if(sleepCounter > SLEEP_MODE_WAIT_SECONDS){
      //disableInterrupts();
      LIS3DH_SetMode(LIS3DH_POWER_DOWN);      
      DelayMs(1);
      AllOff();
      DelayMs(1);
      Debug('o');            
      halt();      
    }
  }else{
    sleepCounter = 0;
  }
  postOld = poseTotalNow;
}

// 测试待机功耗跟状态
void main_power_comsum_test(void)
{
  while(1)
  {
  }
}

void main(void)
{
  //uint8_t mid = 0;
  uint8_t sleepModeCounter = 0;
  uint8_t uartCounter = 0;
  
  CLK_HSICmd(ENABLE);

  CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);

  TIM4_Config();

  TIM2Config();
  
  UART1_DeInit();

  UART1_Init((uint32_t)115200, 
             UART1_WORDLENGTH_8D, 
             UART1_STOPBITS_1, 
             UART1_PARITY_NO,
             UART1_SYNCMODE_CLOCK_DISABLE, 
             UART1_MODE_TXRX_ENABLE);
  
  UART1_Cmd(ENABLE);
  
  //TIM1_Config();
  //GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_FAST);
  //GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);

  GPIO_Init(GPIOC, GPIO_PIN_7, GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_WriteHigh(GPIOC, GPIO_PIN_7);

  GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);

  //GPIO_Init(GPIOD, GPIO_PIN_1, GPIO_MODE_OUT_PP_HIGH_FAST);
  //GPIO_Init(GPIOD, GPIO_PIN_2, GPIO_MODE_OUT_PP_HIGH_FAST);

  //GPIO_WriteHigh(GPIOD, GPIO_PIN_1);
  //GPIO_WriteHigh(GPIOD, GPIO_PIN_2);

  GPIOB->DDR = 0x30; // 配置PD端口的方向寄存器PD3输出
  GPIOB->CR1 = 0x30; // 设置PD3为推挽输出
 
  GsensorInit();

  //TIM2_SetCompare2(2550);
  
  // TIM2_SetCompare1( 2550 );
#if 0
  //事实证明, pwm与亮度有线性关系
  while(1)
  {
    duty+=10;
    // 测试红色亮度跟pwm的关系.
    TIM2_SetCompare3(duty);
    if(duty > 2550){
      duty = 0;
    }
    
    Debug((uint8_t)(duty>>8));
    Debug((uint8_t)duty);
 
    DelayMs(200);
  }

  // printnumber 测试
  while(1){
    prinfNumber(1660);
    prinfNumber(870);
    prinfNumber(99);
    prinfNumber(1);
    DelayMs(1000);
  }
#endif  
  // 更裸
  while(0)
  {
    GetAxisXYZ();
      //GetDutyDirect();
      
    GetDutyOldStyle();
    
  }
  // 抛弃pitch roll,裸着上
  while(1)
  {
    if(catchTime){
      GetAxisXYZ();
      GetDutyDirect();
          
      catchTime = 0;
      
      sleepModeCounter++;
      if(sleepModeCounter > 100){
        PowerManage();
        sleepModeCounter = 0;
      }
    }
    ChangeColorSlowly();
    DelayMs(1);
    
  }
  while(1)
  {
    if(catchTime){
      GetAxisXYZ();
    
      //I2C_SCL_LOW();
      CountPitch(); 
      GetDutyBaseRP();
      //ChangeColor(); 
      catchTime = 0;
    }
    ChangeColorSlowly();
    DelayMs(1);

    //I2C_SCL_HIGH();
    
    //TransColorNew(); 
    //TransGtoColor();
  }
  while (1)
  {
    //DelayMs(2);
    if (1)
    {
      //if(1){
      
      GetAxisXYZ();
      TransColorNew();
      //TransColor((uint16_t)pose.x);
      //TransColor((uint16_t)pose.y);
      //TransColor((uint16_t)pose.z);
      
      //LIS3DH_GetAccAxesRaw(&axesBuff);
      
      //color.b = axesBuff.AXIS_X * 10;
      
      //color.r = TransColor((uint16_t)axesBuff.AXIS_X);
      //color.g = TransColor((uint16_t)axesBuff.AXIS_Y);
      //color.b = TransColor((uint16_t)axesBuff.AXIS_Z);
      catchTime = 0;
      uartCounter ++;
      
      
      //UART1_SendData8('\r');
      //UART1_SendData8('\n');
    }
    
    if(uartCounter > 100)
    {
      uartCounter = 0;
      //UART1_SendData8(pose.x);
      //Delay(20);
      //UART1_SendData8(pose.y);
      //Delay(20);
      //UART1_SendData8(pose.z);
      //Delay(20);
    }
   
    //LIS3DH_GetAccAxesRaw(&axesBuff);
    //TransColor();
    TransGtoColor();
    

    
    //GPIOC->ODR &= (uint8_t)(~GPIO_PIN_4);
    //GPIOC->ODR &= (uint8_t)(~GPIO_PIN_5);
    //SDA_L;

    //DelayMs(2);
    //GPIOC->ODR |= (uint8_t)GPIO_PIN_4;
    //GPIOC->ODR |= (uint8_t)GPIO_PIN_5;
    //SDA_H;
    //LIS3DH_GetWHO_AM_I(&mid);
    //GPIO_WriteReverse(GPIOC, GPIO_PIN_4);
    //GPIO_WriteReverse(GPIOC, GPIO_PIN_5);
    //GPIOC->ODR ^= 0x10;
    //if(counter > 1000){
    //counter = 0;
    //GPIOC->ODR ^= 0x10;
    //}
    //GPIOC->ODR ^= (uint8_t)GPIO_PIN_4;
    //GPIO_WriteReverse(GPIOC, GPIO_PIN_4);
    //GPIO_WriteReverse(GPIOC, GPIO_PIN_5);
    //}
  }
}
void main_001(void)
{

  CLK_HSICmd(ENABLE);

  CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);

  TIM1_Config();

  GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);

  GPIOC->DDR = 0x10; // 配置PD端口的方向寄存器PD3输出
  GPIOC->CR1 = 0x10; // 设置PD3为推挽输出

  while (1)
  {

    //GPIOC->ODR ^= 0x10;
    //DelayMs(2);
    //GPIO_WriteReverse(GPIOC, GPIO_PIN_4);
    //GPIO_WriteReverse(GPIOC, GPIO_PIN_5);
  }
}

void assert_failed(uint8_t *file, uint32_t line)
{
}