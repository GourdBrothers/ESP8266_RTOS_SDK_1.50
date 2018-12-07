/*
 * userFlash.c
 *
 */

#include "esp_common.h"


//SpiFlashOpResult spi_flash_erase_sector(uint16 sec);

/*------------------------------------------------------------
 * XL,20181207
 * return  0: 执行成功
 * return -1: 输入 sector_num,不合法,超出定义的用户数据区
 * return -4: 底层库出错
 */
int Fun_Flash_sector_erase(unsigned int sector_num)
{
    return 0;
}

/*------------------------------------------------------------
 * XL,20181207
 * return  0: 执行成功
 * return -1: 输入 offset,不合法,超出定义的用户数据区
 * return -2: 输入 pBuf,指针为空
 * return -3: 输入 nBytes，长度为0
 * return -4: 底层库出错
 */
int Fun_Flash_WR(int offset,char *pBuf,unsigned int nBytes)
{
    return 0;
}

/*------------------------------------------------------------
 * XL,20181207
 * return  0: 执行成功
 * return -1: 输入 offset,不合法,超出定义的用户数据区
 * return -2: 输入 pBuf,指针为空
 * return -3: 输入 nBytes，长度为0
 * return -4: 底层库出错
 */
int Fun_Flash_RD(int offset,char *pBuf,unsigned int nBytes)
{
    return 0;
}
