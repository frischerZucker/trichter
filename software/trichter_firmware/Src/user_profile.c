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
#define USER_PROFILE_MAX_NAME_LEN 32

static user_profile_t profiles[USER_PROFILE_MAX_NUM_PROFILES];
static uint8_t num_profiles;
static uint8_t profile_idx;

user_profile_t *selected_profile;

void user_profile_init(void)
{
	memset(profiles, 0, sizeof(user_profile_t) * USER_PROFILE_MAX_NUM_PROFILES);
	num_profiles = 0;
	profile_idx = 0;
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

	profile_idx = num_profiles;

	profiles[profile_idx].volume_ul = 0;
	profiles[profile_idx].fastest_attempt_us = SIZE_MAX;
	profiles[profile_idx].attempt_counter = 0;

	strncpy(profiles[profile_idx].name, name, 31);
	profiles[profile_idx].name[31] = '\0';	// If name is >32 chars profile->name would not be null-terminated otherwise.

	num_profiles = num_profiles + 1;

	selected_profile = &profiles[profile_idx];

	return 0;
}
