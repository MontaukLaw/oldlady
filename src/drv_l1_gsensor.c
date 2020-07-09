#include "includes.h"

#define SC7A20_SLAVE_ID    0x32//0x4E ,erichan 20150529
#define DMARD07_SLAVE_ID   0x38

#if (USE_G_SENSOR_NAME == G_SENSOR_SC7A20)
#define G_SlaveAddr SC7A20_SLAVE_ID
#elif (USE_G_SENSOR_NAME == G_SENSOR_DMARD07)
	#define G_SlaveAddr DMARD07_SLAVE_ID
#endif
static uint8_t g_level;

static OS_EVENT *sw_i2c_sem = NULL;

static uint8_t G_sensor_status = 0;

//====================================================================================================
void I2C_delay(uint16_t i) {
	uint16_t j;

	for (j = 0; j < (i << 8); j++)
		i = i;
}

void g_delay(uint16_t i) {
	uint16_t k;

	for (k = 0; k < i; k++)
		I2C_delay(100);
}

//============================================
void I2C_start(void) {
	gpio_write_io(I2C_SCL, DATA_HIGH);					//SCL1
	I2C_delay(1);
	gpio_write_io(I2C_SDA, DATA_HIGH);					//SDA1
	I2C_delay(1);
	gpio_write_io(I2C_SDA, DATA_LOW);					//SDA0
	I2C_delay(1);
}
//===================================================================
void I2C_stop(void) {
	I2C_delay(1);
	gpio_write_io(I2C_SDA, DATA_LOW);					//SDA0
	I2C_delay(1);
	gpio_write_io(I2C_SCL, DATA_HIGH);					//SCL1
	I2C_delay(1);
	gpio_write_io(I2C_SDA, DATA_HIGH);					//SDA1
	I2C_delay(1);
}

//===================================================================
void I2C_w_phase(uint16_t value) {
	uint16_t i;

	for (i = 0; i < 8; i++) {
		gpio_write_io(I2C_SCL, DATA_LOW);					//SCL0
		I2C_delay(1);
		if (value & 0x80)
			gpio_write_io(I2C_SDA, DATA_HIGH);				//SDA1
		else
			gpio_write_io(I2C_SDA, DATA_LOW);				//SDA0
//		sccb_delay (1);
		gpio_write_io(I2C_SCL, DATA_HIGH);					//SCL1
		I2C_delay(1);
		value <<= 1;
	}
	// The 9th bit transmission
	gpio_write_io(I2C_SCL, DATA_LOW);						//SCL0
	gpio_init_io(I2C_SDA, GPIO_INPUT);						//SDA is Hi-Z mode
	I2C_delay(1);
	gpio_write_io(I2C_SCL, DATA_HIGH);						//SCL1
	I2C_delay(1);
	gpio_write_io(I2C_SCL, DATA_LOW);						//SCL0
	gpio_init_io(I2C_SDA, GPIO_OUTPUT);					//SDA is Hi-Z mode
}

//===================================================================
uint16_t I2C_r_phase(void) {
	uint16_t i;
	uint16_t data;

	gpio_init_io(I2C_SDA, GPIO_INPUT);						//SDA is Hi-Z mode
	data = 0x00;
	for (i = 0; i < 8; i++) {
		gpio_write_io(I2C_SCL, DATA_LOW);					//SCL0
		I2C_delay(1);
		gpio_write_io(I2C_SCL, DATA_HIGH);					//SCL1
		data <<= 1;
		data |= (gpio_read_io(I2C_SDA));
		I2C_delay(1);
	}
	// The 9th bit transmission
	gpio_write_io(I2C_SCL, DATA_LOW);						//SCL0
	gpio_init_io(I2C_SDA, GPIO_OUTPUT);					//SDA is output mode
	gpio_write_io(I2C_SDA, DATA_HIGH);						//SDA0, the nighth bit is NA must be 1
	I2C_delay(1);
	gpio_write_io(I2C_SCL, DATA_HIGH);						//SCL1
	I2C_delay(1);
	gpio_write_io(I2C_SCL, DATA_LOW);						//SCL0
	return data;
}

//====================================================================================================
void I2C_gpio_init(INT32U nSCL,			// Clock Port No
		INT32U nSDA				// Data Port No
		) {
	//init IO
	gpio_set_port_attribute(nSCL, ATTRIBUTE_HIGH);
	gpio_set_port_attribute(nSDA, ATTRIBUTE_HIGH);
	gpio_init_io(nSCL, GPIO_OUTPUT);				//set dir
	gpio_init_io(nSDA, GPIO_OUTPUT);				//set dir
	gpio_write_io(nSCL, DATA_HIGH);					//SCL1
	gpio_write_io(nSDA, DATA_HIGH);					//SDA0
}

