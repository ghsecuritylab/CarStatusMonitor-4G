#include <stm32f4xx_hal.h>
 #include <stdbool.h>
 #include "mpu9250_driver.h" 
 #include "osprintf.h"
#include "os_i2c.h"
#define OPT3001_RESULT			0x00
#define OPT3001_CONFIGURATION	0x01
#define OPT3001_LOW_LIMIT		0x02
#define OPT3001_HIGH_LIMIT		0x03
#define OPT3001_MANUFACTURER_ID	0x7e
#define OPT3001_DEVICE_ID		0x7f
#define OPT_ADDR   				0x8A

static const uint16_t OPT3001_CFG_FC_1    = 0x0000;
static const uint16_t OPT3001_CFG_FC_2    = 0x0100;
static const uint16_t OPT3001_CFG_FC_4    = 0x0200;
static const uint16_t OPT3001_CFG_FC_8    = 0x0300;
static const uint16_t OPT3001_CFG_MASK    = 0x0400;
static const uint16_t OPT3001_CFG_POLPOS  = 0x0800;
static const uint16_t OPT3001_CFG_LATCH   = 0x1000;
static const uint16_t OPT3001_CFG_FLAGL   = 0x2000;
static const uint16_t OPT3001_CFG_FLAGH   = 0x4000;
static const uint16_t OPT3001_CFG_CRF     = 0x8000;
static const uint16_t OPT3001_CFG_OVF     = 0x0001;
static const uint16_t OPT3001_CFG_SHDN    = 0x0000;
static const uint16_t OPT3001_CFG_SHOT    = 0x0002;
static const uint16_t OPT3001_CFG_CONT    = 0x0004;
static const uint16_t OPT3001_CFG_100MS   = 0x0000;
static const uint16_t OPT3001_CFG_800MS   = 0x0008;
static const uint16_t OPT3001_CFG_RNAUTO  = 0x00C0;

static const uint16_t OPT3001_CFG = (OPT3001_CFG_FC_1 | OPT3001_CFG_SHOT | OPT3001_CFG_100MS | OPT3001_CFG_RNAUTO );
static const uint16_t OPT3001_CFG_DEFAULT = 0x10C8;

static const uint8_t OPT3001_REG_RESULT         = 0x00;
static const uint8_t OPT3001_REG_CONFIG         = 0x01;
static const uint8_t OPT3001_REG_MAN_ID         = 0x7E;
static const uint8_t OPT3001_REG_CHIP_ID        = 0x7F;

static const uint16_t OPT3001_MAN_ID            = 0x5449;
static const uint16_t OPT3001_CHIP_ID           = 0x3001;
static const uint16_t OPT3001_REG_CONFIG_MASK   = 0xFE1F;

extern I2C_HandleTypeDef hi2c2;

#define    OPT_I2C_Handle  hi2c2

#define I2C_Timeout  0x100
//uint8_t i2c_smbus_read_word_swapped(uint8_t i2c_addr, uint16_t cmd)
//{
//	uint8_t reg_data;
//	Sensors_I2C_ReadRegister(i2c_addr<<1,cmd,1,&reg_data);
//	return reg_data;
//	
//}
// int i2c_smbus_write_word_swapped(uint8_t i2c_addr, uint16_t reg_addr,uint16_t reg_data)
// {
//	  return  Sensors_I2C_WriteRegister(i2c_addr<<1, reg_addr,2,(unsigned char*)&reg_data );

// }
//uint16_t opt3001_read(uint16_t reg_addr )
// {
//	 	uint16_t reg_data;
//	HAL_StatusTypeDef status = HAL_OK;

//	 //	Sensors_I2C_ReadRegister(OPT_ADDR,reg_addr,2,(uint8_t*)&reg_data);
//	  status = HAL_I2C_Mem_Read( &OPT_I2C_Handle, OPT_ADDR, ( uint16_t )reg_addr, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&reg_data, 2,I2C_Timeout );
//	//status =	os_I2C_ReadReg(OPT_ADDR, reg_addr, 2,(uint8_t*)&reg_data) ;
//	return reg_data;
// }

