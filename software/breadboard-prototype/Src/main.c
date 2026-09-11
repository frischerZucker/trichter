#include "main.h"

#include "stdio.h"

#include "flow_sensor.h"
#include "sh1106.h"

UART_HandleTypeDef huart1;
I2C_HandleTypeDef hi2c1;

static flow_sensor_state_t flow_sensor;

static sh1106_t sh1106;

void HAL_GPIO_EXTI_Rising_Callback(uint16_t gpio_pin)
{
	if (gpio_pin == PIN_FLOW_SENSOR_PULSE)
	{
		HAL_GPIO_TogglePin(PORT_LED, PIN_LED);

		flow_sensor_interrupt(&flow_sensor);
	}
}

void system_clock_init(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	__HAL_FLASH_SET_LATENCY(FLASH_LATENCY_0);

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV4;
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

static void gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOA_CLK_ENABLE();

	HAL_GPIO_WritePin(PORT_LED, PIN_LED, GPIO_PIN_RESET);

	GPIO_InitStruct.Pin = PIN_LED;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(PORT_LED, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = PIN_FLOW_SENSOR_PULSE;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(PORT_FLOW_SENSOR_PULSE, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
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

static void i2c_init(void)
{
	hi2c1.Instance = I2C1;
	hi2c1.Init.Timing = 0x00402D41;
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

int main(void)
{
	HAL_Init();

	system_clock_init();
	gpio_init();
	uart_init();
	i2c_init();

	char tx_buffer[64] = {0};

	flow_sensor_init(&flow_sensor);

	sh1106_init(&sh1106, &hi2c1, 0x3c);


	uint32_t last_time = HAL_GetTick();
	uint32_t now = HAL_GetTick();

	while (1)
	{
		now = HAL_GetTick();

		flow_sensor_update(&flow_sensor);

		if (now - last_time >= 1000)
		{
			size_t len = snprintf(tx_buffer, 64, "volume: %u, flowing? %d\n", flow_sensor_get_volume_ul(&flow_sensor)/1000, flow_sensor.is_flowing);
			HAL_UART_Transmit(&huart1, (uint8_t *) tx_buffer, len, 1000);

			last_time = now;
		}
	}
}

void Error_Handler(void)
{
  __disable_irq();
  while (1) {}
}
