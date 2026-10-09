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


static u8g2_t u8g2;

static size_t volume_ul = 0;
static size_t volume_ml_int = 0;
static size_t volume_ml_frac = 0;
static bool is_flowing;

static size_t counter = 0;

void display_set_volume(size_t _volume_ul)
{
	volume_ul = _volume_ul;
	volume_ml_int = volume_ul / 1000;
	volume_ml_frac = volume_ul % 1000;
}

void display_set_is_flowing(bool flowing)
{
	is_flowing = flowing;
}

void display_increment_counter(void)
{
	counter = counter + 1;
}

void display_init(I2C_HandleTypeDef *i2c_handle, uint8_t i2c_address)
{
	u8g2_stm32_hal_init(i2c_handle, i2c_address);

	u8g2_Setup_sh1106_i2c_128x64_noname_1(&u8g2, U8G2_R0, u8g2_stm32_hal_i2c, u8g2_stm32_hal_gpio_and_delay);

	u8g2_InitDisplay(&u8g2);
	u8g2_SetPowerSave(&u8g2, 0);

	u8g2_ClearDisplay(&u8g2);
}

void display_draw_main_view(void)
{
	char is_flowing_str[32];
	char volume_str1[32];
	char volume_str2[32];
	char counter_str[32];

	snprintf(is_flowing_str, 32, "is_flowing: %d", is_flowing);
	snprintf(volume_str1, 32, "volume: %u ul", volume_ul);
	snprintf(volume_str2, 32, "volume: %u,%u ml", volume_ml_int, volume_ml_frac);
	snprintf(counter_str, 32, "counter: %u", counter);

	u8g2_SetFont(&u8g2, u8g2_font_5x7_tr);

	u8g2_FirstPage(&u8g2);
	do
	{
		u8g2_DrawFrame(&u8g2, 0, 0, 128, 64);

		u8g2_DrawStr(&u8g2, 10, 15, volume_str1);
		u8g2_DrawStr(&u8g2, 10, 30, volume_str2);
		u8g2_DrawStr(&u8g2, 10, 45, is_flowing_str);

		u8g2_DrawStr(&u8g2, 10, 60, counter_str);

	}
	while (u8g2_NextPage(&u8g2));
}
