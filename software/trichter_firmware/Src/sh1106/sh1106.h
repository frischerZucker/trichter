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

typedef struct
{
	I2C_HandleTypeDef *i2c_handle;
	uint8_t i2c_address;
} sh1106_t;

int sh1106_init(sh1106_t *sh1106, I2C_HandleTypeDef *i2c_handle, uint8_t i2c_address);

#endif
