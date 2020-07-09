#include "user_comm.h"

unsigned char LedsArray[NB_LEDS * 3];
unsigned int nbLedsBytes = NB_LEDS * 3;
extern Pose_t pose;
extern signed short int SL_ACCEL_X, SL_ACCEL_Y, SL_ACCEL_Z; //三轴数据

void InitColor(void)
{
  uint8_t i, j;
  RGBColor_t colorStart = {0, 0, 0};
  for (j = 0; j < 255; j++)
  {
    colorStart.G = j;

    for (i = 0; i < NB_LEDS; i++)
    {
      RGBSetColor(i, colorStart);
    }
    rgb_SendArray();
    DelayMs(15);
  }
}

void GsensorToWS2812(void)
{
  uint8_t i;

  static RGBColor_t colorNow = {0, 0, 0};

  static uint16_t temp = 0;

  //PrintShort('x', SL_ACCEL_X);
  //PrintShort('y', SL_ACCEL_Y);
  //PrintShort('z', SL_ACCEL_Z);

  if (SL_ACCEL_X >= 0 && SL_ACCEL_X < 128)
  {
    colorNow.R = (uint8_t)SL_ACCEL_X * 2;
  }
  else if (SL_ACCEL_X < 0 && SL_ACCEL_X > -128)
  {
    colorNow.R = (uint8_t)(abs(SL_ACCEL_X)) * 2;
  }
  else if (SL_ACCEL_X == 128 || SL_ACCEL_X == -128)
  {
    colorNow.R = 255;
  }

  if (SL_ACCEL_Y >= 0 && SL_ACCEL_Y < 128)
  {
    colorNow.G = (uint8_t)SL_ACCEL_Y * 2;
  }
  else if (SL_ACCEL_Y < 0 && SL_ACCEL_Y > -128)
  {
    colorNow.G = (uint8_t)(abs(SL_ACCEL_Y)) * 2;
  }
  else if (SL_ACCEL_Y == 128 || SL_ACCEL_Y == -128)
  {
    colorNow.G = 255;
  }

  //if(colorNow.R <=256)
  //colorNow.B = 255 - colorNow.R;
  //if(SL_ACCEL_Z >= 0 && SL_ACCEL_Z < 128){
  //colorNow.B = (uint8_t)SL_ACCEL_Z * 2 ;
  //}else if(SL_ACCEL_Z < 0 && SL_ACCEL_Z > -128){
  //colorNow.B = (uint8_t)(abs(SL_ACCEL_Z)) * 2;
  //}else if(SL_ACCEL_Z == 128 || SL_ACCEL_Z == -128){
  //colorNow.B = 255;
  //}

  //if(colorNow.B >= colorNow.G){
  //colorNow.B = colorNow.B - colorNow.G;
  //}else{

  temp = ((uint16_t)colorNow.G) + ((uint16_t)colorNow.R);

  if (temp >= 255)
  {
    colorNow.B = 0;
  }
  else
  {
    colorNow.B = 255 - (uint8_t)temp;
  }
  //colorNow.B = 255 - colorNow.G - colorNow.R;
  //}

#if 0
  if(pose.x > 125){
    colorNow.R = (255 - pose.x)*2;
  }else{
    colorNow.R = pose.x;
  }
  if(pose.y > 125){
    colorNow.G = (255 - pose.y)*2;
  }else{
    colorNow.G = pose.y;
  }
  if(pose.z > 125){
    colorNow.B = (255 - pose.z)*2;
  }else{
    colorNow.B = pose.z;
  }  
  //colorNow.R = pose.x;
  //colorNow.G = pose.y;
  //colorNow.B = pose.z;
#endif
  if (colorNow.B < 2)
  {
    colorNow.B = 2;
  }
  if (colorNow.R < 2)
  {
    colorNow.R = 2;
  }
  if (colorNow.G < 2)
  {
    colorNow.G = 2;
  }

  for (i = 0; i < NB_LEDS; i++)
  {
    RGBSetColor(i, colorNow);
  }

  //PrintU8('r',colorNow.R);
  //PrintU8('y',colorNow.B);
  //PrintU8('z',pose.z);

  //PrintU8('r', colorNow.R);
  //PrintU8('g', colorNow.G);
  //PrintU8('b', colorNow.B);

  // 发送信号到ws2812
  rgb_SendArray();
}

