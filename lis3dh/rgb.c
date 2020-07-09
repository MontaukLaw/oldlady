#include "user_comm.h"

#define PITCH_PEAK_LEFT    80
#define PITCH_PEAK_RIGHT   100

#define BUTTOM_PEAK_LEFT   -100
#define BUTTOM_PEAK_RIGHT   -80

#define ROLL_PEAK_LEFT     80
#define ROLL_PEAK_RIGHT    100

#define BUTTOM_ROLL_LEFT   -10
#define BUTTOM_ROLL_RIGHT   10

extern signed short int SL_ACCEL_X, SL_ACCEL_Y, SL_ACCEL_Z;

extern short pitch, roll;

PWMDuty_t pwmDuty = {0,0,0};

void PeakPoint(void)
{
  if(pitch > PITCH_PEAK_LEFT && pitch < PITCH_PEAK_RIGHT ){
    pwmDuty.r = MAX_DUTY;
    pwmDuty.b = 0;
  }
  
  if(pitch > BUTTOM_PEAK_LEFT && pitch < BUTTOM_PEAK_RIGHT){
    pwmDuty.b = MAX_DUTY;
    pwmDuty.r = 0;    
  } 
  
  // 水平状态
  if(roll > BUTTOM_ROLL_LEFT && roll < BUTTOM_ROLL_RIGHT)
  {
    pwmDuty.g = 0;
  } 
  
  if(roll > ROLL_PEAK_LEFT && roll < ROLL_PEAK_RIGHT)
  {
    pwmDuty.g = MAX_DUTY;
  }
  prinfNumber(pwmDuty.g);
}

void GetDutyByRP(void)
{
  uint16_t blue = 0;
  uint16_t red = 0;
  uint16_t temp = 0;
  
  short pitchPlusroll = pitch + roll;
  
  // 仰角大于0, 小于90度, 即朝上
  if(pitch >=0 && pitch <= 90){
    
    // 蓝色直接就是仰角的反值
    blue = 90 - (uint16_t)pitch;
    
    // 红色是从最亮180到90
    red = (uint16_t)pitch + 90;    
    
    // 如果向左roll
    if(blue > roll && roll >= 0){
      // 蓝色值减少, roll多少, 减多少
      blue = blue - roll;
    
    // 如果向右roll, 同样roll多少, 减多少.
    }else if( roll < 0 && (pitch + roll) > 0){
      blue = pitch + roll;
    }else{
      blue = 0;
    }
    
    // 如果向左roll
    if(roll >= 0 && roll <= 90){
      // roll从0到90, 红色从180减到0
      red = red - red * ((uint16_t)roll) / 90;
    // 如果向右rool, 红色从180减为0
    }else if(roll < 0 && roll > -90 ){ //&& pitchPlusroll >= 0      
      red = red - red * ((uint16_t)abs(roll)) / 90;
    }else if(roll == -90){
      red = 180;
    }else if(roll < 0 && pitchPlusroll < 0 ){
      //red = 0;
    }else {
      red = 0;
    }
      
    
  // 如果超过90度向上翻  
  }else if(pitch > 90 && pitch < 180){
    blue = 0;
    red = 180;
    
    if(roll >= 0){
      red = (uint16_t)roll;
    }else{
      red = (uint16_t)(abs(roll));
    }
    if(roll > 90){
      //red = 0;
    }
  
   //如果仰角为负, 即朝下
  }else if(pitch < 0 && pitch >= -90){
    blue = (uint16_t)(abs(pitch)) + 90;
    
    red = (uint16_t)(abs(pitch + 90));
    
    // 向左滚
    if(roll >= 0){
      temp = (uint16_t)roll;
      if(red >= temp){
        red = red - temp;
      }
    // 向右滚  
    }else if(roll> -90 && roll < 0){
      temp = (uint16_t)(abs(roll));
      if(red >= temp){
        red = red - temp;
      }
    }     
  }else if(pitch < -90 && pitch >= -180){
    blue = 180;
    red = 0;
  }
  
  pwmDuty.r = red * (MAX_DUTY/180);
  
  pwmDuty.b = blue * (MAX_DUTY/180);
  
  if(roll >= 0){
     pwmDuty.g = (uint16_t)(roll) * (MAX_DUTY/180) ; 
  }
  
  
  prinfNumber(red);  
  
  pwmDuty.b = 0;
  pwmDuty.g = 0;
  //SetRedDuty(pwmDuty.r);
  //SetBlueDuty(pwmDuty.b);
  //SetGreenDuty(pwmDuty.g); 
}

