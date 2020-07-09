#ifndef __SYS_H
#define __SYS_H

void DelayMs(uint16_t ms);

void UptimeRoutine(void);

uint16_t TransColor(uint16_t xyz);

void Delay(uint16_t counter);

void Debug(uint8_t data);

void prinfNumber(uint16_t num);

void PowerOff(void);

void PrintShort(uint8_t prefix, short data);

void PrintU8(uint8_t prefix, uint8_t data);

void PrintInt8(uint8_t prefix, int8_t data);

void Sleep(void);

void Wakeup(void);

#endif