void g_sensor_write(uint8_t id, uint8_t addr, uint8_t data) {
	// 3-Phase write transmission cycle is starting now ...
	I2C_start();									// Transmission start
	I2C_w_phase(id);								// Phase 1: Device ID
	I2C_w_phase(addr);								// Phase 2: Register address. High pass filter enable
	I2C_w_phase(data);								// Phase 3: Data value
	I2C_stop();									// Transmission stop
}

uint16_t g_sensor_read(uint8_t id, uint8_t addr) {
	uint16_t redata;

	I2C_start();									// Transmission start
	I2C_w_phase(id);								// Phase 1: Device ID
	I2C_w_phase(addr);								// Phase 2: Register address. Transient source
	I2C_start();									// Transmission start
	I2C_w_phase(id | 0x01);						// Phase 1 (read)
	redata = (uint8_t) I2C_r_phase();				// Phase 2
	I2C_stop();									// Transmission stop

	return redata;
}

INT32U G_Get_ACC_Data(uint8_t addr, uint8_t Num) {
	uint16_t Redata, temp;

	if (Num == 1) {

		Redata = g_sensor_read(G_SlaveAddr, addr);

	} else {
		temp = g_sensor_read(G_SlaveAddr, addr);
		Redata = g_sensor_read(G_SlaveAddr, addr + 1);

		Redata = (Redata << 8) | (temp & 0x00ff);
	}

	return Redata;
}

void sw_i2c_lock(void) {
	uint8_t err;

	OSSemPend(sw_i2c_sem, 0, &err);
}

void sw_i2c_unlock(void) {
	OSSemPost(sw_i2c_sem);
}

uint8_t ap_gsensor_power_on_get_status(void) {
	return G_sensor_status;
}

void ap_gsensor_power_on_set_status(uint8_t status) {
	if (status)
		G_sensor_status = 1;  //gsensor power on
	else
		G_sensor_status = 0;  //key power on
}

void G_Sensor_SC7A20_Init(uint8_t level) {
	uint8_t temp1;

	uint8_t MTPSETTING, B57H, B1BH, i;

	MTPSETTING = 0x07;
	B57H = 0x00;
	B1BH = 0x08;  //for iicaddr=0x3a

	temp1 = g_sensor_read(G_SlaveAddr, CHIPID);
	DBG_LOG("chip_id = %x\r\n", temp1); DBG_LOG("G_Sensor_SC7A20_Init \r\n");
	if (temp1 != 0x11)  // I2C address fixed
			{
		for (i = 0; i < 3; i++) {
			g_sensor_write(0x30, 0x59, MTPSETTING);
			g_sensor_write(0x30, 0x1e, 0x05);
			g_sensor_write(0x30, 0x1b, B1BH);
			g_sensor_write(0x30, 0x57, B57H);

			g_sensor_write(0x32, 0x59, MTPSETTING);
			g_sensor_write(0x32, 0x1e, 0x05);
			g_sensor_write(0x32, 0x1b, B1BH);
			g_sensor_write(0x32, 0x57, B57H);

			g_sensor_write(0x34, 0x59, MTPSETTING);
			g_sensor_write(0x34, 0x1e, 0x05);
			g_sensor_write(0x34, 0x1b, B1BH);
			g_sensor_write(0x34, 0x57, B57H);

			g_sensor_write(0x36, 0x59, MTPSETTING);
			g_sensor_write(0x36, 0x1e, 0x05);
			g_sensor_write(0x36, 0x1b, B1BH);
			g_sensor_write(0x36, 0x57, B57H);

			g_sensor_write(0x38, 0x59, MTPSETTING);
			g_sensor_write(0x38, 0x1e, 0x05);
			g_sensor_write(0x38, 0x1b, B1BH);
			g_sensor_write(0x38, 0x57, B57H);

			g_sensor_write(0x3a, 0x59, MTPSETTING);
			g_sensor_write(0x3a, 0x1e, 0x05);
			g_sensor_write(0x3a, 0x1b, B1BH);
			g_sensor_write(0x3a, 0x57, B57H);

			g_sensor_write(0x3c, 0x59, MTPSETTING);
			g_sensor_write(0x3c, 0x1e, 0x05);
			g_sensor_write(0x3c, 0x1b, B1BH);
			g_sensor_write(0x3c, 0x57, B57H);

			g_sensor_write(0x3e, 0x59, MTPSETTING);
			g_sensor_write(0x3e, 0x1e, 0x05);
			g_sensor_write(0x3e, 0x1b, B1BH);
			g_sensor_write(0x3e, 0x57, B57H);
		}

		g_sensor_write(IICADDR, 0x1e, 0x05);	   //to comment

	}

	//g_sensor_write(0x32,0x24,0x80);
	//---------------------------------------------------
	temp1 = g_sensor_read(G_SlaveAddr, 0x39);
	if (temp1 & 0x40) //active int flag
			{
		ap_gsensor_power_on_set_status(1);
		DBG_PRINT("==========gsensor power on=========\r\n");
	} else {
		ap_gsensor_power_on_set_status(0);
		DBG_PRINT("==========key power on=============\r\n");
	}

	g_sensor_write(G_SlaveAddr, 0x22, 0x00);
	g_sensor_write(G_SlaveAddr, 0x38, 0x00);
	g_sensor_write(G_SlaveAddr, 0x24, 0x80);
	/*temp1 = g_sensor_read(G_SlaveAddr,0x57);
	 if(!(temp1 & 0x04))
	 {
	 g_sensor_write(G_SlaveAddr,0x1e,0x05);
	 temp1 |= 0x04;
	 g_sensor_write(G_SlaveAddr,0x57,temp1);
	 g_sensor_write(G_SlaveAddr,0x1e,0x15);
	 g_delay(500);
	 }*/
	//else
	//g_sensor_write(G_SlaveAddr,0x1e,0x00);
	g_sensor_write(G_SlaveAddr, 0x20, 0x37);
	g_sensor_read(G_SlaveAddr, 0x26);		//
	g_sensor_write(G_SlaveAddr, 0x21, 0x0d);
	g_sensor_write(G_SlaveAddr, 0x23, 0x80);

}

