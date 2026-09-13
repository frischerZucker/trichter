/*
 * sh1106.h
 *
 *  Created on: Sep 4, 2026
 *      Author: moritz
 */

#ifndef SH1106_H_
#define SH1106_H_

#include "stm32c0xx_hal.h"
#include "stm32c0xx_hal_i2c.h"

#include "stdbool.h"

typedef struct
{
	I2C_HandleTypeDef *i2c_handle;
	uint8_t i2c_address;

	bool is_enabled;

	uint_fast8_t start_column;
	uint8_t display_width;
	uint8_t display_height;
} sh1106_t;

int sh1106_enable(sh1106_t *sh1106, bool enable);

int sh1106_send_display_data(sh1106_t *sh1106, uint8_t *data);

int sh1106_data_set_pixel(sh1106_t *sh1106, uint8_t *data, uint8_t x, uint8_t y, bool set);

int sh1106_init(sh1106_t *sh1106, I2C_HandleTypeDef *i2c_handle, uint8_t i2c_address, uint8_t display_width, uint8_t display_height, uint8_t start_column);

#endif
