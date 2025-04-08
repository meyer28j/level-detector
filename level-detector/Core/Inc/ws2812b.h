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

#define WS2812B_BIT_0_HIGH	26	// approx 0.4us
#define WS2812B_BIT_0_LOW	54	// approx 0.8us
#define WS2812B_BIT_1_HIGH	51	// approx 0.85us
#define WS2812B_BIT_1_LOW	29	// approx 0.45us
#define WS2812B_RESET 		79	// full ARR length

#define NUM_LEDS 64
#define BITS_PER_TRANSFER 24

extern uint16_t led_pwm_data[NUM_LEDS][BITS_PER_TRANSFER];

void WS2812B_init();

void WS2812B_update();

void WS2812B_set_pixel_color(uint16_t led_index, uint8_t g, uint8_t r, uint8_t b);

void WS2812B_point(AccelData acceldata);

#endif /* INC_WS2812B_H_ */
