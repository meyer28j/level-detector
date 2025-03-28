/*
 * i2c.h
 *
 *  Created on: Mar 27, 2025
 *      Author: jacob
 */

#ifndef SRC_I2C_H_
#define SRC_I2C_H_

#define LSM303DLHC_ADDRESS_ACCEL (0x19 << 1) // subaddress for linear accelerometer
#define LSM303DLHC_OUT_X_L_A (0x2C) // register address for X-axis low byte
#define LSM303DLHC_DATA_MAX_BYTES (6) // 2 bytes of data per axis (X_L, X_H, Y_L, Y_H, Z_L, Z_H)
#define LSM303DLHC_FLAG_AUTO_INCREMENT (0X80) // OR with subaddress to signal that the data register should auto-increment

// HAL handler auto-generated in main.c
extern I2C_HandleTypeDef hi2c1;

typedef struct
{
	int16_t x;
	int16_t y;
	int16_t z;
} AccelData;

// use LSM303 auto-increment to read 6 bytes of consecutive data starting at X_L
AccelData read_accelerometer_data();

#endif /* SRC_I2C_H_ */
