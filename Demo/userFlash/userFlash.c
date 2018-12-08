/*
 * userFlash.c
 *
 */

#include "esp_common.h"

#include "userFlashMap.h"

// SpiFlashOpResult spi_flash_erase_sector(uint16 sec);
// SpiFlashOpResult spi_flash_write(uint32 des_addr, uint32 *src_addr, uint32 size);
// SpiFlashOpResult spi_flash_read(uint32 src_addr, uint32 *des_addr, uint32 size);

/*------------------------------------------------------------
 * XL,20181207
 * return  0: 执行成功
 * return -1: 输入 sector_num,不合法,超出定义的用户数据区
 * return -4: 底层库出错
 */
int Fun_Flash_Erase(unsigned int sector_num)
{
    if (sector_num < USER_FLASH_MIN_SECTOR || sector_num > USER_FLASH_MAX_SECTOR)
    {
        return -1;
    }

    SpiFlashOpResult Libret;
    Libret = spi_flash_erase_sector(sector_num);
    if (SPI_FLASH_RESULT_OK != Libret)
    {
        return -4;
    }

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
int Fun_Flash_WR(int offset,unsigned int *pBuf, unsigned int nBytes)
{
    if (offset < USER_FLASH_MIN_ADDR || offset > USER_FLASH_MAX_ADDR)
    {
        return -1;
    }

    if (pBuf == NULL)
    {
        return -2;
    }

    if (nBytes < 1)
    {
        return -3;
    }

    SpiFlashOpResult Libret;
    Libret = spi_flash_write(offset, pBuf, nBytes);
    if (SPI_FLASH_RESULT_OK != Libret)
    {
        return -4;
    }

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
int Fun_Flash_RD(int offset,unsigned int *pBuf,unsigned int nBytes)
{
    if (offset < USER_FLASH_MIN_ADDR || offset > USER_FLASH_MAX_ADDR)
    {
        return -1;
    }

    if (pBuf == NULL)
    {
        return -2;
    }

    if (nBytes < 1)
    {
        return -3;
    }

    SpiFlashOpResult Libret;
    Libret = spi_flash_read(offset, pBuf, nBytes);
    if (SPI_FLASH_RESULT_OK != Libret)
    {
        return -4;
    }

    return 0;
}
