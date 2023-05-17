#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "func-i2c.h"
#include "func-MS5837-02BA.h"

const uint8_t MS5837_ADDR = 0x76;
const uint8_t MS5837_RESET = 0x1E;
const uint8_t MS5837_ADC_READ = 0x00;
const uint8_t MS5837_PROM_READ = 0xA0;
const uint8_t MS5837_CONVERT_D1_8192 = 0x4A;
const uint8_t MS5837_CONVERT_D2_8192 = 0x5A;

//const float MS5837::Pa = 100.0f;
//const float MS5837::bar = 0.001f;
//const float MS5837::mbar = 1.0f;

//const uint8_t MS5837::MS5837_30BA = 0;
//const uint8_t MS5837::MS5837_02BA = 1;
//const uint8_t MS5837::MS5837_UNRECOGNISED = 255;

const uint8_t MS5837_02BA01 = 0x00; // Sensor version: From MS5837_02BA datasheet Version PROM Word 0
const uint8_t MS5837_02BA21 = 0x15; // Sensor version: From MS5837_02BA datasheet Version PROM Word 0
const uint8_t MS5837_30BA26 = 0x1A; // Sensor version: From MS5837_30BA datasheet Version PROM Word 0

double tempC, pressPa;
uint16_t c[7]={0};

pico_i2c i2c_MS5837;

int MS5837_02BA::setup(i2c_inst_t *i2cPort){
	uint8_t cBuff[2]={0};
	i2c_MS5837.writeDirect(i2cPort, MS5837_ADDR, MS5837_RESET, 1); //MS5837-02BA
	sleep_ms(100);
	for(uint8_t i=0; i<7; i++){
		i2c_MS5837.read(i2cPort, MS5837_ADDR, MS5837_PROM_READ+i*2, cBuff, 2);
		c[i] = ((cBuff[0]<<8) | (cBuff[1]));
//		printf("0x%x, 0x%x, %d\n", 0xa0+i*2, c[i], c[i] );
	}
	return 0;
}
	
int MS5837_02BA::readTempPress(i2c_inst_t *i2cPort, double *temp, double *press){
	uint8_t adcRead[3]={0};
	uint32_t d1Data = 0;
	uint32_t d2Data = 0;
	int32_t dt=0, ti=0;
	int64_t tempBuff=0, pBuff=0;
//  double tempBuff=0,pBuff=0;
	int64_t off=0, sens=0, off2=0, sens2=0, sensi=0, offi=0;
	

	i2c_MS5837.writeDirect(i2cPort, MS5837_ADDR, MS5837_CONVERT_D1_8192, 1); 
	sleep_ms(100);

	i2c_MS5837.read(i2cPort, MS5837_ADDR, MS5837_ADC_READ, adcRead, 3);
	d1Data = (adcRead[0]<<16 | (adcRead[1]<<8) | (adcRead[2]));
//	printf("%d\n",d1Data);

	i2c_MS5837.writeDirect(i2cPort, MS5837_ADDR, MS5837_CONVERT_D2_8192, 1); 
	sleep_ms(100);
	i2c_MS5837.read(i2cPort, MS5837_ADDR, MS5837_ADC_READ, adcRead, 3);
	d2Data = (adcRead[0]<<16 | (adcRead[1]<<8) | (adcRead[2]));
//	printf("d2Data(Dec) = %d\n",d2Data);
//	printf("d2Data(Hex) = %x\n",d2Data);

//	dt = d2Data - (c[5] * pow(2,8);
	dt = d2Data - (c[5]<<8);
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

	if (tempBuff <2000.0) {
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


//	printf("Temp = %f [C]\n", temp);
//	printf("Press = %f [mbar]\n", press);
	
	return 0;
}
