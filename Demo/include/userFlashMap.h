/*
 *  userFlashMap.h
 *
 */

#ifndef __USER_FLASH_MAP_H__
#define __USER_FLASH_MAP_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define SECTOR_SIZE 4096

#define USER_FLASH_MAX_SECTOR 0x7F
#define USER_FLASH_MIN_SECTOR 0x7C

#define USER_FLASH_MAX_ADDR 0x7FFFF
#define USER_FLASH_MIN_ADDR 0x7C000

#define USER_FLASH_SECTOR_ADDR 0x7C000

#ifdef __cplusplus
}
#endif

#endif
