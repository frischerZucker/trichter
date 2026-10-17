/*
 * user_profile.c
 *
 *  Created on: Oct 10, 2026
 *      Author: moritz
 */
#include "user_profile.h"

#include "stdint.h"
#include "string.h"

static user_profile_t profiles[USER_PROFILE_MAX_NUM_PROFILES];
static uint8_t num_profiles;
static uint8_t profile_idx;

static user_profile_t invalid_profile = {
		.all_time_volume_ul = 0,
		.attempt_counter = 0,
		.fastest_attempt_ms_per_l = -1,
		.name = "",
};

user_profile_t *selected_profile;
user_profile_t *team_profile = &profiles[USER_ID_TEAM];

user_profile_t *leader_board_volume[USER_PROFILE_MAX_NUM_PROFILES - 2];
user_profile_t *leader_board_speed[USER_PROFILE_MAX_NUM_PROFILES - 2];
user_profile_t *leader_board_attempts[USER_PROFILE_MAX_NUM_PROFILES - 2];

void user_profile_init(void)
{
	memset(profiles, 0, sizeof(user_profile_t) * USER_PROFILE_MAX_NUM_PROFILES);
	num_profiles = 0;
	profile_idx = USER_ID_TEAM;
	selected_profile = &profiles[USER_ID_TEAM];

	for (size_t idx = 0; idx < USER_PROFILE_MAX_NUM_PROFILES - 2; idx = idx + 1)
	{
		leader_board_volume[idx] = &invalid_profile;
		leader_board_speed[idx] = &invalid_profile;
		leader_board_attempts[idx] = &invalid_profile;
	}

	user_profile_add_user("LEADERBRD"); // Lets hijack the first user profile as placeholder for the leader board!
	user_profile_add_user("TEAM");
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

	profiles[idx].id = idx;
	profiles[idx].volume_ul = 0;
	profiles[idx].fastest_attempt_ms_per_l = SIZE_MAX;
	profiles[idx].attempt_counter = 0;
	profiles[idx].start_time_ms = 0;
	profiles[idx].end_time_ms = 0;

	strncpy(profiles[idx].name, name, USER_PROFILE_MAX_NAME_LEN);
	profiles[idx].name[USER_PROFILE_MAX_NAME_LEN] = '\0';	// If name is >USER_PROFILE_MAX_NAME_LEN chars profile->name would not be null-terminated otherwise.

	if (idx >= 2)
	{
		leader_board_volume[idx - 2] = &profiles[idx];
		leader_board_speed[idx - 2] = &profiles[idx];
		leader_board_attempts[idx - 2] = &profiles[idx];
	}

	num_profiles = num_profiles + 1;

	return 0;
}

/*
 * Delete a user profile.
 */
void user_profile_remove_current_user(void)
{
	/* The shared profile for the team should not be deleted. */
	if (profile_idx == USER_ID_TEAM)
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

/*
 * Sort the leader boards for each of the tracked metrics.
 */
void user_profile_update_leader_board(void)
{
	for (size_t idx1 = 0; idx1 < num_profiles - 2; idx1 = idx1 + 1)
	{
		for (size_t idx2 = 0; idx2 < num_profiles - 2; idx2 = idx2 + 1)
		{
			// Leader board for who drank the most -> more is better
			if (leader_board_volume[idx1]->all_time_volume_ul > leader_board_volume[idx2]->all_time_volume_ul)
			{
				user_profile_t *temp = leader_board_volume[idx1];
				leader_board_volume[idx1] = leader_board_volume[idx2];
				leader_board_volume[idx2] = temp;
			}

			// Leader board for who drank the fastest -> lower is better
			if (leader_board_speed[idx1]->fastest_attempt_ms_per_l < leader_board_speed[idx2]->fastest_attempt_ms_per_l)
			{
				user_profile_t *temp = leader_board_speed[idx1];
				leader_board_speed[idx1] = leader_board_speed[idx2];
				leader_board_speed[idx2] = temp;
			}

			// Leader board for who took the most hits -> more is better
			if (leader_board_attempts[idx1]->attempt_counter > leader_board_attempts[idx2]->attempt_counter)
			{
				user_profile_t *temp = leader_board_attempts[idx1];
				leader_board_attempts[idx1] = leader_board_attempts[idx2];
				leader_board_attempts[idx2] = temp;
			}
		}
	}
}
