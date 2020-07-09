#ifndef __GSENSOR__H
#define __GSENSOR__H

void CountPitch(void);

void GsensorInit(void);

void GetAxisXYZ(void);

void GetDutyDirect(void);

void GetDutyOldStyle(void);

uint16_t GetDutySC7A20(uint8_t color);

#endif

