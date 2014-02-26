/* 
 * File: stk.h
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */

#ifndef _STK_H_
#define _STK_H_

#include "list.h"
#include "stkprotocol.h"
#include "stkserver.h"

/*
 *********************************************
 *              stkpacket.c                  *
 *********************************************
 */
int stk_server_socket(void);
stk_client *stk_parse_packet(char *buf, int size, stk_data *data);
int stk_reqlogin_ack(int fd, unsigned int uid, char *buf);
int stk_login_ack(int fd, unsigned int uid, char *buf);
int stk_keepalive_ack(stk_client *client, char *buf);
int stk_getuser_ack(stk_client *client, char *buf);
int stk_getonlineuser_ack(stk_client *client, char *buf);
int stk_getinfo_ack(stk_client *client, char *buf);
int stk_sendmsg_ack(stk_client *client, char *buf, int bytes);
void stk_handle_signal(int signal);


/*
 *********************************************
 *               stkuser.c                   *
 *********************************************
 */
int stk_init_user(void);
stk_client *stk_find_user(unsigned int uid);
stk_client *stk_find_user(unsigned int uid);
stk_client *stk_get_next(stk_client *client);
stk_client *stk_get_user_by_tid(pthread_t tid);


#endif /* _STK_H_ */

