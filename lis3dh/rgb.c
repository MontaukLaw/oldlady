#include "user_comm.h"

#define PITCH_PEAK_LEFT    80
#define PITCH_PEAK_RIGHT   100

#define BUTTOM_PEAK_LEFT   -100
#define BUTTOM_PEAK_RIGHT   -80

#define ROLL_PEAK_LEFT     80
#define ROLL_PEAK_RIGHT    100

#define BUTTOM_ROLL_LEFT   -10
#define BUTTOM_ROLL_RIGHT   10

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
  uint16_t colorTarget = pwmDuty.r;
  static uint16_t colorRNow = 0;
  static uint16_t colorBNow = 0;
  static uint16_t colorGNow = 0;  
  //uint16_t gap = 0;
  if(colorRNow > colorTarget){

    colorRNow -= DUTY_STEP;

  }else if(colorRNow < colorTarget){

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
  
  TIM1_Cmd(DISABLE); 
  GPIO_Init(GPIOD, GPIO_PIN_3, GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_WriteLow(GPIOD, GPIO_PIN_3);
  
  GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_WriteLow(GPIOC, GPIO_PIN_4);
  
  GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_WriteLow(GPIOC, GPIO_PIN_3);
  DelayMs(10);
  //SetGreenDuty(0);
  //SetRedDuty(0);
  //SetBlueDuty(0);
}
