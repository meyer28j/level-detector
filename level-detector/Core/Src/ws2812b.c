/*
 * ws2812b.c
 *
 *  Created on: Apr 7, 2025
 *      Author: jacob
 */

#include "ws2812b.h"

uint32_t led_pwm_data[NUM_LEDS][BITS_PER_TRANSFER];

void WS2812B_init()
{
	WS2812B_clear();

	WS2812B_update();
}

void WS2812B_update()
{
	HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t *)led_pwm_data, (uint16_t)(NUM_LEDS * BITS_PER_TRANSFER));
	HAL_Delay(2);
	HAL_TIM_PWM_Stop_DMA(&htim3, TIM_CHANNEL_1);
	HAL_Delay(2);
}

void WS2812B_clear()
{
	for (int i = 0; i < NUM_LEDS; i++)
	{
		// turn off all leds
		WS2812B_set_pixel_color(i, 0, 0, 0);
	}
}

void WS2812B_set_pixel_color(uint16_t led_index, uint8_t g, uint8_t r, uint8_t b)
{
	// set first communicated data as reset code

	// extract each high and low bit from the 24-bit data series for each pixel's color
	// data sent in MSB first as [8 bits GREEN][8 bits RED][8 bits BLUE]
	for (int i = 0; i < 8; i++)
	{
		led_pwm_data[led_index][i] = (g & (1 << (7 - i))) ? WS2812B_BIT_1_HIGH : WS2812B_BIT_0_HIGH;
	}
	for (int i = 0; i < 8; i++)
	{
		led_pwm_data[led_index][i + 8] = (r & (1 << (7 - i))) ? WS2812B_BIT_1_HIGH : WS2812B_BIT_0_HIGH;
	}
	for (int i = 0; i < 8; i++)
	{
		led_pwm_data[led_index][i + 16] = (b & (1 << (7 - i))) ? WS2812B_BIT_1_HIGH : WS2812B_BIT_0_HIGH;
	}
}

void WS2812B_point(AccelData accel_values)
{
	WS2812B_clear();

	// if level, display center 4 LEDs as green; return
	if (is_level(accel_values))
	{
		static const uint8_t CENTER_PIXELS[4] = {27, 28, 35, 36};
		for (int i = 0; i < sizeof(CENTER_PIXELS) / sizeof(CENTER_PIXELS[0]); i++)
		{
			WS2812B_set_pixel_color(CENTER_PIXELS[i], 32, 0, 0);
		}
		WS2812B_update();
		return;
	}

	// X-AXIS: along breadboard long-side
	// Y-AXIS: along breadboard short-side

	// truncate the axis data to one of 8 steps in the range [0, 7]
	uint8_t x_scaled = (uint8_t)((accel_values.x + MAX_AT_REST) * 7 / (2 * MAX_AT_REST));
	int16_t y_scaled = (uint8_t)((accel_values.y + MAX_AT_REST) * 7 / (2 * MAX_AT_REST));

	uint16_t pixel = x_scaled * 8 + y_scaled;

	WS2812B_set_pixel_color(pixel, 0, 8, 0);
	// select matrix "row" according to y-axis data



	// select matrix "column" according to x-axis data

	return;
}

int16_t abs_impl(int16_t val)
{
	return val ? val >= 0 : -val;
}
