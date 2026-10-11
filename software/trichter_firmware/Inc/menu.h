/*
 * menu.h
 *
 *  Created on: Oct 11, 2026
 *      Author: moritz
 */

#ifndef MENU_H_
#define MENU_H_

#include "stdbool.h"
#include "stddef.h"

#define MENU_MAX_NUM_ENTRIES 3

typedef struct
{
	char name[16];
	void (*action)(void);
} menu_entry_t;

extern menu_entry_t menu_entries[MENU_MAX_NUM_ENTRIES];
extern size_t menu_selected_entry_idx;

/*
 * Initialize the menu.
 */
void menu_init(void);

/*
 * Add a new entry to the menu.
 *
 * @param name Name that is displayed.
 * @param action Function that is executed when the entry is clicked.
 *
 * @returns -1 if adding the entry failed (no space available), 0 otherwise.
 */
int menu_add_entry(char *name, void (*action)(void));

/*
 * Select the next menu entry.
 *
 * @param direction Select either the next or the previous entry.
 */
void menu_cycle_selected_entry(bool direction);

#endif /* MENU_H_ */
