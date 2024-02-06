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
	uint16_t buff;
//	i2c_INA238.writeDirect(i2cPort, MS5837_ADDR, MS5837_RESET, 1); //RESET
	sleep_ms(100);
	printf("OK!\n");
//	i2c_INA238.writeDirect(i2cPort, MS5837_ADDR, 0b00011110, 1); //PROM-READ-START
//	for(uint8_t i=0; i<7; i++){
//		i2c_INA238.read(i2cPort, MS5837_ADDR, MS5837_PROM_READ+i*2, cBuff, 2);
//		c[i] = ((cBuff[0]<<8) | (cBuff[1]));
//		printf("0x%x, 0x%x, %d\n", MS5837_PROM_READ+i*2, c[i], c[i]);
//	}
	i2c_INA228.writeOneByte(i2cPort, INA228_ADDR, INA228_ADC_CONFIG, 0x00);
	sleep_ms(100);
	i2c_INA228.read(i2cPort, INA228_ADDR, 0x3f, cBuff, 2);
	buff = ((cBuff[0] << 8) | (cBuff[1]));
	printf("0x%x, 0x%x, INA238 DEV ID= 0x%x\n", cBuff[0], cBuff[1], buff);
	i2c_INA228.read(i2cPort, INA228_ADDR, 0x3e, cBuff, 2);
	buff = ((cBuff[0] << 8) | (cBuff[1]));
	printf("0x%x, 0x%x, INA238 MAN ID= 0x%x\n", cBuff[0], cBuff[1], buff);
//	i2c_INA238.writeDirect(i2cPort, MS5837_ADDR, MS5837_CONVERT_D2_8192, 1); 
	return 0;
}
/*	
int MS5837_02BA::readTempPress(i2c_inst_t *i2cPort, double *temp, double *press){
	uint8_t adcRead[3]={111,111,111};
	uint32_t d1Data = 0;
	uint32_t d2Data = 0;
	int32_t dt=0, ti=0;
	int64_t tempBuff=0, pBuff=0;
//  double tempBuff=0,pBuff=0;
	int64_t off=0, sens=0, off2=0, sens2=0, sensi=0, offi=0;
	

//	sleep_ms(100);

	i2c_MS5837.writeDirect(i2cPort, MS5837_ADDR, MS5837_CONVERT_D1_8192, 1);
	sleep_ms(20);
	i2c_MS5837.read(i2cPort, MS5837_ADDR, MS5837_ADC_READ, adcRead, 3);
	d1Data = ( (adcRead[0]<<16) | (adcRead[1]<<8) | (adcRead[2]) );
//	printf("d1Data=0x%x\n",d1Data);
//	printf("%d\n",d1Data);

	i2c_MS5837.writeDirect(i2cPort, MS5837_ADDR, MS5837_CONVERT_D2_8192, 1); 
	sleep_ms(20);
	i2c_MS5837.read(i2cPort, MS5837_ADDR, MS5837_ADC_READ, adcRead, 3);
	d2Data = ( (adcRead[0]<<16) | (adcRead[1]<<8) | (adcRead[2]) );
//	printf("d2Data=0x%x\n",d2Data);
//	printf("d2Data(Dec) = %d\n",d2Data);
//	printf("d2Data(Hex) = %x\n",d2Data);
//	printf("READ DONE\n");
//	dt = d2Data - (c[5] * pow(2,8);
	dt = d2Data - (c[5]<<8);
//	printf("dt=%d\n",dt);
//	tempBuff = 2000.0 + dt * c[6] / pow(2,23);
	tempBuff = 2000 + int64_t( (dt>>11)*(c[6]>>12) );
//	printf("dt=%d\n", dt);
//	printf("c5=%d\n", c[5]);
//	printf("c6=%d\n", c[6]);
//	printf("dt*c6=%d\n", dt*c[6]);
//	printf("tempBuff=%d\n", tempBuff);

  off = c[2] * pow(2,17) + (c[4] * dt) / pow(2,6);
  sens = c[1] * pow(2,16) + (c[3] * dt) / pow(2,7);
  pBuff = (d1Data * sens / pow(2,21) - off ) / pow(2,15);
//	printf("%d\n",P);

	if (tempBuff < 2000.0){
		printf("LOW TEMP\n");
		ti = 11 * dt * dt / pow(2,35);
		offi = 31 * (tempBuff - 2000) * (tempBuff - 2000) / pow(2,3);
		sensi = 63 * (tempBuff - 2000) * (tempBuff - 2000) / pow(2,5);
//		printf("Low temp. mode");
//    if (Temp <-1500.0) {
//			OFFi = OFFi + 7 * (Temp + 1500) * (Temp + 1500);
//			SENSi = SENSi + 4 * (Temp + 1500) * (Temp + 1500);
//    }
	} 
	else{
//			Ti = 2 * dT * dT / pow(2,37);
//			OFFi = (Temp - 2000) * (Temp - 2000) / pow(2,4) ;
//			SENSi = 0;
	}

	off2 = off - offi;
	sens2 = sens - sensi;
	*temp = (tempBuff - ti) /100.0;
	*press = (((d1Data * sens2) / pow(2,21) - off2) / pow(2,15)) / 100;
	printf("\n");

//	printf("Temp = %f [C]\n", temp);
//	printf("Press = %f [mbar]\n", press);
	
	return 0;
}
*/
