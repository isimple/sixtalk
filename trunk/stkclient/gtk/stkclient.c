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

client_config client;

G_LOCK_DEFINE(mutex);
int stk_running;

int stk_get_running()
{
    int running;
    G_LOCK(mutex);
    running = stk_running;
    G_UNLOCK(mutex);
    return running;
}

void stk_set_running(int new_running)
{
    G_LOCK(mutex);
    stk_running = new_running;
    G_UNLOCK(mutex);
}

void stk_socket(client_config *client)
{
    unsigned char sendbuf[STK_MAX_PACKET_SIZE] = {0};
    int running;
    int ret;

    ret = stk_init_socket();
    if (ret != 0) {
        stk_message("Socket Init", "stk_init_socket error");
        exit(0);
    }

connect:

    while (1) {
        running = stk_get_running();
        if (running == STK_EXITING) {
            goto exit;
        }
        else if (running == STK_CONNECTE_REQ) {
            stk_set_running(STK_CONNECTING);
            ret = stk_connect(client);
            if (stk_get_running() != STK_CONNECTING) { /* user cancel the login */
                close(client->fd);
                continue;
            }

            if (ret == -1) {
                stk_set_running(STK_CONNECTE_ERR);
            } else {
                ret = stk_login(client->fd, sendbuf, STK_MAX_PACKET_SIZE, client->uid, client->pass);
                if (ret == STK_CLIENT_LOGIN_SUCCESS){
                    stk_buddy buddy;
                    client->state = STK_CLIENT_ONLINE;
                    memset(&buddy, 0 ,sizeof(stk_buddy));

                    ret = stk_send_getbuddyinfo(client->fd, sendbuf, STK_MAX_PACKET_SIZE, client->uid, client->uid, &buddy);
                    if (ret == -1){
                        printf("What to do now, there must be something wrong with socket!\n");
                        stk_set_running(STK_SOCKET_ERR);
                        goto connect;
                    } else {
                        memcpy(client->nickname, buddy.nickname, STK_NICKNAME_SIZE);
                        memcpy(client->city, buddy.city,STK_CITY_SIZE);
                        client->phone = buddy.phone;
                        client->gender = buddy.gender;
                    }

                    ret = stk_send_getbuddy(client->fd, sendbuf, STK_MAX_PACKET_SIZE, client->uid);
                    if (ret == -1){
                        printf("What to do now, there must be something wrong with socket!\n");
                        stk_set_running(STK_SOCKET_ERR);
                        goto connect;
                    }

                    ret = stk_send_getgroup(client->fd, sendbuf, STK_MAX_PACKET_SIZE, client->uid, client);
                    if (ret == -1){
                        printf("What to do now, there must be something wrong with socket!\n");
                        stk_set_running(STK_SOCKET_ERR);
                        goto connect;
                    }

                    stk_set_running(STK_CONNECTED);
                    break;
                } 
                else if (ret == STK_CLIENT_LOGIN_ERROR){
                    stk_set_running(STK_SOCKET_ERR);
                    goto connect;
                }
                else if (ret == STK_CLIENT_LOGIN_INVALID_UID){
                    stk_set_running(STK_USERID_ERR);
                    goto connect;
                }
                else if (ret == STK_CLIENT_LOGIN_INVALID_PASS){
                    stk_set_running(STK_PASSWORD_ERR);
                    goto connect;
                }
                else if (ret == STK_CLIENT_LOGIN_AGAIN){
                    stk_set_running(STK_ALREADY_LOGGIN);
                    goto connect;
                }
                else {
                    stk_set_running(STK_SOCKET_ERR);
                    goto connect;
                }
            } 
        }
        else {
            g_usleep(500000);
        }
    }

    /* In fact, this loop will go into stk_recv_msg then blocked~~~ */
    while (1) {
        running = stk_get_running();
        if (running == STK_EXITING) {
            return;
            //goto exit;
        } else if (running != STK_RUNNING) {
            g_usleep(500000);
        } else {
            /* receive message from server, this will be blocked */
            ret = stk_recv_msg(client);
            if (ret == STK_SOCKET_ERROR) {
                stk_set_running(STK_CONNECTE_ERR);
            } else if (ret == STK_SOCKET_CLOSED) {
                stk_set_running(STK_SERVER_EXIT);
            }
        }
    }

exit:
   close(client->fd);
}

void stk_main(StkWidget *widgets)
{
    ScreenSize screensize;
    int ret;

    /* init main window */
    widgets->mainw = stk_mainwin_create();
    widgets->tray = stk_tray_create(widgets->mainw);
    g_signal_connect(G_OBJECT(widgets->mainw), "destroy", G_CALLBACK (stk_window_exit), (gpointer)widgets->tray);
    g_signal_connect(G_OBJECT(widgets->mainw), "delete_event", G_CALLBACK (stk_window_exit), (gpointer)widgets->tray);

    /* init login window */
    stk_loginwin_create(widgets);

    stk_screensize_get(&screensize);
    gtk_window_move(GTK_WINDOW(widgets->mainw),  
                screensize.width-STK_DISTANCE_RSCREEN-STK_MAINWIN_WIDTH, (screensize.height-STK_MAINWIN_HEIGHT)/2);

    gtk_widget_show_all(widgets->mainw);

    /* set window toplevel, then set usertext as focus. For windows, it's necessary, for Linux, seems no need */
    gtk_window_present(GTK_WINDOW(widgets->mainw));
#if 0
#if GTK_CHECK_VERSION(2,18,0)
    gtk_widget_set_can_focus(widgets.loginw.usertext, GTK_CAN_FOCUS);
#endif
#endif
    gtk_widget_grab_focus(widgets->loginw.usertext);

    stk_set_running(STK_INITIALIZED);

    gdk_threads_enter();
    gtk_main();
    gdk_threads_leave();
}

int main (int argc,char *argv[])
{
    StkWidget widgets;
    GThread *thread1, *thread2;

    memset(&client, 0, sizeof(client));
    memset(&widgets, 0, sizeof(widgets));

    if(!g_thread_supported()) {
        g_thread_init(NULL);
    }
    gdk_threads_init();

    gtk_init(&argc, &argv);

    thread1 = g_thread_create((GThreadFunc)stk_main, (gpointer)&widgets, TRUE, NULL);  
    thread2 = g_thread_create((GThreadFunc)stk_socket, (gpointer)&client, FALSE, NULL);

    /*
     * we cann't set stk_socket to join, because it is blocked, if we wait, it will never end
     * we can use select to do this, maybe later.
     */
    g_thread_join(thread1);
    //g_thread_join(thread2);

    /* 
     * On linux, when program exit, it will close all opened socket,
     * but on Windows will not.
     *
     */
#if defined(WIN32)
    stk_clean_socket(client.fd);
#endif
    stk_clear_buddy(&client);

    return 0;
}

