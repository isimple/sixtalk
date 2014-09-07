/* 
 * File: stkgroup.c
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"
#include "stk.h"

#define BUF_SIZE_MAX      4096
#define STK_UNKNOWN_GROUP  -2

LIST_HEAD(stk_groups);

/* members_str should be like "107-108-109" */
static int stk_parse_member(char *members_str, stk_group *group)
{
    group_member *newmember, *tmp;
    char *cp;
    int member_num = 0;

    cp = strtok(members_str, "-");
    while (cp != NULL) {
        newmember = (group_member *)malloc(sizeof(group_member));
        if (newmember == NULL) {
            printf("what the fuck!\n");
            continue;
        }
        memset(newmember, 0, sizeof(group_member));
        newmember->uid = atoi(cp);
        newmember->next = NULL;
        if (member_num == 0) {
            group->members = newmember;
            tmp = group->members;
        } else {
            tmp->next = newmember;
            tmp = tmp->next;
        }
        member_num++;
        cp = strtok(NULL, "-");
    }
    return member_num;
}

int stk_init_group()
{
    FILE *fd;
    int len;
    char buf[BUF_SIZE_MAX] = {0};
    cJSON *root = NULL;
    cJSON *item = NULL;
    char *out = NULL;
    int i, count;
    stk_group *group;
    char members_str[STK_DEFAULT_SIZE] = {0};

    fd = fopen(STK_GROUP_FILE, "rb");    
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
        group = (stk_group *)malloc(sizeof(stk_group));
        if (group != NULL) {
            memset(group, 0, sizeof(stk_group));
            memset(members_str, 0, sizeof(members_str));
            INIT_LIST_HEAD(&group->list);
            group->gid = cJSON_GetObjectItem(item,"gid")->valueint;
#if 1
            strcpy(group->gname, cJSON_GetObjectItem(item,"gname")->valuestring);
            strcpy(members_str, cJSON_GetObjectItem(item,"members")->valuestring);
#else
            memcpy(group->gname, cJSON_GetObjectItem(item,"gname")->valuestring, STK_GROUP_NAME_SIZE);
            memcpy(members_str, cJSON_GetObjectItem(item,"members")->valuestring, STK_DEFAULT_SIZE);
#endif
            group->member_num = stk_parse_member(members_str, group);
#ifdef STK_DEBUG
            stk_print_group(group);
#endif
            list_add_tail(&group->list, &stk_groups);
        } else {
            printf("stk_init_group: malloc error\n");
            continue;
        }
    }

    free(root);
    return 0;
}

stk_group *stk_find_group(unsigned int gid)
{
    struct list_head *entry;

    list_for_each(entry, &stk_groups) {
        stk_group *group;
        group = list_entry(entry, stk_group, list);
        if (group->gid == gid)
            return group;
    }
    return NULL;
}

int stk_add_group(stk_group *group)
{
    stk_group *new_group;

    new_group = (stk_group *)malloc(sizeof(stk_group));
    if (new_group != NULL) {
        memset(new_group, 0, sizeof(stk_group));
        INIT_LIST_HEAD(&new_group->list);
        new_group->gid = group->gid;
        strcpy(new_group->gname, group->gname);
        /* TODO: what about members? */

        list_add_tail(&new_group->list, &stk_groups);
    } else {
        printf("stk_add_group: malloc error\n");
        return -1;
    }

    return 0;
}

int stk_get_groupnum()
{
    struct list_head *entry;
    int num = 0;

    if (!list_empty(&stk_groups)) {
        list_for_each(entry, &stk_groups) {
            num++;
        }
    }

    return num;
}

stk_group *stk_next_group(stk_group *group)
{
    struct list_head *next_list;
    stk_group *next_group;

    if (group == NULL) {
        next_group = list_entry(stk_groups.next, stk_group, list);
    } else {
        next_list = group->list.next;
        if (next_list == &stk_groups) {
            next_list = next_list->next;
        }
        next_group = list_entry(next_list, stk_group, list);
    }
    return next_group;
}

void stk_clear_group()
{
    stk_group *group, *next_group;
    group_member *tmp, *tmp2;
    int num = stk_get_groupnum();

    group = stk_next_group(NULL);
    while(group != NULL && num--){
        next_group = stk_next_group(group);
        tmp = group->members;
        while(tmp != NULL) {
            tmp2 = tmp->next;
            free(tmp);
            tmp = tmp2;
        }
        free(group);
        group = next_group;
    }
}

int stk_print_group(stk_group *group)
{
    group_member *member;

    if (group == NULL) {
        return STK_UNKNOWN_GROUP;
    }

    printf("====================================================\n");
    printf("Uid:\t\t%d\n", group->gid);
    printf("Groupname:\t%s\n", group->gname);
    printf("Members:\t");
    member = group->members;
    while (member != NULL) {
        printf("%d  ", member->uid);
        member = member->next;
    }
    printf("\n====================================================\n");
}


#if 0
int stk_init_msg(struct chat_message *chatmsg)
{

}

int stk_add_msg(stk_client *client, char *data, int size)
{
    struct chat_message *chatmsg;
    struct chat_message *tmp;

    if (data == NULL)
        return -1;

    chatmsg = (struct chat_message *)malloc(sizeof(struct chat_message));
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
    struct chat_message *tmp;

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
#endif
