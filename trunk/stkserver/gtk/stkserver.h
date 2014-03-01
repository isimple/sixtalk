/* 
 * File: stkserver.h
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */

#ifndef _STKSERVER_H_
#define _STKSERVER_H_

#define STK_SERVER_PORT        9007
#define STK_MAX_CLIENTS        30

#define STK_MAX_PACKET_SIZE    65535
#define STK_NICKNAME_SIZE      32
#define STK_PASS_SIZE          32
#define STK_CITY_SIZE          16
#define STK_LOGIN_REVERSE_SIZE 64

#define STK_DATA_ZERO_LENGTH   0

#define STK_DEFAULT_SIZE       64

#define STK_ID_LENGTH          4
#define STK_PHONE_LENGTH       4
#define STK_GENDER_LENGTH      1

#define STK_ID_NUM_LENGTH      2

#define STK_USER_FILE          "users"

#define STK_CLIENT_OFFLINE     0
#define STK_CLIENT_ONLINE      1

#define STK_GENDER_UNKNOWN     0
#define STK_GENDER_BOY         1
#define STK_GENDER_GIRL        2

#define STK_ERR_TID -1

#define STK_NULL_POINTER       -1

#if defined(WIN32)
#define socket_t  SOCKET
#elif defined(_LINUX_)
#define socket_t  int
#endif

typedef struct{
    unsigned int uid;
    unsigned short cmd;
    char *data;
    int len;
}stk_data;

typedef struct{
    struct list_head list;
    socket_t stkc_fd;
    GThread *stkc_tid;
    stk_data *stkc_data;
    int stkc_state;
    unsigned int  stkc_token;
    unsigned int  stkc_uid;
    unsigned char stkc_nickname[STK_NICKNAME_SIZE];
    unsigned char stkc_pass[STK_PASS_SIZE];
    unsigned char stkc_city[STK_CITY_SIZE];
    unsigned int  stkc_phone;
    unsigned char stkc_gender;
}stk_client;

#endif /* _STKSERVER_H_ */

