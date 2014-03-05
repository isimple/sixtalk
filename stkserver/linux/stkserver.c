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
#include <pthread.h>
#include <signal.h>

#include "stk.h"

void *stk_main(void *arg)
{
    int fd;
    char buf[STK_MAX_PACKET_SIZE] = {0};
    stk_client *client = NULL;
    stk_data *data = NULL;
    int bytes;

    memcpy(&fd, arg, sizeof(fd));

    data = (stk_data *)malloc(sizeof(stk_data));
    signal(SIGUSR1, stk_deliver_msg);

    while(1) {
        stk_client *tmp_client = NULL;

        bytes = recv(fd, buf, STK_MAX_PACKET_SIZE, 0);
        if (bytes == -1){
            printf("recv socket error: %s(errno: %d)",strerror(errno),errno);
            continue;
        } else if (bytes > STK_MAX_PACKET_SIZE){
            printf("receive packet is too large, drop it.");
            continue;
        } else if (bytes == 0){
            printf("peer socket has been shutdown.\n");
            if (client != NULL) {
                stk_user_offline(client);
                client->stkc_state = STK_CLIENT_OFFLINE;
            }
            break;
        } 

        memset(data, 0, sizeof(stk_data));
        tmp_client = stk_parse_packet(buf, bytes, data);

        if (client == NULL) {
            if (data->cmd != STKP_CMD_REQ_LOGIN && data->cmd != STKP_CMD_LOGIN) {
                printf("Error happen, Continue.\n");
                continue;
            }
            client = tmp_client;
        } else {
            client->stkc_data = data;
            if (client != tmp_client) {
                printf("Oops! who's msg!\n");
            }
        }

        printf("Get STK Client msg, CMD: %d\n", data->cmd);

        switch (data->cmd) {
        case STKP_CMD_REQ_LOGIN:
            stk_reqlogin_ack(fd, data->uid, buf);
            break;
        case STKP_CMD_LOGIN:
            stk_login_ack(fd, data->uid, buf);
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
        case STKP_CMD_GET_USER_INFO:
            stk_getuserinfo_ack(client, buf);
            break;
        case STKP_CMD_GET_GROUP:
            stk_getgroup_ack(client, buf);
            break;
        case STKP_CMD_GET_GROUP_INFO:
            stk_getgroupinfo_ack(client, buf);
            break;
        case STKP_CMD_SEND_MSG:
            stk_sendmsg_ack(client, buf, bytes);
            break;
        case STKP_CMD_SEND_GMSG:
            stk_sendgmsg_ack(client, buf, bytes);
            break;
        default:
            printf("Unknow STKP CMD, Drop it.");
            break;
        }
    }
    free(data);
    close(fd);
}

int main(int argc, char argv[])
{
    int server_fd;
    int conn_fd;
    pthread_t tid;
    int err;

    stk_init_group();
    stk_init_user();

    if ((server_fd = stk_server_socket()) == -1){
        printf("create stkserver socket error!exiting....\n");
        exit(0);
    }

    printf("====================================================\n");
    printf("================= STK IM SERVER ====================\n");
    printf("====================================================\n");

    while(1){
        if((conn_fd = accept(server_fd, (struct sockaddr*)NULL, NULL)) == -1){
            printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
            continue;
        }

        err = pthread_create(&tid, NULL, stk_main, (void *)&conn_fd);
        if (err != 0) {
            printf("can't create thread: %s\n", strerror(err));
        }

    }

    //pthread_join();
    stk_clear_user();
    stk_clear_group();

    close(server_fd);

    return 0;
}