void SC7A20_Enter_Interrupt_WakeUp_Mode(uint8_t level) {

	I2C_gpio_init(I2C_SCL, I2C_SDA);
	DBG_PRINT("interupt wake up mode level=%d!\r\n", level);

#if USE_G_SENSOR_ACTIVE ==0
	g_sensor_write(G_SlaveAddr, 0x25, 0x02); //selects active level low for pin INT
#else
	g_sensor_write(G_SlaveAddr,0x25,0x00);   //selects active level high for pin INT
#endif

	switch (level) {
	case 0:   //doesn't map to INT
		g_sensor_write(G_SlaveAddr, 0x38, 0x00);
		g_sensor_write(G_SlaveAddr, 0x22, 0x00);
		return;
		break;
	case 1:   //low
		g_sensor_write(G_SlaveAddr, 0x3a, 40);
		break;
	case 2:   //mid
		g_sensor_write(G_SlaveAddr, 0x3a, 20);
		break;
	case 3:   //high
		g_sensor_write(G_SlaveAddr, 0x3a, 10);
		break;
	}
	g_sensor_write(G_SlaveAddr, 0x3b, 0x7f);
	g_sensor_write(G_SlaveAddr, 0x3c, 0x6a);
	g_sensor_write(G_SlaveAddr, 0x38, 0x15);
	g_sensor_write(G_SlaveAddr, 0x22, 0x80);

}

void ap_gsensor_set_sensitive(uint8_t Gsensor) {
	DBG_PRINT("ap_gsensor_set_sensitive level=%d\r\n", Gsensor);
	sw_i2c_lock();

#if (USE_G_SENSOR_NAME == G_SENSOR_SC7A20)
	switch (Gsensor) {
	case 0:   //doesn't map to INT
		g_sensor_write(G_SlaveAddr, 0x38, 0x00);
		g_sensor_write(G_SlaveAddr, 0x22, 0x00);
		g_level = 0xff;
		break;
	case 1:   //low
		g_sensor_write(G_SlaveAddr, 0x3a, 10);
		g_level = 30;
		break;
	case 2:   //mid
		g_sensor_write(G_SlaveAddr, 0x3a, 20);
		g_level = 50;
		break;
	case 3:   //high
		g_sensor_write(G_SlaveAddr, 0x3a, 40);
		g_level = 70;
		break;
	default:

		break;
	}
	// g_sensor_write(G_SlaveAddr,INT_LATCH, 0x81);	//clear gsensor interrupt flag

#elif (USE_G_SENSOR_NAME == G_SENSOR_DMARD07)

  switch(Gsensor)
	{
		case 1://low	
			g_sensor_write(G_SlaveAddr,DMT_ARD07_CTRL_REG_7,DMT_ARD07_LOW);
			break;
		case 2: 
			g_sensor_write(G_SlaveAddr,DMT_ARD07_CTRL_REG_7, DMT_ARD07_MID);
			break;
		case 3:
			g_sensor_write(G_SlaveAddr,DMT_ARD07_CTRL_REG_7,DMT_ARD07_HIGH);
			break;
	}

#endif

	sw_i2c_unlock();
}

