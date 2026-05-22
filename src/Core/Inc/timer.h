/**
 * @file timer.h
 * @author Jacob Meyer
 * @ brief This file contains the implementation of the timer functions.
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include "stm32f1xx_hal.h"

/**
 * @def MAX_TIME
 * @brief Maximum value for the timer counter.
 *
 * This constant defines the upper limit of the timer's 16-bit counter.
 */
#define MAX_TIME 0xFFFF

/**
 * @brief Starts the specified timer.
 *
 * This function initiates the interrupt-related timer counter.
 * if configured.
 *
 * @param[in] htim The HAL global handler for TIM peripherals.
 */
void timer_start(TIM_HandleTypeDef* htim);

/**
 * @brief Stops the specified timer.
 *
 * This function halts the interrupt-related timer counter.
 *
 * @param[in] htim The HAL global handler for TIM peripherals.
 */
void timer_stop(TIM_HandleTypeDef* htim);

#endif /* INC_TIMER_H_ */