void GetDutyBaseRP(void)
{

  //if(pitch < 0){
    //pitch = abs(pitch);
  //} 
  //prinfNumber((uint16_t) pitch);  
  if(pitch >= 0 && pitch <= 90){
    pwmDuty.r = (uint16_t)(pitch) * (MAX_DUTY/180) + MAX_DUTY/2;  
  }else if(pitch > 90 && pitch < 180 ){
    pwmDuty.r = (uint16_t)(180 - pitch) * (MAX_DUTY/180) + MAX_DUTY/2;  
  }else if(pitch >= -90 && pitch < 0){
     //pitch = abs(pitch); 
    pwmDuty.r = ((uint16_t)(pitch + 90)) * (MAX_DUTY/180);
  }else if(pitch < -90 && pitch > -180){
    pwmDuty.r = ((uint16_t)(abs(pitch + 90))) * (MAX_DUTY/180); 
  }
  
  //if(roll)
  //prinfNumber((uint16_t) roll);  

  pwmDuty.b = MAX_DUTY - pwmDuty.r; 
  
  // roll部分
  if(roll >= 0 && roll <= 90){
    pwmDuty.g = (uint16_t)(roll) * (MAX_DUTY/180) + MAX_DUTY/2;  
  }else if(roll > 90 && roll < 180 ){
    pwmDuty.g = (uint16_t)(180 - roll) * (MAX_DUTY/180) + MAX_DUTY/2; 
  }else if(roll >= -90 && roll < 0){
    pwmDuty.g = ((uint16_t)(roll + 90)) * (MAX_DUTY/180);
  }else if(roll < -90 && roll > -180){
    pwmDuty.g = ((uint16_t)(abs(roll + 90))) * (MAX_DUTY/180); 
  }
  
  PeakPoint();
}

void SetPWMImm(void)
{
  uint16_t colorTarget = pwmDuty.r;
  static uint16_t colorRNow = 0;
  uint16_t gap = 0;
  if(colorRNow > colorTarget){
    gap = colorRNow - colorTarget;

  }else if(colorRNow < colorTarget){
    gap = colorTarget - colorRNow;    
  }

  if(gap > MIN_GAP){
    TIM2_SetCompare2(colorTarget); 
  }
  //if(gap > 200){
    //colorRNow = colorTarget;   
  //}
  colorRNow = colorTarget; 
 
  //DelayMs(1);
  //prinfNumber(gap);
  //Debug((uint8_t)(gap>>8));
  //Debug((uint8_t)gap);

#if 0  
  if(colorTarget > MIN_DUTY)
  {    
    TIM2_SetCompare2(colorTarget);
    //TIM2_SetCompare2(colorRNew); 
  }else{
    TIM2_SetCompare2(MIN_DUTY); 
  }
#endif
  
}

void ChangeColor(void)
{
  GetDutyBaseRP();
  SetPWMImm();
}

void ChangeColorSlowly(void)
{
  //uint16_t colorTarget = pwmDuty.r;
  static uint16_t colorRNow = 0;
  static uint16_t colorBNow = 0;
  static uint16_t colorGNow = 0;  
  //uint16_t gap = 0;
  if(colorRNow > pwmDuty.r){

    colorRNow -= DUTY_STEP;

  }else if(colorRNow < pwmDuty.r){

    colorRNow += DUTY_STEP;    
  }

  if(colorBNow > pwmDuty.b){

    colorBNow -= DUTY_STEP;

  }else if(colorBNow < pwmDuty.b){

    colorBNow += DUTY_STEP;    
  }
  
  if(colorGNow > pwmDuty.g){

    colorGNow -= DUTY_STEP;

  }else if(colorGNow < pwmDuty.g){

    colorGNow += DUTY_STEP;    
  }
  
  //prinfNumber(colorRNow);
  TIM2_SetCompare2(colorRNow); 
  TIM2_SetCompare3(colorBNow);
  TIM2_SetCompare1(colorGNow);
}

void SetBlueDuty(uint16_t blueDuty)
{
  TIM2_SetCompare3(blueDuty);
}

void SetRedDuty(uint16_t redDuty)
{
  TIM2_SetCompare2(redDuty);
}

void SetGreenDuty(uint16_t greenDuty)
{
  TIM2_SetCompare1(greenDuty);
}

void AllOff(void)
{
  while(pwmDuty.r > MIN_DUTY){
    pwmDuty.r --;
    SetRedDuty(pwmDuty.r);  
    DelayMs(1);
  }
  while( pwmDuty.g > MIN_DUTY ){
    pwmDuty.g --;
    SetGreenDuty(pwmDuty.g);
    DelayMs(1);
  }
  while( pwmDuty.b > MIN_DUTY ){
    pwmDuty.b --;
    SetGreenDuty(pwmDuty.b); 
    DelayMs(1);
  }
  
  TIM2_DeInit();
  TIM2_Cmd(DISABLE);
  //disableInterrupts();
  
  DelayMs(100);
  GPIO_Init(GPIOD, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_WriteLow(GPIOD, GPIO_PIN_3);
  
  DelayMs(100);
  GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_WriteLow(GPIOD, GPIO_PIN_4);
  DelayMs(100);
  
  GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_SLOW);  
  GPIO_WriteLow(GPIOA, GPIO_PIN_3);
  
  DelayMs(1000);
  //SetGreenDuty(0);
  //SetRedDuty(0);
  //SetBlueDuty(0);
}

