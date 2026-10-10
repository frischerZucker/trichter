/*
 * user_profile.h
 *
 *  Created on: Oct 10, 2026
 *      Author: moritz
 */

#ifndef USER_PROFILE_H_
#define USER_PROFILE_H_

#include "stddef.h"

typedef struct
{
	char name[32];
	size_t volume_ul;
	size_t all_time_volume_ul;
	size_t fastest_attempt_us;
	size_t attempt_counter;
} user_profile_t;

void user_profile_init(user_profile_t *profile, char *name);

#endif /* USER_PROFILE_H_ */
