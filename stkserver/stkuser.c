/* 
 * File: stkuser.c
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */

#include <stdio.h>

#include "cJSON.h"

#define BUF_SIZE_MAX 4096


int stk_read_user()
{
    FILE *fd;
    int len;
    char buf[BUF_SIZE_MAX] = {0};
    cJSON *root = NULL;
    cJSON *item = NULL;
    char *out = NULL;
    int count;
    stk_client client;

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
        out = cJSON_Print(root);
        printf("%s\n",out);  
        free(out);
    }
    
    count = cJSON_GetArraySize (root);
    for( int i = 0; i < count; i++)
    {
        item = cJSON_GetArrayItem(root, i);
        out = cJSON_Print(item);
        printf("%s\n",out);  
        free(out);

        client.stkc_uid = cJSON_GetObjectItem(item,"uid")->valueint;
        client.stkc_pass = cJSON_GetObjectItem(item,"pass")->valuestring;
        client.stkc_city = cJSON_GetObjectItem(item,"city")->valueint;
        client.stkc_phone = cJSON_GetObjectItem(item,"phone")->valueint;
        client.stkc_gender = cJSON_GetObjectItem(item,"gender")->valueint;
    }
}

stk_client *stk_find_user(unsigned int uid)
{
    return NULL;
}

int stk_add_user(stk_client *client)
{
    return 0;
}
