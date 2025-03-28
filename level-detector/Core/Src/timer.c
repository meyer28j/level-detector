/*
 * timer.c
 *
 *  Created on: Nov 5, 2024
 *      Author: jacob
 */
#include "timer.h"

void timer_start(TIM_HandleTypeDef* htim)
{
	HAL_TIM_Base_Start_IT(htim);
}

void timer_stop(TIM_HandleTypeDef* htim)
{
	HAL_TIM_Base_Stop_IT(htim);
}
