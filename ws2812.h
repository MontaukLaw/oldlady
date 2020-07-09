#ifndef __WS2812__H
#define __WS2812__H

#define GPIOA_PORT    0x5000
#define GPIOB_PORT    0x5005
#define GPIOC_PORT    0x500A
#define GPIOD_PORT    0x500F
#define GPIOE_PORT    0x5014
#define GPIOF_PORT    0x5019
#define GPIOG_PORT    0x501E
#define GPIOH_PORT    0x5023
#define GPIOI_PORT    0x5028

#define _ws2812b_create_port(__set__, __port__, __pin__) #__set__" L:"#__port__", #"#__pin__" \n"
#define ws2812b_create_port(__set__, __port__, __pin__) _ws2812b_create_port(__set__, __port__, __pin__)

#define RGBLedPin_Set   ws2812b_create_port(BSET, WS2812B_PORT, WS2812B_PIN)
#define RGBLedPin_ReSet ws2812b_create_port(BRES, WS2812B_PORT, WS2812B_PIN)

#define WS2812B_PORT  GPIOD_PORT
#define WS2812B_PIN   3

#define NB_LEDS       4

typedef struct
{
  unsigned char R;
  unsigned char G;
  unsigned char B;
}RGBColor_t;


void rainbowCycle(uint8_t wait);

void rgb_SendArray(void);

void RGBSetColor(uint8_t LedId, RGBColor_t Color);

void GsensorToWS2812(void);

void InitColor(void);

void WS2812RedBlink(void);

void WS2812GreenBlink(void);

void WS2812BlueBlink(void);

#endif


