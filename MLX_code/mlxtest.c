//Reference: https://olegkutkov.me/2017/08/10/mlx90614-raspberry/

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stddef.h> // Added for size_t
#include <errno.h>  // Added for errno
#include "mlxtest.h"
#define I2C_DEV_PATH "/dev/i2c-1"

/* Just in case */
#ifndef I2C_SMBUS_READ 
#define I2C_SMBUS_READ 1 
#endif 
#ifndef I2C_SMBUS_WRITE 
#define I2C_SMBUS_WRITE 0 
#endif

typedef union i2c_smbus_data i2c_data;
int fdev;
void i2c_read_data(int , i2c_data* , struct i2c_smbus_ioctl_data*);



int i2c_init()
{
    fdev = open(I2C_DEV_PATH, O_RDWR); // open i2c bus

    if (fdev < 0) {
        fprintf(stderr, "Failed to open I2C interface %s Error: %s\n", I2C_DEV_PATH, strerror(errno));
        return -1;
    }

    unsigned char i2c_addr = 0x5A;

    // set slave device address, default MLX is 0x5A
    if (ioctl(fdev, I2C_SLAVE, i2c_addr) < 0) {
        fprintf(stderr, "Failed to select I2C slave device! Error: %s\n", strerror(errno));
        return -1;
    }

    // enable checksums control
    if (ioctl(fdev, I2C_PEC, 1) < 0) {
        fprintf(stderr, "Failed to enable SMBus packet error checking, error: %s\n", strerror(errno));
        return -1;
    }
    printf("Init_complete\n");
    return 0;

}

float get_temp_data()
{
    // trying to read something from the device using SMBus READ request
    i2c_data data;
    char command = 0x07; // command 0x06 is reading thermopile sensor, see datasheet for all commands
	struct i2c_smbus_ioctl_data updatedata;
	i2c_read_data(command,&data,&updatedata);
	printf("read_complete\n");
    // do actual request
    if (ioctl(fdev, I2C_SMBUS, &updatedata) < 0) {
        fprintf(stderr, "I2C_SMBUS FAILED, error: %s\n", strerror(errno));
        return -1;
    }

    // calculate temperature in Celsius by formula from datasheet
    double temp = (double) data.word;
    temp = (temp * 0.02)-0.01;
    temp = temp - 273.15;

    // print result
    printf("Tamb = %04.2f\n", temp);
    return temp;
}

void i2c_read_data(int command, i2c_data* data, struct i2c_smbus_ioctl_data* updatedata)
{
	updatedata->read_write = I2C_SMBUS_READ;
        updatedata->command = command;
        updatedata->size = I2C_SMBUS_WORD_DATA; // Assuming is defined elsewhere
        updatedata->data = data;
}

