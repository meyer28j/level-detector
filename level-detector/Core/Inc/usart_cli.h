/*
 * usart-cli.h
 *
 *  Created on: Oct 22, 2024
 *      Author: jacob
 */

#ifndef INC_USART_CLI_H_
#define INC_USART_CLI_H_

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h> // itoa
#include "stm32f1xx_hal.h"
#include "i2c.h" // access to reading accelerometer

#define MAX_CLI_LEN 30 		// input character limit
#define MSG_LEN 256		// response character limit
#define TIMEOUT 3000		// maximum wait time

extern uint8_t c_pos; 			// input character position, for tracking /b
extern char input[MAX_CLI_LEN];	// input buffer

void CLIInit(UART_HandleTypeDef* huart);
void RefreshStatus(UART_HandleTypeDef* huart);
void HandleInput(UART_HandleTypeDef* huart, uint8_t c);

#endif /* INC_USART_CLI_H_ */
