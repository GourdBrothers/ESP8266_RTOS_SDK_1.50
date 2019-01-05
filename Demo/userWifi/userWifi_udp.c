/*-----------------------------------------------------------------
 * 
 * userWifi_udp.c
 * Socket based UDP communication
 * ----------------------------------------------------------------
 */

#include "esp_common.h"

#include "lwip/sockets.h"

// udp socket
#define UDP_REMOTE_IP "255.255.255.255"
// #define UDP_REMOTE_IP    "192.168.2.135"
#define UDP_REMOTE_PORT 8080

unsigned char udp_socket = 0;
struct sockaddr_in sAddr;
char udp_rec_Buf[512];
char send_trg = 0;

int udp_socket_new(void)
{
    if ((udp_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        os_printf("udp_socket_new()->err:-1\n");
        return -1;
    }

    memset(&sAddr, 0, sizeof(struct sockaddr_in));
    sAddr.sin_family = AF_INET;
    sAddr.sin_port = htons(UDP_REMOTE_PORT);
    sAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    int ret = 0;
    ret = bind(udp_socket, (struct sockaddr *)&sAddr, sizeof(sAddr));
    if (ret < 0)
    {
        os_printf("usp_socket_new()->bind err:-2\n");
        return -2;
    }

    // char optValue = true;
    // ret = setsockopt(udp_socket, SOL_SOCKET, SO_BROADCAST, (const char *)(&optValue), sizeof(optValue));
    // if (ret < 0)
    // {
    //     os_printf("usp_socket_new()->setsockopt SO_BROADCAST err:-3\n");
    //     return -3;
    // }

    os_printf("udp_socket_new()->ok\n");

    return 0;
}

int udp_socket_read(void)
{
    struct timeval timeout;
    fd_set fdset;

    FD_ZERO(&fdset);
    FD_SET(udp_socket, &fdset);

    timeout.tv_sec = 0;
    timeout.tv_usec = 500 * 1000;

    int ret;
    ret = select(udp_socket + 1, &fdset, NULL, NULL, &timeout);
    if (ret < 0)
    {
        return -1;
    }

    memset(&sAddr, 0, sizeof(struct sockaddr_in));

    int recv_len = 0;
    unsigned int addr_len = sizeof(struct sockaddr_in);
    memset(udp_rec_Buf, 0, 512);
    char rec_Ip_str[16];
    memset(rec_Ip_str, 0, 16);
    unsigned short rec_port = 0;

    if (FD_ISSET(udp_socket, &fdset))
    {
        recv_len = recvfrom(udp_socket, udp_rec_Buf, 512, MSG_DONTWAIT, (struct sockaddr *)&sAddr, &addr_len);
        if (recv_len > 0)
        {
            memcpy(rec_Ip_str, ipaddr_ntoa((const ip_addr_t *)&(sAddr.sin_addr)), 16);
            rec_port = htons(sAddr.sin_port);
            os_printf("udp_socket_read-from [%s:%d]\n", rec_Ip_str, rec_port);
            os_printf("udp_socket_read->ok:%s\n", udp_rec_Buf);
            send_trg = 1;
        }
    }

    return 0;
}

int udp_socket_write(void)
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

    memset(&sAddr, 0, sizeof(struct sockaddr_in));
    sAddr.sin_family = AF_INET;
    sAddr.sin_addr.s_addr = INADDR_BROADCAST;
    sAddr.sin_port = htons(UDP_REMOTE_PORT);

    int send_len = 0;
    unsigned int addr_led = sizeof(struct sockaddr_in);
    memset(udp_rec_Buf, 0, sizeof(udp_rec_Buf));
    memset(udp_rec_Buf, '6', 128);

    send_len = sendto(udp_socket, udp_rec_Buf, 128, 0, (struct sockaddr *)&sAddr, addr_led);
    if (send_len > 0)
    {
        os_printf("udp_socket_write->ok:%d,%s\n", send_len, udp_rec_Buf);
    }

    return 0;
}
