/* 
 * File: stkchat.c
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

void stk_chat_request(GtkWidget *widget, stk_buddy *buddy)
{
    printf("stk_chat_request\n");
}

void stk_voice_request(GtkWidget *widget, stk_buddy *buddy)
{
    printf("voice_request\n");
}

void stk_video_request(GtkWidget *widget, stk_buddy *buddy)
{
    printf("video_request\n");
}

void stk_texttag_create(GtkTextBuffer *buffer, chat_widgets *chat)
{    
    chat->minfo = gtk_text_buffer_create_tag(buffer, "minfo", "foreground", "blue", "size", 8 * PANGO_SCALE, NULL);
    chat->mtext = gtk_text_buffer_create_tag(buffer, "mtext", "foreground", "black", "size", 10 * PANGO_SCALE,
                                                                                    "left_margin", 10, NULL);

    chat->binfo = gtk_text_buffer_create_tag(buffer, "binfo", "foreground", "red", "size", 8 * PANGO_SCALE, NULL);
    chat->btext = gtk_text_buffer_create_tag(buffer, "btext", "foreground", "black", "size", 10 * PANGO_SCALE, 
                                                                                    "left_margin", 10, NULL);
}

static int stk_chatwin_close(GtkWidget *window, stk_buddy *buddy)
{
    buddy->chat.show = FALSE;
    gtk_widget_destroy(buddy->chat.window);

    return TRUE;
}

static void stk_chatwin_create(stk_buddy *buddy)
{
    char buf[STK_DEFAULT_SIZE] = {0};
    GdkColor color;
    GtkWidget *image;
    GtkToolItem *toolitem;
    GtkAccelGroup *gag;
#if !GTK_CHECK_VERSION(2,12,0)
    GtkTooltips *tooltips;
#endif

    sprintf(buf, "Chat With %s", buddy->nickname);
    buddy->chat.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(buddy->chat.window), buf);
    gtk_window_set_position(GTK_WINDOW(buddy->chat.window), GTK_WIN_POS_CENTER);
    gtk_widget_set_size_request(buddy->chat.window, STK_CHATWIN_WIDTH, STK_CHATWIN_HEIGHT);
    gtk_window_set_icon(GTK_WINDOW(buddy->chat.window), gdk_pixbuf_new_from_file(STK_SESSION_PNG, NULL));

    /* set chat window background color */
    gdk_color_parse(MAIN_COLRO_STRING, &color);
    gtk_widget_modify_bg(buddy->chat.window, GTK_STATE_NORMAL, &color);

    /* "quit" button */
    g_signal_connect(G_OBJECT(buddy->chat.window), "destroy", G_CALLBACK(stk_chatwin_close), (gpointer)buddy);

    buddy->chat.toolbox = gtk_hbox_new(FALSE, 0);

#if !GTK_CHECK_VERSION(2,12,0)
    tooltips = gtk_tooltips_new();
#endif

    image = gtk_image_new_from_file(STK_CHAT_PNG);
    toolitem = gtk_tool_button_new(image, "Chat");
#if !GTK_CHECK_VERSION(2,12,0)
    gtk_tool_item_set_tooltip(toolitem, tooltips, "Chat", NULL);
#else
    gtk_tool_item_set_tooltip_text(toolitem, "Chat");
#endif
    gtk_tool_item_set_is_important(toolitem, TRUE);
    g_signal_connect(G_OBJECT(toolitem), "clicked", G_CALLBACK(stk_chat_request), (gpointer)buddy);
    gtk_box_pack_start(GTK_BOX(buddy->chat.toolbox), GTK_WIDGET(toolitem), FALSE, FALSE, 0);

    image = gtk_image_new_from_file(STK_VOICE_PNG);
    toolitem = gtk_tool_button_new(image, "Voice");
#if !GTK_CHECK_VERSION(2,12,0)
    gtk_tool_item_set_tooltip(toolitem, tooltips, "Voice", NULL);
#else
    gtk_tool_item_set_tooltip_text(toolitem, "Voice");
#endif
    gtk_tool_item_set_is_important(toolitem, TRUE);
    g_signal_connect(G_OBJECT(toolitem), "clicked", G_CALLBACK(stk_voice_request), (gpointer)buddy);
    gtk_box_pack_start(GTK_BOX(buddy->chat.toolbox), GTK_WIDGET(toolitem), FALSE, FALSE, 0);

    image = gtk_image_new_from_file(STK_VIDEO_PNG);
    toolitem = gtk_tool_button_new(image, "Video");
    gtk_tool_item_set_is_important(toolitem, TRUE);
#if !GTK_CHECK_VERSION(2,12,0)
    gtk_tool_item_set_tooltip(toolitem, tooltips, "Video", NULL);
#else
    gtk_tool_item_set_tooltip_text(toolitem, "Video");