uint16_t G_sensor_get_int_active(void) {
	uint16_t temp = 0x00;
	INT8S x, y, z = 0;
	sw_i2c_lock();
#if (USE_G_SENSOR_NAME == G_SENSOR_SC7A20)
	{
		x = g_sensor_read(G_SlaveAddr, 0x29); //erichan 20150529
		y = g_sensor_read(G_SlaveAddr, 0x2b); //erichan 20150529
		z = g_sensor_read(G_SlaveAddr, 0x2d); //erichan 20150529
		DBG_PRINT("abs(x)=%d,abs(y)=%d,abs(z)=%d\r\n", abs(x), abs(y), abs(z));
		if ((abs(x) > g_level) || (abs(y) > g_level) || (abs(z) > g_level))
			temp = 0x40;
	}

#elif (USE_G_SENSOR_NAME == G_SENSOR_DMARD07)
	
	temp = g_sensor_read(G_SlaveAddr,DMT_ARD07_CTRL_REG_6);
#endif	
	sw_i2c_unlock();
	return temp;
}
void G_sensor_clear_int_flag(void) {
	sw_i2c_lock();
#if (USE_G_SENSOR_NAME == G_SENSOR_DMARD07)

	g_sensor_read(G_SlaveAddr, DMT_ARD07_CTRL_REG_6);

#endif	   

	sw_i2c_unlock();
}

//=======================================================================
void G_Sensor_DMARD07_Init(uint8_t level) {

	INT32U ReadValue;
	uint8_t i;

	ReadValue = 0;
	ReadValue = g_sensor_read(G_SlaveAddr, 0x0F);
	DBG_PRINT("G sensor Who am I = %02X\r\n", ReadValue);

	if (ReadValue != 0x07) {
		for (i = 0; i < 5; i++) {
			ReadValue = g_sensor_read(G_SlaveAddr, 0x0F);
			DBG_PRINT("G sensor Who am I = %02X\r\n", ReadValue);
			if (ReadValue == 0x07) {
				break;
			}
		}
		if (i == 5) {
			DBG_PRINT("no G sensor\r\n");
		}
	}

	ReadValue = 0;
	ReadValue = g_sensor_read(G_SlaveAddr, DMT_ARD07_CTRL_REG_6);
	if ((ReadValue) && (ReadValue != 0xff)) {
		ap_gsensor_power_on_set_status(1);
		DBG_PRINT("==========gsensor power on=========\r\n");
	} else {
		ap_gsensor_power_on_set_status(0);
		DBG_PRINT("==========key power on=============\r\n");
	}

	ReadValue = 0;
	ReadValue = g_sensor_read(G_SlaveAddr, 0x53);
	DBG_PRINT("G sensor sw reset = %02X\r\n", ReadValue);

	ReadValue = 0;
	ReadValue = g_sensor_read(G_SlaveAddr, 0x0F);
	DBG_PRINT("G sensor Who am I = %02X\r\n", ReadValue);

	// init setting	
	g_sensor_write(G_SlaveAddr, DMT_ARD07_CTRL_REG_1, 0x27); // Normal Power:342Hz, XYZ enable

	//g_sensor_write(G_SlaveAddr,DMT_ARD07_CTRL_REG_1, 0x47);// Low Power:32Hz, XYZ enable 

	g_sensor_write(G_SlaveAddr, DMT_ARD07_CTRL_REG_2, 0x24); // 2G mode, High Pass Filter for INT1, Low pass filter for data

	//g_sensor_write(G_SlaveAddr,DMT_ARD07_CTRL_REG_2, 0x14);// 2G mode, High Pass Filter for INT1, High pass filter for data
	//g_sensor_write(G_SlaveAddr,DMT_ARD07_CTRL_REG_2, 0x10);// 2G mode, Low Pass Filter for INT1, High pass filter for data

	g_sensor_write(G_SlaveAddr, DMT_ARD07_CTRL_REG_3, 0x00);	//  High-pass Filter Cutoff for 0.6 Hz

	g_sensor_write(G_SlaveAddr, DMT_ARD07_CTRL_REG_4, 0x2c);	// No latch, INT SRC1 enable, active 1

	g_sensor_write(G_SlaveAddr, DMT_ARD07_CTRL_REG_9, 0x00);

	g_sensor_write(G_SlaveAddr, DMT_ARD07_CTRL_REG_5, 0x2A);

	g_sensor_write(G_SlaveAddr, DMT_ARD07_CTRL_REG_5, 0x2A);

	g_sensor_write(G_SlaveAddr, DMT_ARD07_CTRL_REG_7, 0x10);	// 0x30 Threshold = 755.9 mg

	switch (level) {
	case 1:	//low
		g_sensor_write(G_SlaveAddr, DMT_ARD07_CTRL_REG_7, DMT_ARD07_LOW);
		break;
	case 2:
		g_sensor_write(G_SlaveAddr, DMT_ARD07_CTRL_REG_7, DMT_ARD07_MID);
		break;
	case 3:
		g_sensor_write(G_SlaveAddr, DMT_ARD07_CTRL_REG_7, DMT_ARD07_HIGH);
		break;
	}

	g_sensor_write(G_SlaveAddr, DMT_ARD07_CTRL_REG_8, 0x08);	//  Duration = 47.1 ms

}

