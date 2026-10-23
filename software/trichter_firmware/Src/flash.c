/*
 * flash.c
 *
 *  Created on: Oct 23, 2026
 *      Author: moritz
 */
#include "flash.h"

#include "main.h"

int flash_write(void *address, uint64_t *data, size_t len)
{
	// Address must be 64-bit-aligned.
	if (((uintptr_t)address & 0x7) != 0)
	{
		return -1;
	}

	HAL_FLASH_Unlock();

	for (size_t idx = 0; idx < len; idx = idx + 1)
	{
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t)&((uint64_t *)address)[idx], data[idx]) != HAL_OK)
		{
			uint32_t error = HAL_FLASH_GetError();
			HAL_FLASH_Lock();
			return error;
		}
	}

	HAL_FLASH_Lock();

	return HAL_OK;
}

int flash_read(void *address, uint64_t *dest, size_t len)
{
	for (size_t idx = 0; idx < len; idx = idx + 1)
	{
		dest[idx] = ((__IO uint64_t *)address)[idx];
	}

	return HAL_OK;
}


/*
 * Erases the page the given address belongs to.
 *
 * EVERYTHING THAT IS STORED ON THIS PAGE WILL BE DELETED!!!
 */
int flash_clear_page(void *address)
{
	uint32_t page_number = ((uint32_t)address - FLASH_BASE) / FLASH_PAGE_SIZE;

	FLASH_EraseInitTypeDef erase_struct = {
		.TypeErase = FLASH_TYPEERASE_PAGES,
		.Page = page_number,
		.NbPages = 1,
	};
	uint32_t page_error;

	HAL_FLASH_Unlock();

	if (HAL_FLASHEx_Erase(&erase_struct, &page_error) != HAL_OK)
	{
		uint32_t error = HAL_FLASH_GetError();
		HAL_FLASH_Lock();
		return error;
	}

	HAL_FLASH_Lock();

	return HAL_OK;
}
