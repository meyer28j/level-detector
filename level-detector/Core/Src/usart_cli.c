/*
 * usart-cli.c
 *
 *  Created on: Oct 22, 2024
 *      Author: jacob
 */


#include <usart_cli.h>
#include "version.h" // for printing build info (git hash, etc)

static const char ANSI_ERASE_SCREEN[] = {"\x1b[2J"};
static const char ANSI_SCROLL_WINDOW[] = {"\x1b[6;20r"};
static const char ANSI_CLEAR_LINE[] = {"\033[2K"};
static const char ANSI_SAVE_CURSOR_POS[] = {"\x1b[s"};
static const char ANSI_RETURN_CURSOR_POS[] = {"\x1b[u"};
static const char ANSI_MOVE_TO_STATUS_LINE[] = {"\x1b[H"};//{"\x1b[2;0f"};
static const char ANSI_HIDE_CURSOR[] = {"\x1b[?25l"};
static const char ANSI_SHOW_CURSOR[] = {"\x1b[?25h"};

static const char INTRO_MSG[] = "**Level Detector Debug Terminal**\r\n";
static const char HELP_MSG[] = "Commands:\r\n"
		"(c)lear - Clear the command window\r\n"
		"(h)elp - Display this message again\r\n";
static const char PROMPT[] = "cmd> ";

uint8_t c_pos = 0;
char input[MAX_CLI_LEN] = {'\0'};
char response[MSG_LEN] = {'\0'};


void CLIInit(UART_HandleTypeDef* huart)
{
	// fetch build info
	//snprintf(build_info, MSG_LEN, "%s %s %s (%s)\r\n",
	//		VERSION_BUILD_DATE, VERSION_BUILD_MACHINE, VERSION_TAG, VERSION_BUILD);

	memset(response, 0, strlen(input)); // clear string buffer

	strlcpy(response, ANSI_ERASE_SCREEN, MSG_LEN); 	// erase entire screen
	strlcat(response, INTRO_MSG, MSG_LEN);			// print intro message
	strlcat(response, GIT_COMMIT, MSG_LEN);			// print build info
	strlcat(response, ANSI_SCROLL_WINDOW, MSG_LEN);	// create scroll window
	strlcat(response, HELP_MSG, MSG_LEN);			// display help message
	strlcat(response, PROMPT, MSG_LEN);				// display command prompt

	HAL_UART_Transmit(huart, (uint8_t*)response, strlen(response), TIMEOUT);
	RefreshStatus(huart);
}

