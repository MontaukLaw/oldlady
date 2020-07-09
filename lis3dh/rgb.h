#ifndef __RGB_H
#define __RGB__H

void ChangeColor(void);

void GetDutyBaseRP(void);

void ChangeColorSlowly(void);

void SetBlueDuty(uint16_t blueDuty);

void SetRedDuty(uint16_t redDuty);

void SetGreenDuty(uint16_t greenDuty);

void AllOff(void);

void ChangeColorByRainbow(void);

void GetDutyByRP(void);

void RedBlink(void);

void GreenBlink(void);

void BlueBlink(void);

uint16_t GsensorToRGB(void);

#endif


