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

#include "stk.h"

unsigned int token = 0;

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

int stk_init_head(stkp_head *head, unsigned short cmd, unsigned int uid)
{
    head->stkp_magic = htons(STKP_MAGIC);
    head->stkp_version = htons(STKP_VERSION);
    head->stkp_cmd = htons(cmd);
    head->stkp_uid = htonl(uid);
    head->stkp_token = htonl(token);
    head->stkp_flag = 0x0;
}

int stk_login(int fd, char *buf, int max_len, unsigned int uid)
{
    stkp_head *head = NULL;
    int len;
    char *tmp = NULL;
    char pass[STK_PASS_SIZE] = {0};

    //printf("start to req login token.\n");

    /* req login */
    stk_init_head((stkp_head *)buf, STKP_CMD_REQ_LOGIN, uid);

    len = htons(STK_DATA_ZERO_LENGTH);
    memcpy(buf+sizeof(stkp_head)-2, &len, 2);

    len = sizeof(stkp_head);
    buf[len++] = STKP_PACKET_TAIL;

    if (len != send(fd, buf, len, 0)) {
        printf("send req login msg error: %s(errno: %d)\n", strerror(errno), errno);
        return STK_CLIENT_LOGIN_ERROR;
    }

    len = recv(fd, buf, STK_MAX_PACKET_SIZE, 0);
    if (len == -1){
        printf("recv socket error: %s(errno: %d)",strerror(errno),errno);
        return STK_CLIENT_LOGIN_ERROR;
    }

    head = (stkp_head *)buf;

    if (head->stkp_magic != htons(STKP_MAGIC) 
        || head->stkp_version != htons(STKP_VERSION)
        || !STKP_TEST_FLAG(head->stkp_flag)
        || head->stkp_uid != htonl(uid)
        || head->stkp_cmd != htons(STKP_CMD_REQ_LOGIN))

    {
        printf("#1. bad msg, drop packet.\n");
        return STK_CLIENT_LOGIN_ERROR;
    }

    token = htonl(head->stkp_token);

    //printf("start to login to STK Server.\n");

    /* login */
    memset(buf, 0, max_len);
    stk_init_head((stkp_head *)buf, STKP_CMD_LOGIN, uid);

    len = htons(STK_PASS_SIZE+STK_LOGIN_REVERSE_SIZE);
    memcpy(buf+sizeof(stkp_head)-2, &len, 2);

    sprintf(pass, "%d", uid);
    memcpy(buf+sizeof(stkp_head), pass, STK_PASS_SIZE);

    len = STK_PASS_SIZE+STK_LOGIN_REVERSE_SIZE;
    buf[len+sizeof(stkp_head)] = STKP_PACKET_TAIL;
    len += sizeof(stkp_head) + 1;

    if (len != send(fd, buf, len, 0)) {
        printf("send login msg error: %s(errno: %d)\n", strerror(errno), errno);
        return STK_CLIENT_LOGIN_ERROR;
    }

    len = recv(fd, buf, STK_MAX_PACKET_SIZE, 0);
    if (len == -1){
        printf("recv socket error: %s(errno: %d)\n",strerror(errno),errno);
        return STK_CLIENT_LOGIN_ERROR;
    }

    head = (stkp_head *)buf;

    if (head->stkp_magic != htons(STKP_MAGIC) 
        || head->stkp_version != htons(STKP_VERSION)
        || !STKP_TEST_FLAG(head->stkp_flag)
        || head->stkp_uid != htonl(uid)
        || head->stkp_cmd != htons(STKP_CMD_LOGIN)
        || head->stkp_token != htonl(token))
    {
        printf("#2. bad msg, drop packet.\n");
        return STK_CLIENT_LOGIN_ERROR;
    } else {
        tmp = buf + sizeof(stkp_head);
        if (*tmp == STK_LOGIN_SUCCESS) {
            return STK_CLIENT_LOGIN_SUCCESS;
        } else if (*tmp == STK_LOGIN_INVALID_UID) {
            return STK_CLIENT_LOGIN_INVALID_UID;
        } else if (*tmp == STK_LOGIN_INVALID_PASS) {
            return STK_CLIENT_LOGIN_INVALID_PASS;
        } else {
            printf("Unknown login reply.\n");
            return STK_CLIENT_LOGIN_ERROR;
        }
    }

}

