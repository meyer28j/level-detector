/**
 * @file usart_cli.h
 * @author Jacob Meyer
 * @brief This file contains the implementation of the UART CLI functions.
 *
 * This module processes input collected from the CLI and processes it
 * according to available commands. It also displays a live-updating status
 * of the linear accelerometer orientation data, including each axes raw data
 * as well as an indicator for if the device is level within a specified tolerance.
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

/**
 * @brief A global variable to store the cursor position in the CLI.
 */
extern uint8_t c_pos; 			// input character position, for tracking /b

/**
 * @brief A global variable for storing the characters of the command the user is currently entering.
 */
extern char input[MAX_CLI_LEN];	// input buffer

/**
 * @brief Initializes the UART peripheral associated with the give handler.
 *
 * This sets up the screen display for the first time and displays the results of the help command.
 *
 * @param[in] huart is the HAL global handler for uart peripherals.
 */
void CLIInit(UART_HandleTypeDef* huart);

/**
 * @brief Refreshes the UART display with updated accelerometer data.
 *
 * @param[in] huart is the HAL global handler for uart peripherals.
 * @param[in] accel_values is LSM303DLHC accelerometer data to be displayed.
 */
void RefreshStatus(UART_HandleTypeDef* huart, AccelData accel_values);

/**
 * @brief Handles individual character input from the UART receive interrupt.
 *
 * This processes the character and adds it to the display at the command line.
 * If a command is executed with enter, it calls the relevant function for that command.
 *
 * @param[in] huart is the HAL global handler for uart peripherals.
 * @param[in] c is the input character.
 */
void HandleInput(UART_HandleTypeDef* huart, uint8_t c);

#endif /* INC_USART_CLI_H_ */
