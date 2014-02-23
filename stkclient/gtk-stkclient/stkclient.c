/* 
 * File: stkclient.c
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stk.h"
#include "stkwidget.h"

STKWIDGETS widgets;
client_config client;

int main (int argc,char *argv[])
{
    int ret;

    memset(&client, 0, sizeof(client));
    memset(&widgets, 0, sizeof(widgets));

    ret = stk_init_socket(&client.fd);
    if (ret != 0) {
        stk_message("Socket Init", "stk_init_socket error");
        exit(0);
    }

#ifdef USE_GTK_THREAD
    if(!g_thread_supported()) {
        g_thread_init(NULL);
    }
    gdk_threads_init();
#endif

    gtk_init(&argc, &argv);

    stk_screen_get(&widgets);

    widgets.mainw = stk_mainwin_create();
    widgets.tray = stk_tray_create(widgets.mainw);
    g_signal_connect(G_OBJECT(widgets.mainw), "destroy", G_CALLBACK (stk_window_exit), (gpointer)widgets.tray);
    g_signal_connect(G_OBJECT(widgets.mainw), "delete_event", G_CALLBACK (stk_window_exit), (gpointer)widgets.tray);

    stk_loginwin_create(&widgets);

    gtk_container_add(GTK_CONTAINER(widgets.mainw), widgets.loginw.layout);

    /* set window position */
    gtk_window_move(GTK_WINDOW(widgets.mainw), 
		           widgets.size.width-40-STK_WINDOW_WIDTH, (widgets.size.height-STK_WINDOW_HEIGHT)/2);
    gtk_widget_show_all(widgets.mainw);

    /* set window toplevel, then set usertext as focus. For windows, it's necessary, for Linux, seems no need */
	gtk_window_present(GTK_WINDOW(widgets.mainw));
#if 0
#if GTK_CHECK_VERSION(2,18,0)
    gtk_widget_set_can_focus(widgets.loginw.usertext, GTK_CAN_FOCUS);
#endif
#endif
    gtk_widget_grab_focus(widgets.loginw.usertext);

#ifdef USE_GTK_THREAD
    gdk_threads_enter();
#endif
    gtk_main();
#ifdef USE_GTK_THREAD
    gdk_threads_leave();
#endif
}

