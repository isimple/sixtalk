/* 
 * File: stkpacket.c
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
#include <signal.h>

#include "stk.h"

void stk_debug_print(char *buf, int len)
{
    int i;
    for (i = 0; i < len; i++) {
        printf("%02x ", buf[i]);
        if (!((i + 1) % 16)){
            printf("\n");
        }
    }
    printf("\r\n");
}


void stk_clean_socket()
{
    stk_client *client = NULL;
    int num = 0;

    num = stk_get_usernum();
    while (num--) {
        client = stk_next_user(client);
        if(client->stkc_state == STK_CLIENT_ONLINE) {
            shutdown(client->stkc_fd, SHUT_RDWR);
            close(client->stkc_fd);
        }
    }
}

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

stk_client *stk_parse_packet(char *buf,int size, stk_data *data)
{
    stkp_head *head = (stkp_head *)buf;
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
        printf("Oops, we get another server message! drop it.\n");
        return NULL;
    }

    len = ntohs(head->stkp_length);
    if (*(buf+sizeof(stkp_head)+len) != STKP_PACKET_TAIL) {
        printf("bad stkp message tail! drop it.\n");
        return NULL;
    }

    data->uid = ntohl(head->stkp_uid);

    client = stk_find_user(data->uid);

    buf += sizeof(stkp_head);
    data->cmd = ntohs(head->stkp_cmd);
    data->data = buf;
    data->len = len;
    return client;
}

void stk_deliver_msg(int signal)
{
    pthread_t tid;
    char msg[STK_MAX_PACKET_SIZE] = {0};
    stk_client *client = NULL;
    stkp_head *head = NULL;
    int len;

    if(signal != SIGUSR1) {
        return ;
    }

    tid = pthread_self();
    client = stk_get_user_by_tid(tid);

    if (client == NULL) {
        printf("What happened, it's impossible...\n");
        return ;
    }

    if (client->stkc_state == STK_CLIENT_OFFLINE) {
        printf("Oops, stkclient not online, hope not go here...\n");
        return ;
    }

    while (client->msg_num > 0) {
        memset(msg, 0, sizeof(msg));
        if (stk_get_msg(client, msg, &len) == -1)
            break;
        head = (stkp_head *)msg;
        head->stkp_flag = 0x1;
        head->stkp_token = htonl(client->stkc_token);

        if (len != send(client->stkc_fd, msg, len, 0)) {
            printf("stk_deliver_msg: deliver msg error\n");
            continue;
        }
    }

    return;
}

int stk_reqlogin_ack(int fd, unsigned int uid, char *buf)
{
    stkp_head *head = (stkp_head *)buf;
    int len;
    head->stkp_flag = 0x1;
    head->stkp_token = htonl(stk_get_token(uid));

    len = sizeof(stkp_head) + 1;

    if (len != send(fd, buf, len, 0)) {
        printf("reply req login msg error: %s(errno: %d)\n", strerror(errno), errno);
        return -1;
    }
}

int stk_login_ack(int fd, unsigned int uid, char *buf)
{
    stkp_head *head = (stkp_head *)buf;
    char pass[STK_PASS_SIZE] = {0};
    stk_client *client = NULL;
    char *tmp = NULL;
    int len;
    int ret;

    ret = stk_get_pass(uid, pass);
    if (ret == STK_NULL_POINTER) {
        printf("Error happen, Ignore message.\n");
        return -1;
    }

    head->stkp_flag = 0x1;
    head->stkp_token = htonl(stk_get_token(uid));

    tmp = buf + sizeof(stkp_head);

    len = 1;
    len = htons(len);
    memcpy(tmp-2, &len, 2);

    if (ret == -2) {
        *tmp = STK_LOGIN_INVALID_UID;
    } else {
        client = stk_find_user(uid);
        if (client->stkc_state) {
            *tmp = STK_LOGIN_AGAIN;
        } else if (!memcmp(tmp, pass, STK_PASS_SIZE)) {
            *tmp = STK_LOGIN_SUCCESS;
            client->stkc_fd = fd;
            client->stkc_tid= pthread_self();
            client->stkc_state = STK_CLIENT_ONLINE;
            stk_print_user(client);
        } else {
            *tmp = STK_LOGIN_INVALID_PASS;
        }
    }

    *(tmp+1) = STKP_PACKET_TAIL;
    len = sizeof(stkp_head) + 2;

    if (len != send(fd, buf, len, 0)) {
        printf("reply login msg error: %s(errno: %d)\n", strerror(errno), errno);
        return -1;
    }

}

int stk_keepalive_ack(stk_client *client, char *buf)
{

}

int stk_getuser_ack(stk_client *client, char *buf)
{
    stkp_head *head = (stkp_head *)buf;
    stk_client *client_next;
    char *tmp = NULL;
    int num = stk_get_usernum();
    unsigned int uid;
    int len;
    int ret;

    head->stkp_flag = 0x1;
    head->stkp_token = htonl(client->stkc_token);

    /* num should not include itself */
    tmp = buf+sizeof(stkp_head);
    num = htons(--num);
    memcpy(tmp, &num, STK_ID_NUM_LENGTH);
    tmp += STK_ID_NUM_LENGTH;

    num = stk_get_usernum()-1;
    client_next = client;
    while (num--) {
        client_next = stk_next_user(client_next);
        uid = client_next->stkc_uid;
        uid = htonl(uid);
        memcpy(tmp, &uid, STK_ID_LENGTH);
        tmp += STK_ID_LENGTH;
    }

    num = stk_get_usernum()-1;
    tmp = buf+sizeof(stkp_head);
    len = htons(STK_ID_NUM_LENGTH + num*STK_ID_LENGTH);
    memcpy(tmp-2, &len, 2);

    len = STK_ID_NUM_LENGTH + num*STK_ID_LENGTH + sizeof(stkp_head);
    buf[len] = STKP_PACKET_TAIL;
    len++;

    if (len != send(client->stkc_fd, buf, len, 0)) {
        printf("send get user error: %s(errno: %d)\n", strerror(errno), errno);
        return -1;
    }
}