// void opt3001_write(uint8_t reg_addr,uint16_t reg_data)
// {
//	 	HAL_StatusTypeDef status = HAL_OK;
//		

//	 	//status =   os_I2C_WriteReg(OPT_ADDR, reg_addr,2,(uint8_t*)&reg_data );

// 	 	  status = HAL_I2C_Mem_Write( &OPT_I2C_Handle, OPT_ADDR, ( uint16_t )reg_addr, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&reg_data, 2,I2C_Timeout );

//	  
// }
 uint16_t opt3001_read(uint16_t reg_addr )
 {
	 	uint16_t reg_data;
	HAL_StatusTypeDef status = HAL_OK;

	 //	Sensors_I2C_ReadRegister(0x45<<1,reg_addr,2,(uint8_t*)&reg_data);
	 //  status = HAL_I2C_Mem_Read( &OPT_I2C_Handle, OPT_ADDR<<1, ( uint16_t )reg_addr, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&reg_data, 2,I2C_Timeout );
	status =	Sensors_I2C_ReadRegister(OPT_ADDR, reg_addr, 2,(uint8_t*)&reg_data) ;
	return reg_data;
 }

 void opt3001_write(uint8_t reg_addr,uint16_t reg_data)
 {
	 	HAL_StatusTypeDef status = HAL_OK;
		

	 	status =   Sensors_I2C_WriteRegister(OPT_ADDR, reg_addr,2,(uint8_t*)&reg_data );

 	 	  // status = HAL_I2C_Mem_Write( &OPT_I2C_Handle, OPT_ADDR<<1, ( uint16_t )reg_addr, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&reg_data, 2,I2C_Timeout );

	  
 }
/*
���ܣ����üĴ���
���룺��
�������
*/
void opt3001_config(void)
{
	uint16_t vCfg = 0;
	
	//12:15 RN  	- ���ò������յķ�Χ ���ֲ�20ҳ��9  ������λ1100������������Χ�Զ�ѡ��
	//11    CT  	- ����ʱ������ 0- 100Ms  1-800Ms
	//10:9  M[1:0]	- ת��ģʽ 00-�ر�ģʽ  01 - ����ת��  10��11 - �������ת��
	//8     OVF     - �������ճ����趨�ķ�Χ��������ֵ �����־
	//7     CRF		- ת�������ֶ� 1-ת�����
	//6     FH		- ת���Ĺ���ֵ ��������ֵ ��λ
	//5     FL		- ת���Ĺ���ֵ С������ֵ ��λ
	//4     L		- �ж����������ģʽ  1-����ģʽ ����ģʽ�¸�����λ�͵�����λINT���  0-�ͺ�ģʽ ������λINT��� ���忴�ֲ�
	//3     POL		- INT �жϱ������������ 0-����  1-����
	//2     ME 		- �����ֶ�
	//0:1   FC		- �������޷�Χ���ϼ���  ����������� ���ڵ��ڼ����趨����  INT����ж�
	
	vCfg = (0x0C<<12);
	vCfg |= (0x01<<11);
	vCfg |= (0x01<<9);
	vCfg |= (0x01<<4);
	opt3001_write(OPT3001_CONFIGURATION, vCfg);
}
/*
���ܣ���ȡ����ID
���룺��
�������
*/
uint16_t opt3001_manufacturer_id(void)
{
	return  opt3001_read(OPT3001_MANUFACTURER_ID);
}
 
/*
���ܣ���ȡ�豸ID
���룺��
�������
*/
uint16_t opt3001_device_id(void)
{
	return opt3001_read(OPT3001_DEVICE_ID);

}
/*
���ܣ���ȡ����������
���룺��
�������
*/
#define ARRY_SIZE 10
uint32_t opt3001_data[ARRY_SIZE];
uint8_t arry_wpr=0;
 
