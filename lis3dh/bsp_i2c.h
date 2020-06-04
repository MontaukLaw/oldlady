#ifndef __BSP_I2C_H
#define __BSP_I2C_H

#include "user_comm.h"

//#define  Sensor_Wr_Addr1    0x32  //DOUT_A1 level GND
//#define  Sensor_Rd_Addr1    0x33

#define  Sensor_Wr_Addr1    0x30  //DOUT_A1 level GND
#define  Sensor_Rd_Addr1    0x31

#define SDA_PORT   GPIOB     //GPIOC
#define SDA_PIN    GPIO_PIN_5 

#define SCL_PORT   GPIOB     //GPIOC
#define SCL_PIN    GPIO_PIN_4

#if 1
#define I2C_SDA_LOW();   SDA_L
#define I2C_SDA_HIGH();  SDA_H

#define I2C_SCL_HIGH();  SCL_H
#define I2C_SCL_LOW();   SCL_L 

#endif

//#define I2C_SDA_LOW();      GPIOB->ODR&=0xDF;
//#define I2C_SDA_HIGH();     GPIOB->ODR|=0x20;

//#define I2C_SCL_LOW();      GPIOB->ODR&=0xEF;
//#define I2C_SCL_HIGH();     GPIOB->ODR|=0x10;


#define SDA_H      GPIO_WriteHigh(SDA_PORT, SDA_PIN);
#define SDA_L      GPIO_WriteLow(SDA_PORT, SDA_PIN);

#define SCL_H      GPIO_WriteHigh(SCL_PORT, SCL_PIN);
#define SCL_L      GPIO_WriteLow(SCL_PORT, SCL_PIN);

#define SDA_OUT    GPIO_Init(SDA_PORT, SDA_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
//SDA_PORT->DDR |= (uint8_t)SDA_PIN;

#define SDA_IN     GPIO_Init(SDA_PORT, SDA_PIN, GPIO_MODE_IN_PU_NO_IT);
//SDA_PORT->DDR &= (uint8_t)(~(SDA_PIN));

#define I2C_SDA_READ()    (GPIO_ReadInputPin(SDA_PORT, SDA_PIN) == SET)

void Sensor_Read_Byte(uint8_t RAddr, uint8_t *RData);

void Sensor_Write_NByte(uint8_t RAddr, uint8_t *WData, uint8_t WLen);

void Sensor_Write_Byte(uint8_t RAddr, uint8_t *WData) ;


#endif


