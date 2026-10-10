/*
 * display.c
 *
 *  Created on: Oct 2, 2026
 *      Author: moritz
 */

#include "display.h"

#include "stdio.h"

#include "u8g2_stm32_hal.h"
#include "u8g2.h"

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

static u8g2_t u8g2;

static bool is_flowing;

static display_view_t view = VIEW_IDLE;

static user_profile_t *user_profile;

void display_set_is_flowing(bool flowing)
{
	is_flowing = flowing;
}

void display_set_view(display_view_t selected_view)
{
	view = selected_view;
}

void display_set_user_profile(user_profile_t *profile)
{
	user_profile = profile;
}

/*
 * Draw a header for the current view.
 */
static void display_draw_header(void)
{
	char *text;
	char *button_text;
	uint8_t button_width;
	uint8_t button_height = 10;

	switch (view) {
		case VIEW_IDLE:
			text = "TRICHTER V0.1";
			button_text = "MENU";
			button_width = u8g2_GetStrWidth(&u8g2, "START") + 4;
			break;

		case VIEW_MENU:
			text = "MENU";
			button_text = "CANCEL";
			button_width = u8g2_GetStrWidth(&u8g2, "CANCEL") + 4;
			break;

		case VIEW_RUNNING:
			text = "MEASURE";
			button_text = "CANCEL";
			button_width = u8g2_GetStrWidth(&u8g2, "CANCEL") + 4;
			break;

		default:
			break;
	}

	u8g2_DrawStr(&u8g2, (DISPLAY_WIDTH - button_width + (button_width - u8g2_GetStrWidth(&u8g2, button_text)) - u8g2_GetStrWidth(&u8g2, text)) / 2, 8, text);
	u8g2_DrawFrame(&u8g2, 0 , 0, DISPLAY_WIDTH - button_width - 2, button_height);
}

void display_init(I2C_HandleTypeDef *i2c_handle, uint8_t i2c_address)
{
	u8g2_stm32_hal_init(i2c_handle, i2c_address);

	u8g2_Setup_sh1106_i2c_128x64_noname_1(&u8g2, U8G2_R0, u8g2_stm32_hal_i2c, u8g2_stm32_hal_gpio_and_delay);

	u8g2_InitDisplay(&u8g2);
	u8g2_SetPowerSave(&u8g2, 0);

	u8g2_ClearDisplay(&u8g2);
}

static void display_draw_idle_view(void)
{
	uint8_t button_width;
	uint8_t button_height;
	char attempts_str[32];
	char all_time_volume_str[32];

	snprintf(all_time_volume_str, 32, "Vol: %u,%u l", user_profile->all_time_volume_ul / 1000000, (user_profile->all_time_volume_ul % 1000000) / 1000);
	snprintf(attempts_str, 32, "Tries: %u", user_profile->attempt_counter);

	u8g2_SetFont(&u8g2, u8g2_font_5x7_tr);

	button_width = u8g2_GetStrWidth(&u8g2, "START") + 4;
	button_height = 10;

	u8g2_FirstPage(&u8g2);
	do
	{
		display_draw_header();

		u8g2_DrawStr(&u8g2, DISPLAY_WIDTH - button_width + (button_width - u8g2_GetStrWidth(&u8g2, "MENU")) / 2, 8, "MENU");
		u8g2_DrawFrame(&u8g2, DISPLAY_WIDTH - button_width , 0, button_width, button_height);

		u8g2_DrawStr(&u8g2, 10, (DISPLAY_HEIGHT / 2) - 10, user_profile->name);
		u8g2_DrawStr(&u8g2, 10, (DISPLAY_HEIGHT / 2), all_time_volume_str);
		u8g2_DrawStr(&u8g2, 10, (DISPLAY_HEIGHT / 2) + 10, attempts_str);

		u8g2_DrawStr(&u8g2, DISPLAY_WIDTH - button_width + (button_width - u8g2_GetStrWidth(&u8g2, "START")) / 2, DISPLAY_HEIGHT - 2, "START");
		u8g2_DrawFrame(&u8g2, DISPLAY_WIDTH - button_width, DISPLAY_HEIGHT - button_height, button_width, button_height);
	}
	while (u8g2_NextPage(&u8g2));
}

static void display_draw_menu_view(void)
{
	uint8_t button_width;
	uint8_t button_height;

	u8g2_SetFont(&u8g2, u8g2_font_5x7_tr);

	button_width = u8g2_GetStrWidth(&u8g2, "CANCEL") + 4;
	button_height = 10;

	u8g2_FirstPage(&u8g2);
	do
	{
		display_draw_header();

		u8g2_DrawStr(&u8g2, DISPLAY_WIDTH - button_width + (button_width - u8g2_GetStrWidth(&u8g2, "CANCEL")) / 2, 8, "CANCEL");
		u8g2_DrawFrame(&u8g2, DISPLAY_WIDTH - button_width , 0, button_width, button_height);

		u8g2_DrawStr(&u8g2, DISPLAY_WIDTH - button_width + (button_width - u8g2_GetStrWidth(&u8g2, "OK")) / 2, DISPLAY_HEIGHT - 2, "OK");
		u8g2_DrawFrame(&u8g2, DISPLAY_WIDTH - button_width, DISPLAY_HEIGHT - button_height, button_width, button_height);
	}
	while (u8g2_NextPage(&u8g2));
}

static void display_draw_main_view(void)
{
	uint8_t button_width;
	uint8_t button_height;
	char is_flowing_str[32];
	char volume_str[32];

	snprintf(is_flowing_str, 32, "is_flowing: %d", is_flowing);
	snprintf(volume_str, 32, "volume: %u,%u ml", user_profile->volume_ul / 1000, user_profile->volume_ul % 1000);

	u8g2_SetFont(&u8g2, u8g2_font_5x7_tr);

	button_width = u8g2_GetStrWidth(&u8g2, "CANCEL") + 4;
	button_height = 10;

	u8g2_FirstPage(&u8g2);
	do
	{
		display_draw_header();

		u8g2_DrawStr(&u8g2, DISPLAY_WIDTH - button_width + (button_width - u8g2_GetStrWidth(&u8g2, "CANCEL")) / 2, 8, "CANCEL");
		u8g2_DrawFrame(&u8g2, DISPLAY_WIDTH - button_width , 0, button_width, button_height);

		u8g2_DrawStr(&u8g2, 10, (DISPLAY_HEIGHT / 2) - 10, volume_str);
		u8g2_DrawStr(&u8g2, 10, (DISPLAY_HEIGHT / 2) + 10, is_flowing_str);

		u8g2_DrawStr(&u8g2, DISPLAY_WIDTH - button_width + (button_width - u8g2_GetStrWidth(&u8g2, "SAVE")) / 2, DISPLAY_HEIGHT - 2, "SAVE");
		u8g2_DrawFrame(&u8g2, DISPLAY_WIDTH - button_width, DISPLAY_HEIGHT - button_height, button_width, button_height);
	}
	while (u8g2_NextPage(&u8g2));
}

void display_draw_view()
{
	switch (view) {
		case VIEW_IDLE:
			display_draw_idle_view();
			break;

		case VIEW_RUNNING:
			display_draw_main_view();
			break;

		case VIEW_MENU:
			display_draw_menu_view();

		default:
			break;
	}
}
