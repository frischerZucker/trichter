/*
 * u8g2_stm32_hal.h
 *
 *  Created on: Oct 2, 2026
 *      Author: moritz
 */

#ifndef U8G2_STM32_HAL_H_
#define U8G2_STM32_HAL_H_

#include "u8g2.h"

#include "main.h"

void u8g2_stm32_hal_init(I2C_HandleTypeDef *i2c_handle, uint8_t i2c_address);

uint8_t u8g2_stm32_hal_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

uint8_t u8g2_stm32_hal_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

#endif /* U8G2_STM32_HAL_H_ */
