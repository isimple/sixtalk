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

#define STK_DEFAULT_SIZE       64

#define STK_CLIENT_OFFLINE     0
#define STK_CLIENT_ONLINE      1

#define STK_GENDER_UNKNOWN     0
#define STK_GENDER_BOY         1
#define STK_GENDER_GIRL        2

#define STK_ERR_TID           -1

#define STK_NULL_POINTER      -1

#define STK_USER_FILE          "users"
#define STK_GROUP_FILE         "groups"

#if defined(WIN32)
#define socket_t  SOCKET
#elif defined(_LINUX_)
#define socket_t  int
#endif

/* notice, msg include stkp head!! */
typedef struct _chat_message{
    int  msg_len;
    char *msg;
    struct _chat_message *next;
}chat_message;

typedef struct{
    unsigned int   uid;
    unsigned short cmd;
    char           *data;
    int            len;
}stk_data;

typedef struct _client_group{
    unsigned int gid;
    struct _client_group *next;
}client_group;

typedef struct{
    struct list_head list;
    unsigned int  stkc_uid;
    unsigned char stkc_nickname[STK_NICKNAME_SIZE];
    unsigned char stkc_pass[STK_PASS_SIZE];
    unsigned char stkc_city[STK_CITY_SIZE];
    unsigned int  stkc_phone;
    unsigned char stkc_gender;
    unsigned int  stkc_token;
    int           stkc_groupnum;
    client_group  *stkc_group;
    socket_t      stkc_fd;
    int           stkc_state;
    GThread       *stkc_tid;
    stk_data      *stkc_data;
    int           msg_num;
    chat_message  *chatmsg;
}stk_client;

typedef struct _group_member{
    unsigned int uid;
    struct _group_member *next;
}group_member;

typedef struct{
    struct list_head list;
    unsigned int  gid;
    unsigned char gname[STK_GROUP_NAME_SIZE];
    int           member_num;
    group_member  *members;
    int           msg_num;
    chat_message  *chatmsg;
}stk_group;

#endif /* _STKSERVER_H_ */