int stk_send_getprofile(int fd, char *buf, int max_len, unsigned int uid, unsigned int n_uid, stk_buddy *buddy)
{
    stkp_head *head = NULL;
    char *tmp = NULL;
    unsigned int uid_n;
    int len;

    if (buf == NULL || buddy == NULL) {
        return -1;
    }

    memset(buf, 0, max_len);
    stk_init_head((stkp_head *)buf, STKP_CMD_GET_INFO, uid);

    tmp = buf+sizeof(stkp_head);
    
    len = htons(STK_ID_LENGTH+STK_NICKNAME_SIZE+STK_CITY_SIZE+STK_PHONE_LENGTH+STK_GENDER_LENGTH);
    memcpy(tmp-2, &len, 2);

    /* Use int to replace string */
    //sprintf(info, "%d", uid);
    //memcpy(tmp, info, STK_ID_LENGTH);
    uid_n = htonl(n_uid);
    memcpy(tmp, &uid_n, STK_ID_LENGTH);

    len = STK_ID_LENGTH+STK_NICKNAME_SIZE+STK_CITY_SIZE+STK_PHONE_LENGTH+STK_GENDER_LENGTH+sizeof(stkp_head);
    buf[len] = STKP_PACKET_TAIL;
    len++;

    if (len != send(fd, buf, len, 0)) {
        printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
        return -1;
    }

    len = recv(fd, buf, STK_MAX_PACKET_SIZE, 0);
    if (len == -1){
        printf("recv socket error: %s(errno: %d)",strerror(errno),errno);
        return -1;
    }

    head = (stkp_head *)buf;

    if (head->stkp_magic != htons(STKP_MAGIC) 
        || head->stkp_version != htons(STKP_VERSION)
        || !STKP_TEST_FLAG(head->stkp_flag)
        || head->stkp_uid != htonl(uid)
        || head->stkp_cmd != htons(STKP_CMD_GET_INFO))

    {
        printf("#3. bad msg, drop packet.\n");
        return -1;
    } else {
        tmp = buf + sizeof(stkp_head);

        memcpy(&buddy->uid, tmp, STK_ID_LENGTH);
        buddy->uid = ntohl(buddy->uid);
        if (buddy->uid == 0) {
            printf("Error, bad uid.\n");
            return -1;
        }

        tmp += STK_ID_LENGTH;
        memcpy(buddy->nickname, tmp, STK_NICKNAME_SIZE);

        tmp += STK_NICKNAME_SIZE;
        memcpy(buddy->city, tmp, STK_CITY_SIZE);

        tmp += STK_CITY_SIZE;
        memcpy(&buddy->phone, tmp, STK_PHONE_LENGTH);
        buddy->phone = ntohl(buddy->phone);

        tmp += STK_PHONE_LENGTH;
        buddy->gender = *tmp;
    }
}

int stk_send_getbuddylist(int fd, char *buf, int max_len, unsigned int uid)
{
    stkp_head *head = NULL;
    char *tmp = NULL;
    int buddy_num;
    unsigned int buddy_uid;
    stk_buddy buddy;
    stk_buddy *next_buddy = NULL;
    int i;
    int len;

    if (buf == NULL) {
        return -1;
    }

    memset(buf, 0, max_len);
    stk_init_head((stkp_head *)buf, STKP_CMD_GET_USER, uid);

    len = htons(STK_DATA_ZERO_LENGTH);
    memcpy(buf+sizeof(stkp_head)-2, &len, 2);

    len = sizeof(stkp_head);
    buf[len++] = STKP_PACKET_TAIL;

    if (len != send(fd, buf, len, 0)) {
        printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
        return -1;
    }

    len = recv(fd, buf, STK_MAX_PACKET_SIZE, 0);
    if (len == -1){
        printf("recv socket error: %s(errno: %d)",strerror(errno),errno);
        return -1;
    }

    head = (stkp_head *)buf;

    if (head->stkp_magic != htons(STKP_MAGIC) 
        || head->stkp_version != htons(STKP_VERSION)
        || !STKP_TEST_FLAG(head->stkp_flag)
        || head->stkp_uid != htonl(uid)
        || head->stkp_cmd != htons(STKP_CMD_GET_USER))

    {
        printf("#4. bad msg, drop packet.\n");
        return -1;
    } else {
        tmp = buf + sizeof(stkp_head);

        memcpy(&buddy_num, tmp, STK_ID_NUM_LENGTH);
        tmp += STK_ID_NUM_LENGTH;

        buddy_num = ntohs(buddy_num);
        if (buddy_num == 0) {
            return 0;
        }

        i = 0;
        while ((i++) < buddy_num) {
            memset(&buddy, 0, sizeof(stk_buddy));
            memcpy(&buddy_uid, tmp, STK_ID_LENGTH);
            tmp += STK_ID_LENGTH;
            buddy.uid = ntohl(buddy_uid);

            if (stk_add_buddy(&buddy) < 0) {
                printf("Error Add buddy.\n");
                continue;
            }
        }

        /* Now, get buddy profile */
        next_buddy = NULL;
        buddy_num = stk_get_buddynum();
        while (buddy_num--) {
            next_buddy = stk_next_buddy(next_buddy);
            memset(&buddy, 0, sizeof(stk_buddy));
            if (stk_send_getprofile(fd, buf, max_len, uid, next_buddy->uid, &buddy) != -1) {
                stk_update_buddy(&buddy);
            }
        }
        return 0;
    }

}


