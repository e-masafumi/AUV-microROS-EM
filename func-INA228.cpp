#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "func-i2c.h"
#include "func-INA228.h"

const uint8_t INA228_ADDR = 0b01000000;
const uint8_t INA228_CONFIG = 0x00;
const uint8_t INA228_ADC_CONFIG = 0x01;

pico_i2c i2c_INA228;

int INA228::setup(i2c_inst_t *i2cPort){
	uint8_t cBuff[2]={1,1};
	uint16_t readBuff;
//	i2c_INA238.writeDirect(i2cPort, MS5837_ADDR, MS5837_RESET, 1); //RESET
	sleep_ms(100);
	printf("OK!\n");
//	i2c_INA238.writeDirect(i2cPort, MS5837_ADDR, 0b00011110, 1); //PROM-READ-START
//	for(uint8_t i=0; i<7; i++){
//		i2c_INA238.read(i2cPort, MS5837_ADDR, MS5837_PROM_READ+i*2, cBuff, 2);
//		c[i] = ((cBuff[0]<<8) | (cBuff[1]));
//		printf("0x%x, 0x%x, %d\n", MS5837_PROM_READ+i*2, c[i], c[i]);
//	}
	uint16_t buff[2] = {0x00, 0x00};
	i2c_INA228.writeMultiByte(i2cPort, INA228_ADDR, INA228_ADC_CONFIG, 0x00, 2);
	sleep_ms(100);
	i2c_INA228.read(i2cPort, INA228_ADDR, 0x3f, cBuff, 2);
	readBuff = ((cBuff[0] << 8) | (cBuff[1]));
	printf("0x%x, 0x%x, INA238 DEV ID= 0x%x\n", cBuff[0], cBuff[1], readBuff);
	i2c_INA228.read(i2cPort, INA228_ADDR, 0x3e, cBuff, 2);
	readBuff = ((cBuff[0] << 8) | (cBuff[1]));
	printf("0x%x, 0x%x, INA238 MAN ID= 0x%x\n", cBuff[0], cBuff[1], readBuff);
//	i2c_INA238.writeDirect(i2cPort, MS5837_ADDR, MS5837_CONVERT_D2_8192, 1); 
	return 0;
}

//int INA228::readCurVolPow(i2c_inst_t *i2cPort, double *current, double *voltage, double *power){
	


//	return 0;
//}
