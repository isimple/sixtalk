/* 
 * File: packet.c
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

#include "stkprotocol.h"
#include "stkserver.h"

int stk_server_socket()
{
    int server_fd;
    struct sockaddr_in servaddr;
    int flag = 1;
 
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
        return -1;
    }
 
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(STK_SERVER_PORT);

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *) &flag, sizeof(flag)) == -1) {
		close(fd);
		return -1;
	}
 
    if(bind(server_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
        printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
        close(fd);
        return -1;
    }
 
    if(listen(server_fd, STK_MAX_CLIENTS) == -1){
        printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
        close(fd);
        return -1;
    }
	return server_fd;
 }

int stk_parse_packet(char *buf,int size)
{
    stkp_head *head = (stkp_head *)buf;
    unsigned int stkc_uid;
    int len;
    stk_client *client;

    if (head->stkp_magic != ntohs(STKP_MAGIC)){
        printf("bad stkp magic, drop packet.\n");
        return -1;
    }

    if (head->stkp_version != ntohs(STKP_VERSION)) {
        printf("bad stkp client version, what now?\n");
        return -1;
    }

    if (STKP_TEST_FLAG(head->stkp_flag)) {
        printf("fuck, we get another server message! drop it.\n");
        return -1;
    }

    len = ntohs(head->stkp_length);
    if (*(buf+sizeof(stkp_head)+len) != STKP_PACKET_TAIL) {
        printf("bad stkp message tail! drop it.\n");
        return -1;
    }

    stkc_uid = ntohl(head->stkp_uid);

    client = stk_find_user(stkc_uid);
    if (client == NULL) {
        client = (stk_client *)malloc(sizeof(stk_client));
        if (client == NULL){
            printf("malloc stk client error: %s(errno: %d)\n",strerror(errno),errno);
            return -1;
        }
        stk_add_user(client);
    }

    buf += sizeof(stkp_head);
	return len;
}


int stk_reqlogin_ack()
{

}

int stk_login_ack()
{

}

int stk_keepalive_ack()
{

}
int stk_getuser_ack()
{

}
int stk_getonlineuser_ack()
{

}
int stk_getinfo_ack()
{

}
int stk_sendmsg_ack()
{

}

