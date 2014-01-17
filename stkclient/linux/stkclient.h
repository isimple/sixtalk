/* 
 * File: stkclient.h
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */

#ifndef _STKCLIENT_H_
#define _STKCLIENT_H_

#define STK_SERVER_PORT 9008
#define STK_MAX_PACKET_SIZE 65535
#define STK_PASS_SIZE 64
#define STK_DEFAULT_SIZE 64

#define STK_CLIENT_STATE_OFFLINE  0
#define STK_CLIENT_STATE_ONLINE   1
#define STK_CLIENT_STATE_REQ      2

typedef struct{
    unsigned short cmd;
    char *data;
    int len;
}stk_data;

typedef struct{
    int stkc_fd;
    stk_data stkc_data;
    int stkc_state;
    unsigned int stkc_token;
    unsigned int stkc_uid;
    unsigned char stkc_pass[STK_PASS_SIZE];
    unsigned char stkc_city[STK_DEFAULT_SIZE];
    unsigned int stkc_phone;
    unsigned char stkc_gender;
}stk_client;

#endif /* _STKCLIENT_H_ */

