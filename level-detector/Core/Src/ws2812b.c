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
	int8_t x0_scaled = accel_values.x < 0 ? NUM_STEPS / 2 : NUM_STEPS / 2 + 1;
	int8_t y0_scaled = accel_values.y > 0 ? NUM_STEPS / 2 : NUM_STEPS / 2 + 1;

	// define line "end" by truncating the axis data to one of 8 positions in the range [0, 7]
	uint8_t x1_scaled = (uint8_t)((accel_values.x + MAX_AT_REST) * NUM_STEPS / (2 * MAX_AT_REST));
	uint8_t y1_scaled = (uint8_t)((accel_values.y + MAX_AT_REST) * NUM_STEPS / (2 * MAX_AT_REST));

	// LEDs cascade serpentine, so even rows must calculate column position from right-to-left

	bresenham_line_serpentine(x0_scaled, y0_scaled, x1_scaled, y1_scaled);

	return;
}

void bresenham_line_serpentine(uint8_t row0, uint8_t col0, uint8_t row1, uint8_t col1)
{
	int16_t dx = abs_impl(col1 - col0);
	int16_t dy = abs_impl(row1 - row0);
	int16_t sx = (col0 < col1) ? 1 : -1;
	int16_t sy = (row0 < row1) ? 1 : -1;
	int16_t err = dx - dy;

	while(1)
	{
		WS2812B_set_pixel_color(row_col_to_pixel(row0, col0), 0, 8, 0);

		if (row0 == row1 && col0 == col1) break;

		int16_t e2 = 2 * err;
		if (e2 > -dy)
		{
			err -= dy;
			col0 += sx;
		}
		if (e2 < dx)
		{
			err += dx;
			row0 += sy;
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
	return val >= 0 ? val : -val;
}
