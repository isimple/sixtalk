/*
 * File: stk.h
 *
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */

#ifndef _STK_H_
#define _STK_H_

#define _WIN32_IE 0x0800

#include <windows.h>
#include <WindowsX.h>
#include <commctrl.h>

#include "stklist.h"
#include "stkprotocol.h"
#include "stkclient.h"

/*
 *********************************************
 *              stkui.c                  *
 *********************************************
 */
void stk_create_ui(HWND hwnd);
void stk_buddywin_create();
void stk_loginbtn_pressed(void);
void stk_chatwin_register();

#if 0

/*
 *********************************************
 *              stkchat.c                  *
 *********************************************
 */
void stk_chat_request(GtkWidget *widget, stk_buddy *buddy);
void stk_voice_request(GtkWidget *widget, stk_buddy *buddy);
void stk_video_request(GtkWidget *widget, stk_buddy *buddy);
void stk_chatwin_show(GtkWidget *widget, stk_buddy *buddy);
gboolean stk_msg_send(GtkWidget *widget, stk_buddy *buddy);
void stk_msg_event(stk_buddy *buddy);


/*
 *********************************************
 *              stkgchat.c                  *
 *********************************************
 */
void stk_gchatwin_show(GtkWidget *widget, stk_group *group);
gboolean stk_gmsg_send(GtkWidget *widget, stk_group *group);
void stk_gmsg_event(stk_group *group);

#endif
/*
 *********************************************
 *              stkpacket.c                  *
 *********************************************
 */
int stk_init_socket(void);
void stk_clean_socket(socket_t fd);
int stk_connect(client_config *config);
int stk_login(socket_t fd, char *buf, int max_len, unsigned int uid, char *password);
int stk_send_getbuddyinfo(socket_t fd, char *buf, int max_len, unsigned int uid, unsigned int n_uid, stk_buddy *buddy);
int stk_send_getbuddy(socket_t fd, char *buf, int max_len, unsigned int uid);
int stk_send_getgroupinfo(socket_t fd, char *buf, int max_len, unsigned int uid, unsigned int gid, stk_group *group);
int stk_send_getgroup(socket_t fd, char *buf, int max_len, unsigned int uid, client_config *client);
int stk_send_msg(socket_t fd, char *buf, int max_len, char *data, int data_len, unsigned int uid, unsigned int id, BOOL gmsg);
int stk_recv_msg(client_config *client);;

/*
 *********************************************
 *               stkbuddy.c                  *
 *********************************************
 */
stk_buddy *stk_find_buddy(unsigned int uid);
int stk_add_buddy(stk_buddy *buddy);
int stk_update_buddy(stk_buddy *buddy);
int stk_get_buddynum(void);
stk_buddy *stk_next_buddy(stk_buddy *buddy);
int stk_add_msg(stk_buddy *buddy, char *data, int size);
int stk_get_msg(stk_buddy *buddy, char *data, int *size, char *ts);
int stk_add_gmsg(stk_group *group, char *data, int size, unsigned int uid);
int stk_get_gmsg(stk_group *group, char *data, int *size, char *ts, unsigned int *uid);

/*
 *********************************************
 *              stkutil.c                  *
 *********************************************
 */
void stk_init_running();
int stk_get_running();
void stk_set_running(int running);
void stk_get_timestamp(char *str);
void stk_play_wav(char *filename);
void stk_log(const char *fmt, ...);

#endif /* _STK_H_ */

