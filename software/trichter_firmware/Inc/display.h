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

#include "user_profile.h"

typedef enum
{
	VIEW_IDLE = 0,
	VIEW_RUNNING,
	VIEW_MENU,
} display_view_t;

void display_init(I2C_HandleTypeDef *i2c_handle, uint8_t i2c_address);

void display_draw_view();

void display_set_view(display_view_t selected_view);
void display_set_user_profile(user_profile_t *profile);
void display_set_is_flowing(bool flowing);

#endif /* DISPLAY_H_ */
