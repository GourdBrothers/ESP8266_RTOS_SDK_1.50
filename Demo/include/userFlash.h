/*
 * userFlash.h
 *
 */

#ifndef __USER_FLASH_H__
#define __USER_FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

#define   USER_FLASH_SECTOR_ADDR  0x7C000

extern int Fun_Flash_sector_erase(unsigned int sector_num);
extern int Fun_Flash_WR(int offset,char *pBuf,unsigned int nBytes);
extern int Fun_Flash_RD(int offset,char *pBuf,unsigned int nBytes);

#ifdef __cplusplus
}
#endif

#endif
