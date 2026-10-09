/*
 * display.h
 *
 *  Created on: Oct 2, 2026
 *      Author: moritz
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "main.h"

#include "stdbool.h"

void display_init(I2C_HandleTypeDef *i2c_handle, uint8_t i2c_address);

void display_draw_main_view(void);

void display_set_volume(size_t volume_ul);
void display_set_is_flowing(bool is_flowing);
void display_increment_counter(void);
void display_decrement_counter(void);

#endif /* DISPLAY_H_ */
