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

#ifdef WIN32
#include <windows.h>
#endif

#ifdef _LINUX_
#include <errno.h>
#endif

#include "stk.h"
#include <gdk/gdkkeysyms.h>

#define USE_GTK_THREAD

#define STK_MAIN_ICON_FILE "img.png"
#define STK_BUDDY_ICON_FILE "buddy.png"

enum
{
  STK_PIXBUF_COL,
  STK_TEXT_COL,
  STK_COL_NUM
};

typedef struct{
    GtkWidget *layout;
    GtkWidget *usertext;
    GtkWidget *passtext;
    GtkWidget *servertext;
}LoginWidget;

typedef struct{
    GtkWidget *mainw;
    GtkStatusIcon *tray;
    GtkWidget *treev;
    GtkWidget *hfix;
    GtkWidget *image;
    LoginWidget loginw;
}StkWidgets;

StkWidgets stkwidgets;
client_config client;
unsigned char sendbuf[STK_MAX_PACKET_SIZE] = {0};

static int msg_in = 0;

/*
 * g_object_get_data g_object_set_data
 *
 *
 */

void clean_login_window()
{
    //GList *list = gtk_container_get_children(GTK_CONTAINER(stkwidgets.loginw));
    //gtk_widget_destroy();
    gtk_widget_destroy(stkwidgets.loginw.layout);

}

static GtkTreeModel *create_model()
{
    GtkListStore *store;

    store = gtk_list_store_new(STK_COL_NUM, GDK_TYPE_PIXBUF, G_TYPE_STRING);
    return GTK_TREE_MODEL(store);
}

static void setup_tree_view (GtkWidget *tree)
{
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    /*
     * Create a new GtkCellRendererText, add it to the tree view column and
     * append the column to the tree view.
     */
    renderer = gtk_cell_renderer_pixbuf_new();
    column = gtk_tree_view_column_new_with_attributes("Pic",renderer, "pixbuf", STK_PIXBUF_COL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW (tree), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Buddys", renderer, "text", STK_TEXT_COL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW (tree), column);
}

static void add_to_tree(GtkWidget *tree, GdkPixbuf *pixbuf, const char *str)
{
    GtkListStore *store;
    GtkTreeIter iter;
    //GdkPixbuf *pixbuf;

    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree)));
    //pixbuf = gdk_pixbuf_new_from_file(STK_BUDDY_ICON_FILE, NULL);

    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, STK_PIXBUF_COL, pixbuf, STK_TEXT_COL, str, -1);
    gdk_pixbuf_unref(pixbuf);
}

static void show_buddy_info(GtkWidget *widget, gpointer data)
{
    stk_buddy *buddy = (stk_buddy *)data;
    char buf[STK_MAX_SIZE] = {0};
    char tmp[STK_DEFAULT_SIZE] = {0};

    sprintf(tmp, "Uid:\t\t\t%d\n", buddy->uid);
	strcat(buf, tmp);
    sprintf(tmp, "Nickname:\t%s\n", buddy->nickname);
	strcat(buf, tmp);
    sprintf(tmp, "City:\t\t%s\n", buddy->city);
	strcat(buf, tmp);
    sprintf(tmp, "Phone:\t\t%d\n", buddy->phone);
	strcat(buf, tmp);
    sprintf(tmp, "Gender:\t\t%s\n", (buddy->gender == STK_GENDER_BOY)?"boy":"girl");
	strcat(buf, tmp);

    stk_message("User", buf);
}

static int close_chat_window(GtkWidget *window, gpointer data)
{
    stk_buddy *buddy = (stk_buddy *)data;

    buddy->chat.show = FALSE;
    gtk_widget_destroy(buddy->chat.window);

    return TRUE;
}

static void show_send_text(stk_buddy *buddy, char *text)
{
    GtkTextIter start,end;
    char buf[STK_MAX_SIZE] = {0};
    char tmp[STK_MAX_SIZE] = {0};

    /* clean input text area */
    gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(buddy->chat.input_buffer),&start,&end);
    gtk_text_buffer_delete(GTK_TEXT_BUFFER(buddy->chat.input_buffer),&start,&end);

    stk_get_timestamp(tmp);
    sprintf(buf, "%s(%d) %s\n    ", client.nickname, client.uid, tmp);

    /* show in show text area */
    gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(buddy->chat.show_buffer),&start,&end);
    gtk_text_buffer_insert(GTK_TEXT_BUFFER(buddy->chat.show_buffer), &end, buf, strlen(buf));
    gtk_text_buffer_insert(GTK_TEXT_BUFFER(buddy->chat.show_buffer), &end, text, strlen(text));
    gtk_text_buffer_insert(GTK_TEXT_BUFFER(buddy->chat.show_buffer), &end, "\n", 1);
}

