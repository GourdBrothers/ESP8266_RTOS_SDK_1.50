/*-----------------------------------------------------------------
 * 
 * userWifi_tcp.c
 * Socket based UDP communication
 * ----------------------------------------------------------------
 */

#include "esp_common.h"

#include "lwip/sockets.h"

int tcp_socket = 0;


int Fun_tcp_socket_new(char *server_ip, u16_t in_port,int *pSocketId)
{
    if (pSocketId == NULL)
    {
        os_printf("Fun_tcp_socket_new->err:-3\n");
        return -3;
    }

    if ((*pSocketId = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        os_printf("Fun_tcp_socket_new->err:-2\n");
        return -2;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(in_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    memset(server_addr.sin_zero, 0, SIN_ZERO_LEN);
    if (connect(*pSocketId, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) == -1)
    {
        closesocket(*pSocketId);
        os_printf("Fun_tcp_socket_new->err:-1\n");
        return -1;
    }
    os_printf("Fun_tcp_socket_new->ok:%d\n",*pSocketId);
    return 0;
}

int Fun_tcp_socket_close(int *pSocketId)
{
    if (pSocketId == NULL)
    {
        return -1;
    }
    closesocket(*pSocketId);
    return 0;
}
