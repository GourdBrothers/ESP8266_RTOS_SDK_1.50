/*-----------------------------------------------------------------
 * 
 * userWifi.c
 * 
 * ----------------------------------------------------------------
 */

#include "esp_common.h"

#include "lwip/sockets.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "userWifi.h"

//-----------------------------------------------------------------
#define TEST_AP_SSID     "xcwk_26b"
#define TEST_AP_PASSWORD "xcwk_26b"


//-----------------------------------------------------------------
int wifi_pro_flow = 0 ;
int stationStatus = 0 ;

int socket_fd=0;
struct sockaddr_in server_addr;
unsigned char udp_msg[6]="12345";
unsigned char from[128];
int fromlen;

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
    int ret;
    

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
                        socket_fd = 0;
                        memset(&server_addr,0,sizeof(struct sockaddr_in));
                        server_addr.sin_family = AF_INET;
                        server_addr.sin_addr.s_addr = INADDR_ANY;
                        server_addr.sin_port = htons(1200);
                        server_addr.sin_len = sizeof(server_addr);
                        break;
                }
                break;

            case WIFI_FLOW_CONNECT_TCP:
                socket_fd = socket(AF_INET,SOCK_DGRAM,0);
                if (socket_fd == -1)
                {
                    os_printf("ESP8266 UDP task > failed to create sock!\n");
                }
                else
                {
                    os_printf("ESP8266 UDP task > sock Ok!:%d\n", socket_fd);
                    Fun_wifi_set_pro_flow(WIFI_FLOW_READ_TCP);
                }
                
                break;

            case WIFI_FLOW_READ_TCP:
                ret = bind(socket_fd,(struct sockaddr *)&server_addr,sizeof(server_addr));
                if(ret!=0)
                {
                    os_printf("ESP8266 UDP task > captdns_task failed to bind sock!\n");
                }else
                {
                    printf("ESP8266 UDP task > bind OK!\n");
                    Fun_wifi_set_pro_flow(WIFI_FLOW_WRITE_TCP);
                }

                break;

            case WIFI_FLOW_WRITE_TCP:
                ret = 0;
                fromlen = 6;
                sendto(socket_fd,(uint8*)udp_msg, ret, 0, (struct sockaddr *)&from, fromlen);

                break;

            case WIFI_FLOW_HOLD:
                break;

            default:
                break;
        }

        vTaskDelay(100/portTICK_RATE_MS);

    }

    vTaskDelete(NULL);

}