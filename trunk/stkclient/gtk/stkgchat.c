/* 
 * File: stkgchat.c
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stk.h"

/*
 * g_object_get_data g_object_set_data
 * g_locale_to_utf8
 *
 */

extern client_config client;

void stk_gtexttag_create(GtkTextBuffer *buffer, gchat_widgets *gchat)
{    
    gchat->minfo = gtk_text_buffer_create_tag(buffer, "minfo", "foreground", "blue", "size", 8 * PANGO_SCALE, NULL);
    gchat->mtext = gtk_text_buffer_create_tag(buffer, "mtext", "foreground", "black", "size", 10 * PANGO_SCALE,
                                                                                    "left_margin", 10, NULL);

    gchat->binfo = gtk_text_buffer_create_tag(buffer, "binfo", "foreground", "red", "size", 8 * PANGO_SCALE, NULL);
    gchat->btext = gtk_text_buffer_create_tag(buffer, "btext", "foreground", "black", "size", 10 * PANGO_SCALE, 
                                                                                    "left_margin", 10, NULL);
}

static int stk_gchatwin_close(GtkWidget *window, stk_group *group)
{
    group->gchat.show = FALSE;
    gtk_widget_destroy(group->gchat.window);

    return TRUE;
}

static void stk_gchatwin_create(stk_group *group)
{
    char buf[STK_DEFAULT_SIZE] = {0};
    GdkColor color;
    GtkWidget *image;
    GtkAccelGroup *gag;

    sprintf(buf, "Chat In Group %s", group->groupname);
    group->gchat.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(group->gchat.window), buf);
    gtk_window_set_position(GTK_WINDOW(group->gchat.window), GTK_WIN_POS_CENTER);
    gtk_widget_set_size_request(group->gchat.window, STK_CHATWIN_WIDTH, STK_CHATWIN_HEIGHT);
    gtk_window_set_icon(GTK_WINDOW(group->gchat.window), gdk_pixbuf_new_from_file(STK_GSESSION_PNG, NULL));

    /* set group chat window background color */
    gdk_color_parse(MAIN_COLRO_STRING, &color);
    gtk_widget_modify_bg(group->gchat.window, GTK_STATE_NORMAL, &color);

    /* "quit" button */
    g_signal_connect(G_OBJECT(group->gchat.window), "destroy", G_CALLBACK(stk_gchatwin_close), (gpointer)group);

    group->gchat.toolbox = gtk_hbox_new(FALSE, 0);

    group->gchat.send_button = gtk_button_new_with_label("   Send   ");
    group->gchat.close_button = gtk_button_new_with_label("   Close   ");

    g_signal_connect(G_OBJECT(group->gchat.close_button), "clicked", G_CALLBACK(stk_gchatwin_close), (gpointer)group);
    g_signal_connect(G_OBJECT(group->gchat.send_button), "clicked", G_CALLBACK(stk_gmsg_send), (gpointer)group);

    gag = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(group->gchat.window), gag);
    gtk_widget_add_accelerator(group->gchat.send_button, "clicked", gag, GDK_Return, 0, GTK_ACCEL_VISIBLE);

    /* text view */
    group->gchat.show_view = gtk_text_view_new();
    group->gchat.input_view = gtk_text_view_new();

    /* text view wrap line automatic, Note, hscrollbar_policy must set to GTK_POLICY_AUTOMATIC*/
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(group->gchat.show_view), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(group->gchat.input_view), GTK_WRAP_WORD_CHAR);

    /* get the buffer of textbox */
    group->gchat.show_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(group->gchat.show_view));
    group->gchat.input_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(group->gchat.input_view));

    stk_gtexttag_create(group->gchat.show_buffer, &group->gchat);

    /* set GtkTextView attribution */
    gtk_text_view_set_editable(GTK_TEXT_VIEW(group->gchat.show_view), FALSE);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(group->gchat.input_view), TRUE);

    /* scroll window */
    group->gchat.show_scrolled = gtk_scrolled_window_new(NULL, NULL);
    group->gchat.input_scrolled = gtk_scrolled_window_new(NULL, NULL);

    /*
     * add view to scroll window 
     * use gtk_container_add to instead gtk_scrolled_window_add_with_viewport seems clear running warning
     */
    //gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(group->gchat.show_scrolled), group->gchat.show_view);
    //gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(group->gchat.input_scrolled), group->gchat.input_view);
    gtk_container_add(GTK_CONTAINER(group->gchat.show_scrolled), group->gchat.show_view);
    gtk_container_add(GTK_CONTAINER(group->gchat.input_scrolled), group->gchat.input_view);

    /* setting scroll window */
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(group->gchat.show_scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(group->gchat.input_scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    /* container something */
    group->gchat.hbox = gtk_hbox_new(FALSE, 2);
    group->gchat.vbox = gtk_vbox_new(FALSE, 2);

    gtk_box_pack_end(GTK_BOX(group->gchat.hbox), group->gchat.close_button, FALSE, FALSE, 2);
    gtk_box_pack_end(GTK_BOX(group->gchat.hbox), group->gchat.send_button, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(group->gchat.vbox),group->gchat.toolbox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(group->gchat.vbox),group->gchat.show_scrolled, TRUE, TRUE, 2);
    gtk_box_pack_start(GTK_BOX(group->gchat.vbox),group->gchat.input_scrolled, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(group->gchat.vbox),group->gchat.hbox, FALSE, FALSE, 2);

    gtk_container_add(GTK_CONTAINER(group->gchat.window), group->gchat.vbox);

    /* set input as focus */
#if 0
#if GTK_CHECK_VERSION(2,18,0)
    gtk_widget_set_can_focus(group->gchat.input_view, GTK_CAN_FOCUS);
#endif
#endif
    gtk_widget_grab_focus(group->gchat.input_view);

    /* set text attribute */
    //gdk_color_parse ("blue", &color);
    //gtk_widget_modify_text(group->gchat.show_view, GTK_STATE_NORMAL, &color);

    group->gchat.show = TRUE;
    //gtk_widget_hide_all(group->gchat.window);

}

void stk_gchatwin_show(GtkWidget *widget, stk_group *group)
{
    if (group == NULL)
        return;

    if (!group->gchat.show)
        stk_gchatwin_create(group);


    //gtk_window_get_position();
    gtk_window_present(GTK_WINDOW(group->gchat.window));
    gtk_widget_show_all(group->gchat.window);
}


gboolean stk_gchatwin_display(stk_group *group)
{
    GtkTextIter end;
    GtkTextMark *endmark;
    stk_buddy *buddy;
    char buf[STK_DEFAULT_SIZE] = {0};
    char timestamp[STK_DEFAULT_SIZE] = {0};
    char msg[STK_MAX_PACKET_SIZE] = {0};
    int msg_len;
    unsigned int uid;

    if (!group->gchat.show)
        stk_gchatwin_create(group);

    while (group->gmsg_num > 0) {
        if (stk_get_gmsg(group, msg, &msg_len, timestamp, &uid) == -1)
            break;
        buddy = stk_find_buddy(uid);
        if (buddy != NULL) {
            sprintf(buf, "%s(%d) %s\n", buddy->nickname, uid, timestamp);
        } else {
            sprintf(buf, "%s(%d) %s\n", "NoName", uid, timestamp);
        }
        gtk_text_buffer_get_end_iter(group->gchat.show_buffer, &end);
        gtk_text_buffer_insert_with_tags(group->gchat.show_buffer, &end, buf, -1, group->gchat.binfo, NULL);
        gtk_text_buffer_insert_with_tags(group->gchat.show_buffer, &end, msg, -1, group->gchat.btext, NULL);
        gtk_text_buffer_insert_with_tags(group->gchat.show_buffer, &end, "\n", -1, group->gchat.btext, NULL);

        /* let the text view scroll to latest message, I seek for solvtion for so much time!!!*/
        endmark = gtk_text_buffer_create_mark(group->gchat.show_buffer, NULL, &end, 1);
        gtk_text_buffer_move_mark(group->gchat.show_buffer, endmark, &end);
        gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(group->gchat.show_view), endmark, 0, TRUE, 1, 1);
#if 0 /* this solvtion works not good, drop it */
        gtk_text_buffer_get_end_iter(group->gchat.show_buffer, &end);
        gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(group->gchat.show_view), &end, 0, TRUE, 1, 1); 
#endif
    }

    //gtk_window_get_position();
    gtk_window_present(GTK_WINDOW(group->gchat.window));
    gtk_widget_show_all(group->gchat.window);
    return FALSE;
}