#endif
    g_signal_connect(G_OBJECT(toolitem), "clicked", G_CALLBACK(stk_video_request), (gpointer)buddy);
    gtk_box_pack_start(GTK_BOX(buddy->chat.toolbox), GTK_WIDGET(toolitem), FALSE, FALSE, 0);

    buddy->chat.send_button = gtk_button_new_with_label("   Send   ");
    buddy->chat.close_button = gtk_button_new_with_label("   Close   ");

    g_signal_connect(G_OBJECT(buddy->chat.close_button), "clicked", G_CALLBACK(stk_chatwin_close), (gpointer)buddy);
    g_signal_connect(G_OBJECT(buddy->chat.send_button), "clicked", G_CALLBACK(stk_msg_send), (gpointer)buddy);

    gag = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(buddy->chat.window), gag);
    gtk_widget_add_accelerator(buddy->chat.send_button, "clicked", gag, GDK_Return, 0, GTK_ACCEL_VISIBLE);

    /* text view */
    buddy->chat.show_view = gtk_text_view_new();
    buddy->chat.input_view = gtk_text_view_new();

    /* text view wrap line automatic, Note, hscrollbar_policy must set to GTK_POLICY_AUTOMATIC*/
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(buddy->chat.show_view), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(buddy->chat.input_view), GTK_WRAP_WORD_CHAR);

    /* get the buffer of textbox */
    buddy->chat.show_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(buddy->chat.show_view));
    buddy->chat.input_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(buddy->chat.input_view));

    stk_texttag_create(buddy->chat.show_buffer, &buddy->chat);

    /* set GtkTextView attribution */
    gtk_text_view_set_editable(GTK_TEXT_VIEW(buddy->chat.show_view), FALSE);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(buddy->chat.input_view), TRUE);

    /* scroll window */
    buddy->chat.show_scrolled = gtk_scrolled_window_new(NULL, NULL);
    buddy->chat.input_scrolled = gtk_scrolled_window_new(NULL, NULL);

    /*
     * add view to scroll window 
     * use gtk_container_add to instead gtk_scrolled_window_add_with_viewport seems clear running warning
     */
    //gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(buddy->chat.show_scrolled), buddy->chat.show_view);
    //gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(buddy->chat.input_scrolled), buddy->chat.input_view);
    gtk_container_add(GTK_CONTAINER(buddy->chat.show_scrolled), buddy->chat.show_view);
    gtk_container_add(GTK_CONTAINER(buddy->chat.input_scrolled), buddy->chat.input_view);

    /* setting scroll window */
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(buddy->chat.show_scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(buddy->chat.input_scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    /* container something */
    buddy->chat.hbox = gtk_hbox_new(FALSE, 2);
    buddy->chat.vbox = gtk_vbox_new(FALSE, 2);

    gtk_box_pack_end(GTK_BOX(buddy->chat.hbox), buddy->chat.close_button, FALSE, FALSE, 2);
    gtk_box_pack_end(GTK_BOX(buddy->chat.hbox), buddy->chat.send_button, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(buddy->chat.vbox),buddy->chat.toolbox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(buddy->chat.vbox),buddy->chat.show_scrolled, TRUE, TRUE, 2);
    gtk_box_pack_start(GTK_BOX(buddy->chat.vbox),buddy->chat.input_scrolled, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(buddy->chat.vbox),buddy->chat.hbox, FALSE, FALSE, 2);

    gtk_container_add(GTK_CONTAINER(buddy->chat.window), buddy->chat.vbox);

    /* set input as focus */
#if 0
#if GTK_CHECK_VERSION(2,18,0)
    gtk_widget_set_can_focus(buddy->chat.input_view, GTK_CAN_FOCUS);
#endif
#endif
    gtk_widget_grab_focus(buddy->chat.input_view);

    /* set text attribute */
    //gdk_color_parse ("blue", &color);
    //gtk_widget_modify_text(buddy->chat.show_view, GTK_STATE_NORMAL, &color);

    buddy->chat.show = TRUE;
    //gtk_widget_hide_all(buddy->chat.window);

}

void stk_chatwin_show(GtkWidget *widget, stk_buddy *buddy)
{
    if (buddy == NULL)
        return;

    if (!buddy->chat.show)
        stk_chatwin_create(buddy);

    //gtk_window_get_position();
    gtk_window_present(GTK_WINDOW(buddy->chat.window));
    gtk_widget_show_all(buddy->chat.window);
}


gboolean stk_chatwin_display(stk_buddy *buddy)
{
    GtkTextIter end;
    GtkTextMark *endmark;
    char buf[STK_DEFAULT_SIZE] = {0};
    char timestamp[STK_DEFAULT_SIZE] = {0};
    char msg[STK_MAX_PACKET_SIZE] = {0};
    int msg_len;

    if (!buddy->chat.show)
        stk_chatwin_create(buddy);

    while (buddy->msg_num > 0) {
        if (stk_get_msg(buddy, msg, &msg_len, timestamp) == -1)
            break;
        sprintf(buf, "%s(%d) %s\n", buddy->nickname, buddy->uid, timestamp);
        gtk_text_buffer_get_end_iter(buddy->chat.show_buffer, &end);
        gtk_text_buffer_insert_with_tags(buddy->chat.show_buffer, &end, buf, -1, buddy->chat.binfo, NULL);
        gtk_text_buffer_insert_with_tags(buddy->chat.show_buffer, &end, msg, -1, buddy->chat.btext, NULL);
        gtk_text_buffer_insert_with_tags(buddy->chat.show_buffer, &end, "\n", -1, buddy->chat.btext, NULL);

        /* let the text view scroll to latest message, I seek for solvtion for so much time!!!*/
        endmark = gtk_text_buffer_create_mark(buddy->chat.show_buffer, NULL, &end, 1);
        gtk_text_buffer_move_mark(buddy->chat.show_buffer, endmark, &end);
        gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(buddy->chat.show_view), endmark, 0, TRUE, 1, 1);
#if 0 /* this solvtion works not good, drop it */
        gtk_text_buffer_get_end_iter(buddy->chat.show_buffer, &end);
        gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(buddy->chat.show_view), &end, 0, TRUE, 1, 1); 
#endif
    }

    //gtk_window_get_position();
    gtk_window_present(GTK_WINDOW(buddy->chat.window));
    gtk_widget_show_all(buddy->chat.window);
    return FALSE;
}

