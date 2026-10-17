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

#define USER_PROFILE_MAX_NAME_LEN 10

typedef struct
{
	char name[USER_PROFILE_MAX_NAME_LEN + 1];
	size_t id;
	size_t volume_ul;
	size_t all_time_volume_ul;
	size_t fastest_attempt_ms_per_l;
	size_t attempt_counter;
	size_t start_time_ms;
	size_t end_time_ms;
} user_profile_t;

extern user_profile_t *selected_profile;
extern user_profile_t *team_profile;

void user_profile_init(void);

int user_profile_add_user(char *name);
void user_profile_remove_current_user(void);
void user_profile_cycle_profiles(bool clockwise);

#endif /* USER_PROFILE_H_ */
