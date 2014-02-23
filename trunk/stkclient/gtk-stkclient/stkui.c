/* 
 * File: stkui.c
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stk.h"

extern client_config client;

void stk_window_hide(GtkWidget *widget, GtkWidget *window)
{
    gtk_window_iconify(GTK_WINDOW(window));
}

void stk_window_show(GtkWidget *widget, GtkWidget *window)
{
#if 0
    /* set window position */
    gtk_window_move(GTK_WINDOW(window), 
           widgets.size.width-40-STK_WINDOW_WIDTH, (widgets.size.height-STK_WINDOW_HEIGHT)/2);
#endif
    gtk_widget_show_all((GtkWidget *)window);
    gtk_window_present(GTK_WINDOW(window));
}

void stk_window_exit(GtkWidget *widget, GtkStatusIcon *tray)
{
    gtk_status_icon_set_visible(tray, FALSE);
    gtk_main_quit();
}

void stk_window_iconify(GtkWidget *widget, GdkEventWindowState *event, gpointer data)
{
    if(event->changed_mask == GDK_WINDOW_STATE_ICONIFIED && event->new_window_state == GDK_WINDOW_STATE_ICONIFIED)
    {
        gtk_widget_hide_all(widget);
    }
}

gboolean stk_window_move(GtkWidget *widget,GdkEventButton *event,gint data)
{
    if(event->type == GDK_BUTTON_PRESS && event->button == 0x1)
    {
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), 
                      event->button, event->x_root, event->y_root,event->time);
    }
}

void stk_screen_get(STKWIDGETS *widgets)
{
    /* get screen and its width and height */
    widgets->screen = gdk_screen_get_default();
    widgets->size.width = gdk_screen_get_width (widgets->screen);
    widgets->size.height = gdk_screen_get_height (widgets->screen);

}

void stk_tray_menu(GtkStatusIcon *statusicon, guint button, guint time, GtkWidget *window)
{
    GtkWidget *menu;
    GtkWidget *item;

    menu = gtk_menu_new();
    item = gtk_menu_item_new_with_label("Show");
    g_signal_connect(item, "activate", G_CALLBACK(stk_window_show), (gpointer)window);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

    if (client.state == STK_CLIENT_ONLINE) {
        item = gtk_menu_item_new_with_label("Hide");
        g_signal_connect(item, "activate", G_CALLBACK(stk_window_hide), (gpointer)window);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    }

    item = gtk_menu_item_new_with_label("Quit");
    g_signal_connect(item, "activate", G_CALLBACK(stk_window_exit), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

    gtk_widget_show_all(menu);
    gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, button, time);
}

GtkWidget *stk_mainwin_create()
{
    GdkColor color;

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_widget_add_events(window, GDK_BUTTON_PRESS_MASK);
	/* do these two in main function */
    //g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK (stk_window_exit), (gpointer)tray);
    //g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK (stk_window_exit), (gpointer)tray);
    g_signal_connect(G_OBJECT(window), "button-press-event", G_CALLBACK(stk_window_move), NULL); 
    g_signal_connect(G_OBJECT(window), "window_state_event", G_CALLBACK(stk_window_iconify), NULL);

    /*
     * set main window attribution
     */
    /* Remove Border and Title Bar */
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);

	/* not to display the window in the task bar, only works under Linux, gtk bugs?  */
#ifdef _LINUX_
#if GTK_CHECK_VERSION(2,2,0)
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(window),TRUE);
#else
    gtk_window_set_type_hint(GTK_WINDOW(window), GDK_WINDOW_TYPE_HINT_DROPDOWN_MENU);
#endif
#endif

    gtk_window_unmaximize(GTK_WINDOW(window));
    gtk_window_set_title(GTK_WINDOW(window), "stkclient");
    gtk_widget_set_size_request(window, STK_WINDOW_WIDTH, STK_WINDOW_HEIGHT);
    //gtk_window_set_default_size(GTK_WINDOW(window), 300, 600);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(window),FALSE);

    /* set window background color */
    //gdk_color_parse("green", &color);
    //gdk_color_parse("#96FA96", &color);
    //gdk_color_parse("#C8C8FA", &color);
    gdk_color_parse(MAIN_COLRO_STRING, &color);
    //color.red = 0xffff;  
    //color.green = 0xffff;  
    //color.blue = 0xffff; 
	gtk_widget_modify_bg(window, GTK_STATE_NORMAL, &color);
	
    gtk_window_set_icon(GTK_WINDOW(window), gdk_pixbuf_new_from_file(STK_ICON_PNG, NULL));

    return window;
}

