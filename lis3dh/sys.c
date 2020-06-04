#include "user_comm.h"

static uint32_t uptime = 0;
static uint32_t delay_time = 0;

extern AxesRaw_t axesBuff;
extern Color_t color;

void Delay(uint16_t counter)
{
  uint8_t i;
  while(counter --){
    for(i=0;i<1;i++){
    
    }
  }
}

void DelayMs(uint16_t ms)
{
  delay_time = uptime + ms;
  while (delay_time > uptime)
  {
  }
}

void Debug(uint8_t data)
{
  UART1_SendData8(data);
  while(!UART1_GetFlagStatus(UART1_FLAG_TC))
  {
    
  }  
      
}

uint16_t TransColor(uint16_t xyz)
{
  uint16_t tmpB = ((uint16_t) xyz)*(10); 
  uint16_t result = 0;
  if( tmpB > (MAX_DUTY/2)){
    result = (MAX_DUTY - tmpB) * 2;
  }else{
    result =  tmpB * 2;
  }
  
  return result;
   
}

void UptimeRoutine(void)
{
  uptime++;
  if (uptime == 0xFFFFFFFF)
  {
    delay_time = 0;
    uptime = 0;
  }
  
}

void prinfNumber(uint16_t num)
{
  if(num>10000){
    Debug('s');
    Debug('b');     
  }else if(num >= 1000){
    Debug((uint8_t)(num/1000 + 0x30)); 
    num = num - (num/1000) * 1000;
    Debug((uint8_t)(num/100 + 0x30));
    num = num - (num/100) * 100;    
    Debug((uint8_t)(num/10 + 0x30));
    num = num - (num/10) * 10;    
    Debug((uint8_t)(num + 0x30));
    Debug('\r');
    Debug('\n');     
  }else if(num >= 100){
    Debug((uint8_t)(num/100 + 0x30));
    num = num - (num/100) * 100;    
    Debug((uint8_t)(num/10 + 0x30));
    num = num - (num/10) * 10;    
    Debug((uint8_t)(num + 0x30));
    Debug('\r');
    Debug('\n');   
  }else if(num < 100 && num >= 10){
    
    Debug((uint8_t)(num/10 + 0x30));
    num = num - (num/10) * 10;    
    Debug((uint8_t)(num + 0x30));
    Debug('\r');
    Debug('\n');     
  }else if(num < 10){
    Debug((uint8_t)(num + 0x30));
    Debug('\r');
    Debug('\n');    
  }
}

