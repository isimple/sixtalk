/* 
 * File: stkuser.c
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "cJSON.h"
#include "stk.h"

#define BUF_SIZE_MAX      4096
#define STK_UNKNOWN_USER  -2

LIST_HEAD(stk_users);

static int stk_init_mygroup(stk_client *client)
{
    stk_group *group;
    group_member *gtmp;
    client_group *ctmp, *ctmp2;
    int gnum = stk_get_groupnum();
    int cgnum = 0;

    group = NULL;
    while(gnum--) {
        group = stk_next_group(group);
        gtmp = group->members;
        while (gtmp != NULL) {
            if (gtmp->uid == client->stkc_uid) {
                ctmp2 = (client_group *)malloc(sizeof(client_group));
                if (ctmp2 == NULL) {
                    printf("what the fuck!\n");
                    break;
                }
                ctmp2->gid = group->gid;
                ctmp2->next = NULL;
                if (cgnum == 0) {
                    client->stkc_group = ctmp2;
                    ctmp = client->stkc_group;
                } else {
                    ctmp->next = ctmp2;
                    ctmp = ctmp->next;
                }
                cgnum++;
                break;
            }
            gtmp = gtmp->next;
        }
    }

    return cgnum;
}

int stk_init_user()
{
    FILE *fd;
    int len;
    char buf[BUF_SIZE_MAX] = {0};
    cJSON *root = NULL;
    cJSON *item = NULL;
    char *out = NULL;
    int i, count;
    stk_client *client;

    fd = fopen(STK_USER_FILE, "rb");    
    fseek(fd, 0, SEEK_END);
    len = ftell(fd);
    fseek(fd, 0, SEEK_SET);
    fread(buf, 1, len, fd);
    fclose(fd);

    root = cJSON_Parse(buf);
    
    if (!root) {
        printf("Error before: [%s]\n",cJSON_GetErrorPtr());
        return -1;
    } else {
#ifdef STK_DEBUG
        out = cJSON_Print(root);
        printf("%s\n",out);  
        free(out);
#endif
    }
    
    count = cJSON_GetArraySize (root);
    for(i = 0; i < count; i++)
    {
        item = cJSON_GetArrayItem(root, i);
#ifdef STK_DEBUG
        out = cJSON_Print(item);
        printf("%s\n",out);  
        free(out);
#endif
        client = (stk_client *)malloc(sizeof(stk_client));
        if (client != NULL) {
            memset(client, 0, sizeof(stk_client));
            stk_user_offline(client);
            INIT_LIST_HEAD(&client->list);
            client->stkc_uid = cJSON_GetObjectItem(item,"uid")->valueint;
#if 1
            strcpy(client->stkc_nickname, cJSON_GetObjectItem(item,"nickname")->valuestring);
            strcpy(client->stkc_pass, cJSON_GetObjectItem(item,"pass")->valuestring);
            strcpy(client->stkc_city, cJSON_GetObjectItem(item,"city")->valuestring);
#else
            memcpy(client->stkc_nickname, cJSON_GetObjectItem(item,"nickname")->valuestring, STK_NICKNAME_SIZE);
            memcpy(client->stkc_pass, cJSON_GetObjectItem(item,"pass")->valuestring, STK_PASS_SIZE);
            memcpy(client->stkc_city, cJSON_GetObjectItem(item,"city")->valuestring, STK_CITY_SIZE);
#endif
            client->stkc_phone = cJSON_GetObjectItem(item,"phone")->valueint;
            client->stkc_gender = cJSON_GetObjectItem(item,"gender")->valueint;
            client->stkc_groupnum = stk_init_mygroup(client);
#ifdef STK_DEBUG
            stk_print_user(client);
#endif
            list_add_tail(&client->list, &stk_users);
        } else {
            printf("malloc error: %s(errno: %d)\n",strerror(errno),errno);
            continue;
        }
    }

    free(root);
    return 0;
}

stk_client *stk_find_user(unsigned int uid)
{
    struct list_head *entry;

    list_for_each(entry, &stk_users) {
        stk_client *client;
        client = list_entry(entry, stk_client, list);
        if (client->stkc_uid == uid)
            return client;
    }
    return NULL;
}

int stk_add_user(stk_client *client)
{
    stk_client *new_client;

    new_client = (stk_client *)malloc(sizeof(stk_client));
    if (new_client != NULL) {
        memset(new_client, 0, sizeof(stk_client));
        stk_user_offline(new_client);
        INIT_LIST_HEAD(&new_client->list);
        new_client->stkc_uid = client->stkc_uid;
        strcpy(new_client->stkc_nickname, client->stkc_nickname);
        strcpy(new_client->stkc_pass, client->stkc_pass);
        strcpy(new_client->stkc_city, client->stkc_city);
        new_client->stkc_phone = client->stkc_phone;
        new_client->stkc_gender = client->stkc_gender;

        list_add_tail(&new_client->list, &stk_users);
    } else {
        printf("malloc error: %s(errno: %d)\n",strerror(errno),errno);
        return -1;
    }

    return 0;
}

int stk_get_pass(unsigned int uid, char *pass)
{
    stk_client *client;

    if (pass == NULL) {
        printf("NULL Pointer, return");
        return STK_NULL_POINTER;
    }

    client = stk_find_user(uid);
    if (client == NULL) {
        return STK_UNKNOWN_USER;
    }
    memcpy(pass, client->stkc_pass, STK_PASS_SIZE);
    return 0;
}

int stk_get_token(unsigned int uid)
{
    stk_client *client;

    client = stk_find_user(uid);
    if (client == NULL) {
        return uid;
    } else {
        return client->stkc_token;
    }
}

pthread_t stk_get_tid(unsigned int uid)
{
    stk_client *client;

    client = stk_find_user(uid);
    if (client == NULL) {
        return STK_ERR_TID;
    } else {
        return client->stkc_tid;
    }
}

stk_client *stk_get_user_by_tid(pthread_t tid)
{
    struct list_head *entry;

    list_for_each(entry, &stk_users) {
        stk_client *client;
        client = list_entry(entry, stk_client, list);
        if (client->stkc_tid == tid)
            return client;
    }
    return NULL;

}


int stk_get_usernum()
{
    struct list_head *entry;
    int num = 0;

    if (!list_empty(&stk_users)) {
        list_for_each(entry, &stk_users) {
            num++;
        }
    }

    return num;
}

stk_client *stk_next_user(stk_client *client)
{
    struct list_head *next_list;
    stk_client *next_client;

    if (client == NULL) {
        next_client = list_entry(stk_users.next, stk_client, list);
    } else {
        next_list = client->list.next;
        if (next_list == &stk_users) {
            next_list = next_list->next;
        }
        next_client = list_entry(next_list, stk_client, list);
    }
    return next_client;
}

int stk_user_offline(stk_client *client)
{
    if (client == NULL) {
        return -1;
    } else {
        client->stkc_fd = -1;
        client->stkc_state = STK_CLIENT_OFFLINE;
        client->stkc_data = NULL;
        return 0;
    }
}

void stk_clear_user()
{
    stk_client *client, *next_client;
    int num = stk_get_usernum();
    client_group *tmp, *tmp2;

    client = stk_next_user(NULL);
    while(client != NULL && num--){
        next_client = stk_next_user(client);
        tmp = client->stkc_group;
        free(client->stkc_data);
        while(tmp != NULL) {
            tmp2 = tmp->next;
            free(tmp);
            tmp = tmp2;
        }
        free(client);
        client = next_client;
    }
}


int stk_print_user(stk_client *client)
{
    client_group  *cgroup = client->stkc_group;
    if (client == NULL) {
        return STK_UNKNOWN_USER;
    }

    printf("====================================================\n");
    printf("=============== stkclient information  =============\n");
    printf("====================================================\n");
    printf("Uid:\t\t%d\n", client->stkc_uid);
    printf("Nickname:\t%s\n", client->stkc_nickname);
    printf("State:\t\t%s\n", (client->stkc_state == STK_CLIENT_ONLINE)?"online":"offline");
    printf("City:\t\t%s\n", client->stkc_city);
    printf("Phone:\t\t%d\n", client->stkc_phone);
    printf("Gender\t\t%s\n", (client->stkc_gender == STK_GENDER_BOY)?"boy":"girl");
    printf("My groups:\t");
    while (cgroup != NULL) {
        printf("%d\t", cgroup->gid);
        cgroup = cgroup->next;
    }
    printf("\n");  
    printf("====================================================\n");
}

int stk_init_msg(chat_message *chatmsg)
{

}

int stk_add_msg(stk_client *client, char *data, int size)
{
    chat_message *chatmsg;
    chat_message *tmp;

    if (client == NULL || data == NULL)
        return -1;

    if (client->stkc_state == STK_CLIENT_OFFLINE) {
        return -1;
    }

    chatmsg = (chat_message *)malloc(sizeof(chat_message));
    if (chatmsg == NULL) {
        printf("Error while malloc for chatmsg\n");
        return -1;
    }

    chatmsg->msg = (char *)malloc(size);
    if (chatmsg->msg == NULL) {
        printf("Error while malloc for chatmsg->msg\n");
        free(chatmsg);
        return -1;
    }

    chatmsg->msg_len = size;
    memcpy(chatmsg->msg, data, size);
    chatmsg->next = chatmsg;

    if (client->chatmsg == NULL || client->msg_num == 0) {
        client->chatmsg = chatmsg;
    } else {
        tmp = client->chatmsg;
        while(tmp != tmp->next) {
            tmp = tmp->next;
        }
        tmp->next = chatmsg;
    }
    client->msg_num++;

    return 0;
}

int stk_get_msg(stk_client *client, char *data, int *size)
{
    chat_message *tmp;

    if (data == NULL)
        return -1;

    tmp = client->chatmsg;
    if(tmp == NULL) {
        return -1;
    }

    *size = tmp->msg_len;
    memcpy(data, tmp->msg, tmp->msg_len);
    client->chatmsg = tmp->next;
    client->msg_num--;

    if (client->msg_num == 0) {
        client->chatmsg == NULL;
    }

    free(tmp->msg);
    free(tmp);

    return 0;
}