static gboolean send_msg(GtkWidget *widget, gpointer data)
{
    stk_buddy *buddy = (stk_buddy *)data;
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
        gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(buddy->chat.input_buffer), &start, &end);
        text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buddy->chat.input_buffer), &start, &end, FALSE);

        /* If there is no input,do nothing but return */
        if(strcmp(text,"")!=0)
        {
            stk_send_msg(client.fd, sendbuf, STK_MAX_SIZE, text, strlen(text), client.uid, buddy->uid);
            show_send_text(buddy, text);
        } else {
            stk_message("message should not NULL...\n");
        }
        free(text);
    }
    return 0;
}

static void init_chat_window(stk_buddy *buddy)
{
    char buf[STK_DEFAULT_SIZE] = {0};
    GtkAccelGroup *gag;

    sprintf(buf, "Chat With %s", buddy->nickname);
    buddy->chat.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(buddy->chat.window), buf);
    gtk_window_set_position(GTK_WINDOW(buddy->chat.window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(buddy->chat.window), 500, 380);

	/* "quit" button */
    g_signal_connect(GTK_OBJECT(buddy->chat.window), "destroy", G_CALLBACK(close_chat_window), (gpointer)buddy);

    buddy->chat.send_button = gtk_button_new_with_label("Send");
    buddy->chat.close_button = gtk_button_new_with_label("Close");

    buddy->chat.show_view = gtk_text_view_new();
    buddy->chat.input_view = gtk_text_view_new();

    /* get the buffer of textbox */
    buddy->chat.show_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(buddy->chat.show_view));
    buddy->chat.input_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(buddy->chat.input_view));

    /* set textbox to diseditable */
    gtk_text_view_set_editable(GTK_TEXT_VIEW(buddy->chat.show_view), FALSE);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(buddy->chat.input_view), TRUE);

    /* scroll window */
    buddy->chat.show_scrolled = gtk_scrolled_window_new(NULL, NULL);
    buddy->chat.input_scrolled = gtk_scrolled_window_new(NULL, NULL);

    /* create a textbox */
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(buddy->chat.show_scrolled), buddy->chat.show_view);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(buddy->chat.input_scrolled), buddy->chat.input_view);

    /* setting of window */
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(buddy->chat.show_scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(buddy->chat.input_scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    buddy->chat.hbox = gtk_hbox_new(FALSE, 2);
    buddy->chat.vbox = gtk_vbox_new(FALSE, 2);

    /* click close to call close_chat_window*/
    g_signal_connect(GTK_OBJECT(buddy->chat.close_button), "clicked", G_CALLBACK(close_chat_window), (gpointer)buddy);

    /* create window */
    gtk_box_pack_end(GTK_BOX(buddy->chat.hbox),buddy->chat.close_button, FALSE, FALSE, 2);
    gtk_box_pack_end(GTK_BOX(buddy->chat.hbox),buddy->chat.send_button, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(buddy->chat.vbox),buddy->chat.show_scrolled, TRUE, TRUE, 2);
    gtk_box_pack_start(GTK_BOX(buddy->chat.vbox),buddy->chat.input_scrolled, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(buddy->chat.vbox),buddy->chat.hbox, FALSE, FALSE, 2);

    gtk_container_add(GTK_CONTAINER(buddy->chat.window), buddy->chat.vbox);

    /* set input as focus */
    gtk_widget_set_can_focus(buddy->chat.input_view, GTK_CAN_FOCUS);
    gtk_widget_grab_focus (buddy->chat.input_view);

    /* click send button ,then call send_msg*/
    g_signal_connect(GTK_OBJECT(buddy->chat.send_button), "clicked", G_CALLBACK(send_msg), (gpointer)buddy);

    gag = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(buddy->chat.window), gag);
    gtk_widget_add_accelerator(buddy->chat.send_button, "clicked", gag, GDK_Return, 0, GTK_ACCEL_VISIBLE); /* GDK_KEY_Return */

    buddy->chat.show = TRUE;
    //gtk_widget_hide_all(buddy->chat.window);

}