GtkStatusIcon *stk_tray_create(GtkWidget *window)
{
    GtkStatusIcon *tray_icon;

    tray_icon = gtk_status_icon_new_from_pixbuf(gdk_pixbuf_new_from_file(STK_ICON_PNG, NULL));

    g_signal_connect(G_OBJECT(tray_icon), "activate", G_CALLBACK(stk_window_show), (gpointer)window);
    g_signal_connect(G_OBJECT(tray_icon), "popup-menu", G_CALLBACK(stk_tray_menu), (gpointer)window); 

#if GTK_CHECK_VERSION(2,10,0)
#if !GTK_CHECK_VERSION(2,16,0)
    gtk_status_icon_set_tooltip(tray_icon, "stkclient");
#else
    gtk_status_icon_set_tooltip_text(tray_icon, "stkclient");
#endif
#endif

    gtk_status_icon_set_visible(tray_icon, TRUE);

    return tray_icon;
}  

static void stk_tree_setup(GtkWidget *tree)
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

static void stk_tree_fill(GtkWidget *tree, const char *str)
{
    GtkListStore *store;
    GtkTreeIter iter;
    GdkPixbuf *pixbuf;

    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree)));
    pixbuf = gdk_pixbuf_new_from_file(STK_BUDDY_PNG, NULL);

    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, STK_PIXBUF_COL, pixbuf, STK_TEXT_COL, str, -1);
    gdk_pixbuf_unref(pixbuf);
}

static void stk_buddy_show(GtkWidget *widget, gpointer data)
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


static gboolean stk_buddy_lclick(GtkWidget *widget, GdkEventButton *event)
{
    GtkTreeView *tree = GTK_TREE_VIEW(widget); 
    GtkTreeSelection *selection;
    GtkTreeModel *model;
    GtkTreeIter iter;
    char *uid;
    stk_buddy *buddy = NULL;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree)); 
    selection = gtk_tree_view_get_selection(tree);
    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) 
        return FALSE;
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
        stk_chatwin_show(NULL, (gpointer)buddy);
        return TRUE;
	} else {
        return FALSE;
	}
}

static void stk_buddy_rclick (stk_buddy *buddy)
{
    GtkWidget *buddy_info, *chat, *separator;

    buddy_info = gtk_menu_item_new_with_label("Show Buddy Info");
    chat = gtk_menu_item_new_with_label("Chat With Buddy");
    separator = gtk_separator_menu_item_new();

    g_signal_connect(G_OBJECT(buddy_info), "activate", G_CALLBACK (stk_buddy_show), (gpointer)buddy);
    g_signal_connect(G_OBJECT(chat), "activate", G_CALLBACK (stk_chatwin_show), (gpointer)buddy);

    gtk_menu_shell_append(GTK_MENU_SHELL (buddy->menu), buddy_info);
    gtk_menu_shell_append(GTK_MENU_SHELL (buddy->menu), separator);
    gtk_menu_shell_append(GTK_MENU_SHELL (buddy->menu), chat);

    gtk_widget_show_all(buddy->menu);
}

