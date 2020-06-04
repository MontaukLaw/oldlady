#ifndef __GSENSOR__H
#define __GSENSOR__H

#define FILTER_ON   0

#include "stm8s.h"

#include "sys.h"
#include "bsp_i2c.h"
#include "lis3dh_driver.h"
#include <math.h>
#include "stdio.h"

#include "gsensor.h"
#include "rgb.h"
#define TIM4_PERIOD       128
#define MAX_DUTY          2550

#define BYTE_BIT          255
#define MIN_GAP           100
#define MIN_DUTY          50

#define MIN_PITCH_FILTER   10
#define MIN_ROLL_FILTER    10

#define DUTY_STEP                10
#define SLEEP_POSE_THRESHOLD     50
#define SLEEP_MODE_WAIT_SECONDS  10
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
