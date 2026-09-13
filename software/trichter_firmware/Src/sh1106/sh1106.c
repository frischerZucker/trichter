/*
 * sh1106.c
 *
 *  Created on: Sep 4, 2026
 *      Author: moritz
 */

#include "sh1106.h"
#include "sh1106_commands.h"

static int sh1106_send_command(sh1106_t *sh1106, uint8_t command)
{
	uint8_t i2c_buffer[2]= {SH1106_MSG_TYPE_COMMAND, command};
	int success = HAL_I2C_Master_Transmit(sh1106->i2c_handle, sh1106->i2c_address, i2c_buffer, 2, 1000);

	return success;
}

static int sh1106_send_data(sh1106_t *sh1106, uint8_t data)
{
	uint8_t i2c_buffer[2]= {SH1106_MSG_TYPE_DATA, data};
	int success = HAL_I2C_Master_Transmit(sh1106->i2c_handle, sh1106->i2c_address, i2c_buffer, 2, 1000);

	return success;
}

int sh1106_enable(sh1106_t *sh1106, bool enable)
{
	uint8_t command = SH1106_CMD_DISABLE_DISPLAY;
	if (enable)
	{
		command = SH1106_CMD_ENABLE_DISPLAY;
	}

	int success = sh1106_send_command(sh1106, command);
	if (success != SUCCESS)
	{
		return success;
	}

	sh1106->is_enabled = enable;

	return SUCCESS;
}

int sh1106_send_display_data(sh1106_t *sh1106, uint8_t *data)
{
	for (uint_fast8_t page = SH1106_PAGE_0; page <= SH1106_PAGE_7; page = page + 1)
	{
		int success = sh1106_send_command(sh1106, SH1106_CMD_SET_PAGE_ADDRESS | page);
		if (success != SUCCESS)
		{
			return success;
		}

		sh1106_send_command(sh1106, SH1106_CMD_SET_LOWER_COLUMN_ADDR | (sh1106->start_column & 0x0f));
		sh1106_send_command(sh1106, SH1106_CMD_SET_HIGHER_COLUMN_ADDR | ((sh1106->start_column & 0xf0) >> 4));

		for (uint_fast8_t column_idx = 0; column_idx < sh1106->display_width; column_idx = column_idx + 1)
		{
			success = sh1106_send_data(sh1106, data[page*sh1106->display_width + column_idx]);
			if (success != SUCCESS)
			{
				return success;
			}
		}
	}

	return SUCCESS;
}

int sh1106_data_set_pixel(sh1106_t *sh1106, uint8_t *data, uint8_t x, uint8_t y, bool set)
{
	if (x > sh1106->display_width || y > sh1106->display_height)
	{
		return ERROR;
	}

	uint_fast8_t page = y / 8;
	uint_fast8_t bit_offset = y % 8;

	if (set == true)
	{
		data[page*sh1106->display_width + x] = data[page*sh1106->display_width + x] | (1 << bit_offset);
	}
	else
	{
		data[page*sh1106->display_width + x] = data[page*sh1106->display_width + x] & ~(1 << bit_offset);
	}

	return SUCCESS;
}

int sh1106_init(sh1106_t *sh1106, I2C_HandleTypeDef *i2c_handle, uint8_t i2c_address, uint8_t display_width, uint8_t display_height, uint8_t start_column)
{
	if (display_width > 132 || display_height > 64 || start_column >= 132)
	{
		return ERROR;
	}

	sh1106->i2c_handle = i2c_handle;
	sh1106->i2c_address = i2c_address << 1; // HAL wants the address shifted one bit to the left.

	sh1106->is_enabled = false;

	sh1106->display_width = display_width;
	sh1106->display_height = display_height;
	sh1106->start_column = start_column;

	return SUCCESS;
}
