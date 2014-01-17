/* 
 * File: packet.c
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "stkprotocol.h"
#include "stkclient.h"

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
		close(server_fd);
		return -1;
	}
 
    if(bind(server_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
        printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
        close(server_fd);
        return -1;
    }
 
    if(listen(server_fd, STK_MAX_CLIENTS) == -1){
        printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
        close(server_fd);
        return -1;
    }
	return server_fd;
 }

stk_client *stk_parse_packet(char *buf,int size)
{
    stkp_head *head = (stkp_head *)buf;
    unsigned int uid;
    int len;
    stk_client *client;

    if (head->stkp_magic != ntohs(STKP_MAGIC)){
        printf("bad stkp magic, drop packet.\n");
        return NULL;
    }

    if (head->stkp_version != ntohs(STKP_VERSION)) {
        printf("bad stkp client version, what now?\n");
        return NULL;
    }

    if (STKP_TEST_FLAG(head->stkp_flag)) {
        printf("fuck, we get another server message! drop it.\n");
        return NULL;
    }

    len = ntohs(head->stkp_length);
    if (*(buf+sizeof(stkp_head)+len) != STKP_PACKET_TAIL) {
        printf("bad stkp message tail! drop it.\n");
        return NULL;
    }

    uid = ntohl(head->stkp_uid);

    client = stk_find_user(uid);
    if (client == NULL) {
        printf("bad stk user, please tell stkc to register.\n");
        return NULL;
    }

    if ((client->stkc_state == STK_CLIENT_STATE_ONLINE) 
        && (client->stkc_token != ntohl(head->stkp_token))) {
        client->stkc_state == STK_CLIENT_STATE_OFFLINE;
	}

    //buf += sizeof(stkp_head);

    client->stkc_data.cmd = ntohs(head->stkp_cmd);
	client->stkc_data.data = buf;
	client->stkc_data.len = len;
	return client;
}


int stk_reqlogin_ack(stk_client *client)
{

}

int stk_login_ack(stk_client *client)
{

}

int stk_keepalive_ack(stk_client *client)
{

}
int stk_getuser_ack(stk_client *client)
{

}
int stk_getonlineuser_ack(stk_client *client)
{

}
int stk_getinfo_ack(stk_client *client)
{

}
int stk_sendmsg_ack(stk_client *client)
{

}

