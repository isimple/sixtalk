/* 
 * File: stk.h
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */

#ifndef _STK_H_
#define _STK_H_

#ifdef WIN32
#include <windows.h>
#include <winsock2.h>
#endif

#ifdef _LINUX_
#include <errno.h>
#endif

#include <gtk/gtk.h>

#include "stklist.h"
#include "stkprotocol.h"
#include "stkserver.h"

//#define STK_DEBUG

/*
 *********************************************
 *              stkui.c                  *
 *********************************************
 */
GtkWidget *stk_window_create(void);
void stk_create_userlist(GtkWidget *window);
gboolean stk_buddytree_update(stk_client *client);


/*
 *********************************************
 *              stkpacket.c                  *
 *********************************************
 */
int stk_init_socket(void);
void stk_clean_socket(void);
int stk_server_socket(void);
stk_client *stk_parse_packet(char *buf, int size, stk_data *data);
gboolean stk_deliver_msg(stk_client *client);
int stk_reqlogin_ack(socket_t fd, unsigned int uid, char *buf);
int stk_login_ack(socket_t fd, unsigned int uid, char *buf);
int stk_keepalive_ack(stk_client *client, char *buf);
int stk_getuser_ack(stk_client *client, char *buf);
int stk_getonlineuser_ack(stk_client *client, char *buf);
int stk_getuserinfo_ack(stk_client *client, char *buf);
int stk_getgroup_ack(stk_client *client, char *buf);
int stk_sendmsg_ack(stk_client *client, char *buf, int bytes);
int stk_sendgmsg_ack(stk_client *client, char *buf, int bytes);
void stk_socket_thread(void *arg);


/*
 *********************************************
 *               stkuser.c                   *
 *********************************************
 */
int stk_init_user(void);
stk_client *stk_find_user(unsigned int uid);
int stk_get_usernum(void);
stk_client *stk_next_user(stk_client *client);
//stk_client *stk_get_user_by_tid(pthread_t tid);
int stk_init_msg(chat_message *chatmsg);
int stk_add_msg(stk_client *client, char *data, int size);
int stk_get_msg(stk_client *client, char *data, int *size);
int stk_print_user(stk_client *client);


/*
 *********************************************
 *               stkgroup.c                   *
 *********************************************
 */
int stk_init_group(void);
stk_group *stk_find_group(unsigned int gid);
int stk_add_group(stk_group *group);
int stk_get_groupnum(void);
stk_group *stk_next_group(stk_group *client);
void stk_clear_group(void);
int stk_print_group(stk_group *group);


#endif /* _STK_H_ */

