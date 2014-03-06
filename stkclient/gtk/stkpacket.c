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

#ifdef _LINUX_
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include "stk.h"

unsigned int token = 0;

#define STK_FLAG_SENDTIMEO 0
#define STK_FLAG_RECVTIMEO 1

#define STK_SOCKET_RECV_TIMEOUT 1
#define STK_SOCKET_SEND_TIMEOUT 5

#if defined(WIN32)
#define STK_SOCKET_BOTH SD_BOTH
#elif defined(_LINUX_)
#define STK_SOCKET_BOTH SHUT_RDWR
#endif

void stk_debug_print(char *buf, int len)
{
    int i;
    for (i = 0; i < len; i++) {
        g_print("%02x ", buf[i]);
        if (!((i + 1) % 16)){
            g_print("\n");
        }
    }
    g_print("\r\n");
}

int stk_init_socket()
{
#if defined(WIN32)
    WSADATA  Ws;

    /* Init Windows Socket */
    if (WSAStartup(MAKEWORD(2,2), &Ws) != 0)
    {
        stk_print("stk_init_socket: init socket error\n");
        return -1;
    }
#elif defined(_LINUX_)
#endif
    return 0;

}

void stk_clean_socket(socket_t fd)
{
    shutdown(fd, STK_SOCKET_BOTH);
    close(fd);
}

