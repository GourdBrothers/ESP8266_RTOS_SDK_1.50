/*-----------------------------------------------------------------
 * 
 * userWifi.c
 * 
 * ----------------------------------------------------------------
 */

#include "esp_common.h"

#include "espconn.h"

#include "lwip/sockets.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "userWifi.h"

//-----------------------------------------------------------------
#define TEST_AP_SSID "xcwk_26b"
#define TEST_AP_PASSWORD "xcwk_26b"

//-----------------------------------------------------------------
extern int usp_socket_new(void);
extern int usp_socket_read(void);
extern int usp_socket_write(void);

//-----------------------------------------------------------------
static int wifi_pro_flow = 0;
int stationStatus = 0;

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

    os_printf("wifi_pro_flow set: %d\n", wifi_pro_flow);
}

int Fun_wifi_get_pro_flow(void)
{
    os_printf("wifi_pro_flow get: %d\n", wifi_pro_flow);
    return wifi_pro_flow;
}

//-----------------------------------------------------------------
void task_wifi_Handle(void *arg)
{
    struct station_config *pConfigValue = NULL;
    struct ip_info sta_ip_info;


    Fun_wifi_set_pro_flow(WIFI_FLOW_GET_PARAM);

    /* need to set opmode before you set config */
    wifi_set_opmode(STATION_MODE);

    os_printf("\n task_wifi_Handle run !\n");

    while (1)
    {

        switch (wifi_pro_flow)
        {
        case WIFI_FLOW_GET_PARAM:
            pConfigValue = (struct station_config *)zalloc(sizeof(struct station_config));
            sprintf((char *)pConfigValue->ssid, TEST_AP_SSID);
            sprintf((char *)pConfigValue->password, TEST_AP_PASSWORD);
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
            switch (stationStatus)
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

            wifi_get_ip_info(STATION_IF, &sta_ip_info);
            os_printf("ssta_ip_info.ip:%d\n", sta_ip_info.ip.addr);
            os_printf("netmask:%d\n", sta_ip_info.netmask.addr);
            os_printf("sta_ip_info.gw:%d\n", sta_ip_info.gw.addr);

            //user_udp_new();
            usp_socket_new();

            Fun_wifi_set_pro_flow(WIFI_FLOW_READ_TCP);

            break;

        case WIFI_FLOW_READ_TCP:
            usp_socket_read();

            extern char send_trg;
            if (send_trg == 1)
            {
                usp_socket_write();
                send_trg = 0;
            }

            break;

        case WIFI_FLOW_WRITE_TCP:

            break;

        case WIFI_FLOW_HOLD:
            break;

        default:
            break;
        }

        vTaskDelay(200 / portTICK_RATE_MS);
    }

    vTaskDelete(NULL);
}