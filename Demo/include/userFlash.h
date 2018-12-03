/*
 * userFlash.h
 *
 */

#ifndef __USER_FLASH_H__
#define __USER_FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

extern int Fun_Flash_WR(int offset,char *pBuf,unsigned int nBytes);
extern int Fun_Flash_RD(int offset,char *pBuf,unsigned int nBytes);

#ifdef __cplusplus
}
#endif

#endif