void RefreshStatus(UART_HandleTypeDef* huart)
{
	// TODO: move this to be timer based
	AccelData accel_values = read_accelerometer_data();

	char message[MSG_LEN] = {'\0'}; 	// the final combined message to transmit
	char level[64] = {'\0'}; 		// TRUE or FALSE based on x/y/z data
	char z_axis[64] = {'\0'}; 		// displays message of each axis data
	char y_axis[64] = {'\0'};
	char x_axis[64] = {'\0'};
	char z_ascii[20] = {'\0'}; 			// stores integer data as string
	char y_ascii[20] = {'\0'};
	char x_ascii[20] = {'\0'};
	char move_to_status[64] = {'\0'};	// ANSI characters for moving to status line
	char data[MSG_LEN] = {'\0'};		// combined messages for each data line
	char move_to_cmd[64] = {'\0'};		// ANSI characters for returning to cmd> line

	if (is_level(accel_values))
	{
		snprintf(level, MSG_LEN, "%sLEVEL: %s\r\n", ANSI_CLEAR_LINE, "TRUE");
	}
	else
	{
		snprintf(level, MSG_LEN, "%sLEVEL: %s\r\n", ANSI_CLEAR_LINE, "FALSE");
	}

	// convert axis data to ascii
	itoa(accel_values.z, z_ascii, 16);
	itoa(accel_values.y, y_ascii, 16);
	itoa(accel_values.x, x_ascii, 16);

	// populate axis data lines
	snprintf(z_axis, MSG_LEN,"%sZ-AXIS: %s\r\n", ANSI_CLEAR_LINE, z_ascii);
	snprintf(y_axis, MSG_LEN,"%sY-AXIS: %s\r\n", ANSI_CLEAR_LINE, y_ascii);
	snprintf(x_axis, MSG_LEN,"%sX-AXIS: %s\r\n", ANSI_CLEAR_LINE, x_ascii);

	// populate each ANSI char and data sequence
	snprintf(move_to_status, MSG_LEN, "%s%s%s%s",
			ANSI_SAVE_CURSOR_POS,
			ANSI_HIDE_CURSOR,
			ANSI_MOVE_TO_STATUS_LINE,
			INTRO_MSG);
	snprintf(data, MSG_LEN, "%s%s%s%s",
			level,
			x_axis,
			y_axis,
			z_axis);
	snprintf(move_to_cmd, MSG_LEN, "%s%s",
			ANSI_RETURN_CURSOR_POS,
			ANSI_SHOW_CURSOR);

	// combine status line rewrite steps into a single message
	snprintf(message, MSG_LEN, "%s%s%s",
			move_to_status,
			data, move_to_cmd);

	HAL_UART_Transmit(huart, (uint8_t*)move_to_status, strlen(move_to_status), TIMEOUT);
	HAL_UART_Transmit(huart, (uint8_t*)data, strlen(data), TIMEOUT);
	HAL_UART_Transmit(huart, (uint8_t*)move_to_cmd, strlen(move_to_cmd), TIMEOUT);
	//HAL_UART_Transmit(huart, (uint8_t*)message, strlen(message), TIMEOUT);

    return;
}

void HandleInput(UART_HandleTypeDef* huart, uint8_t c)
{
	if (c == '\r' || c == '\n')
	{ // user hits 'Enter' key
		// ensure carriage return
		HAL_UART_Transmit(huart, (uint8_t*)"\r\n", 2, TIMEOUT);

		// set last character as string terminal
		input[c_pos] = '\0';

		if (strcmp(input, "help") == 0
			|| strcmp(input, "h") == 0)
		{
			HAL_UART_Transmit(huart, (uint8_t*)HELP_MSG, strlen(HELP_MSG), TIMEOUT);
		}
		else if (strcmp(input, "clear") == 0
				|| strcmp(input,"c") == 0)
		{ // clear the scroll window, rows 7-20
			strlcpy(response, "\x1b[6;0H", MSG_LEN); // move cursor to row 7
			strlcat(response, "\x1b[0J", MSG_LEN); // clear all rows below
			HAL_UART_Transmit(huart, (uint8_t*)response, strlen(response), TIMEOUT);
		}
		else
		{ // invalid command
			strlcpy(response, "'", MSG_LEN);
			strlcat(response, input, MSG_LEN);
			strlcat(response, "' is an unrecognized command.\r\n", MSG_LEN);
			HAL_UART_Transmit(huart, (uint8_t*)response, strlen(response), TIMEOUT);
		}

		memset(input, 0, strlen(input)); // clear string buffer
		c_pos = 0;

		// display input prompt to user
		HAL_UART_Transmit(huart, (uint8_t*)PROMPT, strlen(PROMPT), TIMEOUT);
	}
	else if (c == 0x8)
	{ // user hits 'Backspace' key
		if (c_pos == 0)
		{
			return; // beginning of input, do nothing
		}
		input[c_pos] = 0;
		c_pos--;
		// display character deletion
		HAL_UART_Transmit(huart, (uint8_t*)"\b \b", 3, TIMEOUT);
	}
	else if (c_pos < MAX_CLI_LEN)
	{
		// disallow user to enter over-size command
		// add typed character to input string
		input[c_pos] = c;
		c_pos++;
		// echo input to user
		HAL_UART_Transmit(huart, &c, sizeof (c), TIMEOUT);
	}
}
