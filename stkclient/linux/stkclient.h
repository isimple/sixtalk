/* 
 * File: stkclient.h
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */

#ifndef _STKCLIENT_H_
#define _STKCLIENT_H_

#define STK_SERVER_PORT        9007
#define STK_CLIENT_PORT        9008

#define STK_MAX_PACKET_SIZE    65535
#define STK_STDIN_SIZE         4096
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

#define STK_CLIENT_OFFLINE     0
#define STK_CLIENT_ONLINE      1

#define STK_GENDER_UNKNOWN     0
#define STK_GENDER_BOY         1
#define STK_GENDER_GIRL        2

#define STK_CLIENT_LOGIN_ERROR       -1
#define STK_CLIENT_LOGIN_SUCCESS      0
#define STK_CLIENT_LOGIN_AGAIN        1
#define STK_CLIENT_LOGIN_INVALID_UID  2
#define STK_CLIENT_LOGIN_INVALID_PASS 3

#define STK_NULL_POINTER       -1

#define STK_SOCKET_ERROR       -1
#define STK_SOCKET_CLOSED      -2

#define STK_CLIENT_TIMEOUT      5

typedef struct{
    unsigned int  uid;
    unsigned char nickname[STK_NICKNAME_SIZE];
    unsigned char city[STK_CITY_SIZE];
    unsigned int  phone;
    unsigned char gender;
    unsigned char pass[STK_PASS_SIZE];
    int fd;
}client_config;

typedef struct{
    unsigned int  uid;
    unsigned char nickname[STK_NICKNAME_SIZE];
    unsigned char city[STK_CITY_SIZE];
    unsigned int  phone;
    unsigned char gender;
    struct list_head list;
}stk_buddy;

#endif /* _STKCLIENT_H_ */

