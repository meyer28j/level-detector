/*
 * ws2812b.c
 *
 *  Created on: Apr 7, 2025
 *      Author: jacob
 */

#include "ws2812b.h"

uint16_t led_pwm_data[RESET_FRAMES + NUM_LEDS][BITS_PER_TRANSFER] = {0};

void WS2812B_init()
{
	WS2812B_clear();

	WS2812B_update();
}

void WS2812B_update()
{
	HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t *)led_pwm_data, (uint16_t)((RESET_FRAMES + NUM_LEDS) * BITS_PER_TRANSFER));
	// TODO: Replace delay + stop call with timer callback
	//		 -> DMA is non-blocking
	HAL_Delay(2);
	HAL_TIM_PWM_Stop_DMA(&htim3, TIM_CHANNEL_1);
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
	if (led_index < 0 || led_index > NUM_LEDS)
	{
		// invalid led_index
		return;
	}
	// set first communicated data as reset code by
	// adjusting led_index to end of prefixed reset frame
	led_index += RESET_FRAMES;

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
		return;
	}

	// maximum number of steps from one side of the matrix to the other
	static const uint8_t NUM_STEPS = 8 - 1;

	// X-AXIS: along breadboard long-side: decides COLUMN of pixel position
	// Y-AXIS: along breadboard short-side : decides ROW of pixel position

	// define line "start" as one of 4 center pixels
	int8_t x0_scaled = NUM_STEPS / 2 ? accel_values.x < 0 : NUM_STEPS / 2 + 1;
	int8_t y0_scaled = NUM_STEPS / 2 ? accel_values.y > 0 : NUM_STEPS / 2 + 1;

	// define line "end" by truncating the axis data to one of 8 positions in the range [0, 7]
	uint8_t x1_scaled = (uint8_t)((accel_values.x + MAX_AT_REST) * NUM_STEPS / (2 * MAX_AT_REST));
	uint8_t y1_scaled = (uint8_t)((accel_values.y + MAX_AT_REST) * NUM_STEPS / (2 * MAX_AT_REST));

	// LEDs cascade serpentine, so even rows must calculate column position from right-to-left
	uint16_t pixel = row_col_to_pixel(x1_scaled, y1_scaled);
	WS2812B_set_pixel_color(pixel, 0, 8, 0);

	return;
}

void bresenham_line(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1)
{
	uint8_t m_new = 2 * (y1 - y0);
	uint8_t slope_error_new = m_new - (x1 - x0);
	for (int x = x0, y = y0; x <= x1; x++)
	{
		WS2812B_set_pixel_color(x + y, 0, 8, 0);
		slope_error_new += m_new;

		if (slope_error_new >= 0)
		{
			y++;
			slope_error_new -= 2 * (x1 - x0);
		}
	}
}

static uint16_t row_col_to_pixel(uint8_t row, uint8_t col)
{
	if (row % 2 == 0)
	{
		return row * 8 + col;
	}
	else
	{
		return row * 8 + (7 - col);
	}
}

int16_t abs_impl(int16_t val)
{
	return val ? val >= 0 : -val;
}
