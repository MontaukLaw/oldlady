#include "user_comm.h"

static uint32_t uptime = 0;
static uint32_t delay_time = 0;

extern AxesRaw_t axesBuff;
extern Color_t color;
extern uint8_t runningState;

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
    IWDG_ReloadCounter(); 
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

void PowerOff(void)
{
  
#if MEMS_LIS3DH
  LIS3DH_SetMode(LIS3DH_POWER_DOWN);
#endif
  
#if MEMS_SC7A20C
  // ÈÃmems¹Ø»ú
  Sensor_Write_Byte(0x20, 0x00);
#endif

  GPIO_WriteLow(GPIOC, GPIO_PIN_7);
  
  //halt();
  while(1){}
  
}

void Sleep(void)
{
  GPIO_WriteLow(GPIOC, GPIO_PIN_5);  
  
  runningState = RUNNING_CHARGING_FINISHED;

}
void Wakeup(void)
{
  GPIO_WriteLow(GPIOC, GPIO_PIN_5);
  
  runningState = RUNNING_NORMAL;
}

void PrintU8(uint8_t prefix, uint8_t data)
{
  Debug(prefix);
  Debug(':');
  if(data>0){
   
    prinfNumber((uint16_t)data);
  }else{
    Debug('-');
    prinfNumber((uint16_t)(abs(data)));
  }

}

void PrintInt8(uint8_t prefix, int8_t data)
{
  Debug(prefix);
  Debug(':');
  if(data>0){
   
    prinfNumber((uint16_t)data);
  }else{
    Debug('-');
    prinfNumber((uint16_t)(abs(data)));
  }
}
void PrintShort(uint8_t prefix, short data)
{
  Debug(prefix);
  Debug(':');
  if(data>0){
   
    prinfNumber((uint16_t)data);
  }else{
    Debug('-');
    prinfNumber((uint16_t)(abs(data)));
  }
}
