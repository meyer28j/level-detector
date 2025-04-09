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
	WS2812B_clear();

	WS2812B_update();
}

void WS2812B_update()
{
	HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t*)led_pwm_data, (uint16_t)(NUM_LEDS * BITS_PER_TRANSFER));
	HAL_Delay(200);
	HAL_TIM_PWM_Stop_DMA(&htim3, TIM_CHANNEL_1);
	HAL_Delay(200);
}

void WS2812B_clear()
{
	for (int i = 0; i < NUM_LEDS; i++)
	{
		// turn off all leds
		WS2812B_set_pixel_color(i, 0, 0, 0);
	}
	WS2812B_update();
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

	// truncate the axis data to one of 8 steps in the range [-4, 4]
	int16_t x_scaled = (int8_t)(((int32_t)accel_values.x * 4) / MAX_AT_REST);
	int16_t y_scaled = (int8_t)(((int32_t)accel_values.y * 4) / MAX_AT_REST);

	uint8_t pixels[abs_impl(x_scaled) + abs_impl(y_scaled)];
	uint8_t y_shift[abs_impl(x_scaled) + abs_impl(y_scaled)];
	// select matrix "row" according to y-axis data
	if (y_scaled < 0)
	{
		for (int i = y_scaled; i < 0; i++)
		{

		}
	}


	// select matrix "column" according to x-axis data

	return;
}

int16_t abs_impl(int16_t val)
{
	return val ? val >= 0 : -val;
}
