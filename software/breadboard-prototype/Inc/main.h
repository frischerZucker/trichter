#ifndef __MAIN_H
#define __MAIN_H

#include "stm32c0xx_hal.h"

#define PIN_LED GPIO_PIN_4
#define PORT_LED GPIOA

#define PIN_FLOW_SENSOR_PULSE GPIO_PIN_5
#define PORT_FLOW_SENSOR_PULSE GPIOA

void Error_Handler(void);

#endif
