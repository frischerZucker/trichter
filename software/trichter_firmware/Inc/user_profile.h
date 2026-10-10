/*
 * user_profile.h
 *
 *  Created on: Oct 10, 2026
 *      Author: moritz
 */

#ifndef USER_PROFILE_H_
#define USER_PROFILE_H_

#include "stdbool.h"
#include "stddef.h"

typedef struct
{
	char name[32];
	size_t volume_ul;
	size_t all_time_volume_ul;
	size_t fastest_attempt_us;
	size_t attempt_counter;
} user_profile_t;

extern user_profile_t *selected_profile;

void user_profile_init(void);

int user_profile_add_user(char *name);
void user_profile_cycle_profiles(bool clockwise);

user_profile_t *user_profile_get_selected_profile(void);

#endif /* USER_PROFILE_H_ */
