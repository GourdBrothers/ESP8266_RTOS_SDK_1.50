/*
 * userFlash.h
 *
 */

#ifndef __USER_FLASH_H__
#define __USER_FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "userFlashMap.h"

extern int Fun_Flash_Erase(unsigned int sector_num);
extern int Fun_Flash_WR(int offset,unsigned int *pBuf,unsigned int nBytes);
extern int Fun_Flash_RD(int offset,unsigned int *pBuf,unsigned int nBytes);

#ifdef __cplusplus
}
#endif

#endif
