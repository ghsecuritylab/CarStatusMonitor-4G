#include <stm32f4xx_hal.h>
 #include <stdbool.h>
 #include "mpu9250_driver.h" 

#define OPT3001_RESULT		0x00
#define OPT3001_CONFIGURATION	0x01
#define OPT3001_LOW_LIMIT	0x02
#define OPT3001_HIGH_LIMIT	0x03
#define OPT3001_MANUFACTURER_ID	0x7e
#define OPT3001_DEVICE_ID	0x7f
 
#define OPT_ADDR   0x8A
extern I2C_HandleTypeDef hi2c2;

#define    OPT_I2C_Handle  hi2c2

#define I2C_Timeout  0X100
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
uint16_t opt3001_read(uint8_t reg_addr )
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
uint16_t ID=0;
void opt3001_manufacturer_id(void)
{
	ID = opt3001_read(OPT3001_MANUFACTURER_ID);
}
 
/*
���ܣ���ȡ�豸ID
���룺��
�������
*/
void opt3001_device_id(void)
{
	ID = opt3001_read(OPT3001_DEVICE_ID);

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
void opt_test(void )
{
	uint8_t opt_res;
	opt3001_config();
	HAL_Delay(1000);
	
	while(1)
	{
		opt3001_manufacturer_id();
		printf("manufacturer id is %x",ID);
		HAL_Delay(1000);
		if(ID == 0x5449)
			break;
	}

	opt3001_device_id();
	printf("device id is %x",ID);
 
	while(1)
	{
		opt_res = opt3001_get_lux();
		printf("opt_res is %d",opt_res);
		HAL_Delay(10000);
	}
}