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

#include "cJSON.h"
#include "list.h"
#include "stkserver.h"

#define BUF_SIZE_MAX 4096

LIST_HEAD(stk_users);

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
            INIT_LIST_HEAD(&client->list);
            client->stkc_uid = cJSON_GetObjectItem(item,"uid")->valueint;
            strcpy(client->stkc_pass, cJSON_GetObjectItem(item,"pass")->valuestring);
            strcpy(client->stkc_city, cJSON_GetObjectItem(item,"city")->valuestring);
            client->stkc_phone = cJSON_GetObjectItem(item,"phone")->valueint;
            client->stkc_gender = cJSON_GetObjectItem(item,"gender")->valueint;

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
		if (client->stkc_uid== uid)
			return client;
	}
	return NULL;
}

int stk_add_user(stk_client client)
{
    stk_client *new_client;

    new_client = (stk_client *)malloc(sizeof(stk_client));
    if (new_client != NULL) {
        INIT_LIST_HEAD(&new_client->list);
        new_client->stkc_uid = client.stkc_uid;
        strcpy(new_client->stkc_pass, client.stkc_pass);
        strcpy(new_client->stkc_city, client.stkc_city);
        new_client->stkc_phone = client.stkc_phone;
        new_client->stkc_gender = client.stkc_gender;

        list_add_tail(&new_client->list, &stk_users);
    } else {
        printf("malloc error: %s(errno: %d)\n",strerror(errno),errno);
        return -1;
    }

    return 0;
}
