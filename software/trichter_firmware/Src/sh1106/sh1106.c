/*
 * sh1106.c
 *
 *  Created on: Sep 4, 2026
 *      Author: moritz
 */

#include "sh1106.h"
#include "sh1106_commands.h"

#include "stdio.h"
#include "string.h"

#define SH1106_MSG_TYPE_COMMAND 0x80
#define SH1106_MSG_TYPE_DATA 0x40

static int sh1106_send_command(sh1106_t *sh1106, uint8_t command)
{
	uint8_t i2c_buffer[2]= {SH1106_MSG_TYPE_COMMAND, command};
	int success = HAL_I2C_Master_Transmit(sh1106->i2c_handle, sh1106->i2c_address, i2c_buffer, 2, 1000);

	return success;
}

int sh1106_init(sh1106_t *sh1106, I2C_HandleTypeDef *i2c_handle, uint8_t i2c_address)
{
	sh1106->i2c_handle = i2c_handle;
	sh1106->i2c_address = i2c_address << 1; // HAL wants the address shifted one bit to the left.

	if (sh1106_send_command(sh1106, SH1106_CMD_ENABLE_DISPLAY) != HAL_OK)
	{
		return HAL_ERROR;
	}

	HAL_Delay(100);

	if (sh1106_send_command(sh1106, SH1106_CMD_SET_ENTIRE_DISPLAY_ON) != HAL_OK)
	{
		return HAL_ERROR;
	}

	HAL_Delay(1000);

	if (sh1106_send_command(sh1106, SH1106_CMD_SET_ENTIRE_DISPLAY_OFF) != HAL_OK)
	{
		return HAL_ERROR;
	}

	HAL_Delay(1000);

	if (sh1106_send_command(sh1106, SH1106_CMD_DISABLE_DISPLAY) != HAL_OK)
	{
		return HAL_ERROR;
	}

	return 0;
}
