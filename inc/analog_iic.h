#ifndef __ANALOG_IIC_H__
#define __ANALOG_IIC_H__


#include "prjlib.h"

//Sensor RW Address
#define  Sensor_Wr_Addr1    0x32//DOUT_A1 level GND
#define  Sensor_Rd_Addr1    0x33
//#define  Sensor_Wr_Addr2  0xD8//DOUT_A1 level VDD
//#define  Sensor_Rd_Addr2  0xD9



#define I2C_SCL_HIGH()   do { NRF_GPIO->OUTSET = (1UL << (G_SCL)); } while(0)
#define I2C_SCL_LOW()    do { NRF_GPIO->OUTCLR = (1UL << (G_SCL)); } while(0)

#define I2C_SDA_HIGH()   do { NRF_GPIO->OUTSET = (1UL << (G_SDA));  } while(0)
#define I2C_SDA_LOW()    do { NRF_GPIO->OUTCLR = (1UL << (G_SDA));  } while(0)

#define I2C_SDA_INPUT()  do { NRF_GPIO->DIRCLR = (1UL << (G_SDA));  } while(0)
#define I2C_SDA_OUTPUT() do { NRF_GPIO->DIRSET = (1UL << (G_SDA));  } while(0)

#define I2C_SCL_OUTPUT() do { NRF_GPIO->DIRSET = (1UL << (G_SCL)); } while(0)

#define I2C_SDA_READ()  ((NRF_GPIO->IN >> (G_SDA)) & 0x1UL)



extern void Analog_IIC_Delay(uint8_t n);
extern void Analog_IIC_Pin_Init(void);
extern void Sensor_Write_Byte(uint8_t RAddr, uint8_t *WData);
extern void Sensor_Write_NByte(uint8_t RAddr, uint8_t *WData, uint8_t WLen);
extern void Sensor_Read_Byte(uint8_t RAddr, uint8_t *RData);
extern void Sensor_Read_NByte(uint8_t RAddr, uint8_t *RData, uint8_t RLen);
extern void IIC_SDA_Dir(uint8_t d);

#endif