void stk_buddywin_create(STKWIDGETS *widgets)
{
    char buf[STK_DEFAULT_SIZE] = {0};
    stk_buddy *buddy = NULL;
    unsigned short buddy_num = stk_get_buddynum();
    GtkWidget *vbox;
    GtkWidget *hbox1, *hbox2, *hbox3;
    GtkWidget *label;
    GtkWidget *image;
    GtkWidget *frame;
    GtkWidget *tree;
    GtkListStore *store;
    GtkTreeSelection *select_item;
    //GtkWidget *toolbar;
    GtkToolItem *toolitem;

    sprintf(buf, "%d (%s)", client.uid, client.nickname);

    vbox = gtk_vbox_new(FALSE, 0);
    hbox1 = gtk_hbox_new(FALSE, 0);
    hbox2 = gtk_hbox_new(FALSE, 0);
    hbox3 = gtk_hbox_new(FALSE, 0);

    /* init close and minimize button for main window */
    image = gtk_image_new_from_file(STK_CLOSE_PNG);
	toolitem = gtk_tool_button_new(image, "Close");
    gtk_tool_item_set_tooltip_text(toolitem, "Close");
    gtk_tool_item_set_is_important(toolitem, TRUE);
	g_signal_connect(G_OBJECT(toolitem), "clicked", G_CALLBACK(stk_window_exit), (gpointer)widgets->tray);
    gtk_box_pack_end(GTK_BOX(hbox1), GTK_WIDGET(toolitem), FALSE, FALSE, 0);

    image = gtk_image_new_from_file(STK_MIN_PNG);
	toolitem = gtk_tool_button_new(image, "Minimize");
    gtk_tool_item_set_tooltip_text(toolitem, "Minimize");
    gtk_tool_item_set_is_important(toolitem, TRUE);
	g_signal_connect(G_OBJECT(toolitem), "clicked", G_CALLBACK(stk_window_hide), (gpointer)widgets->mainw);
    gtk_box_pack_end(GTK_BOX(hbox1), GTK_WIDGET(toolitem), FALSE, FALSE, 0);

    /* init my infomation widgets and add to hbox2 */
    label = gtk_label_new(buf);
    image = gtk_image_new_from_file(STK_AVATAR_PNG);
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);	
    gtk_box_pack_start(GTK_BOX(hbox2), image, FALSE, FALSE, 15);
    gtk_box_pack_start(GTK_BOX(hbox2), label, FALSE, FALSE, 10);

    /*  init buddy list frame */
    frame = gtk_frame_new("Buddy List");
    gtk_container_set_border_width(GTK_CONTAINER(frame), 5);

    store = gtk_list_store_new(STK_COL_NUM, GDK_TYPE_PIXBUF, G_TYPE_STRING);
    tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree), FALSE);

    select_item = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
    gtk_tree_selection_set_mode(select_item, GTK_SELECTION_SINGLE);
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(tree), TRUE);
    //gtk_tree_view_set_headers_clickable(GTK_TREE_VIEW(tree), TRUE);
    //gtk_tree_view_set_reorderable(GTK_TREE_VIEW(tree), TRUE);
    gtk_tree_view_set_hover_selection(GTK_TREE_VIEW(tree), TRUE);
    gtk_tree_view_set_hover_expand(GTK_TREE_VIEW(tree), TRUE);

    stk_tree_setup(tree);
    gtk_widget_add_events(tree, GDK_BUTTON_PRESS_MASK);
    g_signal_connect(G_OBJECT(tree), "button-press-event", G_CALLBACK(stk_buddy_lclick), NULL);
    gtk_container_add(GTK_CONTAINER (frame), tree);

    /* init tools, chat voice video  */
    image = gtk_image_new_from_file(STK_CHAT_PNG);
	toolitem = gtk_tool_button_new(image, "Chat");
    gtk_tool_item_set_tooltip_text(toolitem, "Chat");
	gtk_tool_item_set_is_important(toolitem, TRUE);
	g_signal_connect(G_OBJECT(toolitem), "clicked", G_CALLBACK(stk_chat_request), (gpointer)buddy);
    gtk_box_pack_start(GTK_BOX(hbox3), GTK_WIDGET(toolitem), FALSE, FALSE, 0);

    image = gtk_image_new_from_file(STK_VOICE_PNG);
	toolitem = gtk_tool_button_new(image, "Voice");
    gtk_tool_item_set_tooltip_text(toolitem, "Voice");
	gtk_tool_item_set_is_important(toolitem, TRUE);
	g_signal_connect(G_OBJECT(toolitem), "clicked", G_CALLBACK(stk_voice_request), (gpointer)buddy);
    gtk_box_pack_start(GTK_BOX(hbox3), GTK_WIDGET(toolitem), FALSE, FALSE, 0);

    image = gtk_image_new_from_file(STK_VIDEO_PNG);
	toolitem = gtk_tool_button_new(image, "Video");
    gtk_tool_item_set_tooltip_text(toolitem, "Video");
	gtk_tool_item_set_is_important(toolitem, TRUE);
	g_signal_connect(G_OBJECT(toolitem), "clicked", G_CALLBACK(stk_video_request), (gpointer)buddy);
    gtk_box_pack_start(GTK_BOX(hbox3), GTK_WIDGET(toolitem), FALSE, FALSE, 0);

    /* add container to main window */
    gtk_box_pack_start(GTK_BOX(vbox), hbox1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox2, FALSE, FALSE, 30);
    gtk_box_pack_end(GTK_BOX(vbox), hbox3, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(widgets->mainw), vbox);

    while (buddy_num--) {
        buddy = stk_get_next(buddy);
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%d (%s)", buddy->uid, buddy->nickname);
        stk_tree_fill(tree, buf);
        buddy->menu = gtk_menu_new();
        stk_buddy_rclick(buddy);
    }

    widgets->treev= tree;

