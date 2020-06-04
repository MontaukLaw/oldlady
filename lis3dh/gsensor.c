/* Includes ------------------------------------------------------------------*/
#include "user_comm.h"

/* Private typedef -----------------------------------------------------------*/
#define MAGIC_NUMBER_2  57.30   //180 / 3.14159

#define MAGIC_NUMBER_1  0.061   //4000/65536 15.68

extern PWMDuty_t pwmDuty;

short pitch = 0;
short roll = 0;

//int8_t pitch;
/* Private define ------------------------------------------------------------*/
#define SC7A20_WHO_AM_I    0x11
#define SC7A20_SLAVE_ID    0x32
/* Private macros ------------------------------------------------------------*/

Pose_t pose = {0,0,0};

/* Private variables ---------------------------------------------------------*/
uint8_t g_acc_error;

extern int8_t Acc_Data[];
/* Private function prototypes -----------------------------------------------*/

static bool GsensorLis3dhInit(void);


/* Exported functions --------------------------------------------------------*/

/**
 * G-sensor初始货品
 */
void GsensorInit(void) {
	//Analog_IIC_Pin_Init();

	if (GsensorLis3dhInit() == FALSE) {
		g_acc_error = 1;
	}

}


/* Private function prototypes -----------------------------------------------*/
/**
 * G sensor LIS3DH初始化
 *
 * @return 初始化成功返回TRUE
 */
static bool GsensorLis3dhInit(void) {
	bool ret = FALSE;
	uint8_t mid = 0;

	LIS3DH_GetWHO_AM_I(&mid);
	if (mid == 0x33) {
		//Inizialize MEMS Sensor
      
		//set ODR (turn ON device)
		LIS3DH_SetODR(LIS3DH_ODR_100Hz);
        
		//set PowerMode
		LIS3DH_SetMode(LIS3DH_NORMAL);
        
		//set Fullscale
		LIS3DH_SetFullScale(LIS3DH_FULLSCALE_2);
        
		//set axis Enable
		LIS3DH_SetAxis(LIS3DH_X_ENABLE | LIS3DH_Y_ENABLE | LIS3DH_Z_ENABLE);

#if 0
		// 设置中断1输出
		LIS3DH_SetInt1Pin(LIS3DH_I1_INT1_ON_PIN_INT1_ENABLE);
		LIS3DH_HPFAOI1Enable(MEMS_ENABLE);

		// 设置中断阀值为160mg
		LIS3DH_SetInt1Threshold(10);

		//set Interrupt configuration (all enabled)
		LIS3DH_SetIntConfiguration(LIS3DH_INT1_ZHIE_ENABLE |
		LIS3DH_INT1_YHIE_ENABLE |
		LIS3DH_INT1_XHIE_ENABLE |
		LIS3DH_INT1_OR);

		// 中断持续时间，单位是ODR的周期
		LIS3DH_SetInt1Duration(0);
#endif
		ret = TRUE;
		
	} else {
		
	}
	return ret;
}

void GetAxisXYZ(void)
{
  LIS3DH_ReadReg(LIS3DH_OUT_X_H, &pose.x);
  
  LIS3DH_ReadReg(LIS3DH_OUT_Y_H, &pose.y);
  
  LIS3DH_ReadReg(LIS3DH_OUT_Z_H, &pose.z);  
}


void CountPitch(void)
{
  short X,Y,Z;
  static short oldPitch = 0;
  static short oldRoll = 0;
  //Debug('-');
  //Debug('\r');
  //Debug('\n');
  //prinfNumber((uint16_t) pose.x);
  
  //prinfNumber((uint16_t) pose.z);
  
  X = (short)((float)(pose.x * 256) * MAGIC_NUMBER_1);
  //X = (short)((float)(pose.x ) * 16);
  
  Z = (short)((float)(pose.z * 256) * MAGIC_NUMBER_1);
  //Z = (short)((float)(pose.z )* 16);
  
  Y = (short)((float)(pose.y * 256) * MAGIC_NUMBER_1);
  
  pitch = (short)(atan2((float)(0-X),Z) * MAGIC_NUMBER_2); 
  roll  = (short)(atan2((float)(Y),Z) * MAGIC_NUMBER_2); 

#if FILTER_ON  
  // 滤波
  if(oldPitch != 0){
    if(abs(oldPitch - pitch) > MIN_PITCH_FILTER){
      pitch = oldPitch;
    }
  }
   
  if(oldRoll != 0){
    if(abs(oldRoll - roll) > MIN_ROLL_FILTER){
      roll = oldRoll;
    }
  }
#endif  

  oldPitch = pitch;
  oldRoll = roll;
  
}

uint16_t GetDuty(uint8_t color)
{
  uint16_t duty;
  if (color > 125)
  {
    duty = (255 - color) * 20;
  }
  else
  {
    duty = color * 20;
  }
  if (duty < 10)
  {
    duty = 10;
  }
  return duty;
}
#define MIN_DUTY_GAP 100

