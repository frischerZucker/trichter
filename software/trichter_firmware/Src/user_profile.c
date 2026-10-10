/*
 * user_profile.c
 *
 *  Created on: Oct 10, 2026
 *      Author: moritz
 */
#include "user_profile.h"

#include "stdint.h"
#include "string.h"

#define USER_PROFILE_MAX_NAME_LEN 32

void user_profile_init(user_profile_t *profile, char *name)
{
	profile->volume_ul = 0;
	profile->fastest_attempt_us = SIZE_MAX;
	profile->attempt_counter = 0;

	strncpy(profile->name, name, 31);
	profile->name[31] = '\0';	// If name is >32 chars profile->name would not be null-terminated otherwise.
}
