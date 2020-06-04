#include "stm8s.h"
#include "Analog_IIC.h"
/*

 @16M---精确延时到微秒

 */

/*

 模拟IIC引脚方向配置

 */
void Analog_IIC_Pin_Init(void) {
	
  I2C_SDA_STANDARD0_NODRIVE1();

  I2C_SCL_STANDARD0_NODRIVE1();
    
  I2C_SCL_OUTPUT();

  nrf_gpio_cfg_input(G_INT1, NRF_GPIO_PIN_NOPULL);

  //DBG_LOG("analog IIC pin init.");
}

/*

 模拟IIC_SDA引脚方向配置
 参数值  1          输出
 0         输入

 */
void IIC_SDA_Dir(uint8_t d) {
	if (d == 0) {
		I2C_SDA_INPUT();
	} else {
		I2C_SDA_OUTPUT();
	}
}

//产生IIC起始信号
void IIC_Start(void) {
	IIC_SDA_Dir(1); //IIC_SDA线输出
	I2C_SDA_HIGH();     //拉高数据线
	I2C_SCL_HIGH();     //拉高时钟线
	Analog_IIC_Delay(10);
	I2C_SDA_LOW();   //拉低数据线
	Analog_IIC_Delay(10);
	I2C_SCL_LOW();   //拉低时钟线  发送IIC总线开始信号
}

//产生IIC停止信号
void IIC_Stop(void) {
	IIC_SDA_Dir(1); //IIC_SDA线输出
	I2C_SCL_LOW();   //拉低时钟线
	I2C_SDA_LOW();   //拉低数据线
	Analog_IIC_Delay(10);
	I2C_SCL_HIGH();   //拉高时钟线
	Analog_IIC_Delay(10);
	I2C_SDA_HIGH();   //拉高数据线  发送IIC总线停止信号
	Analog_IIC_Delay(10);
}

//产生ACK应答
void IIC_Ack(void) {
	I2C_SCL_LOW();   //拉低时钟线
	IIC_SDA_Dir(1); //IIC_SDA线输出
	I2C_SDA_LOW();   //拉低数据线
	Analog_IIC_Delay(10);
	I2C_SCL_HIGH();   //拉高时钟线
	Analog_IIC_Delay(10);
	I2C_SCL_LOW();   //拉低时钟线
}

//不产生ACK应答
void IIC_NAck(void) {
	I2C_SCL_LOW();   //拉低时钟线
	IIC_SDA_Dir(1); //IIC_SDA线输出
	I2C_SDA_HIGH();   //拉高数据线
	Analog_IIC_Delay(10);
	I2C_SCL_HIGH();   //拉高时钟线
	Analog_IIC_Delay(10);
	I2C_SCL_LOW();   //拉低时钟线
}

//等待应答信号到来
//返回值:1       接收应答失败
//       0          接收应答成功
uint8_t IIC_Wait_Ack(void) {
	uint8_t Wait_TOut_Cnt = 0; //设置等待应答信号超时计数
	IIC_SDA_Dir(0);        //IIC_SDA线输入
	I2C_SDA_HIGH();             //拉高数据线
	Analog_IIC_Delay(10);
	I2C_SCL_HIGH();         //拉高时钟线  等待应答信号
	Analog_IIC_Delay(10);
	while (I2C_SDA_READ()) {
		Wait_TOut_Cnt++;
		if (Wait_TOut_Cnt > 250) {
			IIC_Stop();            //等待应答信号超时  发送IIC总线停止信号
			return 1;
		}
	}
	I2C_SCL_LOW();                 //拉低时钟线  结束应答信号
	return 0;
}

//IIC发送一个字节
void IIC_Write_Byte(uint8_t WByte) {
	uint8_t Wb_Cnt = 0; //写数据位计数
	IIC_SDA_Dir(1); //IIC_SDA线输出
	I2C_SCL_LOW();   //拉低时钟线    开始数据传输
	for (Wb_Cnt = 0; Wb_Cnt < 8; Wb_Cnt++) {
		if (WByte & 0x80) {
			I2C_SDA_HIGH();
		} else {
			I2C_SDA_LOW();
		}
		WByte <<= 1; //数据移位
		Analog_IIC_Delay(10);
		I2C_SCL_HIGH(); //拉高时钟线
		Analog_IIC_Delay(10);
		I2C_SCL_LOW(); //拉低时钟线   准备开始传送数据位
		Analog_IIC_Delay(10);
	}
}