#if defined(USE_GTK_THREAD)
    g_thread_create((GThreadFunc)stk_recv_msg, (gpointer)&client, FALSE, NULL);  
#endif

    gtk_window_present(GTK_WINDOW(widgets->mainw));
    gtk_widget_show_all(widgets->mainw);
}


void stk_loginbtn_pressed(GtkWidget *widget, STKWIDGETS *widgets)
{
    unsigned char sendbuf[STK_MAX_PACKET_SIZE] = {0};
    char buf[STK_DEFAULT_SIZE];
    char *username, *passwd, *serverip;
    int ret;

    memset(buf, 0, sizeof(buf));
    username = (char *)gtk_entry_get_text(GTK_ENTRY(widgets->loginw.usertext));
    passwd = (char *)gtk_entry_get_text(GTK_ENTRY(widgets->loginw.passtext));
    serverip = (char *)gtk_entry_get_text(GTK_ENTRY(widgets->loginw.servertext));

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
        stk_buddy buddy;

        sprintf(buf, "STK Client %d login success.\n", client.uid);
        client.state = STK_CLIENT_ONLINE;

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
#if GTK_CHECK_VERSION(2,10,0)
#if !GTK_CHECK_VERSION(2,16,0)
        gtk_status_icon_set_tooltip(widgets->tray, buf);
#else
        gtk_status_icon_set_tooltip_text(widgets->tray, buf);
#endif
#endif

        gtk_widget_destroy(widgets->loginw.layout);
        stk_buddywin_create(widgets);
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

void stk_loginwin_create(STKWIDGETS *widgets)
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
    GtkWidget *image;
    GtkToolItem *toolitem;

    layout = gtk_fixed_new();
    userlabel = gtk_label_new("Username:");
    usertext = gtk_entry_new();
    passlabel = gtk_label_new("Password:");
    passtext = gtk_entry_new();
    serverlabel = gtk_label_new("Server IP:");
    servertext = gtk_entry_new();
    loginbutton = gtk_button_new_with_label("Login");

    image = gtk_image_new_from_file(STK_CLOSE_PNG);
	toolitem = gtk_tool_button_new(image, "Close");
    gtk_tool_item_set_tooltip_text(toolitem, "Close");
    gtk_tool_item_set_is_important(toolitem, TRUE);
    //gtk_container_set_border_width(GTK_CONTAINER(toolitem), 0); /* seems no need */
	g_signal_connect(G_OBJECT(toolitem), "clicked", G_CALLBACK(stk_window_exit), (gpointer)widgets->tray);

    gtk_fixed_put(GTK_FIXED(layout), GTK_WIDGET(toolitem), 300-45, 0);

	widgets->loginw.layout = layout;
    widgets->loginw.usertext = usertext;
    widgets->loginw.passtext = passtext;
    widgets->loginw.servertext = servertext;

    g_signal_connect(G_OBJECT(loginbutton), "clicked", G_CALLBACK(stk_loginbtn_pressed), (gpointer)widgets);

    gtk_entry_set_visibility(GTK_ENTRY(usertext), TRUE);
    gtk_entry_set_visibility(GTK_ENTRY(passtext), FALSE);
    gtk_entry_set_visibility(GTK_ENTRY(servertext), TRUE);
    gtk_editable_set_editable(GTK_EDITABLE(usertext), TRUE);
    gtk_editable_set_editable(GTK_EDITABLE(passtext), TRUE);
    gtk_editable_set_editable(GTK_EDITABLE(servertext), TRUE);

    gtk_fixed_put(GTK_FIXED(layout), userlabel, 80, 200);
    gtk_fixed_put(GTK_FIXED(layout), usertext, 80, 220);
    gtk_fixed_put(GTK_FIXED(layout), passlabel, 80, 250);
    gtk_fixed_put(GTK_FIXED(layout), passtext, 80, 270);
    gtk_fixed_put(GTK_FIXED(layout), serverlabel, 80, 300);
    gtk_fixed_put(GTK_FIXED(layout), servertext, 80, 320);
    gtk_fixed_put(GTK_FIXED(layout), loginbutton, 80, 350);

    gag = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(widgets->mainw), gag);
    gtk_widget_add_accelerator(loginbutton, "clicked", gag, GDK_Return, 0, GTK_ACCEL_VISIBLE); /* GDK_KEY_Return */
}

