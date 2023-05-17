#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "Adafruit_BNO055_addressMap.h"
#include "func-i2c.h"
#include "func-BNO055.h"

pico_i2c i2c_BNO055;

int BNO055::setup(i2c_inst_t *i2cPort){
	uint8_t readBuff=0;
//	i2c_BNO055.write(i2cPort, BNO055_ADDRESS_A)
	i2c_BNO055.read(i2cPort, BNO055_ADDRESS_A, BNO055_CHIP_ID_ADDR, &readBuff, 1);
	printf("BNO055_CHIP_ID = 0x%x\n", readBuff);
	i2c_BNO055.read(i2cPort, BNO055_ADDRESS_A, BNO055_ACCEL_REV_ID_ADDR, &readBuff, 1);
	printf("BNO055_ACCEL_REV_ID_ADDR = 0x%x\n", readBuff);
	i2c_BNO055.read(i2cPort, BNO055_ADDRESS_A, BNO055_MAG_REV_ID_ADDR, &readBuff, 1);
	printf("BNO055_MAG_REV_ID_ADDR = 0x%x\n", readBuff);
	i2c_BNO055.read(i2cPort, BNO055_ADDRESS_A, BNO055_GYRO_REV_ID_ADDR, &readBuff, 1);
	printf("BNO055_GYRO_REV_ID_ADDR = 0x%x\n", readBuff);
	i2c_BNO055.read(i2cPort, BNO055_ADDRESS_A, BNO055_OPR_MODE_ADDR, &readBuff, 1);
	printf("BNO055_OPR_MODE_ADDR = 0x%x\n", readBuff);

	i2c_BNO055.read(i2cPort, BNO055_ADDRESS_A, BNO055_OPR_MODE_ADDR, &readBuff, 1);
	printf("BNO055_OPR_MODE_ADDR = 0x%x\n", readBuff);
	i2c_BNO055.read(i2cPort, BNO055_ADDRESS_A, BNO055_PWR_MODE_ADDR, &readBuff, 1);
	printf("BNO055_PWR_MODE_ADDR = 0x%x\n", readBuff);

//setting
	printf("\n+++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	i2c_BNO055.writeOneByte(i2cPort, BNO055_ADDRESS_A, BNO055_PWR_MODE_ADDR, POWER_MODE_NORMAL);
	i2c_BNO055.read(i2cPort, BNO055_ADDRESS_A, BNO055_PWR_MODE_ADDR, &readBuff, 1);
	printf("BNO055_PWR_MODE_ADDR = 0x%x\n", readBuff);
	
	i2c_BNO055.writeOneByte(i2cPort, BNO055_ADDRESS_A, BNO055_OPR_MODE_ADDR, OPERATION_MODE_CONFIG);
	i2c_BNO055.read(i2cPort, BNO055_ADDRESS_A, BNO055_OPR_MODE_ADDR, &readBuff, 1);
	printf("BNO055_OPR_MODE_ADDR = 0x%x\n", readBuff);

	i2c_BNO055.read(i2cPort, BNO055_ADDRESS_A, BNO055_UNIT_SEL_ADDR, &readBuff, 1);
	printf("BNO055_UNIT_SEL_ADDR = 0x%b\n", readBuff);
	i2c_BNO055.writeOneByte(i2cPort, BNO055_ADDRESS_A, BNO055_UNIT_SEL_ADDR, 0b10000000);
	i2c_BNO055.read(i2cPort, BNO055_ADDRESS_A, BNO055_UNIT_SEL_ADDR, &readBuff, 1);
	printf("BNO055_UNIT_SEL_ADDR = 0x%b\n", readBuff);

	i2c_BNO055.read(i2cPort, BNO055_ADDRESS_A, BNO055_UNIT_SEL_ADDR, &readBuff, 1);
	printf("BNO055_UNIT_SEL_ADDR = 0x%b\n", readBuff);
	i2c_BNO055.writeOneByte(i2cPort, BNO055_ADDRESS_A, BNO055_UNIT_SEL_ADDR, 0b00010000); //2G, 125Hz,Normal 
	i2c_BNO055.read(i2cPort, BNO055_ADDRESS_A, BNO055_UNIT_SEL_ADDR, &readBuff, 1);
	printf("BNO055_UNIT_SEL_ADDR = 0x%b\n", readBuff);
	
	i2c_BNO055.writeOneByte(i2cPort, BNO055_ADDRESS_A, BNO055_OPR_MODE_ADDR, OPERATION_MODE_NDOF);
	i2c_BNO055.read(i2cPort, BNO055_ADDRESS_A, BNO055_OPR_MODE_ADDR, &readBuff, 1);
	printf("BNO055_OPR_MODE_ADDR = 0x%x\n", readBuff);
	return 0;
}


int BNO055::readAccel(i2c_inst_t *i2cPort, double *x, double *y, double *z){	
	uint8_t buff[6]={0,0,0,0,0,0};
	int16_t xBin, yBin, zBin;

	i2c_BNO055.read(i2cPort, BNO055_ADDRESS_A, BNO055_ACCEL_DATA_X_LSB_ADDR, &buff[0], 1);
	i2c_BNO055.read(i2cPort, BNO055_ADDRESS_A, BNO055_ACCEL_DATA_X_MSB_ADDR, &buff[1], 1);
	i2c_BNO055.read(i2cPort, BNO055_ADDRESS_A, BNO055_ACCEL_DATA_Y_LSB_ADDR, &buff[2], 1);
	i2c_BNO055.read(i2cPort, BNO055_ADDRESS_A, BNO055_ACCEL_DATA_Y_MSB_ADDR, &buff[3], 1);
	i2c_BNO055.read(i2cPort, BNO055_ADDRESS_A, BNO055_ACCEL_DATA_Z_LSB_ADDR, &buff[4], 1);
	i2c_BNO055.read(i2cPort, BNO055_ADDRESS_A, BNO055_ACCEL_DATA_Z_MSB_ADDR, &buff[5], 1);
	xBin = buff[0] << 8 | buff[1];
	yBin = buff[2] << 8 | buff[3];
	zBin = buff[4] << 8 | buff[5];

	*x = (double)xBin/100;
	*y = (double)yBin/100;
	*z = (double)zBin/100;


	return 0;
}


int BNO055::readQuaternion(i2c_inst_t *i2cPort, double *qw, double *qx, double *qy, double *qz){
	

	return 0;
}