int stk_getonlineuser_ack(stk_client *client, char *buf)
{

}

int stk_getuserinfo_ack(stk_client *client, char *buf)
{
    stkp_head *head = (stkp_head *)buf;
    stk_client *user;
    char *tmp = NULL;
    unsigned int uid, phone;
    int len;
    int ret;

    head->stkp_flag = 0x1;
    head->stkp_token = htonl(client->stkc_token);

    tmp = buf + sizeof(stkp_head);

    memcpy(&uid, tmp, STK_ID_LENGTH);
    uid = ntohl(uid);
    user = stk_find_user(uid);
    if (user == NULL) {
        memset(tmp, 0, STK_ID_LENGTH);
    } else {
        tmp += STK_ID_LENGTH;
        memcpy(tmp, user->stkc_nickname, STK_NICKNAME_SIZE);

        tmp += STK_NICKNAME_SIZE;
        memcpy(tmp, user->stkc_city, STK_CITY_SIZE);

        tmp += STK_CITY_SIZE;
        phone = htonl(user->stkc_phone);
        memcpy(tmp, &phone, STK_PHONE_LENGTH);

        tmp += STK_PHONE_LENGTH;
        *tmp = user->stkc_gender;
    }

    tmp = buf + sizeof(stkp_head);
    len = htons(STK_ID_LENGTH+STK_NICKNAME_SIZE+STK_CITY_SIZE+STK_PHONE_LENGTH+STK_GENDER_LENGTH);
    memcpy(tmp-2, &len, 2);

    len = STK_ID_LENGTH+STK_NICKNAME_SIZE+STK_CITY_SIZE+STK_PHONE_LENGTH+STK_GENDER_LENGTH+sizeof(stkp_head);
    buf[len] = STKP_PACKET_TAIL;
    len++;

    if (len != send(client->stkc_fd, buf, len, 0)) {
        printf("send get user profile error: %s(errno: %d)\n", strerror(errno), errno);
        return -1;
    }
}


int stk_getgroup_ack(stk_client *client, char *buf)
{
    stkp_head *head = (stkp_head *)buf;
    client_group *group;
    char *tmp = NULL;
    int num = client->stkc_groupnum;
    unsigned int gid;
    int len;
    int ret;

    head->stkp_flag = 0x1;
    head->stkp_token = htonl(client->stkc_token);

    tmp = buf+sizeof(stkp_head);
    num = htons(num);
    memcpy(tmp, &num, STK_GID_NUM_LENGTH);
    tmp += STK_GID_NUM_LENGTH;

    num = client->stkc_groupnum;
    group = client->stkc_group;
    while (num-- && group != NULL) {
        gid = group->groupid;
        gid = htonl(gid);
        memcpy(tmp, &gid, STK_GID_LENGTH);
        tmp += STK_GID_LENGTH;
        group = group->next;
    }

    num = client->stkc_groupnum;
    tmp = buf+sizeof(stkp_head);
    len = htons(STK_GID_NUM_LENGTH + num*STK_GID_LENGTH);
    memcpy(tmp-2, &len, 2);

    len = STK_GID_NUM_LENGTH + num*STK_GID_LENGTH + sizeof(stkp_head);
    buf[len] = STKP_PACKET_TAIL;
    len++;

    if (len != send(client->stkc_fd, buf, len, 0)) {
        printf("stk_getuser_ack: send get user error\n");
        return -1;
    }
}

