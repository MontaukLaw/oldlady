#include "user_comm.h"

uint8_t SC7A20_Msg = 8;
// 0x77 400hz
// 0x57 100hz
//uint8_t SC7A20_REG[10] = { 0x2f, 0x04, 0x98, 0x05, 0x08, 0x02, 0x05, 0x01, 0x15, 0x80 };
uint8_t SC7A20_REG[10] = { 0x57, 0x04, 0x98, 0x05, 0x08, 0x02, 0x05, 0x01, 0x15, 0x80 };
//uint8_t SC7A20_REG[10] = { 0x27, 0x04, 0x98, 0x05, 0x08, 0x02, 0x05, 0x01, 0x15, 0x80 };
int8_t Acc_Data[3];
uint8_t accIntArr[2]={100,50};

unsigned char X_H,X_L,Y_H,Y_L,Z_H, Z_L; //三轴数据（高、低位）
signed short int SL_ACCEL_X,SL_ACCEL_Y,SL_ACCEL_Z; //三轴数据

extern Pose_t pose;

void G_Sensor_SC7A20_Init(void) {
	uint8_t temp1;

#if 0
	Sensor_Read_Byte(CHIPID, &temp1);
	//DBG_LOG("Chip_ID = %x\r\n", temp1);

	//DBG_LOG("G_Sensor_SC7A20_Init \r\n");
	if (temp1 != 0x11) // I2C address fixed --> 读取系统ID，如果异常就需要重新写入原厂数据了
    {
		//DBG_LOG("Error\r\n");
	}
#endif
    
	/*click */
    Sensor_Write_Byte(0x20, &SC7A20_REG[0]);  //odr 10Hz 开启低功耗  
	Sensor_Write_Byte(0x21, &SC7A20_REG[1]);  //fds -->开启高通滤波器(滤掉地球G)(一定要开启，否则阈值要超过1G，而且动作也要超过1G)
	Sensor_Write_Byte(0x23, &SC7A20_REG[2]);  //range bdu  0x20--0xA8
    
	//SDO 接地
	Sensor_Write_Byte(0x1e, &SC7A20_REG[3]);  //开启控制开关
	Sensor_Write_Byte(0x57, &SC7A20_REG[4]);  //关闭SDO管脚上的上拉电阻

	//Sensor_Write_Byte(0x25, &SC7A20_REG[5]); //selects active level low for pin INT 正常是高电平，有效的时候是低电平
	Sensor_Write_Byte(0x25, &SC7A20_REG[7]); //selects active level low for pin INT 正常是高电平，有效的时候是低电平
	//Sensor_Write_Byte(0x3a, &accIntArr[1]);    //设定中断阈值(触发阈值)
	//Sensor_Write_Byte(0x3b, &SC7A20_REG[6]);

	//Sensor_Write_Byte(0x3c, &SC7A20_REG[7]);
	//Sensor_Write_Byte(0x38, &SC7A20_REG[8]); //前一次中断和后一次中断的保持时间(1就是保持1个ODR，2就是2个ODR(比如10HZ，2就是每次中断保持200mS，200mS期间的中断不响应))
	//Sensor_Write_Byte(0x22, &SC7A20_REG[9]);
    
    //read_acceler_data();
    //DBG_LOG("%x %x %x",Acc_Data[0],Acc_Data[1],Acc_Data[2]);
    
//	g_sensor_write(G_SlaveAddr,0x20,0x2f);  //odr
//	g_sensor_write(G_SlaveAddr,0x21,0x04);  //fds -->开启高通滤波器(滤掉地球G)(一定要开启，否则阈值要超过1G，而且动作也要超过1G)
//	g_sensor_write(G_SlaveAddr,0x23,0x98);  //range bdu  0x20--0xA8
//        
//        //SDO 接地
//        g_sensor_write(G_SlaveAddr,0x1e,0x05);  //开启控制开关
//        g_sensor_write(G_SlaveAddr,0x57,0x08);  //关闭SDO管脚上的上拉电阻

	/*AOI*/
//	g_sensor_write(G_SlaveAddr,0x20,0x2f);  //设置odr
//	g_sensor_write(G_SlaveAddr,0x23,0x98);  //设置量程range bdu  0x20--0xA8
}

/*read accelertion data */
uint8_t read_acceler_data() {
    //uint8_t read_acceler_data(int8_t *buf) {
	uint8_t i;
	uint8_t cd[6];
	for (i = 0; i < 6; i++) {
		Sensor_Read_Byte(0x28 + i, &cd[i]);
	}
	Acc_Data[0] = cd[1];
	Acc_Data[1] = cd[3];
	Acc_Data[2] = cd[5];

	return 0;
}

/*read accelertion data , only X and Y axis*/
uint8_t Read_XY_Data(int8_t *buf) {
	uint8_t cd[2];
	Sensor_Read_Byte(0x28 + 1, &cd[0]);
	Sensor_Read_Byte(0x28 + 3, &cd[1]);
	buf[0] = cd[0];
	buf[1] = cd[1];
	return 0;
}

int8_t AccData8Bit[6];
uint8_t ReadXYZdata(void) {
    //uint8_t read_acceler_data(int8_t *buf) {
	uint8_t i;
	uint8_t cd[6];
	for (i = 0; i < 6; i++) {
		Sensor_Read_Byte(0x28 + i, &cd[i]);
	}
    //pose.x = (uint16_t)()
    //if() 
 	for (i = 0; i < 6; i++) {
	    AccData8Bit[i] = cd[i];
	} 
    
    SL_ACCEL_X = (signed short int)((cd[1]<< 8) | cd[0]); //拼接数据
    SL_ACCEL_Y = (signed short int)((cd[3]<< 8) | cd[2]); //强制数据类型转换
    SL_ACCEL_Z = (signed short int) ((cd[5]<< 8) | cd[4]); //16 位带符号整型数据
    SL_ACCEL_X = SL_ACCEL_X >> 6; //取 10 位带符号整型数据，Y,Z 同理
    
    //SL_ACCEL_X = SL_ACCEL_X + 126;
    //pose.x = (uint8_t)SL_ACCEL_X;

    SL_ACCEL_Y = SL_ACCEL_Y >> 6; //取 10 位带符号整型数据，Y,Z 同理
    //SL_ACCEL_Y = SL_ACCEL_Y + 126;
    //pose.y = (uint8_t)SL_ACCEL_Y;
    
    SL_ACCEL_Z = SL_ACCEL_Z >> 6; //取 10 位带符号整型数据，Y,Z 同理
    //SL_ACCEL_Z = SL_ACCEL_Z + 126;
    //pose.z = (uint8_t)SL_ACCEL_Z;
    
	return 0;
}