void show_chat_window(GtkWidget *widget, gpointer data)
{
    stk_buddy *buddy = (stk_buddy *)data;

    if (!buddy->chat.show)
        init_chat_window(buddy);
    gtk_widget_show_all(buddy->chat.window);
}

gboolean show_chat_text(stk_buddy *buddy)
{
    GtkTextIter start,end;
    char buf[STK_DEFAULT_SIZE] = {0};
    char timestamp[STK_DEFAULT_SIZE] = {0};
    char msg[STK_MAX_SIZE] = {0};
    int msg_len;

    if (!buddy->chat.show)
        init_chat_window(buddy);

    while (buddy->msg_num > 0) {
        if (stk_get_msg(buddy, msg, &msg_len, timestamp) == -1)
            break;
        sprintf(buf, "%s(%d) %s\n    ", buddy->nickname, buddy->uid, timestamp);
        gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(buddy->chat.show_buffer), &start,&end);
        gtk_text_buffer_insert(GTK_TEXT_BUFFER(buddy->chat.show_buffer), &end, buf, strlen(buf));
        gtk_text_buffer_insert(GTK_TEXT_BUFFER(buddy->chat.show_buffer), &end, msg, msg_len);
        gtk_text_buffer_insert(GTK_TEXT_BUFFER(buddy->chat.show_buffer), &end, "\n", 1);
    }

    gtk_widget_show_all(buddy->chat.window);
	return FALSE;
}

#ifdef STK_USE_NOTIFY
void notify_buddy(stk_buddy *buddy)
{
    int x, y, toward;

    /* check positon of image put in hfix in fuction init_buddylist_window */
    x = 0;
	y = 20;
    toward = 1;
    msg_in = 1;

    while(msg_in) {
        gtk_status_icon_set_blinking(stkwidgets.tray, TRUE);
        g_usleep(1000*250);
        gdk_threads_enter();
        gtk_fixed_move(GTK_FIXED(stkwidgets.hfix),stkwidgets.image, x, y);
        switch(toward)
        {
        case 1:
            x = x - 2;
            y = y - 2;
            toward = 2;
            break;
        case 2:
            x = x + 2;
            y = y + 2;
            toward = 3;
            break;
        case 3:
            x = x + 2;
            y = y - 2;
            toward = 4;
            break;
        case 4:
            x = x - 2;
            y = y + 2;
            toward = 1;
        }
        gdk_threads_leave();
    }

    gtk_status_icon_set_blinking(stkwidgets.tray, FALSE);
    gtk_fixed_move(GTK_FIXED(stkwidgets.hfix),stkwidgets.image, 0, 20);
}
#else
void notify_buddy(stk_buddy *buddy)
{
    /* when use g_idle_add, fuction must be return FALSE!!! or on Windows it will work bad */
    g_idle_add((GSourceFunc)show_chat_text, (gpointer)buddy);

    //show_chat_text(buddy);
}
#endif

static void create_popup_menu (stk_buddy *buddy)
{
    GtkWidget *buddy_info, *chat, *separator;

    buddy_info = gtk_menu_item_new_with_label ("Show Buddy Info");
    chat = gtk_menu_item_new_with_label ("Chat With Buddy");
    separator = gtk_separator_menu_item_new ();

    g_signal_connect (G_OBJECT (buddy_info), "activate", G_CALLBACK (show_buddy_info), (gpointer)buddy);
    g_signal_connect (G_OBJECT (chat), "activate", G_CALLBACK (show_chat_window), (gpointer)buddy);

    gtk_menu_shell_append (GTK_MENU_SHELL (buddy->menu), buddy_info);
    gtk_menu_shell_append (GTK_MENU_SHELL (buddy->menu), separator);
    gtk_menu_shell_append (GTK_MENU_SHELL (buddy->menu), chat);

    //gtk_menu_attach_to_widget (GTK_MENU (buddy->menu), buddy, NULL);
    gtk_widget_show_all (buddy->menu);
}

static gboolean popup_chat_window(GtkWidget *widget, GdkEventButton *event)
{
    unsigned short buddy_num = stk_get_buddynum();
    stk_buddy *buddy = NULL;

    if(event->type == GDK_2BUTTON_PRESS && event->button == 0x1) {
#ifdef STK_USE_NOTIFY
        msg_in = 0;
#endif
        while (buddy_num--) {
            buddy = stk_get_next(buddy);
            if (buddy->msg_num)
                show_chat_text(buddy);
        }
        return TRUE;
	} else {
        return FALSE;
	}
}