static void SetColor(RGBColor_t color){
  uint8_t i;  
  for (i = 0; i < NB_LEDS; i++)
  {
    RGBSetColor(i, color);
  }  
}
void WS2812BlueBlink(void)
{
  uint8_t j;
  RGBColor_t colorBlink = {0, 0, 0};

  for(j = 0; j<4; j++){
    colorBlink.B = 255;
    SetColor(colorBlink);
    rgb_SendArray();
    DelayMs(1000);
  
    colorBlink.B = 0;

    SetColor(colorBlink);
    rgb_SendArray();
    
    DelayMs(1000);
  }  
}

void WS2812GreenBlink(void)
{
  uint8_t j;
  RGBColor_t colorBlink = {0, 0, 0};

  for(j = 0; j<4; j++){
    colorBlink.G = 255;
    SetColor(colorBlink);
    rgb_SendArray();
    DelayMs(1000);
  
    colorBlink.G = 0;

    SetColor(colorBlink);
    rgb_SendArray();
    
    DelayMs(1000);
  }

}

void WS2812RedBlink(void)
{
  uint8_t j;
  RGBColor_t colorBlink = {0, 0, 0};

  for(j = 0; j<4; j++){
    colorBlink.R = 255;
    SetColor(colorBlink);
    rgb_SendArray();
    DelayMs(1000);
  
    colorBlink.R = 0;

    SetColor(colorBlink);
    rgb_SendArray();
    
    DelayMs(1000);
  }
}

void RGBSetColor(uint8_t LedId, RGBColor_t Color)
{
  if (LedId >= NB_LEDS)
    return; //to avoid overflow
  LedsArray[LedId * 3] = Color.G;
  LedsArray[LedId * 3 + 1] = Color.R;
  LedsArray[LedId * 3 + 2] = Color.B;
}

void rgb_SetColor(unsigned char LedId, RGBColor_t Color)
{
  if (LedId > NB_LEDS)
    return; //to avoid overflow
  LedsArray[LedId * 3] = Color.G;
  LedsArray[LedId * 3 + 1] = Color.R;
  LedsArray[LedId * 3 + 2] = Color.B;
}

RGBColor_t Wheel(unsigned char WheelPos)
{
  WheelPos = 255 - WheelPos;
  RGBColor_t color;
  if (WheelPos < 85)
  {
    color.R = 255 - WheelPos * 3;
    color.G = 0;
    color.B = WheelPos * 3;
    return color;
  }
  if (WheelPos < 170)
  {
    WheelPos -= 85;
    color.R = 0;
    color.G = WheelPos * 3;
    color.B = 255 - WheelPos * 3;
    return color;
  }

  WheelPos -= 170;
  color.R = WheelPos * 3;
  color.G = 255 - WheelPos * 3;
  color.B = 0;
  return color;
}

void rainbowCycle(uint8_t wait)
{
  uint16_t i, j;
  for (j = 0; j < 256 * 5; j++)
  { // 5 cycles of all colors on wheel
    for (i = 0; i < NB_LEDS; i++)
    {
      rgb_SetColor(i, Wheel(((i * 256 / 10) + j) & 255));
    }
    rgb_SendArray();
    DelayMs(wait);
  }
}