int stk_getgroupinfo_ack(stk_client *client, char *buf)
{
    stkp_head *head = (stkp_head *)buf;
    stk_group *group;
    group_member *member;
    char *tmp = NULL;
    unsigned int gid, uid;
    unsigned short num;
    int len;
    int ret;

    head->stkp_flag = 0x1;
    head->stkp_token = htonl(client->stkc_token);

    tmp = buf + sizeof(stkp_head);

    memcpy(&gid, tmp, STK_GID_LENGTH);
    gid = ntohl(gid);
    group = stk_find_group(gid);
    if (group == NULL) {
        memset(tmp, 0, STK_GID_LENGTH);
    } else {
        tmp += STK_GID_LENGTH;
        memcpy(tmp, group->groupname, STK_GROUP_NAME_SIZE);

        tmp += STK_GROUP_NAME_SIZE;
        num = htons(group->member_num);
        memcpy(tmp, &num, STK_GID_NUM_LENGTH);

        tmp += STK_GID_NUM_LENGTH;
        num = group->member_num;
        member = group->members;
        while (num-- && member != NULL) {
            uid = htonl(member->uid);
            memcpy(tmp, &uid, STK_ID_LENGTH);
            tmp += STK_ID_LENGTH;
            member = member->next;
        }
    }

    num = group->member_num;
    tmp = buf + sizeof(stkp_head);
    len = htons(STK_GID_LENGTH+STK_GROUP_NAME_SIZE+STK_GID_NUM_LENGTH+num*STK_ID_LENGTH);
    memcpy(tmp-2, &len, 2);

    len = STK_GID_LENGTH+STK_GROUP_NAME_SIZE+STK_GID_NUM_LENGTH+num*STK_ID_LENGTH+sizeof(stkp_head);
    buf[len] = STKP_PACKET_TAIL;
    len++;

    if (len != send(client->stkc_fd, buf, len, 0)) {
        printf("stk_getuserinfo_ack: send get user profile error\n");
        return -1;
    }
}

int stk_sendmsg_ack(stk_client *client, char *buf, int bytes)
{
    stk_client *user;
    char *tmp = NULL;
    unsigned int uid;
    int ret;

    tmp = buf + sizeof(stkp_head);

    memcpy(&uid, tmp, STK_ID_LENGTH);
    uid = ntohl(uid);
    user = stk_find_user(uid);
    if (user == NULL) {
        /* msg to a unknow user, I beleive it's not impossible */
        return 0;
    } else if (user->stkc_state == STK_CLIENT_OFFLINE){
        /* user offline, need to do something? */
        return 0;
    } else if (user->stkc_state == STK_CLIENT_ONLINE){
        if (!stk_add_msg(user, buf, bytes)) {
            pthread_kill(user->stkc_tid, SIGUSR1);
        }
        return 0;
    }
}

int stk_sendgmsg_ack(stk_client *client, char *buf, int bytes)
{
    stk_group *group;
    group_member *member;
    stk_client *user;
    char *tmp = NULL;
    unsigned int gid;
    int ret;
    int num;

    tmp = buf + sizeof(stkp_head);

    memcpy(&gid, tmp, STK_GID_LENGTH);
    gid = ntohl(gid);
    group = stk_find_group(gid);
    if (group == NULL) {
        /* msg to a unknow group, I beleive it's not impossible */
        return 0;
    } else {
        num = group->member_num;
        member = group->members;
        while (num-- && member != NULL) {
            if (member->uid == client->stkc_uid) {
                member = member->next;
                continue;
            }
            user = stk_find_user(member->uid);

            if (user == NULL) {
                return 0;
            } else if (user->stkc_state == STK_CLIENT_OFFLINE) {
                /* user offline, need to do something? */
                return 0;
            } else if (user->stkc_state == STK_CLIENT_ONLINE) {
                if (!stk_add_msg(user, buf, bytes)) {
                    pthread_kill(user->stkc_tid, SIGUSR1);
                }
            }
            member = member->next;
        }
        return 0;
    }
}

