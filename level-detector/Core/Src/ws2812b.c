/*
 * ws2812b.c
 *
 *  Created on: Apr 7, 2025
 *      Author: jacob
 */

#include "ws2812b.h"

uint16_t led_pwm_data[NUM_LEDS][BITS_PER_TRANSFER];

void WS2812B_init()
{
	for (int i = 0; i < NUM_LEDS; i++)
	{
		// turn off all leds
		WS2812B_set_pixel_color(i, 0, 0, 0);
	}

	WS2812B_update();
}

void WS2812B_update()
{
	HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t*)led_pwm_data, (uint16_t)(NUM_LEDS * BITS_PER_TRANSFER));
	HAL_Delay(200);
	HAL_TIM_PWM_Stop_DMA(&htim3, TIM_CHANNEL_1);
	HAL_Delay(200);

void WS2812B_set_pixel_color(uint16_t led_index, uint8_t g, uint8_t r, uint8_t b)
{
	// extract each high and low bit from the 24-bit data series for each pixel's color
	// data sent in MSB first as [8 bits GREEN][8 bits RED][8 bits BLUE]
	for (int i = 0; i < 8; i++)
	{
		led_pwm_data[led_index][i] = (g & (1 << (7 - i))) ? WS2812B_BIT_1_HIGH : WS2812B_BIT_0_HIGH;
	}
	for (int i = 7; i >= 0; i--)
	{
		led_pwm_data[led_index][i + 8] = (r & (1 << (7 - i))) ? WS2812B_BIT_1_HIGH : WS2812B_BIT_0_HIGH;
	}
	for (int i = 7; i >= 0; i--)
	{
		led_pwm_data[led_index][i + 16] = (b & (1 << (7 - i))) ? WS2812B_BIT_1_HIGH : WS2812B_BIT_0_HIGH;
	}
}
