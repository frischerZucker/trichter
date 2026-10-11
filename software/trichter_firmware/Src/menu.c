/*
 * menu.c
 *
 *  Created on: Oct 11, 2026
 *      Author: moritz
 */
#include "menu.h"

#include "string.h"

menu_entry_t menu_entries[MENU_MAX_NUM_ENTRIES];
size_t menu_selected_entry_idx;
size_t menu_num_entries;

/*
 * Select the next menu entry.
 *
 * @param direction Select either the next or the previous entry.
 */
void menu_cycle_selected_entry(bool direction)
{
	if (direction)
	{
		menu_selected_entry_idx = menu_selected_entry_idx + 1;
		if (menu_selected_entry_idx >= menu_num_entries)
		{
			menu_selected_entry_idx = 0;
		}
	}
	else
	{
		menu_selected_entry_idx = menu_selected_entry_idx - 1;
		if (menu_selected_entry_idx >= menu_num_entries)
		{
			menu_selected_entry_idx = menu_num_entries - 1;
		}
	}
}

/*
 * Add a new entry to the menu.
 *
 * @param name Name that is displayed.
 * @param action Function that is executed when the entry is clicked.
 *
 * @returns -1 if adding the entry failed (no space available), 0 otherwise.
 */
int menu_add_entry(char *name, void (*action)(void))
{
	if (menu_num_entries >= MENU_MAX_NUM_ENTRIES)
	{
		return -1;
	}

	strncpy(menu_entries[menu_num_entries].name, name, 15);
	menu_entries[menu_num_entries].name[15] = '\0';
	menu_entries[menu_num_entries].action = action;

	menu_num_entries = menu_num_entries + 1;

	return 0;
}

/*
 * Initialize the menu.
 */
void menu_init(void)
{
	menu_selected_entry_idx = 0;
	menu_num_entries = 0;
}
