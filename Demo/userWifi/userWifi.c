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
#define TEST_AP_SSID     "xcwk_26b"
#define TEST_AP_PASSWORD "xcwk_26b"


//-----------------------------------------------------------------
int wifi_pro_flow = 0 ;
int stationStatus = 0 ;

// udp 
struct espconn user_udp_espconn;

void user_udp_sent_cb(void *arg)   //发送
{
	os_printf("\r\n发送成功！\r\n");

}

void user_udp_recv_cb(void *arg,char *pdata, unsigned short len)     //接收
 {
	os_printf("接收数据：%s", pdata);

	//每次发送数据确保端口参数不变
	user_udp_espconn.proto.udp = (esp_udp *) os_zalloc(sizeof(esp_udp));
	user_udp_espconn.type = ESPCONN_UDP;
	user_udp_espconn.proto.udp->local_port = 2000;
	user_udp_espconn.proto.udp->remote_port = 8686;
	const char udp_remote_ip[4] = { 255, 255, 255, 255 };
	memcpy(user_udp_espconn.proto.udp->remote_ip, udp_remote_ip, 4);

	espconn_sent((struct espconn *) arg, (uint8*)"已经收到啦！", strlen("已经收到啦!"));
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

                user_udp_espconn.proto.udp = (esp_udp *) os_zalloc(sizeof(esp_udp));//分配空间
                user_udp_espconn.type = ESPCONN_UDP;	 		  // 设置类型为UDP协议
                user_udp_espconn.proto.udp->local_port = 2000;	  // 本地端口
                user_udp_espconn.proto.udp->remote_port = 8686;   // 目标端口
                const char udp_remote_ip[4] = { 255, 255, 255, 255 };  // 目标IP地址（广播）
                memcpy(user_udp_espconn.proto.udp->remote_ip, udp_remote_ip, 4);

                espconn_regist_recvcb(&user_udp_espconn, user_udp_recv_cb);    // 接收
                espconn_regist_sentcb(&user_udp_espconn, user_udp_sent_cb);    // 发送
                espconn_create(&user_udp_espconn);	 		  //建立 UDP 传输

                break;

            case WIFI_FLOW_READ_TCP:

                break;

            case WIFI_FLOW_WRITE_TCP:

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