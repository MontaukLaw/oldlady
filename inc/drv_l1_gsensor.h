#ifndef __DRV_L1_GSENSOR_H__
#define __DRV_L1_GSENSOR_H__

#define P_SENSITIVE_LOW	     0x60
#define P_SENSITIVE_MID	     0x38
#define P_SENSITIVE_HIGH	 0x10

#define DMT_SENSITIVE_LOW	 0xB0
#define DMT_SENSITIVE_MID	 0x60
#define DMT_SENSITIVE_HIGH	 0x20

//==========================================
//    SC7A20 Register
//==========================================
#define SOFT_RESET    0x00
//#define CHIPID        0x01
#define CHIPID        0x0f// erichan 20150529

#define ACC_X_LSB     0x02
#define ACC_X_MSB     0x03

#define ACC_Y_LSB     0x04
#define ACC_Y_MSB     0x05

#define ACC_Z_LSB     0x06
#define ACC_Z_MSB     0x07

#define MOTION_FLAG   0x09
#define NEWDATA_FLAG  0x0A

#define TAP_ACTIVE_STATUS 0x0B  

#define RESOLUTION_RANGE 0x0F  // Resolution bit[3:2] -- 00:14bit 
                               //                        01:12bit 
                               //                        10:10bit
                               //                        11:8bit 
                               
                               // FS bit[1:0]         -- 00:+/-2g 
                               //                        01:+/-4g 
                               //                        10:+/-8g
                               //                        11:+/-16g 
#define ODR_AXIS      0x10 
#define MODE_BW       0x11                             
#define SWAP_POLARITY 0x12
#define INT_SET1      0x16
#define INT_SET2      0x17
#define INT_MAP1      0x19 
#define INT_MAP2      0x1A
#define INT_CONFIG    0x20 
#define INT_LATCH     0x21
#define FREEFALL_DUR  0x22
#define FREEFALL_THS  0x23
#define FREEFALL_HYST 0x24
#define ACTIVE_DUR    0x27
#define ACTIVE_THS    0x28
#define TAP_DUR       0x2A
#define TAP_THS       0x2B
#define ORIENT_HYST   0x2C
#define Z_BLOCK       0x2D
#define SELF_TEST     0x32
#define ENGINEERING_MODE   0x7f   
//==========================================================
// ARD07 Register
//===========================================================

#define DMT_ARD07_CTRL_REG_1	0x44
#define DMT_ARD07_CTRL_REG_2	0x45
#define DMT_ARD07_CTRL_REG_3	0x46
#define DMT_ARD07_CTRL_REG_4	0x47
#define DMT_ARD07_CTRL_REG_5	0x4A
#define DMT_ARD07_CTRL_REG_6	0x4B
#define DMT_ARD07_CTRL_REG_7	0x4C
#define DMT_ARD07_CTRL_REG_8	0x4D
#define DMT_ARD07_CTRL_REG_9	0x48
#define DMT_ARD07_REG_XOUT	0x41
#define DMT_ARD07_REG_YOUT	0x42
#define DMT_ARD07_REG_ZOUT	0x43
#define DMT_ARD07_REG_STS	0x49


#define DMT_ARD07_LOW		0x60
#define DMT_ARD07_MID		0x38
#define DMT_ARD07_HIGH		0x07

//==========================================================
//extern void G_Sensor_SC7A20_Init(uint8_t *Acc_Int_Thr);
//extern void G_Sensor_SC7A20_Init(uint8_t Acc_Int_Thr);
extern void SC7A20_Enter_Interrupt_WakeUp_Mode(uint8_t level);
extern void g_sensor_write(uint8_t id, uint8_t addr, uint8_t data);
extern uint16_t g_sensor_read(uint8_t id, uint8_t addr);
extern void sw_i2c_lock(void);
extern void sw_i2c_unlock(void);
extern void ap_gsensor_set_sensitive(uint8_t Gsensor);
extern void G_Sensor_Init(uint8_t level);
extern void G_Sensor_park_mode_init(uint8_t level);
extern void DMARD07_Enter_Interrupt_WakeUp_Mode(uint8_t level);
extern void G_Sensor_DMARD07_Init(uint8_t level);
extern void G_sensor_clear_int_flag(void);
extern uint16_t G_sensor_get_int_active(void);

extern void ap_gsensor_power_on_set_status(uint8_t status);
extern uint8_t ap_gsensor_power_on_get_status(void);

extern void I2C_delay (uint16_t i) ;
extern void I2C_start (void);
extern void I2C_stop (void);
extern void I2C_w_phase (uint16_t value);
extern uint16_t I2C_r_phase (void);
extern void I2C_gpio_init (uint32_t nSCL,uint32_t nSDA);


#endif
