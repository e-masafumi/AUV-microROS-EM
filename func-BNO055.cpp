#include <stdio.h>
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
	i2c_BNO055.read(i2cPort, BNO055_ADDRESS_A, BNO055_OPR_MODE_ADDR, &readBuff, 1);
	printf("BNO055_OPR_MODE_ADDR = 0x%x\n", readBuff);
	i2c_BNO055.read(i2cPort, BNO055_ADDRESS_A, BNO055_PWR_MODE_ADDR, &readBuff, 1);
	printf("BNO055_PWR_MODE_ADDR = 0x%x\n", readBuff);
	return 0;
}



