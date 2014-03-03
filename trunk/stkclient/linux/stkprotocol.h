/* 
 * File: stkprotocol.h
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */

#ifndef _STKPROTOCOL_H_
#define _STKPROTOCOL_H_


/* list of known STKP commands */
enum {
    STKP_CMD_REQ_LOGIN       = 0x0001,
    STKP_CMD_LOGIN           = 0x0002,
    STKP_CMD_KEEPALIVE       = 0x0003,
    STKP_CMD_LOGOUT          = 0x0004,
    STKP_CMD_GET_USER        = 0x0005,
    STKP_CMD_GET_ONLINE_USER = 0x0006,
    STKP_CMD_GET_INFO        = 0x0007,
    STKP_CMD_SEND_MSG        = 0x0008,
    STKP_CMD_REPLY_MSG       = 0x0009,
};

#define STKP_TEST_FLAG(x)      (x&0x1)

#define STKP_MAGIC             0x5354
#define STKP_VERSION           0x0001

#define STK_LOGIN_SUCCESS      0
#define STK_LOGIN_AGAIN        1
#define STK_LOGIN_INVALID_UID  2
#define STK_LOGIN_INVALID_PASS 3

#define STKP_PACKET_TAIL       0x07

typedef struct{
    unsigned short stkp_magic;
    unsigned short stkp_version;
    unsigned short stkp_cmd;
    unsigned short stkp_sid;
    unsigned int   stkp_uid;
    unsigned int   stkp_token;
    unsigned char  stkp_reserve;
    unsigned char  stkp_flag;
    unsigned short stkp_length;
}stkp_head;

#endif /* _STKPROTOCOL_H_ */

