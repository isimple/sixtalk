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

#if 1//def _LINUX_
#include <errno.h>
#endif

#include <gtk/gtk.h>

#include "stklist.h"
#include "stkprotocol.h"
#include "stkclient.h"
#include "stkwidget.h"

//#define STK_GUI_DEBUG

/*
 *********************************************
 *               stkbuddy.c                  *
 *********************************************
 */
stk_buddy *stk_find_buddy(unsigned int uid);
int stk_add_buddy(stk_buddy *buddy);
int stk_update_buddy(stk_buddy *buddy);
unsigned short stk_get_buddynum(void);
stk_buddy *stk_get_next(stk_buddy *buddy);


/*
 *********************************************
 *              stkpacket.c                  *
 *********************************************
 */
int stk_recv_msg(client_config *client);


/*
 *********************************************
 *              stkui.c                  *
 *********************************************
 */
GtkWidget *stk_mainwin_create(void);
void stk_window_exit(GtkWidget *widget, GtkStatusIcon *tray);
GtkStatusIcon *stk_tray_create(GtkWidget *window);
void stk_loginwin_create(STKWIDGETS *widgets);


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


#endif /* _STK_H_ */

