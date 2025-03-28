/*
 * i2c.c
 *
 *  Created on: Mar 27, 2025
 *      Author: jacob
 */

#include "i2c.h"

AccelData read_accelerometer_data()
{
	HAL_StatusTypeDef return_status;
	uint8_t raw_data[LSM303DLHC_DATA_MAX_BYTES]; // data returned in 2 bytes
	AccelData accel_values;

	return_status = HAL_I2C_Mem_Read(hi2c1, LSM303DLHC_ADDRESS_ACCEL | LSM303DLHC_FLAG_AUTO_INCREMENT,
			LSM303DLHC_OUT_X_L_A, 1, raw_data, LSM303DLHC_DATA_MAX_BYTES, HAL_MAX_DELAY);
	if (return_status != HAL_OK)
	{
		accel_values.x = -1;
		accel_values.y = -1;
		accel_values.z = -1;
	}
	else
	{
		accel_values.x = (int16_t)(raw_data[0] | (raw_data[1] << 8));
		accel_values.y = (int16_t)(raw_data[2] | (raw_data[3] << 8));
		accel_values.z = (int16_t)(raw_data[4] | (raw_data[5] << 8));
	}

	return accel_values;
}

