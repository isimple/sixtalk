/* 
 * File: stkserver.c
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stk.h"

void stk_socket()
{
    socket_t server_fd, conn_fd;
    GThread *tid;

    if (stk_init_socket() != 0) {
        //stk_message("Socket Init", "stk_init_socket error");
        exit(0);
    }

    if ((server_fd = stk_server_socket()) == -1){
        printf("create stkserver socket error!exiting....\n");
        exit(0);
    }

    printf("====================================================\n");
    printf("================= STK IM SERVER ====================\n");
    printf("====================================================\n");

    while(1){
        if((conn_fd = accept(server_fd, (struct sockaddr*)NULL, NULL)) == -1){
            printf("stk_socket: accept socket error");
            continue;
        }

        tid = g_thread_create((GThreadFunc)stk_socket_thread, (gpointer)&conn_fd, FALSE, NULL);  
        if (tid == NULL) {
            printf("stk_socket: can't create thread\n");
        }

    }

    close(server_fd);

}

void stk_main()
{
    GtkWidget *window;
    int ret;

    /* group must be init before user. */
    stk_init_group();
    stk_init_user();

    window = stk_window_create();
    stk_create_userlist(window);

    gtk_widget_show_all(window);

    gdk_threads_enter();
    gtk_main();
    gdk_threads_leave();
}

int main (int argc,char *argv[])
{
    GThread *thread1, *thread2;

    if(!g_thread_supported()) {
        g_thread_init(NULL);
    }
    gdk_threads_init();

    gtk_init(&argc, &argv);

    thread1 = g_thread_create((GThreadFunc)stk_main, NULL, TRUE, NULL);
    thread2 = g_thread_create((GThreadFunc)stk_socket, NULL, FALSE, NULL);
    if (thread1 == NULL || thread2 == NULL) {
        printf("main: can't create thread stk_socket\n");
    }

    g_thread_join (thread1);

    /* 
     * On linux, when program exit, it will close all opened socket,
     * but on Windows will not.
     *
     */
#if defined(WIN32)
    stk_clean_socket();
#endif

    stk_clear_user();
    stk_clear_group();

    return 0;
}

