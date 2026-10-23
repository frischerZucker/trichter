/*
 * flash.h
 *
 *  Created on: Oct 23, 2026
 *      Author: moritz
 */

#ifndef FLASH_H_
#define FLASH_H_

#include "stdint.h"
#include "stddef.h"

#define FLASH_USER_PAGE_BASE_ADDRESS (void *)0x0800f800
#define FLASH_USER_PAGE_NUMBER 31

int flash_write(void *address, uint64_t *data, size_t len);
int flash_read(void *address, uint64_t *dest, size_t len);

/*
 * Erases the page the given address belongs to.
 *
 * EVERYTHING THAT IS STORED ON THIS PAGE WILL BE DELETED!!!
 */
int flash_clear_page(void *address);

#endif /* FLASH_H_ */
