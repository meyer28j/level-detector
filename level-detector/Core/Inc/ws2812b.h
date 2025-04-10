/**
 * @file ws2812b.h
 * @author Jacob Meyer
 * @brief This file contains the implementation of PWM DMA functions for the WS2812B LED Matrix.
 *
 *
 */

#ifndef INC_WS2812B_H_
#define INC_WS2812B_H_

#include "stm32f1xx_hal.h"
#include "i2c.h" // AccelData struct

// defined in main.c by HAL library and .ioc file generation
extern TIM_HandleTypeDef htim3;
extern DMA_HandleTypeDef hdma_tim3_ch1_trig;

#define WS2812B_BIT_0_HIGH	25	// approx 0.4us
#define WS2812B_BIT_1_HIGH	54	// approx 0.85us

#define NUM_LEDS 64
#define BITS_PER_TRANSFER 24	// 3 colors, 8 bits per color
#define RESET_FRAMES 17			// shortest duration of LOW data to communicate "reset" signal

#define MATRIX_LENGTH 8

// RESET_FRAMES prepended to data to signify a new dataframe is being sent
extern uint16_t led_pwm_data[RESET_FRAMES + NUM_LEDS][BITS_PER_TRANSFER];

void WS2812B_init();

void WS2812B_update();

void WS2812B_clear();

void WS2812B_set_pixel_color(uint16_t led_index, uint8_t g, uint8_t r, uint8_t b);

void WS2812B_point(AccelData accel_values);

void bresenham_line_serpentine(uint8_t row0, uint8_t col0, uint8_t row1, uint8_t col1);

uint16_t row_col_to_pixel(uint8_t row, uint8_t col);

int16_t abs_impl(int16_t val);

#endif /* INC_WS2812B_H_ */
