//#include <stdio.h>
//#include <math.h>
//
#include <bits/stdc++.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
//#include "hardware/uart.h"
#include "hardware/pwm.h"
#include "pico/binary_info.h"

#include "func-pwm.h"
#include "func-i2c.h"
#include "func-MS5837-02BA.h"
#include "func-BNO055.h"

#include "sd_card.h"
#include "ff.h"

const uint LED_PIN = 25;
const uint I2C1_SDA_PIN = 14;
const uint I2C1_SCL_PIN = 15;
pico_pwm pwm;
pico_i2c i2c;
MS5837_02BA MS5837;
BNO055 BNO055;

bool reserved_addr(uint8_t addr){
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

int main(){
//	bi_decl(bi_program_description("This is a test binary."));
//	bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));

	uint8_t data=0;
	double outTemp=0.0;
	double outPress=0.0;
	double xAccel=0.0;
	double yAccel=0.0;
	double zAccel=0.0;
	double xMag=0.0;
	double yMag=0.0;
	double zMag=0.0;
	double pSurface=0.0;
	double tempSurface=0.0;
	double turgetDepth=0.0;
	int i=0;

	//SD
	FRESULT fr;
	FATFS fs;
	FIL fil;
	int ret;
	char buf[100];
	char filename[] = "test02.txt";

	printf("start");

	sleep_ms(5000);

	stdio_init_all();
	pwm.setup();
	i2c.setup(i2c1, 400*1000);
	gpio_set_function(I2C1_SDA_PIN, GPIO_FUNC_I2C);	//raspi mother ver1.0
	gpio_set_function(I2C1_SCL_PIN, GPIO_FUNC_I2C);	//raspi mother ver1.0
	gpio_pull_up(I2C1_SDA_PIN);
  gpio_pull_up(I2C1_SCL_PIN);
	bi_decl(bi_2pins_with_func(I2C1_SDA_PIN, I2C1_SCL_PIN, GPIO_FUNC_I2C));

	sleep_ms(5000);
  printf("\nI2C Bus Scan. SDA=GP%d SCL=GP%d\n", I2C1_SDA_PIN, I2C1_SCL_PIN);
  printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

  for (int addr = 0; addr < (1 << 7); ++addr) {
		if (addr % 16 == 0) {
			printf("%02x ", addr);
		}

        // Perform a 1-byte dummy read from the probe address. If a slave
        // acknowledges this address, the function returns the number of bytes
        // transferred. If the address byte is ignored, the function returns
        // -1.

        // Skip over any reserved addresses.
    int ret;
    uint8_t rxdata;
    if (reserved_addr(addr))
			ret = PICO_ERROR_GENERIC;
    else
			ret = i2c_read_blocking(i2c1, addr, &rxdata, 1, false);

      printf(ret < 0 ? "." : "@");
      printf(addr % 16 == 15 ? "\n" : "  ");
  }
    printf("Done.\n");
	
	printf("HelloUART!\n");
	
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);
	sleep_ms(1000);

	i2c.read(i2c1, 0x28, 0x00, &data, 1);
	printf("0x%x\n", data);

	i2c.read(i2c1, 0x28, 0x01, &data, 1);
	printf("0x%x\n", data);
	
	i2c.read(i2c1, 0x77, 0x75, &data, 1); //BME680
	printf("0x%x\n", data);

	MS5837.setup(i2c1);
	BNO055.setup(i2c1);
	

	printf("TestDone\n");

	MS5837.readTempPress(i2c1, &tempSurface, &pSurface);
	printf("SurfaceTemp = %f [C]\n", tempSurface);
	printf("SurfacePress = %f [mbar]\n", pSurface);





    // Initialize SD card
    if (!sd_init_driver()) {
        printf("ERROR: Could not initialize SD card\r\n");
        while (true);
    }

    // Mount drive
    fr = f_mount(&fs, "0:", 1);
    if (fr != FR_OK) {
        printf("ERROR: Could not mount filesystem (%d)\r\n", fr);
        while (true);
    }

    // Open file for writing ()
    fr = f_open(&fil, filename, FA_WRITE | FA_CREATE_ALWAYS);
    if (fr != FR_OK) {
        printf("ERROR: Could not open file (%d)\r\n", fr);
        while (true);
    }

    // Write something to file
    ret = f_printf(&fil, "This is another test\r\n");
    if (ret < 0) {
        printf("ERROR: Could not write to file (%d)\r\n", ret);
        f_close(&fil);
        while (true);
    }
    ret = f_printf(&fil, "of writing to an SD card.\r\n");
    if (ret < 0) {
        printf("ERROR: Could not write to file (%d)\r\n", ret);
        f_close(&fil);
        while (true);
    }

    // Close file
    fr = f_close(&fil);
    if (fr != FR_OK) {
        printf("ERROR: Could not close file (%d)\r\n", fr);
        while (true);
    }

    // Open file for reading
    fr = f_open(&fil, filename, FA_READ);
    if (fr != FR_OK) {
        printf("ERROR: Could not open file (%d)\r\n", fr);
        while (true);
    }

    // Print every line in file over serial
    printf("Reading from file '%s':\r\n", filename);
    printf("---\r\n");
    while (f_gets(buf, sizeof(buf), &fil)) {
        printf(buf);
    }
    printf("\r\n---\r\n");

    // Close file
    fr = f_close(&fil);
    if (fr != FR_OK) {
        printf("ERROR: Could not close file (%d)\r\n", fr);
        while (true);
    }

    // Unmount drive
    f_unmount("0:");






	while(1) {
		MS5837.readTempPress(i2c1, &outTemp, &outPress);
		BNO055.readAccel(i2c1, &xAccel, &yAccel, &zAccel);
		BNO055.readMag(i2c1, &xMag, &yMag, &zMag);
		
		printf("+++++OutPut Start+++++\n");
		printf("Temp = %f [C]\n", outTemp);
		printf("Press = %f [mbar]\n", outPress);
		printf("\n");
		printf("X Accel = %f m/s^2\n", xAccel);
		printf("Y Accel = %f m/s^2\n", yAccel);
		printf("Z Accel = %f m/s^2\n\n", zAccel);
		printf("|G| = %f \n\n", sqrt(pow(xAccel,2)+pow(yAccel,2)+pow(zAccel,2)));
		printf("\n");
		printf("X Mag = %f uT\n", xMag);
		printf("Y Mag = %f uT\n", yMag);
		printf("Z Mag = %f uT\n\n", zMag);
		printf("|T| = %f uT\n\n", sqrt(pow(xMag,2)+pow(yMag,2)+pow(zMag,2)));
		printf("Azimuth = %f deg\n\n", atan2(yMag, xMag)*180/M_PI);
		printf("+++++OutPut End+++++\n");
		printf("\n");
		printf("X Accel = %f \n", xAccel);
		printf("Y Accel = %f \n", yAccel);
		printf("Z Accel = %f \n\n", zAccel);
		printf("|G| = %f \n\n", sqrt(pow(xAccel,2)+pow(yAccel,2)+pow(zAccel,2)));
		gpio_put(LED_PIN, 0);
		sleep_ms(1000);
		gpio_put(LED_PIN, 1);
//		puts("Hello World\n");
		sleep_ms(500);
		pwm.duty(0, (0.75+i*0.01));
		if(i >= 15){
			i=i;
		}
		else{
			i++;
		}
	}
}
