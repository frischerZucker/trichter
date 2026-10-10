#include "main.h"

#include "stdio.h"
#include "string.h"

#include "display.h"
#include "flow_sensor.h"
#include "trichter_state.h"
#include "user_profile.h"

#define ENCODER_DEBOUNCE_TIME_MS 50

static I2C_HandleTypeDef hi2c1;

static UART_HandleTypeDef huart1;

static flow_sensor_state_t flow_sensor;

static trichter_state_t state = STATE_IDLE;

static bool btn_back = false;
static bool last_btn_back = false;
static bool btn_confirm = false;
static bool last_btn_confirm = false;
static bool btn_push = false;
static bool last_btn_push = false;

static bool display_dirty = true;

static uint32_t encoder_last_tick = 0;
static int encoder_delta = 0;

static void system_clock_init(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct =
	{ 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct =
	{ 0 };

	__HAL_FLASH_SET_LATENCY(FLASH_LATENCY_0);

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		Error_Handler();
	}
}

static void i2c_init(void)
{
	hi2c1.Instance = I2C1;
	hi2c1.Init.Timing = 0x0090194B;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
	{
		Error_Handler();
	}
}

static void uart_init(void)
{
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
}

static void gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct =
	{ 0 };

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitStruct.Pin = PIN_IO0 | PIN_BTN_CONFIRM | PIN_ENCODER_PUSH | PIN_ENCODER_B;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(PORT_IO0, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = PIN_FLOW_PULSE | PIN_ENCODER_A;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(PORT_FLOW_PULSE, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = PIN_BTN_BACK | PIN_IO5 | PIN_IO4 | PIN_IO3 | PIN_IO2 | PIN_IO1;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(PORT_BTN_BACK, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(EXTI0_1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);

	HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
}

static void process_user_input(void)
{
	if (encoder_delta != 0)
	{
		switch (state) {
			case STATE_IDLE:
				bool cycle_direction = encoder_delta > 0;

				user_profile_cycle_profiles(cycle_direction);
				display_set_user_profile(selected_profile);

				display_dirty = true;
				encoder_delta = 0;
				break;

			default:
				break;
		}
	}

	btn_back = !HAL_GPIO_ReadPin(PORT_BTN_BACK, PIN_BTN_BACK);
	if (btn_back && last_btn_back == false)
	{
		switch (state) {
			case STATE_IDLE:
				/* Open the menu. */
				state = STATE_MENU;
				display_set_view(VIEW_MENU);
				break;

			case STATE_MENU:
				/* Close the menu. */
				state = STATE_IDLE;
				display_set_view(VIEW_IDLE);
				break;

			case STATE_RUNNING:
				/* Discard the try. */
				state = STATE_IDLE;
				display_set_view(VIEW_IDLE);
				break;

			default:
				break;
		}
		display_dirty = true;
	}
	last_btn_back = btn_back;

	btn_confirm = !HAL_GPIO_ReadPin(PORT_BTN_CONFIRM, PIN_BTN_CONFIRM);
	if (btn_confirm && last_btn_confirm == false)
	{
		switch (state) {
			case STATE_IDLE:
				selected_profile->volume_ul = 0;
				flow_sensor_reset(&flow_sensor);

				state = STATE_RUNNING;
				display_set_view(VIEW_RUNNING);
				break;

			case STATE_RUNNING:
				selected_profile->all_time_volume_ul = selected_profile->all_time_volume_ul + selected_profile->volume_ul;
				selected_profile->attempt_counter = selected_profile->attempt_counter + 1;

				state = STATE_IDLE;
				display_set_view(VIEW_IDLE);
				break;

			default:
				break;
		}
		display_dirty = true;
	}
	last_btn_confirm = btn_confirm;

	/*
	 * Pushing the encoder button sometimes triggers the flow sensor, glitches the display and/or crashes the device...
	 * Most of the time it does nothing. -> Is it really clicking? In a review the buttons casing was too large.
	 * Right now idk why, so just dont push the button i guess. ¯\_(ツ)_/¯
	 */
//	btn_push = !HAL_GPIO_ReadPin(PORT_ENCODER_PUSH, PIN_ENCODER_PUSH);
//	if (btn_push && last_btn_push == false)
//	{
//		display_increment_counter();
//		display_dirty = true;
//	}
//	last_btn_push = btn_push;
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t gpio_pin)
{
	switch (gpio_pin) {
		case PIN_FLOW_PULSE:
			flow_sensor_interrupt(&flow_sensor);
			break;

		case PIN_ENCODER_A:
			uint32_t now = HAL_GetTick();
			if (now - encoder_last_tick < ENCODER_DEBOUNCE_TIME_MS)
			{
				break;
			}
			encoder_last_tick = now;

			/* counter-clockwise rotation */
			if (HAL_GPIO_ReadPin(PORT_ENCODER_B, PIN_ENCODER_B))
			{
				encoder_delta = encoder_delta - 1;
			}
			/* clockwise rotation */
			else
			{
				encoder_delta = encoder_delta + 1;
			}

			break;

		default:
			break;
	}
}

int main(void)
{
	HAL_Init();

	system_clock_init();
	i2c_init();
	uart_init();
	gpio_init();

	display_init(&hi2c1, 0x3c);

	flow_sensor_init(&flow_sensor);

	user_profile_init();
	user_profile_add_user("1 Joe Biden");
	user_profile_add_user("2 deine mom");
	user_profile_add_user("3 Omen");

	display_set_user_profile(selected_profile);
	display_set_view(VIEW_IDLE);

	while (1)
	{
		process_user_input();

		switch (state) {
			case STATE_IDLE:
				break;

			case STATE_RUNNING:
				if (flow_sensor_update(&flow_sensor))
				{
					selected_profile->volume_ul = flow_sensor_get_volume_ul(&flow_sensor);
					display_set_is_flowing(flow_sensor.is_flowing);
					display_dirty = true;
				}

				break;
			default:
				break;
		}

		if (display_dirty)
		{
			display_draw_view();
			display_dirty = false;
		}
	}
}

void Error_Handler(void)
{
	__disable_irq();
	while (1)
	{
	}
}