void DMARD07_Enter_Interrupt_WakeUp_Mode(uint8_t level) {
	INT32U ReadValue;

	ReadValue = g_sensor_read(G_SlaveAddr, 0x53);
	g_sensor_write(G_SlaveAddr, DMT_ARD07_CTRL_REG_1, 0x27); // normal mode, enable interrupt,data rate 342HZ
	g_sensor_write(G_SlaveAddr, DMT_ARD07_CTRL_REG_2, 0x24); // +-2g data low filter,int 1 source hight filter
	g_sensor_write(G_SlaveAddr, DMT_ARD07_CTRL_REG_3, 0x00);
	g_sensor_write(G_SlaveAddr, DMT_ARD07_CTRL_REG_4, 0x64); //ativity high ,latch  SRC1, SRC2 latch disable, int pin to SRC1
	g_sensor_write(G_SlaveAddr, DMT_ARD07_CTRL_REG_9, 0x00); //auto  Awake function disable

	switch (level) {
	case 1: //low
		g_sensor_write(G_SlaveAddr, DMT_ARD07_CTRL_REG_7, DMT_ARD07_LOW);
		break;
	case 2:
		g_sensor_write(G_SlaveAddr, DMT_ARD07_CTRL_REG_7, DMT_ARD07_MID);
		break;
	case 3:
		g_sensor_write(G_SlaveAddr, DMT_ARD07_CTRL_REG_7, DMT_ARD07_HIGH);
		break;
	}
	if (level)
		g_sensor_write(G_SlaveAddr, DMT_ARD07_CTRL_REG_4, 0x64); //map to INT pin
	else
		g_sensor_write(G_SlaveAddr, DMT_ARD07_CTRL_REG_4, 0x2c); //doesn't map to INT

	g_sensor_write(G_SlaveAddr, DMT_ARD07_CTRL_REG_8, 0x04);
	g_sensor_write(G_SlaveAddr, DMT_ARD07_CTRL_REG_5, 0xea);
	ReadValue = g_sensor_read(G_SlaveAddr, DMT_ARD07_CTRL_REG_4);

	DBG_PRINT("end reg[0x47]=%x\n", ReadValue);
}

void G_Sensor_Init(uint8_t level) {
	if (sw_i2c_sem == NULL)
		sw_i2c_sem = OSSemCreate(1);

	G_sensor_status = 0;
	I2C_gpio_init(I2C_SCL, I2C_SDA);

#if (USE_G_SENSOR_NAME == G_SENSOR_SC7A20)
	G_Sensor_SC7A20_Init(level);
#elif (USE_G_SENSOR_NAME == G_SENSOR_DMARD07)
			G_Sensor_DMARD07_Init(level);
	#endif

}

void G_Sensor_park_mode_init(uint8_t level) {
	I2C_gpio_init(I2C_SCL, I2C_SDA);

#if (USE_G_SENSOR_NAME == G_SENSOR_SC7A20)

	SC7A20_Enter_Interrupt_WakeUp_Mode(level);

#elif (USE_G_SENSOR_NAME == G_SENSOR_DMARD07)
	
	   DMARD07_Enter_Interrupt_WakeUp_Mode(level);
		
	#endif

}
