/*
    This is a test for my algorithm to store user data on flash.
*/
#include "stddef.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define PAGE_SIZE 0x800
#define PAGE_SIZE_64 PAGE_SIZE / 8

#define USER_PROFILE_MAX_NUM_PROFILES 10
#define USER_PROFILE_MAX_NAME_LEN 10

#define ROUND_UP(x, y) (((x / y) + 1) * y)

typedef struct
{
    char name[USER_PROFILE_MAX_NAME_LEN + 1];   // has to be saved
    uint32_t id;                                // has to be saved
    uint32_t volume_ul;                         // only relavant for a running measurement -> no need to save
    uint32_t all_time_volume_ul;                // has to be saved
    uint32_t fastest_attempt_ms_per_l;          // has to be saved
    uint32_t attempt_counter;                   // has to be saved
    uint32_t start_time_ms;                     // only relavant for a running measurement -> no need to save
    uint32_t end_time_ms;                       // only relavant for a running measurement -> no need to save
} user_profile_t;

/*
    Excludes temporary user data. -> Less space required.
*/
typedef struct
{
    char name[USER_PROFILE_MAX_NAME_LEN + 1];
    uint32_t id;
    uint32_t all_time_volume_ul;
    uint32_t fastest_attempt_ms_per_l;
    uint32_t attempt_counter;
} save_file_user_data_t;

/*
    Header for searching saves in flash.
*/
typedef struct
{
    uint8_t id;         // 0 -> invalid, >0 -> id of the current header (first header is 1, second 2, ...; resets when the page is deleted)
    uint8_t num_users;  // 255 should be more than enough
} save_file_header_t;

/*
    Dummy page. The STM32C051 uses 2kB pages that can be accessed with 64b granularity.
*/
static char page[PAGE_SIZE];
static uint64_t *page_64 = (uint64_t *)page;

/*
    User profiles that need to be saved.
*/
user_profile_t profiles[USER_PROFILE_MAX_NUM_PROFILES];

/*
    Write some data to a profile.
*/
void set_profile(user_profile_t *p, char *name, uint32_t id, uint32_t vol, uint32_t speed, uint32_t counter)
{
    p[id].attempt_counter = counter;
    p[id].fastest_attempt_ms_per_l = speed;
    p[id].all_time_volume_ul = vol;

    strncpy(p[id].name, name, USER_PROFILE_MAX_NAME_LEN);
    p[id].name[USER_PROFILE_MAX_NAME_LEN] = '\0';
}

/*
    Calculate how much space a save file needs for [num_users] users.
*/
size_t get_save_file_size(size_t num_users)
{
    return ROUND_UP((sizeof(save_file_header_t) + num_users * sizeof(save_file_user_data_t)), 64);
}

/*
    Delete the content of the whole page.

    The uC only allows clearing the whole page at once. I want to minimize this ot reduce wear of the flash.
*/
void clear_page(uint64_t *p)
{
    memset(p, 0, PAGE_SIZE);
}

/*
    Looks for the last valid header that can be found at the page.
    Stops when a header with an invalid id (=0) is encountered.
*/
void *find_newest_header(void *p)
{
    size_t idx = 0;
    save_file_header_t *header = NULL;

    // There is no valid header at the beginning -> There are no headers -> NULL 
    if (((save_file_header_t *)p)->id == 0)
    {
        return NULL;
    }

    while (idx < PAGE_SIZE)
    {
        save_file_header_t *temp = (save_file_header_t *)&p[idx];

        if (temp->id > 0)
        {
            idx = idx + get_save_file_size(temp->num_users);
            header = temp;
        }
        else
        {
            break;
        }
    }

    return (void *)header;
}

/*
    Find space to store the next save file.

    Finds the last header that was saved.
    If there is none, data can be saved at the beginning of the page.
    Otherwise calculates the size of the last save file and offsets its headers address by this amount to get a base address for the next save file.

    Checks if there is enough space to save the save file.

    Returns NULL if there is not enough space, otherwise the address of the save file.
*/
void *find_place_for_next_header(void *p_base, size_t num_users, uint8_t *next_id)
{
    void *last_haeder = find_newest_header(p_base);

    // There is no header -> Next header can be saved at the beginning of p
    if (last_haeder == NULL)
    {
        *next_id = 1;
        return p_base; 
    }

    *next_id = ((save_file_header_t *)last_haeder)->id + 1;

    void *new_header = last_haeder + get_save_file_size(((save_file_header_t *)last_haeder)->num_users);

    printf("[find_place_for_next_header]: %lu\n", new_header - p_base);

    if (new_header - p_base + get_save_file_size(((save_file_header_t *)last_haeder)->num_users) > PAGE_SIZE)
    {
        return NULL;
    }

    return new_header;
}