//IIC读取一个字节
//参数值:1       发送Ack
//       0          不发送Ack
uint8_t IIC_Read_Byte(uint8_t SF_Ack) {
	uint8_t Rb_Cnt = 0; //读数据位计数
	uint8_t RByte = 0; //读字节
	IIC_SDA_Dir(0); //SDA设置为输入
	for (Rb_Cnt = 0; Rb_Cnt < 8; Rb_Cnt++) {
		I2C_SCL_LOW(); //拉低时钟线   准备开始传送数据位
		Analog_IIC_Delay(10);
		I2C_SCL_HIGH(); //拉高时钟线
		RByte <<= 1; //数据移位
		if (I2C_SDA_READ()) {
			RByte++;
		}
		Analog_IIC_Delay(10);
	}
	if (!SF_Ack) {    //0    不发送Ack
		IIC_NAck();  //发送NAck
	} else {           //1        发送Ack
		IIC_Ack();   //发送Ack
	}
	return RByte;
}

/*

 写一个字节

 */
void Sensor_Write_Byte(uint8_t RAddr, uint8_t *WData) {
	IIC_Start();                                      //发送IIC起始信号
	IIC_Write_Byte(Sensor_Wr_Addr1);     //发送IIC写地址
	IIC_Wait_Ack();                                   //等待IIC应答信号
	IIC_Write_Byte(RAddr);                //发送IIC寄存器地址
	IIC_Wait_Ack();                               //等待IIC应答信号
	IIC_Write_Byte(*WData);           //发送写入寄存器的数据
	IIC_Wait_Ack();                       //等待IIC应答信号
	IIC_Stop();                                           //发送IIC停止信号
}

/*

 写N个字节

 */
void Sensor_Write_NByte(uint8_t RAddr, uint8_t *WData, uint8_t WLen) {
	uint8_t WB_Cnt = 0;
	IIC_Start();                                      //发送IIC起始信号
	IIC_Write_Byte(Sensor_Wr_Addr1); //发送IIC写地址
	IIC_Wait_Ack();                                   //等待IIC应答信号
	IIC_Write_Byte(RAddr);                //发送IIC寄存器地址
	IIC_Wait_Ack();                               //等待IIC应答信号
	for (WB_Cnt = 0; WB_Cnt < WLen; WB_Cnt++) {
		IIC_Write_Byte(WData[WB_Cnt]); //连续读取寄存器的数据      等待应答信号
		IIC_Wait_Ack();                         //等待IIC应答信号
	}
	IIC_Stop();                                           //发送IIC停止信号
}

/*

 读一个字节

 */
void Sensor_Read_Byte(uint8_t RAddr, uint8_t *RData) {
	IIC_Start();                                      //发送IIC起始信号
	IIC_Write_Byte(Sensor_Wr_Addr1); //发送IIC写地址
	IIC_Wait_Ack();                               //等待IIC应答信号
	IIC_Write_Byte(RAddr);                //发送IIC寄存器地址
	IIC_Wait_Ack();                               //等待IIC应答信号
	IIC_Start();                                  //发送IIC起始信号
	IIC_Write_Byte(Sensor_Rd_Addr1); //发送IIC读地址
	IIC_Wait_Ack();                                   //等待IIC应答信号
	*RData = IIC_Read_Byte(0);            //读取寄存器的数据
	IIC_Stop();                                           //发送IIC停止信号
}

/*

 读N个字节

 */
void Sensor_Read_NByte(uint8_t RAddr, uint8_t *RData, uint8_t RLen) {
	uint8_t RB_Cnt = 0;                                       //读字节计数
	IIC_Start();                                         //发送IIC起始信号
	IIC_Write_Byte(Sensor_Wr_Addr1);   //发送IIC写地址
	IIC_Wait_Ack();                                      //等待IIC应答信号
	IIC_Write_Byte(RAddr);                         //发送IIC寄存器地址
	IIC_Wait_Ack();                                    //等待IIC应答信号
	IIC_Start();                                       //发送IIC起始信号
	IIC_Write_Byte(Sensor_Rd_Addr1);   //发送IIC读地址
	IIC_Wait_Ack();                                        //等待IIC应答信号
	for (RB_Cnt = 0; RB_Cnt < (RLen - 1); RB_Cnt++) {
		RData[RB_Cnt] = IIC_Read_Byte(1); //连续读取寄存器的数据       等待应答信号
	}
	RData[RB_Cnt] = IIC_Read_Byte(0);  //读取寄存器最后一个数据	不等待应答信号
	IIC_Stop();                                            //发送IIC停止信号
}

