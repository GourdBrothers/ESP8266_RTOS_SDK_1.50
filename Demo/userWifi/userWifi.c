/*-----------------------------------------------------------------
 * 
 * userWifi.c
 * 
 * ----------------------------------------------------------------
 */

#include "esp_common.h"

#include "espconn.h"

#include "lwip/sockets.h"
//#include "lwip/def.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "userWifi.h"

//-----------------------------------------------------------------
#define TEST_AP_SSID     "xcwk_26b"
#define TEST_AP_PASSWORD "xcwk_26b"


//-----------------------------------------------------------------
int wifi_pro_flow = 0 ;
int stationStatus = 0 ;

struct ip_info sta_ip_info;

// udp 
struct espconn user_udp_espconn;
char udp_rec_Buf[512];
// static const char udp_remote_ip[4] ICACHE_RODATA_ATTR STORE_ATTR = {192, 168, 2, 135};
static const char udp_remote_ip[4] ICACHE_RODATA_ATTR STORE_ATTR = {255, 255, 255, 255};


void user_udp_recv_cb(void *arg, char *pdata, unsigned short len) //接收
{
    if (len > 0)
    {
        os_printf("user_udp_recv_cb set:len %d,%s\n", len, pdata);
        memset(udp_rec_Buf, 0, 512);
        memcpy(udp_rec_Buf, pdata, len);
        os_printf("UDP data will sending:%s\n", udp_rec_Buf);
        espconn_sent((struct espconn *)arg, (uint8 *)udp_rec_Buf, strlen(udp_rec_Buf));
    }
}

// udp socket
#define UDP_REMOTE_IP    "255.255.255.255"
// #define UDP_REMOTE_IP    "192.168.2.135"
#define UDP_REMOTE_PORT  8080
unsigned char udp_socket = 0;
struct sockaddr_in sAddr;

int user_udp_new(void)
{
    user_udp_espconn.proto.udp = (esp_udp *)os_zalloc(sizeof(esp_udp));
    user_udp_espconn.type = ESPCONN_UDP;
    user_udp_espconn.proto.udp->local_port = 8080;
    user_udp_espconn.proto.udp->remote_port = 8080;
    memcpy(user_udp_espconn.proto.udp->remote_ip, udp_remote_ip, 4);
    //espconn_regist_recvcb(&user_udp_espconn, user_udp_recv_cb);
    //espconn_regist_sentcb(&user_udp_espconn, user_udp_sent_cb);
    char ret;
    ret = espconn_create(&user_udp_espconn); // 建立 UDP 传输
    return ret;
}

