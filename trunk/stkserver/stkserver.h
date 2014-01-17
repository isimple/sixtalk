/* 
 * File: stkserver.h
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */

#ifndef _STKSERVER_H_
#define _STKSERVER_H_

#define STK_SERVER_PORT 9007
#define STK_MAX_CLIENTS 30
#define STK_MAX_PACKET_SIZE 65535
#define STK_PASS_SIZE 64
#define STK_DEFAULT_SIZE 64

#define STK_USER_FILE "users"

typedef struct{
    int fd;
    unsigned int stkc_uid;
    unsigned char stkc_pass[STK_PASS_SIZE];
    unsigned char stkc_city[STK_DEFAULT_SIZE];
    unsigned int stkc_phone;
    unsigned char stkc_gender;
}stk_client;

#endif /* _STKSERVER_H_ */

