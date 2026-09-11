/*
 * sh1106.c
 *
 *  Created on: Sep 4, 2026
 *      Author: moritz
 */

#include "sh1106.h"

#include "stdio.h"
#include "string.h"

#define SH1106_MSG_TYPE_COMMAND 0x80
#define SH1106_MSG_TYPE_DATA 0x40

#define SH1106_COMMAND_FORCE_DISLAY_ON 0b10100101
#define SH1106_COMMAND_FORCE_DISLAY_OFF 0b10100100

#define SH1106_COMMAND_DISPLAY_ON 0b10101111
#define SH1106_COMMAND_DISPLAY_OFF 0b10101110

static int sh1106_send_command(sh1106_t *sh1106, uint8_t command)
{
	uint8_t i2c_buffer[2]= {SH1106_COMMAND_OPERATION, command};
	int success = HAL_I2C_Master_Transmit(sh1106->i2c_handle, sh1106->i2c_address, i2c_buffer, 2, 1000);

	return success;
}

int sh1106_init(sh1106_t *sh1106, I2C_HandleTypeDef *i2c_handle, uint8_t i2c_address)
{
	sh1106->i2c_handle = i2c_handle;
	sh1106->i2c_address = i2c_address << 1; // HAL wants the address shifted one bit to the left.

	if (sh1106_send_command(sh1106, SH1106_COMMAND_DISPLAY_ON) != HAL_OK)
	{
		return HAL_ERROR;
	}

	HAL_Delay(100);

	if (sh1106_send_command(sh1106, SH1106_COMMAND_FORCE_DISLAY_ON) != HAL_OK)
	{
		return HAL_ERROR;
	}

	HAL_Delay(1000);

	if (sh1106_send_command(sh1106, SH1106_COMMAND_FORCE_DISLAY_OFF) != HAL_OK)
	{
		return HAL_ERROR;
	}

	HAL_Delay(100);

	if (sh1106_send_command(sh1106, SH1106_COMMAND_DISPLAY_OFF) != HAL_OK)
	{
		return HAL_ERROR;
	}

	return 0;
}
