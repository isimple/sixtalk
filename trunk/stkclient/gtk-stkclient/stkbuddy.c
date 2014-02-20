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
        stk_print("malloc error: %s(errno: %d)\n",strerror(errno),errno);
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

unsigned short stk_get_buddynum()
{
    struct list_head *entry;
    unsigned short num = 0;

    if (!list_empty(&stk_buddys)) {
        list_for_each(entry, &stk_buddys) {
            num++;
        }
    }

    return num;
}

stk_buddy *stk_get_next(stk_buddy *buddy)
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

int stk_init_msg(struct chat_message *chatmsg)
{

}

int stk_add_msg(stk_buddy *buddy, char *data, int size)
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
    struct chat_message *tmp;

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

