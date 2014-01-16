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

typedef struct{
    int fd;
    unsigned int stkc_uid;
}stk_client;

#endif /* _STKSERVER_H_ */