/*
    Save data to an address.
*/
void save_data(void *p, user_profile_t *users, size_t num_users, uint8_t header_id)
{
    save_file_user_data_t save_data[num_users];

    for (size_t idx = 0; idx < num_users; idx = idx + 1) {
        save_data[idx].attempt_counter = users[idx].attempt_counter;
        save_data[idx].fastest_attempt_ms_per_l = users[idx].fastest_attempt_ms_per_l;
        save_data[idx].all_time_volume_ul = users[idx].all_time_volume_ul;

        strncpy(save_data[idx].name, users[idx].name, USER_PROFILE_MAX_NAME_LEN);
        save_data[idx].name[USER_PROFILE_MAX_NAME_LEN] = '\0';
    }

    save_file_header_t header = {
        .id = header_id,
        .num_users = num_users,
    };

    printf("[save_data]: header before saving: id=%d, num_users=%d\n", ((save_file_header_t *)p)->id, ((save_file_header_t *)p)->num_users);
    memcpy(p, (void *)&header, sizeof(save_file_header_t));
    printf("[save_data]: header after saving: id=%d, num_users=%d\n", ((save_file_header_t *)p)->id, ((save_file_header_t *)p)->num_users);
    memcpy(p + sizeof(save_file_header_t), (void *)save_data, sizeof(save_data));
}

/*
    Save a save file.

    Searches for an address for the save file.
    Saves the data to that address if there is enough space.
    Returns an error if not. 
*/
int save_file(void *p, user_profile_t *users, size_t num_users)
{
    uint8_t header_id;

    void *save_file_ptr = find_place_for_next_header(p, num_users, &header_id);
    printf("[save_file]: File @ %lu\n", save_file_ptr - p);

    if (save_file_ptr == NULL)
    {
        printf("There is not enough space to save the data.\n");
        return -1;
    }

    save_data(save_file_ptr, users, num_users, header_id);

    return 0;
}

int main(int argc, char **argv)
{
    set_profile(profiles, "leaderbrd", 0, 0, 0, 0);
    set_profile(profiles, "team", 0, 0, 0, 0);
    set_profile(profiles, "p1", 0, 0, 0, 0);
    set_profile(profiles, "p2", 0, 0, 0, 0);
    set_profile(profiles, "p3", 0, 0, 0, 0);
    set_profile(profiles, "p4", 0, 0, 0, 0);
    set_profile(profiles, "p5", 0, 0, 0, 0);
    set_profile(profiles, "p6", 0, 0, 0, 0);
    set_profile(profiles, "p7", 0, 0, 0, 0);
    set_profile(profiles, "p8", 0, 0, 0, 0);


    clear_page(page_64);

    size_t save_file_size = (sizeof(save_file_header_t) + (USER_PROFILE_MAX_NUM_PROFILES - 1) * sizeof(save_file_user_data_t));
    size_t save_file_size_64_aligned = ROUND_UP((sizeof(save_file_header_t) + (USER_PROFILE_MAX_NUM_PROFILES - 1) * sizeof(save_file_user_data_t)), 64);

    printf("[Sizes]\nHeader: %lu\nUser data: %lu\n", sizeof(save_file_header_t), sizeof(save_file_user_data_t));
    printf("Save file size: %lu\n Save file size (64 aligned): %lu\n", save_file_size, save_file_size_64_aligned);
    printf("A page could store %lu (not aligned) / %lu (64 aligned) saves @ %d users\n", PAGE_SIZE / save_file_size, PAGE_SIZE / save_file_size_64_aligned, USER_PROFILE_MAX_NUM_PROFILES - 1);

    printf("------------------------\n");

    // save_data((void *)page_64, profiles, USER_PROFILE_MAX_NUM_PROFILES - 1, 1);

    for (size_t n = 0; n < 10; n = n + 1)
    {
        printf("--- file %lu ---\n", n + 1);

        int success = save_file(page_64, profiles, USER_PROFILE_MAX_NUM_PROFILES - 1);
        if (success == 0)
        {
            printf("Successfully save file.\n");
        }
        else
        {
            printf("Could not save the file: Out of memory.\nClear the page & save again...\n");
            
            clear_page((void *)page_64);
            
            int success = save_file(page_64, profiles, USER_PROFILE_MAX_NUM_PROFILES - 1);
            if (success == 0)
            {
                printf("Successfully save file.\n");
            }
            else
            {
                printf("Could not save the file: Clearing the page did not help.\n");
            }
        }
    }

    return 0;
}