void rgb_SendArray(void)
{

  unsigned char int_state = __get_interrupt_state();
  __disable_interrupt();
  asm(
      "lb_intiLoop:        \n"
      "LDW      X,       #0xFFFF       \n"
      // set -1 in X, so that first inc gets 0, as inc has to be in the beginning of the loop
      // 往寄存器X里面写入0xFFFF
      "LDW      Y,       L:nbLedsBytes \n" //2
      // 往寄存器Y里面写入nbLedBytes, 即灯数量*每个灯需要3个字节(8bit)
      "lb_begin_loop:      \n"
      // 程序开始
      //---------------------------------------------------------------
      //--------------- bit 0 -----------------------------------------
      "INCW    X         \n" // L t+2
      // X加1
      "lb_start_bit0:          \n" //
      //
      "LD      A,     (L:LedsArray,X)\n" //
      RGBLedPin_Set
      "AND     A,       #0x80  \n" // H t2
      //
      "JREQ    L:lb_bit0_Send_0      \n" // H0 t3 t4 : 2 jump to Zero, 1 Stay in One + next nop
      "lb_bit0_Send_1:         \n"       //------Send 1 : 800ns High, 450ns Low (12,8) 750ns,500ns
      "nop         \n"                   // H1 t5
      "nop         \n"                   // H1 t6
      "nop         \n"                   // H1 t7
      "nop         \n"                   // H1 t8
      "nop         \n"                   // H1 t9
      "nop         \n"                   // H1 t10
      "nop         \n"                   // H1 t11
      "nop         \n"                   // H1 t12
      RGBLedPin_ReSet                    // L1 t1
      "nop         \n"                   // L1 t2
      "JRA     L:lb_start_bit1       \n" // L1 JRA:2 t4
                                         // L1 NextBitTest:4  t8
      "lb_bit0_Send_0:         \n"       //------Send 0 : 400ns High, 850ns Low (7,13) 375ns,875ns
                                         // H0 t4
      RGBLedPin_ReSet                    // L0 t1
      "nop         \n"                   // L0 t1
      "nop         \n"                   // L0 t2
      "nop         \n"                   // L0 t3
      "nop         \n"                   // L0 t4
      "nop         \n"                   // L0 t5
      "nop         \n"                   // L0 t6
      "nop         \n"                   // L0 t7
      "nop         \n"                   // L0 t8
                                         //NextBitTest:4+SP = 5. L t13
                                         //--------------- bit 1 -----------------------------------------
      "lb_start_bit1:          \n"       //
      "LD      A,     (L:LedsArray,X)\n" //1
      "AND     A,       #0x40  \n"       //1
      "JREQ    L:lb_bit1_Send_0      \n" //2 jump to Zero, 1 Stay in One + next nop
      "lb_bit1_Send_1:         \n"
      "nop         \n"                   //1 to have send0 send1 equality
      RGBLedPin_Set                      // H t1
      "nop         \n"                   // H t2
      "nop         \n"                   // H t3
      "nop         \n"                   // H t4
      "nop         \n"                   // H t5
      "nop         \n"                   // H t6
      "nop         \n"                   // H t7
      "nop         \n"                   // H t8
      "nop         \n"                   // H t9
      "nop         \n"                   // H t10
      "nop         \n"                   // H t11
      "nop         \n"                   // H t12
      RGBLedPin_ReSet                    // L t1
      "nop         \n"                   // L t2
      "JRA     L:lb_start_bit2       \n" // L JRA:2 t4
                                         // L NextBitTest:4  t8
      "lb_bit1_Send_0:         \n"       //------Send 0 : 400ns High, 850ns Low (6,14) 375ns,875ns
      RGBLedPin_Set                      // H t1
      "nop         \n"                   // H t2
      "nop         \n"                   // H t3
      "nop         \n"                   // H t4
      "nop         \n"                   // H t5
      "nop         \n"                   // H t6
      RGBLedPin_ReSet                    // L t1
      "nop         \n"                   // L t1
      "nop         \n"                   // L t2
      "nop         \n"                   // L t3
      "nop         \n"                   // L t4
      "nop         \n"                   // L t5
      "nop         \n"                   // L t6
      "nop         \n"                   // L t7
      "nop         \n"                   // L t8
      "nop         \n"                   // L t9
                                         //NextBitTest:4+SP = 5. L t14
                                         //--------------- bit 2 -----------------------------------------
      "lb_start_bit2:          \n"       //
      "LD      A,     (L:LedsArray,X)\n" //1
      "AND     A,       #0x20  \n"       //1
      "JREQ    L:lb_bit2_Send_0      \n" //2 jump to Zero, 1 Stay in One + next nop
      "lb_bit2_Send_1:         \n"
      "nop         \n"                   //1 to have send0 send1 equality
      RGBLedPin_Set                      // H t1
      "nop         \n"                   // H t2
      "nop         \n"                   // H t3
      "nop         \n"                   // H t4
      "nop         \n"                   // H t5
      "nop         \n"                   // H t6
      "nop         \n"                   // H t7
      "nop         \n"                   // H t8
      "nop         \n"                   // H t9
      "nop         \n"                   // H t10
      "nop         \n"                   // H t11
      "nop         \n"                   // H t12
      RGBLedPin_ReSet                    // L t1
      "nop         \n"                   // L t2
      "JRA     L:lb_start_bit3       \n" // L JRA:2 t4
                                         // L NextBitTest:4  t8
      "lb_bit2_Send_0:         \n"       //------Send 0 : 400ns High, 850ns Low (6,14) 375ns,875ns
      RGBLedPin_Set                      // H t1
      "nop         \n"                   // H t2
      "nop         \n"                   // H t3
      "nop         \n"                   // H t4
      "nop         \n"                   // H t5
      "nop         \n"                   // H t6
      RGBLedPin_ReSet                    // L t1
      "nop         \n"                   // L t1
      "nop         \n"                   // L t2
      "nop         \n"                   // L t3
      "nop         \n"                   // L t4
      "nop         \n"                   // L t5
      "nop         \n"                   // L t6
      "nop         \n"                   // L t7
      "nop         \n"                   // L t8
      "nop         \n"                   // L t9
                                         //NextBitTest:4+SP = 5. L t14
                                         //--------------- bit 3 -----------------------------------------
      "lb_start_bit3:          \n"       //
      "LD      A,     (L:LedsArray,X)\n" //1
      "AND     A,       #0x10  \n"       //1
      "JREQ    L:lb_bit3_Send_0      \n" //2 jump to Zero, 1 Stay in One + next nop
      "lb_bit3_Send_1:         \n"
      "nop         \n"                   //1 to have send0 send1 equality
      RGBLedPin_Set                      // H t1
      "nop         \n"                   // H t2
      "nop         \n"                   // H t3
      "nop         \n"                   // H t4
      "nop         \n"                   // H t5
      "nop         \n"                   // H t6
      "nop         \n"                   // H t7
      "nop         \n"                   // H t8
      "nop         \n"                   // H t9
      "nop         \n"                   // H t10
      "nop         \n"                   // H t11
      "nop         \n"                   // H t12
      RGBLedPin_ReSet                    // L t1
      "nop         \n"                   // L t2
      "JRA     L:lb_start_bit4       \n" // L JRA:2 t4
                                         // L NextBitTest:4  t8
      "lb_bit3_Send_0:         \n"       //------Send 0 : 400ns High, 850ns Low (6,14) 375ns,875ns
      RGBLedPin_Set                      // H t1
      "nop         \n"                   // H t2
      "nop         \n"                   // H t3
      "nop         \n"                   // H t4
      "nop         \n"                   // H t5
      "nop         \n"                   // H t6
      RGBLedPin_ReSet                    // L t1
      "nop         \n"                   // L t1
      "nop         \n"                   // L t2
      "nop         \n"                   // L t3
      "nop         \n"                   // L t4
      "nop         \n"                   // L t5
      "nop         \n"                   // L t6
      "nop         \n"                   // L t7
      "nop         \n"                   // L t8
      "nop         \n"                   // L t9
                                         //NextBitTest:4+SP = 5. L t14
                                         //--------------- bit 4 -----------------------------------------
      "lb_start_bit4:          \n"       //
      "LD      A,     (L:LedsArray,X)\n" //1
      "AND     A,       #0x08  \n"       //1
      "JREQ    L:lb_bit4_Send_0      \n" //2 jump to Zero, 1 Stay in One + next nop
      "lb_bit4_Send_1:         \n"
      "nop         \n"                   //1 to have send0 send1 equality
      RGBLedPin_Set                      // H t1
      "nop         \n"                   // H t2
      "nop         \n"                   // H t3
      "nop         \n"                   // H t4
      "nop         \n"                   // H t5
      "nop         \n"                   // H t6
      "nop         \n"                   // H t7
      "nop         \n"                   // H t8
      "nop         \n"                   // H t9
      "nop         \n"                   // H t10
      "nop         \n"                   // H t11
      "nop         \n"                   // H t12
      RGBLedPin_ReSet                    // L t1
      "nop         \n"                   // L t2
      "JRA     L:lb_start_bit5       \n" // L JRA:2 t4
                                         // L NextBitTest:4  t8
      "lb_bit4_Send_0:         \n"       //------Send 0 : 400ns High, 850ns Low (6,14) 375ns,875ns
      RGBLedPin_Set                      // H t1
      "nop         \n"                   // H t2
      "nop         \n"                   // H t3
      "nop         \n"                   // H t4
      "nop         \n"                   // H t5
      "nop         \n"                   // H t6
      RGBLedPin_ReSet                    // L t1
      "nop         \n"                   // L t1
      "nop         \n"                   // L t2
      "nop         \n"                   // L t3
      "nop         \n"                   // L t4
      "nop         \n"                   // L t5
      "nop         \n"                   // L t6
      "nop         \n"                   // L t7
      "nop         \n"                   // L t8
      "nop         \n"                   // L t9
                                         //NextBitTest:4+SP = 5. L t14
                                         //--------------- bit 5 -----------------------------------------
      "lb_start_bit5:          \n"       //
      "LD      A,     (L:LedsArray,X)\n" //1
      "AND     A,       #0x04  \n"       //1
      "JREQ    L:lb_bit5_Send_0      \n" //2 jump to Zero, 1 Stay in One + next nop
      "lb_bit5_Send_1:         \n"
      "nop         \n"                   //1 to have send0 send1 equality
      RGBLedPin_Set                      // H t1
      "nop         \n"                   // H t2
      "nop         \n"                   // H t3
      "nop         \n"                   // H t4
      "nop         \n"                   // H t5
      "nop         \n"                   // H t6
      "nop         \n"                   // H t7
      "nop         \n"                   // H t8
      "nop         \n"                   // H t9
      "nop         \n"                   // H t10
      "nop         \n"                   // H t11
      "nop         \n"                   // H t12
      RGBLedPin_ReSet                    // L t1
      "nop         \n"                   // L t2
      "JRA     L:lb_start_bit6       \n" // L JRA:2 t4
                                         // L NextBitTest:4  t8
      "lb_bit5_Send_0:         \n"       //------Send 0 : 400ns High, 850ns Low (6,14) 375ns,875ns
      RGBLedPin_Set                      // H t1
      "nop         \n"                   // H t2
      "nop         \n"                   // H t3
      "nop         \n"                   // H t4
      "nop         \n"                   // H t5
      "nop         \n"                   // H t6
      RGBLedPin_ReSet                    // L t1
      "nop         \n"                   // L t1
      "nop         \n"                   // L t2
      "nop         \n"                   // L t3
      "nop         \n"                   // L t4
      "nop         \n"                   // L t5
      "nop         \n"                   // L t6
      "nop         \n"                   // L t7
      "nop         \n"                   // L t8
      "nop         \n"                   // L t9
                                         //NextBitTest:4+SP = 5. L t14
                                         //--------------- bit 6 -----------------------------------------
      "lb_start_bit6:          \n"       //
      "LD      A,     (L:LedsArray,X)\n" //1
      "AND     A,       #0x02  \n"       //1
      "JREQ    L:lb_bit6_Send_0      \n" //2 jump to Zero, 1 Stay in One + next nop
      "lb_bit6_Send_1:         \n"
      "nop         \n"                   //1 to have send0 send1 equality
      RGBLedPin_Set                      // H t1
      "nop         \n"                   // H t2
      "nop         \n"                   // H t3
      "nop         \n"                   // H t4
      "nop         \n"                   // H t5
      "nop         \n"                   // H t6
      "nop         \n"                   // H t7
      "nop         \n"                   // H t8
      "nop         \n"                   // H t9
      "nop         \n"                   // H t10
      "nop         \n"                   // H t11
      "nop         \n"                   // H t12
      RGBLedPin_ReSet                    // L t1
      "nop         \n"                   // L t2
      "JRA     L:lb_start_bit7       \n" // L JRA:2 t4
                                         // L NextBitTest:4  t8
      "lb_bit6_Send_0:         \n"       //------Send 0 : 400ns High, 850ns Low (6,14) 375ns,875ns
      RGBLedPin_Set                      // H t1
      "nop         \n"                   // H t2
      "nop         \n"                   // H t3
      "nop         \n"                   // H t4
      "nop         \n"                   // H t5
      "nop         \n"                   // H t6
      RGBLedPin_ReSet                    // L t1
      "nop         \n"                   // L t1
      "nop         \n"                   // L t2
      "nop         \n"                   // L t3
      "nop         \n"                   // L t4
      "nop         \n"                   // L t5
      "nop         \n"                   // L t6
      "nop         \n"                   // L t7
      "nop         \n"                   // L t8
      "nop         \n"                   // L t9
                                         //NextBitTest:4+SP = 5. L t14
                                         //--------------- bit 7 -----------------------------------------
      "lb_start_bit7:          \n"       //
      "LD      A,     (L:LedsArray,X)\n" //1
      "AND     A,       #0x01  \n"       //1
      "JREQ    L:lb_bit7_Send_0      \n" //2 jump to Zero, 1 Stay in One + next nop
      "lb_bit7_Send_1:         \n"
      "nop         \n"                   //1 to have send0 send1 equality
      RGBLedPin_Set                      // H t1
      "nop         \n"                   // H t2
      "nop         \n"                   // H t3
      "nop         \n"                   // H t4
      "nop         \n"                   // H t5
      "nop         \n"                   // H t6
      "nop         \n"                   // H t7
      "nop         \n"                   // H t8
      "nop         \n"                   // H t9
      "nop         \n"                   // H t10
      "nop         \n"                   // H t11
      "nop         \n"                   // H t12
      RGBLedPin_ReSet                    // L t1
      "DECW    Y         \n"             //2
      "JREQ    L:lb_exit       \n"       //1 on loop, 2 jmp to exit
      "JP      L:lb_begin_loop       \n" //5
                                         //
      "lb_bit7_Send_0:         \n"       //------Send 0 : 400ns High, 850ns Low (6,14) 375ns,875ns
      RGBLedPin_Set                      // H t1
      "nop         \n"                   // H t2
      "nop         \n"                   // H t3
      "nop         \n"                   // H t4
      "nop         \n"                   // H t5
      "nop         \n"                   // H t6
      RGBLedPin_ReSet                    // L t1
      "nop         \n"                   // L t1
      "nop         \n"                   // L t2
      "nop         \n"                   // L t3
      "nop         \n"                   // L t4
      "nop         \n"                   // L t5
      "nop         \n"                   // L t6
                                         // L DECW 2, JREQ 1, 2 = 5 =>   t14
                                         //--------------------------------------------------------
                                         //--------------------------------------------------------
      "DECW    Y         \n"             //2
      "JREQ    L:lb_exit       \n"       //1 on loop, 2 jmp to exit
      "JP      L:lb_begin_loop       \n" //5
      "lb_exit:nop");

  __set_interrupt_state(int_state);
}