static void stk_msg_show(stk_buddy *buddy, char *text)
{
    GtkTextIter start,end;
    GtkTextMark *endmark;
    char buf[STK_MAX_PACKET_SIZE] = {0};
    char tmp[STK_DEFAULT_SIZE] = {0};

    /* clean input text area */
    gtk_text_buffer_get_bounds(buddy->chat.input_buffer, &start, &end);
    gtk_text_buffer_delete(buddy->chat.input_buffer, &start, &end);

    stk_get_timestamp(tmp);
    sprintf(buf, "%s(%d) %s\n", client.nickname, client.uid, tmp);

    /* show in show text area */
    gtk_text_buffer_get_end_iter(buddy->chat.show_buffer, &end);
    gtk_text_buffer_insert_with_tags(buddy->chat.show_buffer, &end, buf, -1, buddy->chat.minfo, NULL);
    gtk_text_buffer_insert_with_tags(buddy->chat.show_buffer, &end, text, -1, buddy->chat.mtext, NULL);
    gtk_text_buffer_insert_with_tags(buddy->chat.show_buffer, &end, "\n", -1, buddy->chat.mtext, NULL);

    /* let the text view scroll to latest message, I seek for solvtion for so much time!!!*/
    endmark = gtk_text_buffer_create_mark(buddy->chat.show_buffer, NULL, &end, 1);
    gtk_text_buffer_move_mark(buddy->chat.show_buffer, endmark, &end);
    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(buddy->chat.show_view), endmark, 0, TRUE, 1, 1);
#if 0 /* this solvtion works not good, drop it */
    gtk_text_buffer_get_end_iter(buddy->chat.show_buffer, &end);
    gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(buddy->chat.show_view), &end, 0, TRUE, 1, 1); 
#endif
}

gboolean stk_msg_send(GtkWidget *widget, stk_buddy *buddy)
{
    unsigned char sendbuf[STK_MAX_PACKET_SIZE] = {0};
    GtkTextIter start,end;
    char *text;
    int len;

    if(0){//STK_CLIENT_OFFLINE == buddy->state) {
        stk_message(NULL, "Buddy offline...\n");
    } else {
        text = (char *)malloc(STK_MAX_SIZE);
        if(text == NULL)
        {
            stk_message(NULL, "malloc failed\n");
            return -1;
        }

        /* get text */
        gtk_text_buffer_get_bounds(buddy->chat.input_buffer, &start, &end);
        text = gtk_text_buffer_get_text(buddy->chat.input_buffer, &start, &end, FALSE);

        /* If there is no input,do nothing but return */
        if(strcmp(text,"")!=0)
        {
            stk_send_msg(client.fd, sendbuf, STK_MAX_PACKET_SIZE, text, strlen(text), client.uid, buddy->uid, FALSE);
            stk_msg_show(buddy, text);
        } else {
            stk_message(NULL, "Message should not NULL...\n");
        }
        free(text);
    }
    return 0;
}

void stk_msg_event(stk_buddy *buddy)
{

    /* Play a wav to nitify user msg is coming */
    g_thread_create((GThreadFunc)stk_play_wav, (gpointer)&STK_MSG_COMING_WAV, FALSE, NULL);  

    /* when use g_idle_add, fuction must be return FALSE!!! or on Windows it will work bad */
    g_idle_add((GSourceFunc)stk_chatwin_display, (gpointer)buddy);
}