static gboolean popup_menu(GtkWidget *widget, GdkEventButton *event)
{
    GtkTreeView *tree = GTK_TREE_VIEW(widget); 
    GtkTreeSelection *selection;
    GtkTreeModel *model;
    GtkTreeIter iter;
    char *uid;
    stk_buddy *buddy = NULL;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree)); 
    selection = gtk_tree_view_get_selection(tree);
    gtk_tree_selection_get_selected(selection, &model, &iter);
    gtk_tree_model_get(model, &iter, STK_TEXT_COL, &uid, -1);

    uid[3] = '\0';
    buddy = stk_find_buddy(atoi(uid));

    /* this can not be happen!! */
    if (buddy == NULL) {
        stk_message("STK Error", "Bad Buddy");
        return FALSE;
    }

    if(event->type == GDK_BUTTON_PRESS && event->button == 0x3) {
        gtk_menu_popup(GTK_MENU(buddy->menu), NULL, NULL,NULL, NULL, event->button, event->time);
        return TRUE;
    } else if (event->type == GDK_2BUTTON_PRESS && event->button == 0x1) {
        show_chat_window(NULL, (gpointer)buddy);
        return TRUE;
	} else {
        return FALSE;
	}
}

void init_buddylist_window()
{
    char buf[STK_DEFAULT_SIZE] = {0};
    stk_buddy *buddy = NULL;
    unsigned short buddy_num = stk_get_buddynum();
    GtkWidget *window = stkwidgets.mainw;
    GtkWidget *vfix;
    GtkWidget *hfix;
    GtkWidget *label;
    GtkWidget *image;
    GtkWidget *tree;
    GtkTreeStore *store;
    GtkTreeSelection *select_item;

    sprintf(buf, "%d (%s)", client.uid, client.nickname);

    vfix = gtk_fixed_new();
    hfix = gtk_fixed_new();
    label = gtk_label_new(buf);
    image = gtk_image_new_from_file(STK_MAIN_ICON_FILE);

    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);	
    gtk_fixed_put((GtkFixed *)hfix, image, 0, 20);
    gtk_fixed_put((GtkFixed *)hfix, label, 90, 30);
    gtk_fixed_put((GtkFixed *)vfix, hfix, 20, 10);

    g_signal_connect(G_OBJECT(image), "button-press-event", G_CALLBACK (popup_chat_window), NULL);

    tree = gtk_tree_view_new_with_model(create_model());
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree), FALSE);

    select_item = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
    gtk_tree_selection_set_mode(select_item, GTK_SELECTION_SINGLE);
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(tree), TRUE);
    //gtk_tree_view_set_headers_clickable(GTK_TREE_VIEW(tree), TRUE);
    //gtk_tree_view_set_reorderable(GTK_TREE_VIEW(tree), TRUE);
    gtk_tree_view_set_hover_selection(GTK_TREE_VIEW(tree), TRUE);
    gtk_tree_view_set_hover_expand(GTK_TREE_VIEW(tree), TRUE);

    setup_tree_view(tree);

    gtk_fixed_put((GtkFixed *)vfix, tree, 20, 120);
    gtk_container_add(GTK_CONTAINER(window), vfix);

    while (buddy_num--) {
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(STK_BUDDY_ICON_FILE, NULL);

        buddy = stk_get_next(buddy);
        buddy->pixbuf = pixbuf;
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%d (%s)", buddy->uid, buddy->nickname);
        add_to_tree(tree, pixbuf, buf);
        buddy->menu = gtk_menu_new();
        create_popup_menu(buddy);
		//init_chat_window(buddy);
    }

    stkwidgets.hfix = hfix;
    stkwidgets.image = image;
    stkwidgets.treev= tree;

#if defined(USE_GTK_THREAD)
    g_thread_create((GThreadFunc)stk_recv_msg, (gpointer)&client, FALSE, NULL);  
#endif

    g_signal_connect(G_OBJECT(tree), "button-press-event", G_CALLBACK (popup_menu), NULL);

    gtk_widget_show_all(window);
}


