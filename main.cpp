//#include <stdio.h>
//#include <math.h>

#include <bits/stdc++.h>
#include <string>
#include <vector>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "hardware/pwm.h"
#include "pico/binary_info.h"
#include "extern.h"

#include "func-pwm.h"
#include "func-i2c.h"
#include "func-uart.h"
#include "func-MS5837-02BA.h"
#include "func-BNO055.h"
#include "func-INA228.h"

#include "sd_card.h"
#include "ff.h"

const uint LED_PIN = 25;
const uint I2C0_SDA_PIN = 8;
const uint I2C0_SCL_PIN = 9;
const uint I2C1_SDA_PIN = 14;
const uint I2C1_SCL_PIN = 15;
volatile bool exeFlag = false;
struct repeating_timer st_timer;
pico_pwm pwm;
pico_i2c i2c;
pico_uart uart;
MS5837_02BA MS5837;
BNO055 BNO055;
INA228 INA228;
static semaphore_t sem;


bool reserved_addr(uint8_t addr){
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}
int64_t alarm_callback(alarm_id_t id, void *user_data) {
	printf("Timer %d fired!\n", (int) id);
	exeFlag = true;
	// Can return a value here in us to fire in the future
	return 0;
}

bool repeating_timer_callback(struct repeating_timer *t) {
//	printf("Repeat at %lld\n", time_us_64());
	exeFlag = true;
	return true;
}

void core1_main(void){
	int actualBaudrate[2]={12000,12000};
	char readNMEA[30][30];
	int messageBlockCnt = 0;
	bool messageFinishFlag = false;
	char nmeaBlockCnt=0;
	char nmeaCharCnt=0;
	actualBaudrate[0] = uart.setup(uart0, 9600, 8, 1);
	sleep_ms(3000);
	printf("UART actual baudrate,core1 0: %d, 1: %d\n", actualBaudrate[0], actualBaudrate[1]);
	while(1){
		if(uart0DataInFlag){
			readNMEA[nmeaBlockCnt][nmeaCharCnt] = uart0ReadBuff;
			uart0DataInFlag = false;
			nmeaCharCnt++;
			if(uart0ReadBuff == 0x2c){							//,
				nmeaBlockCnt++;
				nmeaCharCnt = 0;
			}
			else if(uart0ReadBuff == 0x0a){							//<LF>
				nmeaBlockCnt = 0;
				nmeaCharCnt = 0;
				messageFinishFlag = true;
			}
		}
		sem_acquire_blocking(&sem);
			if(messageFinishFlag){
				for(int i=0; i++; i<30){
					for(int j=0; j++; j<30){
						printf("%c",readNMEA[i][j]);
					}
				}
				messageFinishFlag = false;
			}
//			if(messageFinishFlag){
//				printf("%s", splitNMEA);
//				messageFinishFlag = false;
//			}
		sem_release(&sem);
//		if(messageStartFlag){
//			printf("messageStart from core1");
//		}
//		if(messageFinishFlag){
//			printf("messageFinish from core1");
//			messageFinishFlag = false;
//		}
//		printf("Core 1, UART0 is mine, %dbps", actualBaudrate[0]);
//		sleep_ms(1000);
//		printf("%s\n\n\n", splitNMEA[2]);
	}
}



int main(){
//	bi_decl(bi_program_description("This is a test binary."));
//	bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));

	uint8_t data=0;
//	uint32_t timeBuff_32=0;
	uint64_t timeBuff_64=0;

/*	double outTemp=0.0;
	double outPress=0.0;
	double xAccel=0.0;
	double yAccel=0.0;
	double zAccel=0.0;
	double xMag=0.0;
	double yMag=0.0;
	double zMag=0.0;*/
	double pSurface=0.0;
	double tempSurface=0.0;
	double turgetDepth=0.0;
	int i=0;
//	int actualBaudrate[2]={12000,12000};

	struct sensorsData{
		uint32_t timeBuff_32=0;
		double outTemp=0.0;
		double outPress=0.0;
		double xAccel=0.0;
		double yAccel=0.0;
		double zAccel=0.0;
		double xMag=0.0;
		double yMag=0.0;
		double zMag=0.0;
	};
	
	struct sensorsData logData;

	FRESULT fr;
	FATFS fs;
	FIL fil;
	int ret;
	char buf[100];
//	char filename[] = "log.dat";
	char filename[] = "log.txt";

	sem_init(&sem, 1, 1);
	printf("start");
	stdio_init_all();
	multicore_launch_core1(core1_main);
	pwm.setup();
//	actualBaudrate[0] = uart.setup(uart0, 9600, 8, 1);
//	actualBaudrate[1] = uart.setup(uart1, 9600, 8, 1);

	i2c.setup(i2c0, 400*1000);
	i2c.setup(i2c1, 400*1000);
	gpio_set_function(I2C0_SDA_PIN, GPIO_FUNC_I2C);	//raspi mother ver1.0
	gpio_set_function(I2C0_SCL_PIN, GPIO_FUNC_I2C);	//raspi mother ver1.0
	gpio_set_function(I2C1_SDA_PIN, GPIO_FUNC_I2C);	//raspi mother ver1.0
	gpio_set_function(I2C1_SCL_PIN, GPIO_FUNC_I2C);	//raspi mother ver1.0
	gpio_pull_up(I2C0_SDA_PIN);
	gpio_pull_up(I2C0_SCL_PIN);
	gpio_pull_up(I2C1_SDA_PIN);
	gpio_pull_up(I2C1_SCL_PIN);
	bi_decl(bi_2pins_with_func(I2C0_SDA_PIN, I2C0_SCL_PIN, GPIO_FUNC_I2C));
	bi_decl(bi_2pins_with_func(I2C1_SDA_PIN, I2C1_SCL_PIN, GPIO_FUNC_I2C));

	sleep_ms(500);
/*  printf("\nI2C Bus Scan. SDA=GP%d SCL=GP%d\n", I2C1_SDA_PIN, I2C1_SCL_PIN);
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
  }*/
  printf("Done.\n");
	
//	printf("UART actual baudrate, 0: %d, 1: %d\n", actualBaudrate[0], actualBaudrate[1]);
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);
	printf("HelloLED!\n");
	sleep_ms(500);