int usp_socket_new(void)
{
    memset(&sAddr,0,sizeof(struct sockaddr_in));
    sAddr.sin_family = AF_INET;
    // sAddr.sin_addr.s_addr = inet_addr(UDP_REMOTE_IP);
    sAddr.sin_addr.s_addr = (u32_t)(sta_ip_info.ip.addr);
    sAddr.sin_port = htons(UDP_REMOTE_PORT);

    if ((udp_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
       return -1;
        os_printf("udp_socket_new()->err:-1\n");
    }

    int ret=0;
    ret = bind(udp_socket, (struct sockaddr*)&sAddr,sizeof(sAddr));
    if(ret<0)
    {
       return -2;
       os_printf("usp_socket_new()->err:-2\n");
    }

    os_printf("udp_socket_new()->ok\n");

    return 0;
}

int usp_socket_read(void)
{
    struct timeval timeout;
    fd_set fdset;

    FD_ZERO(&fdset);
    FD_SET(udp_socket, &fdset);

    timeout.tv_sec = 0;
    timeout.tv_usec = 500 * 1000;

    int ret;
    ret = select(udp_socket + 1,&fdset, NULL, NULL, &timeout);
    if (ret < 0)
    {
        return -1;
    }

    memset(&sAddr,0,sizeof(struct sockaddr_in));
    sAddr.sin_family = AF_INET;
    sAddr.sin_addr.s_addr = inet_addr(UDP_REMOTE_IP);
    sAddr.sin_port = htons(UDP_REMOTE_PORT);

    int recv_len=0;
    unsigned int addr_led = sizeof(struct sockaddr_in);
    memset(udp_rec_Buf,0,512);
    if (FD_ISSET(udp_socket, &fdset))
    {
        recv_len = recvfrom(udp_socket, udp_rec_Buf, 512, MSG_DONTWAIT,
                            (struct sockaddr *)&sAddr, &addr_led);
        if (recv_len > 0)
        {
            os_printf("udp_socket_read->ok:%s\n",udp_rec_Buf);
       }
    }

    return 0;
}

int usp_socket_write(void)
{
    struct timeval timeout;
    fd_set fdset;

    FD_ZERO(&fdset);
    FD_SET(udp_socket, &fdset);

    timeout.tv_sec = 0;
    timeout.tv_usec = 500 * 1000;

    int ret;
    ret = select(udp_socket + 1, NULL, &fdset, NULL, &timeout);
    if (ret < 0)
    {
        return -1;
    }

    memset(&sAddr,0,sizeof(struct sockaddr_in));
    sAddr.sin_family = AF_INET;
    sAddr.sin_addr.s_addr = inet_addr(UDP_REMOTE_IP);
    sAddr.sin_port = htons(UDP_REMOTE_PORT);

    int send_len=0;
    unsigned int addr_led = sizeof(struct sockaddr_in);
    memset(udp_rec_Buf,'0',512);
    send_len = sendto(udp_socket, udp_rec_Buf, 512, MSG_DONTWAIT,
                            (struct sockaddr *)&sAddr, addr_led);
    if(send_len>0){
       os_printf("udp_socket_write->ok:%d,%s\n",send_len,udp_rec_Buf);
    }

    return 0;
}

//-----------------------------------------------------------------
void Fun_wifi_set_pro_flow(int newFlow)
{
    if (newFlow < WIFI_FLOW_GET_PARAM || newFlow > WIFI_FLOW_HOLD)
    {
        wifi_pro_flow = WIFI_FLOW_GET_PARAM;
    }
    else
    {
        wifi_pro_flow = newFlow;
    }

    os_printf("wifi_pro_flow set: %d\n",wifi_pro_flow);
}

int  Fun_wifi_get_pro_flow(void)
{
    os_printf("wifi_pro_flow get: %d\n",wifi_pro_flow);
    return wifi_pro_flow;
}

//-----------------------------------------------------------------
void task_wifi_Handle(void *arg)
{
    struct station_config *pConfigValue = NULL;

    Fun_wifi_set_pro_flow(WIFI_FLOW_GET_PARAM);

    /* need to set opmode before you set config */
    wifi_set_opmode(STATION_MODE);

    os_printf("\n task_wifi_Handle run !\n");

    while(1)
    {
        
        switch (wifi_pro_flow)
        {
            case WIFI_FLOW_GET_PARAM:
                pConfigValue = (struct station_config*)zalloc(sizeof(struct station_config));
                sprintf((char*)pConfigValue->ssid,TEST_AP_SSID);
                sprintf((char*)pConfigValue->password,TEST_AP_PASSWORD);
                wifi_station_set_config_current(pConfigValue);
                free(pConfigValue); // memfree

                Fun_wifi_set_pro_flow(WIFI_FLOW_CONNECT_AP);
                break;

            case WIFI_FLOW_CONNECT_AP:
                wifi_station_connect();
                stationStatus = 0;

                Fun_wifi_set_pro_flow(WIFI_FLOW_GET_STATE);
                break;

            case WIFI_FLOW_GET_STATE:
                stationStatus = wifi_station_get_connect_status();
                switch(stationStatus)
                {
                    case STATION_IDLE:
                        os_printf("stationStatus = STATION_IDLE!\n");
                        break;
                    case STATION_CONNECTING:
                        os_printf("stationStatus = STATION_CONNECTING!\n");
                        break;
                    case STATION_WRONG_PASSWORD:
                        os_printf("stationStatus = STATION_WRONG_PASSWORD!\n");
                        break;
                    case STATION_NO_AP_FOUND:
                        os_printf("stationStatus = STATION_NO_AP_FOUND!\n");
                        break;
                    case STATION_CONNECT_FAIL:
                        os_printf("stationStatus = STATION_CONNECT_FAIL!\n");
                        break;
                    case STATION_GOT_IP:
                        os_printf("stationStatus = STATION_GOT_IP!\n");
                        Fun_wifi_set_pro_flow(WIFI_FLOW_CONNECT_TCP);
                        break;
                }
                break;

            case WIFI_FLOW_CONNECT_TCP:

                wifi_get_ip_info(STATION_IF,&sta_ip_info);
                os_printf("ssta_ip_info.ip:%d\n",sta_ip_info.ip.addr);
                os_printf("netmask:%d\n",sta_ip_info.netmask.addr);
                os_printf("sta_ip_info.gw:%d\n",sta_ip_info.gw.addr);

                //user_udp_new();
                usp_socket_new();

                usp_socket_write();
                
                Fun_wifi_set_pro_flow(WIFI_FLOW_READ_TCP);

                break;

            case WIFI_FLOW_READ_TCP:
                usp_socket_read();

                break;

            case WIFI_FLOW_WRITE_TCP:

                break;

            case WIFI_FLOW_HOLD:
                break;

            default:
                break;
        }

        vTaskDelay(200/portTICK_RATE_MS);

    }

    vTaskDelete(NULL);

}