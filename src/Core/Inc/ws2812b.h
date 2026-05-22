/**
 * @file ws2812b.h
 * @author Jacob Meyer
 * @brief This file contains the implementation of PWM DMA functions for the WS2812B LED Matrix.
 *
 * Data is updated in the led_pwm_data array using one or more utility functions. After updating
 * the data, WS2812B_update() must be called to send the data to be displayed on the LED matrix.
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
/**
 * @brief Each pixel can display 3 colors, with 8 bits of data per color
 */
#define BITS_PER_TRANSFER 24
/**
 * @brief The shortest duration of LOW data to communicate the "reset" signal, approximately 50 microseconds.
 */
#define RESET_FRAMES 17
#define MATRIX_LENGTH 8

// RESET_FRAMES prepended to data to signify a new dataframe is being sent
/**
 * @brief The data container for information being sent to the WS2812B LED matrix.
 *
 * The array is prepended by a number of LOW-signal reset frames for each transmission.
 * Data is sent to the matrix after updating this variable by calling the WS2812B_update() function.
 */
extern uint16_t led_pwm_data[RESET_FRAMES + NUM_LEDS][BITS_PER_TRANSFER];

/**
 * @brief Clears the led_pwm_data array and updates the matrix to have all LEDs off.
 */
void WS2812B_init();

/**
 * @brief Calls the HAL PWM DMA function to transmit the led_pwm_data array to the LED matrix.
 */
void WS2812B_update();

/**
 * @brief Clears the led_pwm_data array.
 */
void WS2812B_clear();

/**
 * @brief Sets a specified pixel to the given color combination.
 *
 * This function shifts the given led_index to the end of the reset frame before applying
 * the update to the pixel to ensure the reset frame remains intact.
 *
 * @param[in] led_index the index of the LED to update
 * @param[in] g green color data
 * @param[in] r red color data
 * @param[in] b blue color data
 */
void WS2812B_set_pixel_color(uint16_t led_index, uint8_t g, uint8_t r, uint8_t b);

/**
 * @brief Sets the center 4 pixels to the given color combination.
 *
 * @param[in] g green color data
 * @param[in] r red color data
 * @param[in] b blue color data
 */
void WS2812B_set_center_color(uint8_t g, uint8_t r, uint8_t b);

/**
 * @brief Sets the edges of the LED matrix to the specified color combination if the accelerometer values are out-of-bounds.
 *
 * @param[in] accel_values accelerometer data
 * @param[in] g green color data
 * @param[in] r red color data
 * @param[in] b blue color data
 */
void WS2812B_set_edge_colors(AccelData accel_values, uint8_t g, uint8_t r, uint8_t b);

/**
 * @brief Sets the center LED colors according to how near-level the device
 * is and draws a red line to indicate how the device should be oriented.
 *
 * If the accelerometer data is within the level threshold, this function sets
 * the center LEDs to green and returns. Otherwise it sets the center LED colors
 * and draws a red line by calling bresenham_line_serpentine() according to the
 * given accelerometer data.
 *
 * @param[in] accel_values accelerometer data
 */
void WS2812B_point(AccelData accel_values);

/**
 * @brief Algorithm that updates the led_pwm_data array to draw a red line from one point to another.
 *
 * This algorithm handles the LED positions reversing every second row as the LEDs are positioned
 * in a "serpentine" row-major order.
 *
 * @param[in] row0 the starting pixel row of the line
 * @param[in] col0 the starting pixel column of the line
 * @param[in] row1 the ending pixel row of the line
 * @param[in] col1 the ending pixel column of the line
 */

void bresenham_line_serpentine(uint8_t row0, uint8_t col0, uint8_t row1, uint8_t col1);


/**
 * @brief Converts a given row and column to the LED matrix pixel position according to serpentine row-major order.
 *
 * @param[in] row the pixel row position
 * @param[in] col the pixel column position
 * @return a pixel corresponding to the given row and column
 */
uint16_t row_col_to_pixel(uint8_t row, uint8_t col);

/**
 * @brief An implementation of the abs() function to avoid including the C stdlib
 *
 * @param[in] the number to take the absolute value of
 * @return the absolute value of the given number
 */
int16_t abs_impl(int16_t val);

#endif /* INC_WS2812B_H_ */
