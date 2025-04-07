/*
 * i2c.c
 *
 *  Created on: Mar 27, 2025
 *      Author: jacob
 */

#include "i2c.h"

HAL_StatusTypeDef accel_init()
{
	uint8_t ctrl_reg1_a_config = 0b01010111; // ODR = 100Hz, LPen = 0, Zen = 1, Yen = 1, Xen = 1

	return HAL_I2C_Mem_Write(&hi2c1, LSM303DLHC_ADDRESS_ACCEL, LSM303DLHC_CTRL_REG1_A, 1,
			&ctrl_reg1_a_config, 1, HAL_MAX_DELAY);
}

AccelData read_accelerometer_data()
{
	HAL_StatusTypeDef return_status;
	uint8_t raw_data[LSM303DLHC_DATA_MAX_BYTES]; // data returned in 2 bytes
	AccelData accel_values;

	return_status = HAL_I2C_Mem_Read(&hi2c1, LSM303DLHC_ADDRESS_ACCEL, (LSM303DLHC_OUT_X_L_A | LSM303DLHC_FLAG_AUTO_INCREMENT),
			1, raw_data, LSM303DLHC_DATA_MAX_BYTES, HAL_MAX_DELAY);
	if (return_status != HAL_OK)
	{
		accel_values.x = -1;
		accel_values.y = -1;
		accel_values.z = -1;
	}
	else
	{
		accel_values.z = (int16_t)(raw_data[0] | (raw_data[1] << 8));
		accel_values.y = (int16_t)(raw_data[2] | (raw_data[3] << 8));
		accel_values.x = (int16_t)(raw_data[4] | (raw_data[5] << 8));
	}

	// enable LED if level
	if (is_level(accel_values))
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	}

	return accel_values;
}

uint8_t is_level(AccelData accel_values)
{
	// enable LED if level
	if (accel_values.y < LEVEL_THRESHOLD && accel_values.y > -LEVEL_THRESHOLD &&
			accel_values.x < LEVEL_THRESHOLD && accel_values.x > -LEVEL_THRESHOLD)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