static void stk_gmsg_show(stk_group *group, char *text)
{
    GtkTextIter start,end;
    GtkTextMark *endmark;
    char buf[STK_MAX_PACKET_SIZE] = {0};
    char tmp[STK_DEFAULT_SIZE] = {0};

    /* clean input text area */
    gtk_text_buffer_get_bounds(group->gchat.input_buffer, &start, &end);
    gtk_text_buffer_delete(group->gchat.input_buffer, &start, &end);

    stk_get_timestamp(tmp);
    sprintf(buf, "%s(%d) %s\n", client.nickname, client.uid, tmp);

    /* show in show text area */
    gtk_text_buffer_get_end_iter(group->gchat.show_buffer, &end);
    gtk_text_buffer_insert_with_tags(group->gchat.show_buffer, &end, buf, -1, group->gchat.minfo, NULL);
    gtk_text_buffer_insert_with_tags(group->gchat.show_buffer, &end, text, -1, group->gchat.mtext, NULL);
    gtk_text_buffer_insert_with_tags(group->gchat.show_buffer, &end, "\n", -1, group->gchat.mtext, NULL);

    /* let the text view scroll to latest message, I seek for solvtion for so much time!!!*/
    endmark = gtk_text_buffer_create_mark(group->gchat.show_buffer, NULL, &end, 1);
    gtk_text_buffer_move_mark(group->gchat.show_buffer, endmark, &end);
    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(group->gchat.show_view), endmark, 0, TRUE, 1, 1);
