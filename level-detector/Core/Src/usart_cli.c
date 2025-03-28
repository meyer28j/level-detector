/*
 * usart-cli.c
 *
 *  Created on: Oct 22, 2024
 *      Author: jacob
 */


#include <usart_cli.h>

static const char ANSI_ERASE_SCREEN[] = {"\x1b[2J"};
static const char ANSI_SCROLL_WINDOW[] = {"\x1b[5;30r"};
static const char ANSI_CLEAR_LINE[] = {"\033[2K"};
static const char ANSI_SAVE_CURSOR_POS[] = {"\x1b[s"};
static const char ANSI_RETURN_CURSOR_POS[] = {"\x1b[u"};
static const char ANSI_MOVE_TO_STATUS_LINE[] = {"\x1b[2;0f"};

static const char INTRO_MSG[] = "**Level Detector Debug terminal**\r\n";
static const char HELP_MSG[] = "Commands:\r\n"
		"(c)lear - Clear the command window\r\n"
		"(h)elp - Display this message again\r\n";
static const char PROMPT[] = "cmd> ";

uint8_t c_pos = 0;
char input[MAX_CLI_LEN] = {'\0'};
char response[MSG_LEN] = {'\0'};


void CLIInit(UART_HandleTypeDef* huart)
{
	memset(response, 0, strlen(input)); // clear string buffer

	strlcpy(response, ANSI_ERASE_SCREEN, MSG_LEN); 	// erase entire screen
	strlcat(response, INTRO_MSG, MSG_LEN);			// print intro message
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

	char message[MSG_LEN] = {'\0'};
	char level[MSG_LEN] = {'\0'};
	char z_axis[MSG_LEN] = {'\0'};
	char z_ascii[20];
	itoa(accel_values.z, z_ascii, 16); // convert axis data to ascii

	strlcpy(level, ANSI_CLEAR_LINE, MSG_LEN);
	strlcat(level, "LEVEL: ", MSG_LEN);
	strlcat(level, "[UNINITIALIZED]\r\n", MSG_LEN);

	strlcpy(z_axis, ANSI_CLEAR_LINE, MSG_LEN);
	strlcat(z_axis, "Z-AXIS: ", MSG_LEN);
	strlcat(z_axis, z_ascii, MSG_LEN);

	strlcpy(message, ANSI_SAVE_CURSOR_POS, MSG_LEN);
	// move cursor to status line
	strlcat(message, ANSI_MOVE_TO_STATUS_LINE, MSG_LEN);
	// rewrite status line
	strlcat(message, level, MSG_LEN);
	strlcat(message, z_axis, MSG_LEN);
	// move cursor to previous position
	strlcat(message, ANSI_RETURN_CURSOR_POS, MSG_LEN);

	HAL_UART_Transmit(huart, (uint8_t*)message, strlen(message), TIMEOUT);

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
			strlcpy(response, "\x1b[4;0H", MSG_LEN); // move cursor to row 7
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
