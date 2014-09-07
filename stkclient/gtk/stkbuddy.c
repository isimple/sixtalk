/* 
 * File: stkbuddy.c
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "stk.h"

LIST_HEAD(stk_buddys);

int stk_init_buddy()
{
    return 0;
}

stk_buddy *stk_find_buddy(unsigned int uid)
{
    struct list_head *entry;

    list_for_each(entry, &stk_buddys) {
        stk_buddy *buddy;
        buddy = list_entry(entry, stk_buddy, list);
        if (buddy->uid == uid)
            return buddy;
    }
    return NULL;
}

int stk_add_buddy(stk_buddy *buddy)
{
    stk_buddy *new_buddy;

    if (buddy == NULL) {
        return STK_NULL_POINTER;
    }

    new_buddy = (stk_buddy *)malloc(sizeof(stk_buddy));
    if (new_buddy != NULL) {
        memset(new_buddy, 0, sizeof(stk_buddy));
        INIT_LIST_HEAD(&new_buddy->list);
        new_buddy->uid = buddy->uid;
        memcpy(new_buddy->nickname, buddy->nickname, STK_NICKNAME_SIZE);
        memcpy(new_buddy->city, buddy->city, STK_CITY_SIZE);
        new_buddy->phone = buddy->phone;
        new_buddy->gender = buddy->gender;

        list_add_tail(&new_buddy->list, &stk_buddys);
    } else {
        stk_print("malloc error, what now?\n");
        return -2;
    }

    return 0;
}

int stk_update_buddy(stk_buddy *buddy)
{
    stk_buddy *new_buddy;

    if (buddy == NULL) {
        return STK_NULL_POINTER;
    }

    new_buddy = stk_find_buddy(buddy->uid);
    if (new_buddy != NULL) {
        new_buddy->uid = buddy->uid;
        memcpy(new_buddy->nickname, buddy->nickname, STK_NICKNAME_SIZE);
        memcpy(new_buddy->city, buddy->city, STK_CITY_SIZE);
        new_buddy->phone = buddy->phone;
        new_buddy->gender = buddy->gender;
    } else {
        stk_print("Error, No such buddy.\n");
        return -1;
    }

    return 0;
}

int stk_get_buddynum()
{
    struct list_head *entry;
    int num = 0;

    if (!list_empty(&stk_buddys)) {
        list_for_each(entry, &stk_buddys) {
            num++;
        }
    }

    return num;
}

stk_buddy *stk_next_buddy(stk_buddy *buddy)
{
    struct list_head *next_list;
    stk_buddy *next_buddy;

    if (buddy == NULL) {
        next_buddy = list_entry(stk_buddys.next, stk_buddy, list);
    } else {
        next_list = buddy->list.next;
        if (next_list == &stk_buddys) {
            next_list = next_list->next;
        }
        next_buddy = list_entry(next_list, stk_buddy, list);
    }

    return next_buddy;
}

void stk_get_buddyinfo(stk_buddy *buddy, char *buf)
{
    char tmp[STK_DEFAULT_SIZE] = {0};

    if (buddy == NULL || buf == NULL){
        return;
    }

    sprintf(tmp, "Uid:\t\t\t%d\n", buddy->uid);
    strcat(buf, tmp);
    sprintf(tmp, "Nickname:\t%s\n", buddy->nickname);
    strcat(buf, tmp);
    sprintf(tmp, "City:\t\t%s\n", buddy->city);
    strcat(buf, tmp);
    sprintf(tmp, "Phone:\t\t%d\n", buddy->phone);
    strcat(buf, tmp);
    sprintf(tmp, "Gender:\t\t%s\n", (buddy->gender == STK_GENDER_BOY)?"boy":"girl");
    strcat(buf, tmp);

    return;
}

int stk_clear_buddy(client_config *config)
{
    stk_buddy *buddy, *next_buddy;
    stk_group *group, *next_group;
    group_member *member, *next_member;
    int num, num2;

    /* free buddy */
    num = stk_get_buddynum();
    buddy = stk_next_buddy(NULL);
    while(buddy != NULL && num--){
        next_buddy = stk_next_buddy(buddy);
        free(buddy);
        buddy = next_buddy;
    }

    /* free client */
    num = config->group_num;
    group = config->group;
    while(group != NULL && num--){
        num2 = group->member_num;
        member = group->members;
        while (member != NULL && num2--) {
            next_member = member->next;
            free(member);
            member = next_member;
        }

        next_group = group->next;
        free(group);
        group = next_group;
    }
}

#if 0
int stk_print_buddy(stk_buddy *buddy)
{
    if (buddy == NULL) {
        return -1;
    }

    printf("====================================================\n");
    printf("=============== STK Client information  ============\n");
    printf("====================================================\n");
    printf("Uid:\t\t%d\n", buddy->uid);
    printf("Nickname:\t%s\n", buddy->nickname);
    printf("City:\t\t%s\n", buddy->city);
    printf("Phone:\t\t%d\n", buddy->phone);
    printf("Gender\t\t%s\n", (buddy->gender == STK_GENDER_BOY)?"boy":"girl");
    printf("====================================================\n");
    fflush(stdout);
}

void stk_print_buddylist()
{
    stk_buddy *buddy = NULL;
    unsigned short buddy_num = stk_get_buddynum();

    printf("====================================================\n");
    printf("=============== Buddy List information  ============\n");
    printf("====================================================\n");

    while (buddy_num--) {
        buddy = stk_get_next(buddy);

        printf("Uid:\t\t%d\n", buddy->uid);
        printf("Nickname:\t%s\n", buddy->nickname);
        printf("City:\t\t%s\n", buddy->city);
        printf("Phone:\t\t%d\n", buddy->phone);
        printf("Gender\t\t%s\n", (buddy->gender == STK_GENDER_BOY)?"boy":"girl");
        printf("====================================================\n");

    }
    fflush(stdout);
}
#endif

