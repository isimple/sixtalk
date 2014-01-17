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

#include "list.h"
#include "stkprotocol.h"
#include "stkserver.h"

int main(int argc, char argv[])
{
    int server_fd;
    int conn_fd;
    int bytes;
    //int conn_fd[STK_MAX_CLIENTS];
    char buf[STK_MAX_PACKET_SIZE] = {0};
    stk_client *client;

    stk_init_user();

    if ((server_fd = stk_server_socket()) == -1){
        printf("create stkserver socket error!exiting....\n");
        exit(0);
    }

    printf("====================================================\n");
    printf("=============== waiting for stk client  ============\n");
    printf("====================================================\n");

    while(1){
        if((conn_fd = accept(server_fd, (struct sockaddr*)NULL, NULL)) == -1){
            printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
            continue;
        }

        memset(buf, 0, sizeof(buf));
        bytes = recv(conn_fd, buf, STK_MAX_PACKET_SIZE, 0);
        if (bytes == -1){
            printf("recv socket error: %s(errno: %d)",strerror(errno),errno);
            continue;
        }
        if (bytes > STK_MAX_PACKET_SIZE){
            printf("receive packet is too large, drop it.");
            continue;
        }

        client = stk_parse_packet(buf, bytes);
        if (client == NULL || client->stkc_data.len < 0){
            printf("bad stkp message, continue.");
            continue;
        }

        client->stkc_fd = conn_fd;

        switch (client->stkc_data.cmd) {
        case STKP_CMD_REQ_LOGIN:
            stk_reqlogin_ack(client);
            break;
        case STKP_CMD_LOGIN:
            stk_login_ack(client);
            break;
        case STKP_CMD_KEEPALIVE:
            stk_keepalive_ack(client);
            break;
        case STKP_CMD_LOGOUT:
            /* do something */
            break;
        case STKP_CMD_GET_USER:
            stk_getuser_ack(client);
            break;
        case STKP_CMD_GET_ONLINE_USER:
            stk_getonlineuser_ack(client);
            break;
        case STKP_CMD_USER_INFO:
            stk_getinfo_ack(client);
            break;
        case STKP_CMD_SEND_MSG:
            stk_sendmsg_ack(client);
            break;
        default:
            printf("unknow stkp cmd, drop it.");
        }

    }

    close(server_fd);

    return 0;
}
