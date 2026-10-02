/*
 * flow_sensor.h
 *
 *  Created on: Sep 2, 2026
 *      Author: moritz
 */

#ifndef FLOW_SENSOR_H_
#define FLOW_SENSOR_H_

#include "stdbool.h"
#include "stddef.h"

typedef struct flow_sensor_state
{
	size_t pulse_count;
	bool is_flowing;

	size_t _last_pulse_count;
} flow_sensor_state_t;

/* Initialize a flow sensor struct. */
void flow_sensor_init(flow_sensor_state_t *sensor);

/* Call this in the main loop. */
void flow_sensor_update(flow_sensor_state_t *sensor);
/* Call this in the external interrupt. */
void flow_sensor_interrupt(flow_sensor_state_t *sensor);

/* Reset the counter and is_flowing flag. */
void flow_sensor_reset(flow_sensor_state_t *sensor);

/* Return the volume since last reset in ul. */
size_t flow_sensor_get_volume_ul(flow_sensor_state_t *sensor);

#endif
