/*
 * u8g2_stm32_hal.c
 *
 *  Created on: Oct 2, 2026
 *      Author: moritz
 */

#include "u8g2_stm32_hal.h"

#include "string.h"

static I2C_HandleTypeDef *u8g2_stm32_hal_i2c_handle;
static uint8_t u8g2_stm32_hal_i2c_address_shifted;

void u8g2_stm32_hal_init(I2C_HandleTypeDef *i2c_handle, uint8_t i2c_address)
{
	u8g2_stm32_hal_i2c_handle = i2c_handle;
	u8g2_stm32_hal_i2c_address_shifted = i2c_address << 1;
}

uint8_t u8g2_stm32_hal_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	static uint8_t buffer[32];
	static uint16_t idx;

	switch (msg)
	{
	case U8X8_MSG_BYTE_INIT:
		break;

	case U8X8_MSG_BYTE_SET_DC:
		break;

	case U8X8_MSG_BYTE_START_TRANSFER:
		idx = 0;
		break;

	case U8X8_MSG_BYTE_SEND:
		memcpy(&buffer[idx], arg_ptr, arg_int);
		idx = idx + arg_int;
		break;

	case U8X8_MSG_BYTE_END_TRANSFER:
		HAL_I2C_Master_Transmit(u8g2_stm32_hal_i2c_handle, u8g2_stm32_hal_i2c_address_shifted, buffer, idx, 100);
		break;
	default:
		return 0;
	}
	return 1;
}

uint8_t u8g2_stm32_hal_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	switch (msg)
	{
	case U8X8_MSG_DELAY_MILLI:
		HAL_Delay(arg_int);
		break;
	}
	return 1;
}