int stk_connect(client_config *config)
{
    struct sockaddr_in servaddr;
    int ret = -1;

    if((config->fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        stk_print("stk_connect: create socket error\n");
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(STK_SERVER_PORT);
    servaddr.sin_addr.s_addr = inet_addr(config->serverip);
    if( servaddr.sin_addr.s_addr == INADDR_NONE ){
        stk_print("stk_connect: inet_addr error for %s\n", config->serverip);
        return -1;
    }

    /* set timeout too short seems get socket err */
    //stk_set_socket_timeout(config->fd, STK_FLAG_SENDTIMEO, STK_SOCKET_SEND_TIMEOUT);
    //stk_set_socket_timeout(config->fd, STK_FLAG_RECVTIMEO, STK_SOCKET_RECV_TIMEOUT);

    if( connect(config->fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        stk_print("stk_connect: connect error\n");
        return -1;
    }

    return 0;
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

int stk_login(socket_t fd, char *buf, int max_len, unsigned int uid, char *password)
{
    stkp_head *head = NULL;
    int len;
    char *tmp = NULL;
    char pass[STK_PASS_SIZE] = {0};

    //g_print("start to req login token.\n");

    if (buf == NULL) {
        return -1;
    }
    memset(buf, 0, max_len);

    /* req login */
    stk_init_head((stkp_head *)buf, STKP_CMD_REQ_LOGIN, uid);

    len = htons(STK_DATA_ZERO_LENGTH);
    memcpy(buf+sizeof(stkp_head)-2, &len, 2);

    len = sizeof(stkp_head);
    buf[len++] = STKP_PACKET_TAIL;

    if (len != send(fd, buf, len, 0)) {
        stk_print("stk_login: send req login msg error\n");
        return STK_CLIENT_LOGIN_ERROR;
    }

    len = recv(fd, buf, STK_MAX_PACKET_SIZE, 0);
    if (len == -1){
        stk_print("stk_login: recv socket error(req login)\n");
        return STK_CLIENT_LOGIN_ERROR;
    }

    head = (stkp_head *)buf;

    if (head->stkp_magic != htons(STKP_MAGIC) 
        || head->stkp_version != htons(STKP_VERSION)
        || !STKP_TEST_FLAG(head->stkp_flag)
        || head->stkp_uid != htonl(uid)
        || head->stkp_cmd != htons(STKP_CMD_REQ_LOGIN))

    {
        stk_print("#1. bad msg, drop packet.\n");
        return STK_CLIENT_LOGIN_ERROR;
    }

    token = htonl(head->stkp_token);

    //g_print("start to login to STK Server.\n");

    /* login */
    memset(buf, 0, max_len);
    stk_init_head((stkp_head *)buf, STKP_CMD_LOGIN, uid);

    len = htons(STK_PASS_SIZE+STK_LOGIN_REVERSE_SIZE);
    memcpy(buf+sizeof(stkp_head)-2, &len, 2);

    //sprintf(pass, "%d", uid);
    memcpy(pass, password, STK_PASS_SIZE);
    memcpy(buf+sizeof(stkp_head), pass, STK_PASS_SIZE);

    len = STK_PASS_SIZE+STK_LOGIN_REVERSE_SIZE;
    buf[len+sizeof(stkp_head)] = STKP_PACKET_TAIL;
    len += sizeof(stkp_head) + 1;

    if (len != send(fd, buf, len, 0)) {
        stk_print("stk_login: send login msg error\n");
        return STK_CLIENT_LOGIN_ERROR;
    }

    len = recv(fd, buf, STK_MAX_PACKET_SIZE, 0);
    if (len == -1){
        stk_print("stk_login: recv socket error(login)\n");
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
        stk_print("#2. bad msg, drop packet.\n");
        return STK_CLIENT_LOGIN_ERROR;
    } else {
        tmp = buf + sizeof(stkp_head);
        if (*tmp == STK_LOGIN_SUCCESS) {
            return STK_CLIENT_LOGIN_SUCCESS;
        } else if (*tmp == STK_LOGIN_INVALID_UID) {
            return STK_CLIENT_LOGIN_INVALID_UID;
        } else if (*tmp == STK_LOGIN_INVALID_PASS) {
            return STK_CLIENT_LOGIN_INVALID_PASS;
        } else if (*tmp == STK_LOGIN_AGAIN) {
            return STK_CLIENT_LOGIN_AGAIN;
        } else {
            stk_print("stk_login: Unknown login reply.\n");
            return STK_CLIENT_LOGIN_ERROR;
        }
    }

}

int stk_send_getbuddyinfo(socket_t fd, char *buf, int max_len, unsigned int uid, unsigned int n_uid, stk_buddy *buddy)
{
    stkp_head *head = NULL;
    char *tmp = NULL;
    unsigned int uid_n;
    int len;

    if (buf == NULL || buddy == NULL) {
        return -1;
    }

    memset(buf, 0, max_len);
    stk_init_head((stkp_head *)buf, STKP_CMD_GET_USER_INFO, uid);

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
        stk_print("stk_send_getbuddyinfo: send msg error\n");
        return -1;
    }

    len = recv(fd, buf, STK_MAX_PACKET_SIZE, 0);
    if (len == -1){
        stk_print("stk_send_getbuddyinfo: recv socket error\n");
        return -1;
    }

    head = (stkp_head *)buf;

    if (head->stkp_magic != htons(STKP_MAGIC) 
        || head->stkp_version != htons(STKP_VERSION)
        || !STKP_TEST_FLAG(head->stkp_flag)
        || head->stkp_uid != htonl(uid)
        || head->stkp_cmd != htons(STKP_CMD_GET_USER_INFO))

    {
        stk_print("stk_send_getbuddyinfo: bad msg, drop packet.\n");
        return -1;
    } else {
        tmp = buf + sizeof(stkp_head);

        memcpy(&buddy->uid, tmp, STK_ID_LENGTH);
        buddy->uid = ntohl(buddy->uid);
        if (buddy->uid == 0) {
            stk_print("stk_send_getbuddyinfo: Error, bad uid.\n");
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

int stk_send_getbuddy(socket_t fd, char *buf, int max_len, unsigned int uid)
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
        stk_print("stk_send_getbuddy: send msg error\n");
        return -1;
    }

    len = recv(fd, buf, STK_MAX_PACKET_SIZE, 0);
    if (len == -1){
        stk_print("stk_send_getbuddy: recv socket error\n");
        return -1;
    }

    head = (stkp_head *)buf;

    if (head->stkp_magic != htons(STKP_MAGIC) 
        || head->stkp_version != htons(STKP_VERSION)
        || !STKP_TEST_FLAG(head->stkp_flag)
        || head->stkp_uid != htonl(uid)
        || head->stkp_cmd != htons(STKP_CMD_GET_USER))

    {
        stk_print("stk_send_getbuddy: bad msg, drop packet.\n");
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
                stk_print("stk_send_getbuddy: Error Add buddy.\n");
                continue;
            }
        }

        /* now, get buddy profile */
        next_buddy = NULL;
        buddy_num = stk_get_buddynum();
        while (buddy_num--) {
            next_buddy = stk_next_buddy(next_buddy);
            memset(&buddy, 0, sizeof(stk_buddy));
            if (stk_send_getbuddyinfo(fd, buf, max_len, uid, next_buddy->uid, &buddy) != -1) {
                stk_update_buddy(&buddy);
            }
        }
        return 0;
    }

}

int stk_send_getgroupinfo(socket_t fd, char *buf, int max_len, unsigned int uid, unsigned int gid, stk_group *group)
{
    stkp_head *head = NULL;
    group_member *member, *member_tmp;
    char *tmp = NULL;
    unsigned int gid_n, memberid;
    unsigned short num, i;
    int len;

    if (buf == NULL) {
        return -1;
    }

    memset(buf, 0, max_len);
    stk_init_head((stkp_head *)buf, STKP_CMD_GET_GROUP_INFO, uid);

    tmp = buf+sizeof(stkp_head);
    
    len = htons(STK_GID_LENGTH+STK_GROUP_NAME_SIZE+STK_ID_NUM_LENGTH);
    memcpy(tmp-2, &len, 2);

    /* Use int to replace string */
    //sprintf(info, "%d", uid);
    //memcpy(tmp, info, STK_ID_LENGTH);
    gid_n = htonl(gid);
    memcpy(tmp, &gid_n, STK_GID_LENGTH);

    len = STK_GID_LENGTH+STK_GROUP_NAME_SIZE+STK_ID_NUM_LENGTH+sizeof(stkp_head);
    buf[len] = STKP_PACKET_TAIL;
    len++;

    if (len != send(fd, buf, len, 0)) {
        stk_print("stk_send_getgroupinfo: send msg error\n");
        return -1;
    }

    len = recv(fd, buf, STK_MAX_PACKET_SIZE, 0);
    if (len == -1){
        stk_print("stk_send_getgroupinfo: recv socket error\n");
        return -1;
    }

    head = (stkp_head *)buf;

    if (head->stkp_magic != htons(STKP_MAGIC) 
        || head->stkp_version != htons(STKP_VERSION)
        || !STKP_TEST_FLAG(head->stkp_flag)
        || head->stkp_uid != htonl(uid)
        || head->stkp_cmd != htons(STKP_CMD_GET_GROUP_INFO))

    {
        stk_print("stk_send_getgroupinfo: bad msg, drop packet.\n");
        return -1;
    } else {
        tmp = buf + sizeof(stkp_head);

        memcpy(&gid_n, tmp, STK_GID_LENGTH);
        gid_n = ntohl(gid_n);
        if (gid_n != gid || gid_n == 0) {
            stk_print("stk_send_getgroupinfo: Error, bad gid.\n");
            return -1;
        }

        tmp += STK_GID_LENGTH;
        memcpy(group->groupname, tmp, STK_GROUP_NAME_SIZE);

        tmp += STK_GROUP_NAME_SIZE;
        memcpy(&num, tmp, STK_ID_NUM_LENGTH);
        num = ntohs(num);
        group->member_num = num;
        tmp += STK_ID_NUM_LENGTH;

        i = 0;
        while (i < num) {
            memcpy(&memberid, tmp, STK_ID_LENGTH);
            tmp += STK_ID_LENGTH;
            memberid = ntohl(memberid);

            member = (group_member *)malloc(sizeof(group_member));
            member->uid = memberid;
            member->next = NULL;
            if (i == 0) {
                group->members = member;
                member_tmp = group->members;
            } else {
                member_tmp->next = member;
                member_tmp = member_tmp->next;
            }
            i++;
        }
        return 0;
    }
}


int stk_send_getgroup(socket_t fd, char *buf, int max_len, unsigned int uid, client_config *client)
{
    stkp_head *head = NULL;
    char *tmp = NULL;
    int group_num;
    unsigned int group_uid;
    stk_group *gtmp1, *gtmp2;;
    int i;
    int len;

    if (buf == NULL) {
        return -1;
    }

    memset(buf, 0, max_len);
    stk_init_head((stkp_head *)buf, STKP_CMD_GET_GROUP, uid);

    len = htons(STK_DATA_ZERO_LENGTH);
    memcpy(buf+sizeof(stkp_head)-2, &len, 2);

    len = sizeof(stkp_head);
    buf[len++] = STKP_PACKET_TAIL;

    if (len != send(fd, buf, len, 0)) {
        stk_print("stk_send_getgroup: send msg error\n");
        return -1;
    }

    len = recv(fd, buf, STK_MAX_PACKET_SIZE, 0);
    if (len == -1){
        stk_print("stk_send_getgroup: recv socket error\n");
        return -1;
    }

    head = (stkp_head *)buf;

    if (head->stkp_magic != htons(STKP_MAGIC) 
        || head->stkp_version != htons(STKP_VERSION)
        || !STKP_TEST_FLAG(head->stkp_flag)
        || head->stkp_uid != htonl(uid)
        || head->stkp_cmd != htons(STKP_CMD_GET_GROUP))

    {
        stk_print("stk_send_getgroup: bad msg, drop packet.\n");
        return -1;
    } else {
        tmp = buf + sizeof(stkp_head);

        memcpy(&group_num, tmp, STK_GID_NUM_LENGTH);
        tmp += STK_GID_NUM_LENGTH;

        group_num = ntohs(group_num);
        client->group_num = group_num;
        i = 0;
        while (i < group_num) {
            memcpy(&group_uid, tmp, STK_GID_LENGTH);
            tmp += STK_GID_LENGTH;
            group_uid = ntohl(group_uid);

            gtmp2 = (stk_group *)malloc(sizeof(stk_group));
            if (gtmp2 == NULL) {
                printf("what the fuck!\n");
                continue;
            }
            memset(gtmp2, 0, sizeof(stk_group));
            gtmp2->groupid = group_uid;
            gtmp2->next = NULL;
            if (i == 0) {
                client->group = gtmp2;
                gtmp1 = client->group;
            } else {
                gtmp1->next = gtmp2;
                gtmp1 = gtmp1->next;
            }
            i++;
        }

        /* now, get group info */
        group_num = client->group_num;
        gtmp1 = client->group;
        while (group_num-- && gtmp1 != NULL) {
            group_uid = gtmp1->groupid;
            if (stk_send_getgroupinfo(fd, buf, max_len, uid, group_uid, gtmp1) == -1) {
                stk_print("stk_send_getgroup: something wrong when get group info.\n");
            }
            gtmp1 = gtmp1->next;
        }

        return 0;
    }

}


int stk_send_msg(socket_t fd, char *buf, int max_len, char *data, int data_len, 
                                unsigned int uid, unsigned int id, gboolean gmsg)
{
    stkp_head *head = NULL;
    char *tmp = NULL;
    unsigned int id_n;
    unsigned short cmd;
    int len;

    if (buf == NULL) {
        return -1;
    }

    if (gmsg) {
        cmd = STKP_CMD_SEND_GMSG;
    } else {
        cmd = STKP_CMD_SEND_MSG;
    }
		
    memset(buf, 0, max_len);
    stk_init_head((stkp_head *)buf, cmd, uid);

    tmp = buf+sizeof(stkp_head);
    
    len = htons(STK_ID_LENGTH+data_len);
    memcpy(tmp-2, &len, 2);

    id_n = htonl(id);
    memcpy(tmp, &id_n, STK_ID_LENGTH);

    tmp += STK_ID_LENGTH;
    memcpy(tmp, data, data_len);

    len = STK_ID_LENGTH+data_len+sizeof(stkp_head);
    buf[len] = STKP_PACKET_TAIL;
    len++;

    if (len != send(fd, buf, len, 0)) {
        stk_print("stk_send_msg: send msg error\n");
        return -1;
    }
}


int stk_recv_msg(client_config *client)
{
    unsigned char recvbuf[STK_MAX_PACKET_SIZE] = {0};
    stkp_head *head = NULL;
    char *tmp = NULL;
    unsigned int uid, gid;
    int len;

    while (1) {
        len = recv(client->fd, recvbuf, STK_MAX_PACKET_SIZE, 0);
        if (len == -1){
            stk_print("stk_recv_msg: recv socket error\n");
            return STK_SOCKET_ERROR;
        } else if (len == 0) {
            if (stk_get_running() == STK_EXITING)
                return 0;
            stk_print("\n\nSTK Server close the socket, exiting...\n\n");
            return STK_SOCKET_CLOSED;
        }

        head = (stkp_head *)recvbuf;

        if (head->stkp_magic != htons(STKP_MAGIC) 
            || head->stkp_version != htons(STKP_VERSION)
            || !STKP_TEST_FLAG(head->stkp_flag))
        {
            stk_print("#5. bad msg, drop packet.\n");
            continue;
        } else {
            unsigned short size;
            unsigned short cmd;
            char data[STK_MAX_PACKET_SIZE] = {0};
            stk_buddy *buddy;
            stk_group *group;
            int num;
            gboolean found;

            cmd = ntohs(head->stkp_cmd);
            switch (cmd) {
            case STKP_CMD_KEEPALIVE:
                //stk_keepalive_ack(client, buf);
                break;
            case STKP_CMD_SEND_MSG:
                tmp = recvbuf + sizeof(stkp_head);
                memcpy(&size, tmp-2, 2);

                size = ntohs(size) - STK_ID_LENGTH;
                tmp += STK_ID_LENGTH;
                memcpy(data, tmp, size);
                data[size] = '\0';
                uid = ntohl(head->stkp_uid);
                buddy = stk_find_buddy(uid);

                if (buddy == NULL) 
                    stk_print("Bad buddy!!\n");
                else {
                    if (!stk_add_msg(buddy, data, size)) {
                        stk_msg_event(buddy);
                    }
                }
                break;
            case STKP_CMD_SEND_GMSG:
                tmp = recvbuf + sizeof(stkp_head);
                memcpy(&size, tmp-2, 2);

                size = ntohs(size) - STK_ID_LENGTH;
                tmp += STK_ID_LENGTH;
                memcpy(data, tmp, size);
                data[size] = '\0';

                uid = ntohl(head->stkp_uid);
                memcpy(&gid, tmp-4, STK_GID_LENGTH);
                gid = ntohl(gid);

                found = FALSE;
                num = client->group_num;
                group = client->group;
                while (num-- && group != NULL) {
                    if (group->groupid == gid) {
						found = TRUE;
                        break;
                    }
                    group = group->next;
				}

                if (found && group != NULL) {
                    if (!stk_add_gmsg(group, data, size, uid)) {
                        stk_gmsg_event(group);
                    }
                }
                break;
            default:
                stk_print("stk_recv_msg: Bad STKP CMD, Drop it.\n");
                break;
            }
        }
    }
    return 0;
}

