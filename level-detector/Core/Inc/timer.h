/*
 * timer.h
 *
 *  Created on: Nov 5, 2024
 *      Author: jacob
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include "stm32f1xx_hal.h"

#define MAX_TIME 0xFFFF
/*
 * TIMER control functions
 */
void timer_start(TIM_HandleTypeDef* htim);
void timer_stop(TIM_HandleTypeDef* htim);

#endif /* INC_TIMER_H_ */