#define RAINBOW_DIRC_RED_GREEN    0x01
#define RAINBOW_DIRC_GREEN_BLUE  0x02
#define RAINBOW_DIRC_BLUE_RED  0x03

Color_t rainbow = {255,0,0};

void RainbowChang(void)
{
  // 从红色开始

  static uint8_t rainbowDirection = RAINBOW_DIRC_RED_GREEN;
  
  switch(rainbowDirection){
    
  case RAINBOW_DIRC_RED_GREEN:
    rainbow.r--;
    rainbow.g++;
    if(rainbow.r == 0){
      rainbowDirection = RAINBOW_DIRC_GREEN_BLUE;
    }
      
    break;
      
  case RAINBOW_DIRC_GREEN_BLUE:
    rainbow.g--;
    rainbow.b++;
    if(rainbow.g == 0){
      rainbowDirection = RAINBOW_DIRC_BLUE_RED;
    } 
    
    break;

  case RAINBOW_DIRC_BLUE_RED:
    rainbow.b--;
    rainbow.r++;
    if(rainbow.b == 0){
      rainbowDirection = RAINBOW_DIRC_RED_GREEN;
    } 
    
    break;
    
  }

}


uint16_t GsensorToRGB(void)
{

  static RGBColor_t colorNow = {0, 0, 0};

  static uint16_t temp = 0;

  //PrintShort('x', SL_ACCEL_X);
  //PrintShort('y', SL_ACCEL_Y);
  //PrintShort('z', SL_ACCEL_Z);

  if (SL_ACCEL_X >= 0 && SL_ACCEL_X < 128)
  {
    colorNow.R = (uint8_t)SL_ACCEL_X * 2;
  }
  else if (SL_ACCEL_X < 0 && SL_ACCEL_X > -128)
  {
    colorNow.R = (uint8_t)(abs(SL_ACCEL_X)) * 2;
  }
  else if (SL_ACCEL_X == 128 || SL_ACCEL_X == -128)
  {
    colorNow.R = 255;
  }

  if (SL_ACCEL_Y >= 0 && SL_ACCEL_Y < 128)
  {
    colorNow.G = (uint8_t)SL_ACCEL_Y * 2;
  }
  else if (SL_ACCEL_Y < 0 && SL_ACCEL_Y > -128)
  {
    colorNow.G = (uint8_t)(abs(SL_ACCEL_Y)) * 2;
  }
  else if (SL_ACCEL_Y == 128 || SL_ACCEL_Y == -128)
  {
    colorNow.G = 255;
  }

  temp = ((uint16_t)colorNow.G) + ((uint16_t)colorNow.R);

  if (temp >= 255)
  {
    colorNow.B = 0;
  }
  else
  {
    colorNow.B = 255 - (uint8_t)temp;
  }

  if (colorNow.B < 2)
  {
    colorNow.B = 2;
  }
  if (colorNow.R < 2)
  {
    colorNow.R = 2;
  }
  if (colorNow.G < 2)
  {
    colorNow.G = 2;
  }
  
  pwmDuty.r = ((uint16_t)colorNow.R ) * 10;
  pwmDuty.g = ((uint16_t)colorNow.G ) * 10;
  pwmDuty.b = ((uint16_t)colorNow.B ) * 10;
  
  //PrintU8('r',colorNow.R);
  //PrintU8('y',colorNow.B);
  //PrintU8('z',pose.z);

  //PrintU8('r', colorNow.R);
  //PrintU8('g', colorNow.G);
  //PrintU8('b', colorNow.B);

  //TIM2_SetCompare2(colorNow.R); 
  //TIM2_SetCompare3(colorNow.B);
  //TIM2_SetCompare1(colorNow.G);
 
}


void ChangeColorByRainbow(void)
{
  RainbowChang();
  SetBlueDuty(rainbow.b * 5);  
  SetGreenDuty(rainbow.g * 5);
  SetRedDuty(rainbow.r * 5);  
}

void RedBlink(void)
{
  uint8_t i;
  SetGreenDuty(0);
  SetBlueDuty(0);
  for(i=0;i<3;i++){
    SetRedDuty(MAX_DUTY);
    DelayMs(1000);
    SetRedDuty(0);
    DelayMs(1000);
  }
}

void GreenBlink(void)
{
  uint8_t i;
  SetRedDuty(0);
  SetBlueDuty(0);
  for(i=0;i<3;i++){
    SetGreenDuty(MAX_DUTY);
    DelayMs(1000);
    SetGreenDuty(0);
    DelayMs(1000);
  }
}

void BlueBlink(void)
{
  uint8_t i;
  SetRedDuty(0);
  SetGreenDuty(0);
  for(i=0;i<3;i++){
    SetBlueDuty(MAX_DUTY);
    DelayMs(1000);
    SetBlueDuty(0);
    DelayMs(1000);
  }
}