void login_pressed(GtkWidget *widget, LoginWidget *loginwidget)
{
    char buf[STK_DEFAULT_SIZE];
    char *username, *passwd, *serverip;
    int ret;

    memset(buf, 0, sizeof(buf));
    username = (char *)gtk_entry_get_text(GTK_ENTRY(loginwidget->usertext));
    passwd = (char *)gtk_entry_get_text(GTK_ENTRY(loginwidget->passtext));
    serverip = (char *)gtk_entry_get_text(GTK_ENTRY(loginwidget->servertext));

    if (username[0] == '\0') {
        char *err = "Username is NULL\n";
        strcpy(buf, err);
        goto error;
    } else {
        client.uid = atoi(username);
    }

    if (passwd[0] == '\0') {
        char *err = "Password is NULL\n";
        strcpy(buf, err);
        goto error; 
    } else {
        strcpy(client.pass, passwd);
    }

    if (serverip[0] == '\0') {
        char *err = "Server IP is NULL\n";
        strcpy(buf, err);
        goto error; 
    } else {
        strcpy(client.serverip, serverip);
    }

    ret = stk_connect(&client);
    if (ret == -1){
        sprintf(buf, "STK Client %d connect error\n", client.uid);
        goto error;
    }


    ret = stk_login(client.fd, sendbuf, STK_MAX_PACKET_SIZE, client.uid, client.pass);

    if (ret == STK_CLIENT_LOGIN_SUCCESS){
        sprintf(buf, "STK Client %d login success.\n", client.uid);

        stk_buddy buddy;
        memset(&buddy, 0 ,sizeof(stk_buddy));
        ret = stk_send_getprofile(client.fd, sendbuf, STK_MAX_PACKET_SIZE, client.uid, client.uid, &buddy);
        if (ret == -1){
            sprintf(buf, "STK Client %d get profile failed.\n", client.uid);
            goto error;
        } else {
            memcpy(client.nickname, buddy.nickname, STK_NICKNAME_SIZE);
            memcpy(client.city, buddy.city,STK_CITY_SIZE);
            client.phone = buddy.phone;
            client.gender = buddy.gender;
        }

        ret = stk_send_getbuddylist(client.fd, sendbuf, STK_MAX_PACKET_SIZE, client.uid);
        if (ret == -1){
            sprintf(buf, "STK Client %d get buddy list failed.\n", client.uid);
            goto error;
        }
        sprintf(buf, "stkclient:%d(%s)", client.uid, client.nickname);
        gtk_status_icon_set_tooltip_text(stkwidgets.tray, buf);

        clean_login_window();
        init_buddylist_window();
        return;
    } else if (ret == STK_CLIENT_LOGIN_ERROR){
        sprintf(buf, "STK Client %d login failed.\n", client.uid);
    } else if (ret == STK_CLIENT_LOGIN_INVALID_UID){
        sprintf(buf, "STK Client %d unregistered.\n", client.uid);
    } else if (ret == STK_CLIENT_LOGIN_INVALID_PASS){
        sprintf(buf, "STK Client %d authenticate failed.\n", client.uid);
    } else {
        sprintf(buf, "STK Client %d login bad.\n", client.uid);
    }

error:

    stk_message("Login Result", buf);
}

void gtk_window_show(GtkWidget *widget, gpointer data)
{
    gtk_widget_show_all((GtkWidget *)data);
    gtk_window_present(GTK_WINDOW(data));
}

void gtk_main_exit()
{
    gtk_status_icon_set_visible(stkwidgets.tray, FALSE);
    gtk_main_quit();
}

void popup_tip(GtkStatusIcon *statusicon, guint button, guint time)
{
    GtkWidget *menu;
    GtkWidget *quit;

    menu = gtk_menu_new();
    quit = gtk_menu_item_new_with_label ("Quit");
    g_signal_connect(quit, "activate", G_CALLBACK(gtk_main_exit), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), quit);

    gtk_widget_show_all(menu);
    gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, button, time);
}

void create_tray(GtkWidget *window)
{
    GtkStatusIcon *tray_icon;

    tray_icon = gtk_status_icon_new_from_pixbuf(gdk_pixbuf_new_from_file(STK_MAIN_ICON_FILE, NULL));

    g_signal_connect(G_OBJECT(tray_icon), "activate", G_CALLBACK(gtk_window_show), (gpointer)window);
    g_signal_connect(G_OBJECT(tray_icon), "popup-menu", G_CALLBACK(popup_tip), NULL); 

    gtk_status_icon_set_tooltip_text(tray_icon, "stkclient");
    gtk_status_icon_set_visible(tray_icon, TRUE);

    stkwidgets.tray = tray_icon;
    return;
}  