uint8_t opt3001_get_lux(void)
{
	uint8_t		vRval 	= 0;
	uint16_t  	vCfg 	= 0;
	uint16_t  	vDat 	= 0;
	
	uint16_t  	vDatE = 0;
	uint16_t  	vDatR = 0;
	
	float   vFval 		= 0.0;
	float   vLsbSize 	= 0.0;
	float   vFlux 		= 0;
	
	vCfg = opt3001_read(OPT3001_CONFIGURATION);
	vCfg |= (0x01<<9);
	opt3001_write(OPT3001_CONFIGURATION, vCfg);		//���βɼ�����
	
	vCfg = opt3001_read(OPT3001_CONFIGURATION);
	HAL_Delay(900);								//����800Ms
	
	vCfg = opt3001_read(OPT3001_CONFIGURATION);
	if((vCfg&(0x01<<7)) )						//���ղɼ����
	{
		vDat = opt3001_read(OPT3001_RESULT);
		
		vDatE = ((vDat&0xF000)>>12);
		vDatR = (vDat&0x0FFF);
		
		vFval = (0x01<<vDatE);
		vLsbSize = (0.01f * vFval);
		
		vFlux  = (vLsbSize * (float)vDatR);
		opt3001_data[arry_wpr] = ((vFlux)*100.0f);//͸����ǲ���Ҫ���� �����ɫ����Ƽ�*1.8���� *vp_Lux = ((vFlux*1.8)*100.0)
		osprintf("flux is %d\r\n",opt3001_data[arry_wpr]);
		arry_wpr++;	
		if(arry_wpr >= ARRY_SIZE)		
		{
		arry_wpr = 0;
		}
	}
	else
	{
		vRval = 0x01;//���ղɼ�ʧ��
	}
	
	return vRval;
}


int OPT3001_readSensor(void)
{
    uint16_t data;
    uint8_t tmp[2];
    
    /* start measurement */
  //  tmp[0] = OPT3001_CONFIGURATION;
    tmp[0] = OPT3001_CFG & 0xFF;
    tmp[1] = OPT3001_CFG >> 8;
    
   opt3001_write(OPT3001_CONFIGURATION,(uint16_t)tmp);
    // os_I2C_WriteReg(OPT_ADDR, reg_addr,2,(uint8_t*)&reg_data );
    /* wait till measurement is finished */
    int i = 100;
    do {
        /* 10 ms delay */
        HAL_Delay(10);

		data = opt3001_read(OPT3001_CONFIGURATION);
        
        if (data & OPT3001_CFG_CRF) {
            break;
        }

        i--;
    } while (i);
    
    if (i == 0) {
        return -1;
    }

    /* read result */
    tmp[0] = OPT3001_REG_RESULT;
  //  i2c.write(OPT3001_REG_RESULT, tmp, 1);
    opt3001_read(OPT3001_REG_RESULT);
        
    /* swap bytes, as OPT3001 sends MSB first and I2C driver expects LSB first */
    data = (data << 8) | (data >> 8);

    /* calculate lux per LSB */
    uint8_t exp = (data >> 12) & 0x0F;
    uint32_t lsb_size_x100 = (1 << exp);
    
    /* remove 4-bit exponent and leave 12-bit mantissa only */
    data &= 0x0FFF;
    
    return (((int)data * lsb_size_x100) / 100);
}
uint16_t ID;
void opt_test(void )
{
	uint8_t opt_res;
	opt3001_config();
	HAL_Delay(1000);
	
	while(1)
	{
		ID = opt3001_manufacturer_id();
		printf("manufacturer id is %x",ID);
		HAL_Delay(1000);
		if(ID == 0x5449)
			break;
	}

	ID = opt3001_device_id();
	printf("device id is %x",ID);
 
	while(1)
	{
		opt_res = opt3001_get_lux();
		printf("opt_res is %d",opt_res);
		HAL_Delay(10000);
	}
}