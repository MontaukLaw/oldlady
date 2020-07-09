#ifndef __USER_COMM__H
#define __USER_COMM__H

#define FILTER_ON   0
#include <stdlib.h>
#include "stm8s.h"

#include "gsensor.h"
#include "sys.h"
#include "bsp_i2c.h"
#include "lis3dh_driver.h"
//#include "math.h"
#include "stdio.h"
#include "sc7a20.h"
#include "rgb.h"
#include "drv_l1_gsensor.h"
#include "ws2812.h"
#include "bsp_adc.h"
#include "bsp_uart.h"
#include "bsp_eeprom.h"

#define TIM4_PERIOD       128
#define MAX_DUTY          2550
#define BASE_DUTY         (MAX_DUTY/2)

#define BYTE_BIT          255
#define MIN_GAP           100
#define MIN_DUTY          50

#define MIN_PITCH_FILTER   10
#define MIN_ROLL_FILTER    10

#define DUTY_STEP                50
#define SLEEP_POSE_THRESHOLD     50
#define SLEEP_MODE_WAIT_SECONDS  60

#define SLEEP_MODE_WAIT_COUNTERS  300  //´óÔ¼13Ãë

#define RUNNING_CHARNING            0x01
#define RUNNING_NORMAL              0x02
#define RUNNING_CHARGING_FINISHED   0x03
#define RUNNING_TESTING             0x04

#define INPUT_3_0V          515
#define INPUT_3_5V          550
//532 Ô¼3.4·ü
#define MIN_BAT_ADC         532
//#define MIN_BAT_ADC         570

#define LOW_VOL_MAX         10

#define MEMS_SC7A20C         1

#define MEMS_LIS3DH          0

#define WS2812               0
#define RGB_PWM              1

typedef struct {
  uint16_t r;
  uint16_t g;
  uint16_t b;
} Color_t;

typedef struct {
  uint8_t x;
  uint8_t y;
  uint8_t z;
} Pose_t;

typedef struct {
  uint16_t r;
  uint16_t g;
  uint16_t b;
} PWMDuty_t;



#endif