void stk_get_groupinfo(stk_group *group, char *buf)
{
    group_member *member;
    char tmp[STK_DEFAULT_SIZE] = {0};
    char tmp2[STK_DEFAULT_SIZE] = {0};
    unsigned short num;

    if (group == NULL || buf == NULL){
        return;
    }

    sprintf(tmp, "GroupID:\t\t%d\n", group->gid);
    strcat(buf, tmp);

    memset(tmp, 0, sizeof(tmp));
    sprintf(tmp, "GroupName:\t%s\n", group->gname);
    strcat(buf, tmp);

    memset(tmp, 0, sizeof(tmp));
    sprintf(tmp, "Members:\t");
    num = group->member_num;
    member = group->members;
    while (num-- && member != NULL) {
        sprintf(tmp2, "%u ", member->uid);
        strcat(tmp, tmp2);
        member = member->next;
    }
    strcat(buf, tmp);

    return;
}

int stk_add_msg(stk_buddy *buddy, char *data, int size)
{
    chat_message *chatmsg;
    chat_message *tmp;

    if (data == NULL)
        return -1;

    chatmsg = (chat_message *)malloc(sizeof(chat_message));
    if (chatmsg == NULL) {
        printf("Error while malloc for chatmsg\n");
        return -1;
    }

    chatmsg->timestamp = (char *)malloc(STK_DEFAULT_SIZE);
    if (chatmsg->timestamp == NULL) {
        printf("Error while malloc for chatmsg->timestamp\n");
        free(chatmsg);
        return -1;
    }

    chatmsg->msg = (char *)malloc(size);
    if (chatmsg->msg == NULL) {
        printf("Error while malloc for chatmsg->msg\n");
        free(chatmsg->timestamp);
        free(chatmsg);
        return -1;
    }

    chatmsg->msg_len = size;
    stk_get_timestamp(chatmsg->timestamp);
    memcpy(chatmsg->msg, data, size);
    chatmsg->next = chatmsg;

    if (buddy->chatmsg == NULL || buddy->msg_num == 0) {
        buddy->chatmsg = chatmsg;
    } else {
        tmp = buddy->chatmsg;
        while(tmp != tmp->next) {
            tmp = tmp->next;
        }
        tmp->next = chatmsg;
    }
    buddy->msg_num++;

    return 0;
}

int stk_get_msg(stk_buddy *buddy, char *data, int *size, char *ts)
{
    chat_message *tmp;

    if (data == NULL || ts == NULL)
        return -1;

    tmp = buddy->chatmsg;
    if(tmp == NULL) {
        return -1;
    }

    *size = tmp->msg_len;
    memcpy(data, tmp->msg, tmp->msg_len);
    strcpy(ts, tmp->timestamp);
    buddy->chatmsg = tmp->next;
    buddy->msg_num--;

    if (buddy->msg_num == 0) {
        buddy->chatmsg == NULL;
    }

    free(tmp->msg);
    free(tmp->timestamp);
    free(tmp);

    return 0;
}

int stk_add_gmsg(stk_group *group, char *data, int size, unsigned int uid)
{
    gchat_message *gchatmsg;
    gchat_message *gtmp;

    if (data == NULL)
        return -1;

    gchatmsg = (gchat_message *)malloc(sizeof(gchat_message));
    if (gchatmsg == NULL) {
        printf("Error while malloc for gchatmsg\n");
        return -1;
    }

    gchatmsg->timestamp = (char *)malloc(STK_DEFAULT_SIZE);
    if (gchatmsg->timestamp == NULL) {
        printf("Error while malloc for chatmsg->timestamp\n");
        free(gchatmsg);
        return -1;
    }

    gchatmsg->msg = (char *)malloc(size);
    if (gchatmsg->msg == NULL) {
        printf("Error while malloc for gchatmsg->msg\n");
        free(gchatmsg->timestamp);
        free(gchatmsg);
        return -1;
    }

    gchatmsg->uid = uid;
    gchatmsg->msg_len = size;
    stk_get_timestamp(gchatmsg->timestamp);
    memcpy(gchatmsg->msg, data, size);
    gchatmsg->next = gchatmsg;

    if (group->gchatmsg == NULL || group->gmsg_num == 0) {
        group->gchatmsg = gchatmsg;
    } else {
        gtmp = group->gchatmsg;
        while(gtmp != gtmp->next) {
            gtmp = gtmp->next;
        }
        gtmp->next = gchatmsg;
    }
    group->gmsg_num++;

    return 0;
}

int stk_get_gmsg(stk_group *group, char *data, int *size, char *ts, unsigned int *uid)
{
    gchat_message *gtmp;

    if (data == NULL || ts == NULL)
        return -1;

    gtmp = group->gchatmsg;
    if(gtmp == NULL) {
        return -1;
    }

    *size = gtmp->msg_len;
    *uid = gtmp->uid;
    memcpy(data, gtmp->msg, gtmp->msg_len);
    strcpy(ts, gtmp->timestamp);
    group->gchatmsg = gtmp->next;
    group->gmsg_num--;

    if (group->gmsg_num == 0) {
        group->gchatmsg == NULL;
    }

    free(gtmp->msg);
    free(gtmp->timestamp);
    free(gtmp);

    return 0;
}