void GetDutyOldStyle(void)
{
  static uint16_t blueDutyNow = 0;
  static uint16_t redDutyNow = 0;
  static uint16_t greenDutyNow = 0;
  
  __IO uint16_t blueDutyTarget = GetDuty(pose.y);
  __IO uint16_t redDutyTarget = GetDuty(pose.x);
  __IO uint16_t greenDutyTarget = GetDuty(pose.z);
  
  if (blueDutyNow < blueDutyTarget)
  {
    if ((blueDutyTarget - blueDutyNow) > MIN_DUTY_GAP)
    {
      blueDutyNow += DUTY_STEP;
    }
  }
  else
  {
    if ((blueDutyNow - blueDutyTarget) > MIN_DUTY_GAP)
    {
      blueDutyNow-= DUTY_STEP;
    }
  }

  if (redDutyNow < redDutyTarget)
  {
    if ((redDutyTarget - redDutyNow) > MIN_DUTY_GAP)
    {
      redDutyNow += DUTY_STEP;
    }
  }
  else
  {
    if ((redDutyNow - redDutyTarget) > MIN_DUTY_GAP)
    {
      redDutyNow -= DUTY_STEP;
    }
  }

  if (greenDutyNow < greenDutyTarget)
  {
    if ((greenDutyTarget - greenDutyNow) > MIN_DUTY_GAP)
    {
      greenDutyNow += DUTY_STEP;
    }
  }
  else
  {
    if ((greenDutyNow - greenDutyTarget) > MIN_DUTY_GAP)
    {
      greenDutyNow -= DUTY_STEP;
    }
  }
  SetBlueDuty(blueDutyNow);
  SetRedDuty(redDutyNow);
  SetGreenDuty(greenDutyNow);

}


void GetDutyDirect(void)
{

  uint8_t tempX = pose.x;
  uint8_t tempY = pose.y;
  uint8_t tempZ = pose.z;
  
  uint8_t oldGreenDuty = 0;

    
  if(tempX < 0x40){    
    pwmDuty.r = (uint16_t) (tempX)  * 40;  
  }else if(tempX > 0xC0){
    pwmDuty.r = ((uint16_t) (0xFF - tempX))*40; 
  }
  
  if(tempX <0x10 || tempX > 0xF0){
    pwmDuty.r = MIN_DUTY;
  }

  
#if 0   
  // x是红色
  if(tempX < 0x10){
    pwmDuty.r = 0;
  }else if(tempX < 0x40){    
    pwmDuty.r =  ((uint16_t) (0x40 - tempX )) * 20;     
  }else if(tempX > 0xF0){
    pwmDuty.r = MAX_DUTY;
  }else if(tempX > 0xC0){    
    pwmDuty.r = ((uint16_t) (0xFF - tempX) ) * 20 + 1250;     
  }

  
  //pwmDuty.b = MAX_DUTY - pwmDuty.r;


  // y是蓝色
  if(tempY < 0x40){    
    pwmDuty.b =  ((uint16_t) tempY ) * 20 + 1250;     
  }else if(tempY > 0xC0){    
    pwmDuty.b = ((uint16_t) ( tempY - 0xC0) ) * 20 ;     
  }  


  
  // z是绿色
  if(tempZ < 0x40){    
    pwmDuty.g =  ((uint16_t) (0x40 - tempZ )) * 20;     
  }else if(tempZ > 0xC0){    
    pwmDuty.g = ((uint16_t) (0xFF - tempZ) ) * 20 + 1250;     
  }
#endif
  

  if(tempZ < 0x40){ 
    //pwmDuty.b = 0;
    //pwmDuty.r = 0;
    if((tempY > 0x10 && tempY < 0x40) || (tempY < 0xE0 && tempY > 0xD0 )){
      pwmDuty.g =  ((uint16_t) (0x40 - tempZ )) * 40;       
    }else{
      //pwmDuty.g = MIN_DUTY;
    }
    if(tempZ > 50){
      pwmDuty.g = MIN_DUTY;
    }
  }  
#if 0
  if(oldGreenDuty != 0){
    if(abs(oldGreenDuty - pwmDuty.g) > 300){
      pwmDuty.g = oldGreenDuty;
    }
  }
  
  oldGreenDuty = pwmDuty.g;

#endif
  
  if(MAX_DUTY > (pwmDuty.r + pwmDuty.g)){
    
    pwmDuty.b = MAX_DUTY - (pwmDuty.r + pwmDuty.g);
    
    if(pwmDuty.b < MIN_DUTY){
      pwmDuty.b = MIN_DUTY; 
    }
  }else{
    pwmDuty.b = MIN_DUTY;   
  }
  
  
  
  //pwmDuty.b = abs(MAX_DUTY - (pwmDuty.r + pwmDuty.g));

  //Debug('x');
  //prinfNumber((uint16_t) tempX);
  //Debug('y');
  //prinfNumber((uint16_t) tempY);
  //Debug('z');
  //prinfNumber((uint16_t) tempZ);
  //prinfNumber(pwmDuty.g);
  //prinfNumber(pwmDuty.r);
  //prinfNumber(pwmDuty.b); 
  
}
