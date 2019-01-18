/*
 *  userNTP.c
 *  Accessing NTP Server to Acquire Time
 * 
 */

#include "esp_common.h"

#include "lwip/apps/sntp.h"
#include "lwip/apps/sntp_time.h"

// #define NTP0_SERVER_NAME "ntp1.aliyun.com"  //0.cn.pool.ntp.org
// #define NTP1_SERVER_NAME "ntp2.aliyun.com"
// #define NTP2_SERVER_NAME "ntp3.aliyun.com"

#define NTP0_SERVER_NAME "0.cn.pool.ntp.org"
#define NTP1_SERVER_NAME "1.cn.pool.ntp.org"
#define NTP2_SERVER_NAME "2.cn.pool.ntp.org"

int utc_time;

void Fun_userNTP_Init(void)
{
    sntp_stop();

    sntp_setservername(0,NTP0_SERVER_NAME);
    sntp_setservername(1,NTP1_SERVER_NAME);
    sntp_setservername(2,NTP2_SERVER_NAME);

    sntp_init();

    
}

void Fun_userNTP_GetTime(void)
{
    u32_t read_time = 0;
    read_time = sntp_get_current_timestamp();
    if (read_time != 0)
    {
        char *pTime;
        utc_time = read_time;
        os_printf("Get Utctimes:%d,%d\n", utc_time);
        pTime = sntp_get_real_time(utc_time);
        os_printf("Get Utctimes:%s\n", pTime);
    }
}
