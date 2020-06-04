#include "stm8s.h"
#include "stm8s_i2c.h"

#define FAST_I2C_MODE
#define BUFFERSIZE  10

#ifdef FAST_I2C_MODE
#define I2C_SPEED 300000
#else
#define I2C_SPEED 100000
#endif

#define SLAVE_ADDRESS LIS3DH_I2C_ADD_H

#define LIS3DH_I2C_ADD_H   0x33U

__IO uint8_t RxBuffer[BUFFERSIZE];

void I2CInit(void)
{
  /* I2C Initialize */
  I2C_Init(I2C_SPEED, 0xA0, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, 16);
  /* Enable Buffer and Event Interrupt*/
  I2C_ITConfig((I2C_IT_TypeDef)(I2C_IT_EVT | I2C_IT_BUF) , ENABLE);
  
  enableInterrupts();

}

void GensorInit(void)
{


}

void main(void)
{
  /* system_clock / 1 */
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  I2CInit();
  while(1){
   /* Send START condition */
  I2C_GenerateSTART(ENABLE);

  I2C_Send7bitAddress(SLAVE_ADDRESS, I2C_DIRECTION_RX);

  while ((I2C_GetFlagStatus(I2C_FLAG_TRANSFERFINISHED) == RESET)); /* Poll on BTF */
    
  I2C_SendData(0x0F);

  while ((I2C_GetFlagStatus(I2C_FLAG_TRANSFERFINISHED) == RESET)); /* Poll on BTF */  
  
  /* Read a byte from the Slave */
  RxBuffer[0] = I2C_ReceiveData();

  while ((I2C_GetFlagStatus(I2C_FLAG_TRANSFERFINISHED) == RESET)); /* Poll on BTF */  
    
  I2C_GenerateSTOP(ENABLE);  
  
  }
}

void assert_failed(uint8_t* file, uint32_t line){

}
