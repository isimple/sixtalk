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

/*
 *********************************************
 *              stkui.c                  *
 *********************************************
 */
GtkWidget *stk_window_create(void);
void stk_create_userlist(GtkWidget *window);
gboolean stk_tree_update(stk_client *client);


/*
 *********************************************
 *              stkpacket.c                  *
 *********************************************
 */
int stk_init_socket(void);
void stk_clean_socket(void);
int stk_server_socket(void);
stk_client *stk_parse_packet(char *buf, int size, stk_data *data);
int stk_reqlogin_ack(socket_t fd, unsigned int uid, char *buf);
int stk_login_ack(socket_t fd, unsigned int uid, char *buf);
int stk_keepalive_ack(stk_client *client, char *buf);
int stk_getuser_ack(stk_client *client, char *buf);
int stk_getonlineuser_ack(stk_client *client, char *buf);
int stk_getinfo_ack(stk_client *client, char *buf);
int stk_sendmsg_ack(stk_client *client, char *buf, int bytes);
void stk_handle_signal(int signal);
void stk_socket_thread(void *arg);


/*
 *********************************************
 *               stkuser.c                   *
 *********************************************
 */
int stk_init_user(void);
stk_client *stk_find_user(unsigned int uid);
stk_client *stk_find_user(unsigned int uid);
stk_client *stk_get_next(stk_client *client);
//stk_client *stk_get_user_by_tid(pthread_t tid);


#endif /* _STK_H_ */

