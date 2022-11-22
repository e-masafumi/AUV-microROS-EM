#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "func-i2c.h"

#ifndef FUNC_BNO055_H
#define FUNC_BNO055_H

class BNO055 : public pico_i2c{
	public:
		int setup(i2c_inst_t *i2cPort);
		int readTempPress(i2c_inst_t *i2CPort, double *temp, double *press);
};

#endif
