/* 
 * File: stkclient.c
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "stk.h"

client_config client;

int main(int argc, char *argv[])
{
    char recvbuf[STK_MAX_PACKET_SIZE] = {0};
    char sendbuf[STK_MAX_PACKET_SIZE] = {0};
    stk_buddy buddy;
    struct sockaddr_in servaddr;
    int len;
    fd_set rset, wset;
    int max_fd;
    struct timeval to;
    int input_fd;
    int ret;

    if( argc != 3){
        printf("usage: %s <ipaddress> <uid>\n", argv[0]);
        exit(0);
    }

    memset(&client, 0, sizeof(client_config));
    client.uid = atoi(argv[2]);
    input_fd = fileno(stdin);

    if((client.fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }
 
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(STK_SERVER_PORT);
    if( inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0){
        printf("inet_pton error for %s\n",argv[1]);
        exit(0);
    }

    if( connect(client.fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }

    memset(sendbuf, 0, sizeof(sendbuf));

    ret = stk_login(client.fd, sendbuf, STK_MAX_PACKET_SIZE, client.uid);
    if (ret == STK_CLIENT_LOGIN_ERROR){
        printf("======== STK Client [%d] login failed ========\n", client.uid);
        exit(0);
    } else if (ret == STK_CLIENT_LOGIN_INVALID_UID){
        printf("======== STK Client [%d] unregistered ========\n", client.uid);
        exit(0);
    } else if (ret == STK_CLIENT_LOGIN_INVALID_PASS){
        printf("======== STK Client [%d] authenticate failed ========\n", client.uid);
        exit(0);
    }

    memset(&buddy, 0 ,sizeof(stk_buddy));
    ret = stk_send_getprofile(client.fd, sendbuf, STK_MAX_PACKET_SIZE, client.uid, client.uid, &buddy);
    if (ret == -1){
        printf("======== STK Client [%d] get profile failed ========\n", client.uid);
        exit(0);
    } else {
        memcpy(client.nickname, buddy.nickname, STK_NICKNAME_SIZE);
        memcpy(client.city, buddy.city,STK_CITY_SIZE);
        client.phone = buddy.phone;
        client.gender = buddy.gender;
    }

    ret = stk_send_getbuddylist(client.fd, sendbuf, STK_MAX_PACKET_SIZE, client.uid);
    if (ret == -1){
        printf("======== STK Client [%d] get buddy list failed ========\n", client.uid);
        exit(0);
    }

    stk_print_hello();
    printf("Please input command: ");
    fflush(stdout);

    while(1){
        FD_ZERO(&rset);
        FD_ZERO(&wset);

        FD_SET(input_fd,&rset);
        FD_SET(client.fd,&rset);

        max_fd = (input_fd > client.fd) ? input_fd : client.fd;
        to.tv_sec  = STK_CLIENT_TIMEOUT;
        to.tv_usec = 0;

        ret = select(max_fd+1, &rset, &wset, NULL, &to);
        if ( ret < 0){
            /* error happened with select */
            printf("select error: %s(errno: %d)\n",strerror(errno),errno);
            continue;
        } else if ( ret == 0 ) {
            /* select timeout */
        } else { 
            /* data is available */
            if (FD_ISSET(input_fd, &rset)) 
            {
                stk_hanle_input(input_fd, &client);
            }
            if (FD_ISSET(client.fd, &rset))
            {
                if (stk_handle_msg(&client, recvbuf) == STK_SOCKET_CLOSED)
                    break;
            }
        }
    }
    close(client.fd);
    return 0;
}
