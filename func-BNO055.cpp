#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "Adafruit_BNO055_addressMap.h"
#include "func-BNO055.h"



pico_i2c i2c_BNO055;

int BNO055::setup(i2c_inst_t *i2cPort){

	return 0;
}