//	i2c.read(i2c1, 0x28, 0x00, &data, 1);
//	printf("0x%x\n", data);

//	i2c.read(i2c1, 0x28, 0x01, &data, 1);
//	printf("0x%x\n", data);
	
//	i2c.read(i2c1, 0x77, 0x75, &data, 1); //BME680
//	printf("0x%x\n", data);

	BNO055.setup(i2c1);
	printf("HelloBNO055!\n");
	sleep_ms(100);
	MS5837.setup(i2c1);
	printf("HelloMS5837!\n");
	sleep_ms(100);
	printf("OK!\n");
	INA228.setup(i2c0);
	printf("HelloINA238!\n");
	sleep_ms(100);
	

	printf("TestDone\n");
	sleep_ms(100);

	printf("uint32_t: %lu\n", sizeof(logData.timeBuff_32));
	printf("double: %lu\n", sizeof(logData.outTemp));
	printf("logData: %lu\n", sizeof(logData));
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
  ret = f_printf(&fil, "Test\r\n");
	if (ret < 0) {
		printf("ERROR: Could not write to file (%d)\r\n", ret);
		f_close(&fil);
		while (true);
	}
/*    ret = f_printf(&fil, "of writing to an SD card.\r\n");
    if (ret < 0) {
        printf("ERROR: Could not write to file (%d)\r\n", ret);
        f_close(&fil);
        while (true);
    }
*/
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
//    f_unmount("0:");


    // Open file for writing ()
	fr = f_open(&fil, filename, FA_WRITE | FA_CREATE_ALWAYS);
	if (fr != FR_OK) {
		printf("ERROR: Could not open file (%d)\r\n", fr);
    while (true);
  }
//    ret = f_printf(&fil, "Time, Temp, Press, ax, ay, az, Bx, By, Bz\r\n");
//    ret = f_write(&fil, "Time, Temp, Press, ax, ay, az, Bx, By, Bz\r\n");


	add_repeating_timer_us(100*1000, repeating_timer_callback, NULL, &st_timer);


	while(1) {
		if(exeFlag == false){
//			printf("Skipped\n");
			continue;
		}
		
		logData.timeBuff_32 = time_us_32();
		MS5837.readTempPress(i2c1, &logData.outTemp, &logData.outPress);
		BNO055.readAccel(i2c1, &logData.xAccel, &logData.yAccel, &logData.zAccel);
		BNO055.readMag(i2c1, &logData.xMag, &logData.yMag, &logData.zMag);
//		printf("%d, %f, %f\n",logData.timeBuff_32, logData.outTemp, logData.outPress);

		// Write something to file
    ret = f_printf(&fil, "%d, %lf, %lf,%lf,%lf,%lf,%lf,%lf,%lf\r\n", logData.timeBuff_32, logData.outTemp, logData.outPress,logData.xAccel,logData.yAccel,logData.zAccel,logData.xMag,logData.yMag,logData.zMag);
//		writeBuff[] = {timeBuff_32, outTemp, outPress, xAccel, yAccel, zAccel, xMag, yMag, zMag};
//		writeSize=sizeof(writeBuff);
/*		size_t writeSize=sizeof(logData);
		unsigned int writtenSize=0;
    ret = f_write(&fil, &logData, int(writeSize), &writtenSize);
		if(writeSize != writtenSize){
			printf("DISK FULL");
		}
		else{
		}
*/	
/*    ret = f_printf(&fil, "%lf, %lf, %lf,", xAccel, yAccel, zAccel);
    ret = f_printf(&fil, "%lf, %lf, %lf", xMag, yMag, zMag);
    ret = f_printf(&fil, "\r\n");*/
    if (ret < 0) {
        printf("ERROR: Could not write to file (%d)\r\n", ret);
        f_close(&fil);
        while (true);
    }

/*	
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
*/
//		sleep_ms(1000);
		gpio_put(LED_PIN, 1);
//		printf("%s\n\n\n", splitNMEA[2]);
//		puts("Hello World\n");
		pwm.duty(0, (0.75+i*0.001));
		pwm.duty(1, (0.75+i*0.001));
		if(i >= 10){
			i=0;
			gpio_put(LED_PIN, 0);
//			sleep_ms(1000);
//			printf("PWM RESET\r\n");
			// Close file
			fr = f_close(&fil);
			if (fr != FR_OK) {
				printf("ERROR: Could not close file (%d)\r\n", fr);
        while (true);
			}
			// Open file for writing ()
			fr = f_open(&fil, filename, FA_WRITE | FA_OPEN_ALWAYS);
			if (fr != FR_OK) {
        printf("ERROR: Could not open file (%d)\r\n", fr);
        while (true);
			}
			//Move to end
			ret = f_lseek(&fil, f_size(&fil));
		}
		else{
			i++;
		}
//		logData.timeBuff_32 = time_us_32();
		sem_acquire_blocking(&sem);
		printf("%d, %f, %f\n\n",logData.timeBuff_32, logData.outTemp, logData.outPress);
		
//		printf("%c", uart0ReadBuff);
//		if(messageFinishFlag){
//			printf("messageFinish from core0");
//			messageFinishFlag = false;
//		}
		sem_release(&sem);
		exeFlag = false;
	}
}