#if 0 /* this solvtion works not good, drop it */
    gtk_text_buffer_get_end_iter(group->gchat.show_buffer, &end);
    gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(group->gchat.show_view), &end, 0, TRUE, 1, 1); 
#endif
}

gboolean stk_gmsg_send(GtkWidget *widget, stk_group *group)
{
    unsigned char sendbuf[STK_MAX_PACKET_SIZE] = {0};
    GtkTextIter start,end;
    char *text;
    int len;

    text = (char *)malloc(STK_MAX_SIZE);
    if(text == NULL)
    {
        stk_message(NULL, "malloc failed\n");
        return -1;
    }

    /* get text */
    gtk_text_buffer_get_bounds(group->gchat.input_buffer, &start, &end);
    text = gtk_text_buffer_get_text(group->gchat.input_buffer, &start, &end, FALSE);

    /* If there is no input,do nothing but return */
    if(strcmp(text,"")!=0) {
        stk_send_msg(client.fd, sendbuf, STK_MAX_PACKET_SIZE, text, strlen(text),
                                                            client.uid, group->groupid, TRUE);
        stk_gmsg_show(group, text);
    } else {
        stk_message(NULL, "Message should not NULL...\n");
    }
    free(text);
    return 0;
}

void stk_gmsg_event(stk_group *group)
{

    /* Play a wav to nitify user msg is coming */
    g_thread_create((GThreadFunc)stk_play_wav, (gpointer)&STK_MSG_COMING_WAV, FALSE, NULL);  

    /* when use g_idle_add, fuction must be return FALSE!!! or on Windows it will work bad */
    g_idle_add((GSourceFunc)stk_gchatwin_display, (gpointer)group);
}


