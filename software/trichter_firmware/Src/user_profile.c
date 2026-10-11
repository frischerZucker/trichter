/*
 * user_profile.c
 *
 *  Created on: Oct 10, 2026
 *      Author: moritz
 */
#include "user_profile.h"

#include "stdint.h"
#include "string.h"

#define USER_PROFILE_MAX_NUM_PROFILES 10

static user_profile_t profiles[USER_PROFILE_MAX_NUM_PROFILES];
static uint8_t num_profiles;
static uint8_t profile_idx;

user_profile_t *selected_profile;
user_profile_t *team_profile = &profiles[0];

void user_profile_init(void)
{
	memset(profiles, 0, sizeof(user_profile_t) * USER_PROFILE_MAX_NUM_PROFILES);
	num_profiles = 0;
	profile_idx = 0;
	selected_profile = &profiles[0];
}

void user_profile_cycle_profiles(bool clockwise)
{
	if (clockwise)
	{
		profile_idx = profile_idx + 1;
		if (profile_idx >= num_profiles)
		{
			profile_idx = 0;
		}
	}
	else
	{
		profile_idx = profile_idx - 1;
		if (profile_idx >= num_profiles)
		{
			profile_idx = num_profiles - 1;
		}
	}

	selected_profile = &profiles[profile_idx];
}

int user_profile_add_user(char *name)
{
	if (num_profiles >= USER_PROFILE_MAX_NUM_PROFILES)
	{
		return -1;
	}

	size_t idx = num_profiles;

	profiles[idx].volume_ul = 0;
	profiles[idx].fastest_attempt_ms_per_l = SIZE_MAX;
	profiles[idx].attempt_counter = 0;
	profiles[idx].start_time_ms = 0;
	profiles[idx].end_time_ms = 0;

	strncpy(profiles[idx].name, name, USER_PROFILE_MAX_NAME_LEN);
	profiles[idx].name[USER_PROFILE_MAX_NAME_LEN] = '\0';	// If name is >USER_PROFILE_MAX_NAME_LEN chars profile->name would not be null-terminated otherwise.

	num_profiles = num_profiles + 1;

	return 0;
}

/*
 * Delete a user profile.
 */
void user_profile_remove_current_user(void)
{
	/* The shared profile for the team should not be deleted. */
	if (profile_idx == 0)
	{
		return;
	}

	for (size_t idx = profile_idx; idx < num_profiles - 1; idx = idx + 1)
	{
		profiles[idx] = profiles[idx + 1];
	}

	num_profiles = num_profiles - 1;
	profile_idx = profile_idx - 1;
	if (profile_idx >= num_profiles)
	{
		profile_idx = 0;
	}

	selected_profile = &profiles[profile_idx];
}
