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
    STKP_CMD_GET_USER_INFO   = 0x0007,
    STKP_CMD_GET_GROUP       = 0x0008,
    STKP_CMD_GET_GROUP_INFO  = 0x0009,
    STKP_CMD_SEND_MSG        = 0x000A,
    STKP_CMD_REPLY_MSG       = 0x000B,
    STKP_CMD_SEND_GMSG       = 0x000C,
    STKP_CMD_REPLY_GMSG      = 0x000D,
    STKP_CMD_END             = 0x00FF
};

#define STKP_TEST_FLAG(x)      (x&0x1)

#define STKP_MAGIC             0x5354
#define STKP_VERSION           0x0001

#define STKP_DEF_TOKEN         0x0

#define STK_LOGIN_REVERSE_SIZE 64
#define STK_DATA_ZERO_LENGTH   0

/* size about user */
#define STK_ID_LENGTH          4
#define STK_NICKNAME_SIZE      32
#define STK_PASS_SIZE          32
#define STK_CITY_SIZE          16
#define STK_PHONE_LENGTH       4
#define STK_GENDER_LENGTH      1
#define STK_ID_NUM_LENGTH      2

/* size about group */
#define STK_GID_LENGTH         4
#define STK_GID_NUM_LENGTH     2
#define STK_GROUP_NAME_SIZE    32

/* login result value */
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

