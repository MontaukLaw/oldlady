#ifndef __SYS_H
#define __SYS_H

#include "user_comm.h"
#include "lis3dh_driver.h"

void DelayMs(uint16_t ms);

void UptimeRoutine(void);

uint16_t TransColor(uint16_t xyz);

void Delay(uint16_t counter);

void Debug(uint8_t data);

void prinfNumber(uint16_t num);

#endif


