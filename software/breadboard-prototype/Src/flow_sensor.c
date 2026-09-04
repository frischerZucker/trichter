/*
 * flow_sensor.c
 *
 *  Created on: Sep 2, 2026
 *      Author: moritz
 */

#include "flow_sensor.h"

#include "stdint.h"

#include "stm32c0xx_hal.h"

#define FLOW_SENSOR_UL_PER_PULSE 2525
#define FLOW_SENSOR_UPDATE_PERIOD_MS 100

static uint32_t last_update_time = 0;

/* Initialize a flow sensor struct. */
void flow_sensor_init(flow_sensor_state_t *sensor)
{
	sensor->is_flowing = false;
	sensor->pulse_count = 0;
	sensor->_last_pulse_count = 0;
}

/* Call this in the main loop. */
void flow_sensor_update(flow_sensor_state_t *sensor)
{
	uint32_t current_update_time = HAL_GetTick();

	/* Check if the sensor is still generating pulses once per second. */
	if (current_update_time - last_update_time > FLOW_SENSOR_UPDATE_PERIOD_MS)
	{
		/* If the counter did not increase, there is nothing flowing through the sensor. */
		if (sensor->pulse_count > sensor->_last_pulse_count)
		{
			sensor->is_flowing = true;
			sensor->_last_pulse_count = sensor->pulse_count;
		}
		else
		{
			sensor-> is_flowing = false;
		}

		last_update_time = current_update_time;
	}
}

/* Call this in the external interrupt. */
void flow_sensor_interrupt(flow_sensor_state_t *sensor)
{
	sensor->pulse_count = sensor->pulse_count + 1;
}

/* Reset the counter and is_flowing flag. */
void flow_sensor_reset(flow_sensor_state_t *sensor)
{
	sensor->is_flowing = false;
	sensor->pulse_count = 0;
	sensor->_last_pulse_count = 0;
}

/* Return the volume since last reset in ul. */
size_t flow_sensor_get_volume_ul(flow_sensor_state_t *sensor)
{
	return sensor->pulse_count * FLOW_SENSOR_UL_PER_PULSE;
}