int stk_send_msg(int fd, char *buf, int max_len, char *data, int data_len, unsigned int uid, unsigned int n_uid)
{
    stkp_head *head = NULL;
    char *tmp = NULL;
    unsigned int uid_n;
    int len;

    if (buf == NULL) {
        return -1;
    }

    memset(buf, 0, max_len);
    stk_init_head((stkp_head *)buf, STKP_CMD_SEND_MSG, uid);

    tmp = buf+sizeof(stkp_head);
    
    len = htons(STK_ID_LENGTH+data_len);
    memcpy(tmp-2, &len, 2);

    uid_n = htonl(n_uid);
    memcpy(tmp, &uid_n, STK_ID_LENGTH);

    tmp += STK_ID_LENGTH;
    memcpy(tmp, data, data_len);

    len = STK_ID_LENGTH+data_len+sizeof(stkp_head);
    buf[len] = STKP_PACKET_TAIL;
    len++;

    if (len != send(fd, buf, len, 0)) {
        printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
        return -1;
    }
}


int stk_handle_msg(client_config *client, char *buf)
{
    stkp_head *head = NULL;
    char *tmp = NULL;
    unsigned int uid;
    int len;

    len = recv(client->fd, buf, STK_MAX_PACKET_SIZE, 0);
    if (len == -1){
        printf("recv socket error: %s(errno: %d)",strerror(errno),errno);
        return STK_SOCKET_ERROR;
    } else if (len == 0) {
        printf("\n\nSTK Server close the socket, exiting...\n\n");
        return STK_SOCKET_CLOSED;
    }

    head = (stkp_head *)buf;

    if (head->stkp_magic != htons(STKP_MAGIC) 
        || head->stkp_version != htons(STKP_VERSION)
        || !STKP_TEST_FLAG(head->stkp_flag))
    {
        printf("#5. bad msg, drop packet.\n");
        return -1;
    } else {
        unsigned short size;
        unsigned short cmd;
        char data[4096] = {0};
        stk_buddy *buddy;

        cmd = ntohs(head->stkp_cmd);
        switch (cmd) {
        case STKP_CMD_KEEPALIVE:
            //stk_keepalive_ack(client, buf);
            break;
        case STKP_CMD_SEND_MSG:
            tmp = buf + sizeof(stkp_head);
            memcpy(&size, tmp-2, 2);

            size = ntohs(size) - STK_ID_LENGTH;
            tmp += STK_ID_LENGTH;
            memcpy(data, tmp, size);
            data[size] = '\0';
            uid = ntohl(head->stkp_uid);
            buddy = stk_find_buddy(uid);

            if (buddy == NULL) 
                printf("Bad buddy!!\n");
            else {
                fflush(stdout);
                printf("\n====================================================\n");
                printf("%s talk to %s: %s\n", buddy->nickname, client->nickname, data);
                printf("====================================================\n");
                fflush(stdout);
            }
            break;
        default:
            printf("Bad STKP CMD, Drop it.");
            break;
        }
    }
    return 0;
}