void gtk_window_callback(GtkWidget *widget, GdkEventWindowState *event, gpointer data)
{
    if(event->changed_mask == GDK_WINDOW_STATE_ICONIFIED && event->new_window_state == GDK_WINDOW_STATE_ICONIFIED)
    {
        gtk_widget_hide_all(widget);
    }
}

void init_main_window(int argc,char *argv[], GtkWidget *window)
{
    g_signal_connect (G_OBJECT(window), "destroy", G_CALLBACK (gtk_main_exit), NULL);
    g_signal_connect (G_OBJECT(window), "delete_event", G_CALLBACK (gtk_main_exit), NULL);

    /* set main window attribution */
    gtk_window_unmaximize(GTK_WINDOW(window));
    gtk_window_set_resizable (GTK_WINDOW(window), TRUE);
    gtk_window_set_title (GTK_WINDOW(window), "stkclient");
    gtk_widget_set_size_request(window, 300, 600);
    //gtk_window_set_default_size(GTK_WINDOW(window), 300, 600);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(window),FALSE);

    gtk_window_set_icon(GTK_WINDOW(window), gdk_pixbuf_new_from_file(STK_MAIN_ICON_FILE, NULL));

    g_signal_connect(window, "window_state_event", G_CALLBACK(gtk_window_callback), NULL);
}

void init_login_window(LoginWidget *loginwidget)
{
    GtkWidget *layout;
    GtkWidget *userlabel;
    GtkWidget *usertext;
    GtkWidget *passlabel;
    GtkWidget *passtext;
    GtkWidget *serverlabel;
    GtkWidget *servertext;
    GtkWidget *loginbutton;
    GtkAccelGroup *gag;

    layout = gtk_fixed_new();
    userlabel = gtk_label_new ("Username:");
    usertext = gtk_entry_new();
    passlabel = gtk_label_new ("Password:");
    passtext = gtk_entry_new();
    serverlabel = gtk_label_new ("Server IP:");
    servertext = gtk_entry_new();
    loginbutton = gtk_button_new_with_label("Login");

    loginwidget->layout = layout;
    loginwidget->usertext = usertext;
    loginwidget->passtext = passtext;
    loginwidget->servertext = servertext;

    g_signal_connect(G_OBJECT(loginbutton), "clicked", G_CALLBACK (login_pressed), (gpointer)loginwidget);


    gtk_entry_set_visibility(GTK_ENTRY(usertext), TRUE);
    gtk_entry_set_visibility(GTK_ENTRY(passtext), FALSE);
    gtk_entry_set_visibility(GTK_ENTRY(servertext), TRUE);
    gtk_editable_set_editable(GTK_EDITABLE(usertext), TRUE);
    gtk_editable_set_editable(GTK_EDITABLE(passtext), TRUE);
    gtk_editable_set_editable(GTK_EDITABLE(servertext), TRUE);

    gtk_fixed_put((GtkFixed *)layout, userlabel, 80, 200);
    gtk_fixed_put((GtkFixed *)layout, usertext, 80, 220);
    gtk_fixed_put((GtkFixed *)layout, passlabel, 80, 250);
    gtk_fixed_put((GtkFixed *)layout, passtext, 80, 270);
    gtk_fixed_put((GtkFixed *)layout, serverlabel, 80, 300);
    gtk_fixed_put((GtkFixed *)layout, servertext, 80, 320);
    gtk_fixed_put((GtkFixed *)layout, loginbutton, 80, 350);

    gag = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(stkwidgets.mainw), gag);
    gtk_widget_add_accelerator(loginbutton, "clicked", gag, GDK_Return, 0, GTK_ACCEL_VISIBLE); /* GDK_KEY_Return */
}

int main (int argc,char *argv[])
{
    int ret;

    memset(&client, 0, sizeof(client));
    memset(&stkwidgets, 0, sizeof(stkwidgets));

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

    gtk_init (&argc, &argv);
    stkwidgets.mainw = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    init_main_window(argc, argv, stkwidgets.mainw);

    create_tray(stkwidgets.mainw);

    init_login_window(&stkwidgets.loginw);

    gtk_container_add(GTK_CONTAINER(stkwidgets.mainw), stkwidgets.loginw.layout);

    gtk_widget_show_all(stkwidgets.mainw);

#ifdef USE_GTK_THREAD
    gdk_threads_enter();
#endif
    gtk_main();
#ifdef USE_GTK_THREAD
    gdk_threads_leave();
#endif
}

