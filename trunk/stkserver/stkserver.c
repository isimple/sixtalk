/* 
 * File: stkserver.c
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "stk.h"

int main(int argc, char argv[])
{
    int server_fd;
    int conn_fd;
    int bytes;
    //int conn_fd[STK_MAX_CLIENTS];
    char buf[STK_MAX_PACKET_SIZE] = {0};
    stk_data *data = (stk_data *)malloc(sizeof(stk_data));
    stk_client *client;

    stk_init_user();

    if ((server_fd = stk_server_socket()) == -1){
        printf("create stkserver socket error!exiting....\n");
        exit(0);
    }

    printf("====================================================\n");
    printf("================= STK IM SERVER ====================\n");
    printf("====================================================\n");

    if((conn_fd = accept(server_fd, (struct sockaddr*)NULL, NULL)) == -1){
        printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
    }

    while(1){
#if 0
        if((conn_fd = accept(server_fd, (struct sockaddr*)NULL, NULL)) == -1){
            printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
            continue;
        }
#endif
        memset(buf, 0, sizeof(buf));
        bytes = recv(conn_fd, buf, STK_MAX_PACKET_SIZE, 0);
        if (bytes == -1){
            printf("recv socket error: %s(errno: %d)",strerror(errno),errno);
            continue;
        } else if (bytes > STK_MAX_PACKET_SIZE){
            printf("receive packet is too large, drop it.");
            continue;
        } else if (bytes == 0){
            printf("peer socket has been shutdown.\n");
            break;
        } 

        memset(data, 0, sizeof(stk_data));
        client = stk_parse_packet(buf, bytes, data);
        if (client != NULL ){
            client->stkc_data = data;
        } else if (data->cmd != STKP_CMD_REQ_LOGIN && data->cmd != STKP_CMD_LOGIN) {
            printf("Error happen, Continue.\n");
            continue;
        }

        printf("Get STK Client msg, CMD: %d\n", data->cmd);

        switch (data->cmd) {
        case STKP_CMD_REQ_LOGIN:
            stk_reqlogin_ack(conn_fd, data->uid, buf);
            break;
        case STKP_CMD_LOGIN:
            stk_login_ack(conn_fd, data->uid, buf);
            break;
        case STKP_CMD_KEEPALIVE:
            stk_keepalive_ack(client, buf);
            break;
        case STKP_CMD_LOGOUT:
            /* do something */
            break;
        case STKP_CMD_GET_USER:
            stk_getuser_ack(client, buf);
            break;
        case STKP_CMD_GET_ONLINE_USER:
            stk_getonlineuser_ack(client, buf);
            break;
        case STKP_CMD_GET_INFO:
            stk_getinfo_ack(client, buf);
            break;
        case STKP_CMD_SEND_MSG:
            stk_sendmsg_ack(client, buf);
            break;
        default:
            printf("Unknow STKP CMD, Drop it.");
            break;
        }

    }

    close(server_fd);

    return 0;
}
