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
#include "stkclient.h"

/*
 *********************************************
 *              stkui.c                  *
 *********************************************
 */
void stk_hanle_input(int fd, client_config *client);

/*
 *********************************************
 *              stkpacket.c                  *
 *********************************************
 */
int stk_login(int fd, char *buf, int max_len, unsigned int uid);
int stk_send_getprofile(int fd, char *buf, int max_len, unsigned int uid, unsigned int n_uid, stk_buddy *buddy);
int stk_send_getbuddylist(int fd, char *buf, int max_len, unsigned int uid);
int stk_send_msg(int fd, char *buf, int max_len, char *data, int data_len, unsigned int uid, unsigned int n_uid);
int stk_handle_msg(client_config *client, char *buf);

/*
 *********************************************
 *               stkbuddy.c                  *
 *********************************************
 */
int stk_init_buddy(void);
stk_buddy *stk_find_buddy(unsigned int uid);
int stk_add_buddy(stk_buddy *buddy);
int stk_update_buddy(stk_buddy *buddy);
int stk_get_buddynum(void);
stk_buddy *stk_next_buddy(stk_buddy *buddy);
int stk_print_buddy(stk_buddy *buddy);
void stk_print_buddylist(void);

#endif /* _STK_H_ */

