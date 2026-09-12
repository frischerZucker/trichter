#include "main.h"
#include "stm32c0xx_it.h"

void NMI_Handler(void)
{
	while (1) {}
}

void HardFault_Handler(void)
{
	while (1) {}
}

void SVC_Handler(void) {}

void PendSV_Handler(void) {}

void SysTick_Handler(void)
{
	HAL_IncTick();
}

void EXTI0_1_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(PIN_FLOW_PULSE);
}

void EXTI4_15_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(PIN_ENCODER_A);
	HAL_GPIO_EXTI_IRQHandler(PIN_ENCODER_B);
}
