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

gboolean stk_window_hide(GtkWidget *widget, GtkWidget *window)
{
    gtk_window_iconify(GTK_WINDOW(window));
    return TRUE;
}

gboolean stk_window_hide2(GtkWidget *widget,  GdkEventButton *event, GtkWidget *window)
{
    if(event->type == GDK_BUTTON_RELEASE && event->button == 0x1) {
        gtk_window_iconify(GTK_WINDOW(window));
        return TRUE;
    } else {
        return FALSE;
    }
}

/* if gtk_window_move() doesnot called before this, the position of window is decided by window manager */
gboolean stk_window_show(GtkWidget *widget, GtkWidget *window)
{
    gtk_widget_show_all(window);
    /* set window toplevel, then set usertext as focus. For windows, it's necessary, for Linux, seems no need */
    gtk_window_present(GTK_WINDOW(window));
    return TRUE;
}

gboolean stk_window_exit(GtkWidget *widget, GtkStatusIcon *tray)
{
    gtk_status_icon_set_visible(tray, FALSE);
    gtk_main_quit();
    stk_set_running(STK_EXITING);
    return TRUE;
}

gboolean stk_window_exit2(GtkWidget *widget,  GdkEventButton *event, GtkStatusIcon *tray)
{
    if(event->type == GDK_BUTTON_RELEASE && event->button == 0x1) {
        gtk_status_icon_set_visible(tray, FALSE);
        gtk_main_quit();
        stk_set_running(STK_EXITING);
        return TRUE;
    } else {
        return FALSE;
    }
}

gboolean stk_window_iconify(GtkWidget *widget, GdkEventWindowState *event, gpointer data)
{
    if(event->changed_mask == GDK_WINDOW_STATE_ICONIFIED && event->new_window_state == GDK_WINDOW_STATE_ICONIFIED)
    {
        gtk_widget_hide_all(widget);
        return TRUE;
    } else {
        return FALSE;
    }
}

gboolean stk_window_move(GtkWidget *widget,GdkEventButton *event,gint data)
{
    if(event->type == GDK_BUTTON_PRESS && event->button == 0x1)
    {
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), 
                      event->button, event->x_root, event->y_root,event->time);
        return TRUE;
    } else {
        return TRUE;
    }
}

void stk_screensize_get(ScreenSize *screensize)
{
    GdkScreen *screen;

    screen = gdk_screen_get_default();
    screensize->width = gdk_screen_get_width(screen);
    screensize->height = gdk_screen_get_height(screen);
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
    g_signal_connect(item, "activate", G_CALLBACK(stk_window_exit), (gpointer)statusicon);
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
    gtk_widget_set_size_request(window, STK_MAINWIN_WIDTH, STK_MAINWIN_HEIGHT);
    //gtk_window_set_default_size(GTK_WINDOW(window), 300, 600);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(window),FALSE);
    gtk_window_set_keep_above(GTK_WINDOW(window),TRUE);

    /* set window background color */
    //gdk_color_parse("green", &color);
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

static void stk_buddytree_setup(GtkWidget *tree)
{
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    /*
     * Create a new GtkCellRendererText, add it to the tree view column and
     * append the column to the tree view.
     */
    renderer = gtk_cell_renderer_pixbuf_new();
    column = gtk_tree_view_column_new_with_attributes("Pic",renderer, "pixbuf", STK_BUDDY_PIXBUF_COL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW (tree), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Uid", renderer, "text", STK_BUDDY_ID_COL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW (tree), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Uname", renderer, "text", STK_BUDDY_NAME_COL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW (tree), column);
}

static void stk_buddytree_fill(GtkWidget *tree, const char *idstr, const char *namestr)
{
    GtkListStore *store;
    GtkTreeIter iter;
    GdkPixbuf *pixbuf;

    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree)));
    pixbuf = gdk_pixbuf_new_from_file(STK_BUDDY_PNG, NULL);

    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, STK_BUDDY_PIXBUF_COL, pixbuf, 
                        STK_BUDDY_ID_COL, idstr, STK_BUDDY_NAME_COL, namestr,  -1);
    gdk_pixbuf_unref(pixbuf);
}

static void stk_buddy_show(GtkWidget *widget, gpointer data)
{
    stk_buddy *buddy = (stk_buddy *)data;
    char buf[STK_MAX_SIZE] = {0};

    stk_get_buddyinfo(buddy, buf);

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
    gtk_tree_model_get(model, &iter, STK_BUDDY_ID_COL, &uid, -1);

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

    g_signal_connect(G_OBJECT(buddy_info), "activate", G_CALLBACK(stk_buddy_show), (gpointer)buddy);
    g_signal_connect(G_OBJECT(chat), "activate", G_CALLBACK(stk_chatwin_show), (gpointer)buddy);

    gtk_menu_shell_append(GTK_MENU_SHELL(buddy->menu), buddy_info);
    gtk_menu_shell_append(GTK_MENU_SHELL(buddy->menu), separator);
    gtk_menu_shell_append(GTK_MENU_SHELL(buddy->menu), chat);

    gtk_widget_show_all(buddy->menu);
}


static void stk_grouptree_setup(GtkWidget *tree)
{
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    /*
     * Create a new GtkCellRendererText, add it to the tree view column and
     * append the column to the tree view.
     */
    renderer = gtk_cell_renderer_pixbuf_new();
    column = gtk_tree_view_column_new_with_attributes("Pic",renderer, "pixbuf", STK_GROUP_PIXBUF_COL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW (tree), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Gid", renderer, "text", STK_GROUP_ID_COL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW (tree), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Gname", renderer, "text", STK_GROUP_NAME_COL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW (tree), column);
}

static void stk_grouptree_fill(GtkWidget *tree, const char *idstr, const char *namestr)
{
    GtkListStore *store;
    GtkTreeIter iter;
    GdkPixbuf *pixbuf;

    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree)));
    pixbuf = gdk_pixbuf_new_from_file(STK_GROUP_PNG, NULL);

    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, STK_GROUP_PIXBUF_COL, pixbuf, 
                        STK_GROUP_ID_COL, idstr, STK_GROUP_NAME_COL, namestr,  -1);
    gdk_pixbuf_unref(pixbuf);
}

static void stk_group_show(GtkWidget *widget, gpointer data)
{
    stk_group *group = (stk_group *)data;
    char buf[STK_MAX_SIZE] = {0};

    stk_get_groupinfo(group, buf);

    stk_message("Group", buf);
}

static gboolean stk_group_lclick(GtkWidget *widget, GdkEventButton *event)
{
    GtkTreeView *tree = GTK_TREE_VIEW(widget); 
    GtkTreeSelection *selection;
    GtkTreeModel *model;
    GtkTreeIter iter;
    stk_group *group = NULL;
    char *gidstr;
    unsigned int gid;
    int num;
    gboolean found = FALSE;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree)); 
    selection = gtk_tree_view_get_selection(tree);
    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) 
        return FALSE;
    gtk_tree_model_get(model, &iter, STK_GROUP_ID_COL, &gidstr, -1);

    gid = atoi(gidstr);
    num = client.group_num;
    group = client.group;
    while (num-- && group != NULL) {
        if (group->groupid == gid) {
            found = TRUE;
            break;
        }
        group = group->next;
    }

    /* this can not be happen!! */
    if (!found || group == NULL) {
        stk_message("STK Error", "Bad Group");
        return FALSE;
    }

    if(event->type == GDK_BUTTON_PRESS && event->button == 0x3) {
        gtk_menu_popup(GTK_MENU(group->menu), NULL, NULL,NULL, NULL, event->button, event->time);
        return TRUE;
    } else if (event->type == GDK_2BUTTON_PRESS && event->button == 0x1) {
        stk_gchatwin_show(NULL, (gpointer)group);
        return TRUE;
    } else {
        return FALSE;
    }
}

static void stk_group_rclick (stk_group *group)
{
    GtkWidget *group_info, *gchat, *separator;

    group_info = gtk_menu_item_new_with_label("Show Group Info");
    gchat = gtk_menu_item_new_with_label("Chat In Group");
    separator = gtk_separator_menu_item_new();

    g_signal_connect(G_OBJECT(group_info), "activate", G_CALLBACK(stk_group_show), (gpointer)group);
    g_signal_connect(G_OBJECT(gchat), "activate", G_CALLBACK(stk_gchatwin_show), (gpointer)group);

    gtk_menu_shell_append(GTK_MENU_SHELL(group->menu), group_info);
    gtk_menu_shell_append(GTK_MENU_SHELL(group->menu), separator);
    gtk_menu_shell_append(GTK_MENU_SHELL(group->menu), gchat);

    gtk_widget_show_all(group->menu);
}

gboolean stk_check_socket(StkWidget *widgets)
{
    int running;

    running = stk_get_running();

    if (running == STK_SOCKET_ERR) {
        /* socket err, maybe network is down */
        stk_window_exit(NULL, widgets->tray);
        return FALSE;
    } else if (running == STK_SERVER_EXIT) {
        /* server exit... */
        stk_window_exit(NULL, widgets->tray);
        return FALSE;
    } else {
        return TRUE;
    }
}

void stk_buddywin_create(StkWidget *widgets)
{
    char buf[STK_DEFAULT_SIZE] = {0};
    stk_buddy *buddy = NULL;
    stk_group *group;
    int num;
    GtkWidget *notebook;
    GtkWidget *label;
    GtkWidget *sw_buddy, *sw_group;
    GtkWidget *vbox;
    GtkWidget *hbox1, *hbox2, *hbox3;
    GtkWidget *image;
    GtkWidget *tree;
    GtkListStore *store;
    GtkTreeSelection *select_item;
    GtkRequisition *imgsize;
    GtkToolItem *toolitem;
#if !GTK_CHECK_VERSION(2,12,0)
    GtkTooltips *tooltips;
#endif

    sprintf(buf, "%d (%s)", client.uid, client.nickname);

    vbox = gtk_vbox_new(FALSE, 0);
    hbox1 = gtk_hbox_new(FALSE, 0);
    hbox2 = gtk_hbox_new(FALSE, 0);
    hbox3 = gtk_hbox_new(FALSE, 0);

    notebook = gtk_notebook_new();
    gtk_container_set_border_width(GTK_CONTAINER (notebook), 6);

#if !GTK_CHECK_VERSION(2,12,0)
    tooltips = gtk_tooltips_new();
#endif

#if 0
    /* init close and minimize button for main window */
    image = gtk_image_new_from_file(STK_CLOSE_PNG);
    toolitem = gtk_tool_button_new(image, "Close");
#if !GTK_CHECK_VERSION(2,12,0)
    gtk_tool_item_set_tooltip(toolitem, tooltips, "Close", NULL);
#else
    gtk_tool_item_set_tooltip_text(toolitem, "Close");
#endif
    gtk_tool_item_set_is_important(toolitem, TRUE);
    g_signal_connect(G_OBJECT(toolitem), "clicked", G_CALLBACK(stk_window_exit), (gpointer)widgets->tray);
    gtk_box_pack_end(GTK_BOX(hbox1), GTK_WIDGET(toolitem), FALSE, FALSE, 0);

    image = gtk_image_new_from_file(STK_MIN_PNG);
    toolitem = gtk_tool_button_new(image, "Minimize");
#if !GTK_CHECK_VERSION(2,12,0)
    gtk_tool_item_set_tooltip(toolitem, tooltips, "Minimize", NULL);
#else
    gtk_tool_item_set_tooltip_text(toolitem, "Minimize");
#endif
    gtk_tool_item_set_is_important(toolitem, TRUE);
    g_signal_connect(G_OBJECT(toolitem), "clicked", G_CALLBACK(stk_window_hide), (gpointer)widgets->mainw);
    gtk_box_pack_end(GTK_BOX(hbox1), GTK_WIDGET(toolitem), FALSE, FALSE, 0);
#else
    memset(&widgets->minbtn, 0, sizeof(ImageButton));
    strcpy(widgets->minbtn.normal, STK_MIN_NORMAL_PNG);
    strcpy(widgets->minbtn.on_enter, STK_MIN_HLIGHT_PNG);
    strcpy(widgets->minbtn.on_press, STK_MIN_DOWN_PNG);
    strcpy(widgets->minbtn.tips, "Minimize");

    stk_create_imgbtn(&widgets->cbtn, stk_window_exit2, (gpointer)widgets->tray);
    stk_create_imgbtn(&widgets->minbtn, stk_window_hide2, (gpointer)widgets->mainw);
    gtk_event_box_set_visible_window(GTK_EVENT_BOX(widgets->cbtn.event_box), FALSE);
    gtk_event_box_set_visible_window(GTK_EVENT_BOX(widgets->minbtn.event_box), FALSE);

    gtk_box_pack_end(GTK_BOX(hbox1), widgets->cbtn.event_box, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(hbox1), widgets->minbtn.event_box, FALSE, FALSE, 0);

#endif
    /* init my infomation widgets and add to hbox2 */
    label = gtk_label_new(buf);
    if (client.gender == STK_GENDER_BOY) {
        image = gtk_image_new_from_file(STK_AVATAR_PNG);
    } else {
        image = gtk_image_new_from_file(STK_AVATAR_GIRL_PNG);
    }
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
    gtk_box_pack_start(GTK_BOX(hbox2), image, FALSE, FALSE, 15);
    gtk_box_pack_start(GTK_BOX(hbox2), label, FALSE, FALSE, 10);

    /* init tools, chat voice video  */
    image = gtk_image_new_from_file(STK_CHAT_PNG);
    toolitem = gtk_tool_button_new(image, "Chat");
#if !GTK_CHECK_VERSION(2,12,0)
    gtk_tool_item_set_tooltip(toolitem, tooltips, "Chat", NULL);
#else
    gtk_tool_item_set_tooltip_text(toolitem, "Chat");
#endif
    gtk_tool_item_set_is_important(toolitem, TRUE);
    g_signal_connect(G_OBJECT(toolitem), "clicked", G_CALLBACK(stk_chat_request), (gpointer)buddy);
    gtk_box_pack_start(GTK_BOX(hbox3), GTK_WIDGET(toolitem), FALSE, FALSE, 0);

    image = gtk_image_new_from_file(STK_VOICE_PNG);
    toolitem = gtk_tool_button_new(image, "Voice");
#if !GTK_CHECK_VERSION(2,12,0)
    gtk_tool_item_set_tooltip(toolitem, tooltips, "Voice", NULL);
#else
    gtk_tool_item_set_tooltip_text(toolitem, "Voice");
#endif
    gtk_tool_item_set_is_important(toolitem, TRUE);
    g_signal_connect(G_OBJECT(toolitem), "clicked", G_CALLBACK(stk_voice_request), (gpointer)buddy);
    gtk_box_pack_start(GTK_BOX(hbox3), GTK_WIDGET(toolitem), FALSE, FALSE, 0);

    image = gtk_image_new_from_file(STK_VIDEO_PNG);
    toolitem = gtk_tool_button_new(image, "Video");
#if !GTK_CHECK_VERSION(2,12,0)
    gtk_tool_item_set_tooltip(toolitem, tooltips, "Video", NULL);
#else
    gtk_tool_item_set_tooltip_text(toolitem, "Video");
#endif
    gtk_tool_item_set_is_important(toolitem, TRUE);
    g_signal_connect(G_OBJECT(toolitem), "clicked", G_CALLBACK(stk_video_request), (gpointer)buddy);
    gtk_box_pack_start(GTK_BOX(hbox3), GTK_WIDGET(toolitem), FALSE, FALSE, 0);

    /* add container to main window */
    gtk_box_pack_start(GTK_BOX(vbox), hbox1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox2, FALSE, FALSE, 30);
    gtk_box_pack_end(GTK_BOX(vbox), hbox3, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(widgets->mainw), vbox);

    /* buddy list treeview  */
    sw_buddy = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(sw_buddy), GTK_SHADOW_ETCHED_IN);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw_buddy), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

    store = gtk_list_store_new(STK_BUDDY_COL_NUM, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING);
    tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree), FALSE);

    select_item = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
    gtk_tree_selection_set_mode(select_item, GTK_SELECTION_SINGLE);
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(tree), TRUE);
    gtk_tree_view_set_reorderable(GTK_TREE_VIEW(tree), TRUE);
    gtk_tree_view_set_hover_selection(GTK_TREE_VIEW(tree), TRUE);
    gtk_tree_view_set_hover_expand(GTK_TREE_VIEW(tree), TRUE);

    stk_buddytree_setup(tree);
    gtk_widget_add_events(tree, GDK_BUTTON_PRESS_MASK);
    g_signal_connect(G_OBJECT(tree), "button-press-event", G_CALLBACK(stk_buddy_lclick), NULL);
    gtk_container_add(GTK_CONTAINER (sw_buddy), tree);

    num = stk_get_buddynum();
    while (num--) {
        char uid[STK_DEFAULT_SIZE] = {0};
        char uname[STK_DEFAULT_SIZE] = {0};
        buddy = stk_next_buddy(buddy);
        sprintf(uid, "%u", buddy->uid);
        sprintf(uname, "%s", buddy->nickname);
        stk_buddytree_fill(tree, uid, uname);
        buddy->menu = gtk_menu_new();
        stk_buddy_rclick(buddy);
    }
    widgets->buddytree= tree;

    /* group list treeview  */
    sw_group = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(sw_group), GTK_SHADOW_ETCHED_IN);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw_group), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

    store = gtk_list_store_new(STK_GROUP_COL_NUM, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING);
    tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree), FALSE);

    select_item = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
    gtk_tree_selection_set_mode(select_item, GTK_SELECTION_SINGLE);
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(tree), TRUE);
    gtk_tree_view_set_reorderable(GTK_TREE_VIEW(tree), TRUE);
    gtk_tree_view_set_hover_selection(GTK_TREE_VIEW(tree), TRUE);
    gtk_tree_view_set_hover_expand(GTK_TREE_VIEW(tree), TRUE);

    stk_grouptree_setup(tree);
    gtk_widget_add_events(tree, GDK_BUTTON_PRESS_MASK);
    g_signal_connect(G_OBJECT(tree), "button-press-event", G_CALLBACK(stk_group_lclick), NULL);
    gtk_container_add(GTK_CONTAINER (sw_group), tree);

    num = client.group_num;
    group = client.group;
    while (num-- && group != NULL) {
        char gid[STK_DEFAULT_SIZE] = {0};
        char gname[STK_DEFAULT_SIZE] = {0};
        sprintf(gid, "%u", group->groupid);
        strcpy(gname, group->groupname);
        stk_grouptree_fill(tree, gid, gname);
        group->menu = gtk_menu_new();
        stk_group_rclick(group);
        group = group->next;
    }

    widgets->grouptree= tree;

    /* add buddy and group to notebook */
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), sw_buddy, gtk_label_new ("User List"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), sw_group, gtk_label_new ("Group List"));

    g_timeout_add(2000, (GSourceFunc)stk_check_socket, (gpointer)widgets); /* every 2s */

    gtk_window_present(GTK_WINDOW(widgets->mainw));
    gtk_widget_show_all(widgets->mainw);
}

void stk_login_cancle(GtkWidget *widget, StkWidget *widgets)
{
    stk_set_running(STK_INITIALIZED);
}

void stk_loginret_show(int show, char *buf, GtkWidget *retlabel)
{
    if(show) {
        gtk_label_set_text(GTK_LABEL(retlabel), buf);
        gtk_widget_show(retlabel);
    } else {
        gtk_label_set_text(GTK_LABEL(retlabel), NULL);
        gtk_widget_hide(retlabel);
    }
}

gboolean stk_waiting(StkWidget *widgets)
{
    int running;
    char buf[STK_DEFAULT_SIZE] = {0};

    running = stk_get_running();

    if (running == STK_CONNECTING || running == STK_CONNECTE_REQ) {
        /* just continue */
        return TRUE;
    } else if (running == STK_SOCKET_ERR || running == STK_CONNECTE_ERR) {
        stk_set_running(STK_INITIALIZED);
        strcpy(buf, "Connect to stkserver falied.");
        goto failed;
    } else if (running == STK_USERID_ERR) {
        stk_set_running(STK_INITIALIZED);
        strcpy(buf, "Invalid Username.");
        goto failed;
    } else if (running == STK_PASSWORD_ERR) {
        stk_set_running(STK_INITIALIZED);
        strcpy(buf, "Invalid Password.");
        goto failed;
    } else if (running == STK_ALREADY_LOGGIN) {
        stk_set_running(STK_INITIALIZED);
        strcpy(buf, "Already Logined in.");
        goto failed;
    } else if (running == STK_INITIALIZED) {
        /* user cancle the login */
        goto failed;
    } else if (running == STK_CONNECTED) {
        /* doing nothing, continue to below */
    } else {
        return TRUE;
    }

success:
    /* it seems we donot need layout anymore, destory it */
    gtk_widget_destroy(widgets->loginw.layout);
    gtk_container_remove(GTK_CONTAINER(widgets->mainw), widgets->loginw.connlayout);

    sprintf(buf, "stkclient:%d(%s)", client.uid, client.nickname);
#if GTK_CHECK_VERSION(2,10,0)
#if !GTK_CHECK_VERSION(2,16,0)
    gtk_status_icon_set_tooltip(widgets->tray, buf);
#else
    gtk_status_icon_set_tooltip_text(widgets->tray, buf);
#endif
#endif

    stk_buddywin_create(widgets);
    stk_set_running(STK_RUNNING);

    return FALSE;

failed:
    gtk_container_remove(GTK_CONTAINER(widgets->mainw), widgets->loginw.connlayout);
    gtk_container_add(GTK_CONTAINER(widgets->mainw), widgets->loginw.layout);

    if (buf[0] != '\0') {
        stk_loginret_show(TRUE, buf, widgets->loginw.resultlabel);
    }

    gtk_widget_show_all(widgets->mainw);
    return FALSE;
}

void stk_connectwin_create(StkWidget *widgets)
{
    int height;
    GtkWidget *layout;
    GtkWidget *image1, *image2, *btnimage;
    GtkRequisition imgsize1, imgsize2;
#ifdef LOGIN_STYLE_NEW
    GtkToolItem *canclebtn;
#else
    GtkWidget *canclebtn;
#endif

    layout = gtk_fixed_new();
    image1 = gtk_image_new_from_file(STK_BEATU_PNG);
    image2 = gtk_image_new_from_file(STK_CONNECT_PNG);
    btnimage = gtk_image_new_from_file(STK_CANCEL_PNG);
    widgets->loginw.connlayout = layout;

    gtk_widget_size_request(image1, &imgsize1);
    gtk_fixed_put(GTK_FIXED(layout), image1, (STK_MAINWIN_WIDTH-imgsize1.width)/2, (STK_MAINWIN_HEIGHT-imgsize1.height)/2);

    gtk_widget_size_request(image2, &imgsize2);
    height = STK_MAINWIN_HEIGHT/2 + imgsize1.height/2 + imgsize2.height;
    gtk_fixed_put(GTK_FIXED(layout), image2, (STK_MAINWIN_WIDTH-imgsize2.width)/2, height);


#ifdef LOGIN_STYLE_NEW
    canclebtn = gtk_tool_button_new(btnimage, "Login");
    gtk_tool_item_set_is_important(canclebtn, TRUE);
#else
    canclebtn = gtk_button_new();
    gtk_button_set_image(GTK_BUTTON(canclebtn), btnimage);
#endif

    g_signal_connect(G_OBJECT(canclebtn), "clicked", G_CALLBACK(stk_login_cancle), (gpointer)widgets);
    gtk_fixed_put(GTK_FIXED(layout), GTK_WIDGET(canclebtn), (STK_MAINWIN_WIDTH-imgsize2.width)/2, height+imgsize2.height);

    /* we may use layout again, make a ref, unref it if we really donot need it */
    g_object_ref((gpointer)widgets->loginw.layout);

    gtk_container_remove(GTK_CONTAINER(widgets->mainw), widgets->loginw.layout);
    gtk_container_add(GTK_CONTAINER(widgets->mainw), layout);
    gtk_widget_show_all(widgets->mainw);
}

void stk_loginbtn_pressed(GtkWidget *widget, StkWidget *widgets)
{
    char buf[STK_DEFAULT_SIZE];
    char *username, *passwd, *serverip;
    int ret;

    memset(buf, 0, sizeof(buf));
    username = (char *)gtk_entry_get_text(GTK_ENTRY(widgets->loginw.usertext));
    passwd = (char *)gtk_entry_get_text(GTK_ENTRY(widgets->loginw.passtext));
    serverip = (char *)gtk_entry_get_text(GTK_ENTRY(widgets->loginw.servertext));

    /* safety check */
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
    stk_set_running(STK_CONNECTE_REQ);
    g_timeout_add(500, (GSourceFunc)stk_waiting, (gpointer)widgets); /* every 0.5s */
    stk_connectwin_create(widgets);
    return;

error:

    stk_loginret_show(TRUE, buf, widgets->loginw.resultlabel);
}

void stk_loginwin_create(StkWidget *widgets)
{
    GtkWidget *layout;
    GtkWidget *userlabel;
    GtkWidget *usertext;
    GtkWidget *passlabel;
    GtkWidget *passtext;
    GtkWidget *serverlabel;
    GtkWidget *servertext;
#ifdef LOGIN_STYLE_NEW
    GtkToolItem *loginbtn;
#else
    GtkWidget *loginbtn;
#endif
    GtkWidget *loginimg;
    GtkAccelGroup *gag;
    GtkToolItem *toolitem;
    GtkRequisition imgsize;
    GtkWidget *image;
    int width, height;

    layout = gtk_fixed_new();
    userlabel = gtk_label_new("Username:");
    usertext = gtk_entry_new();
    passlabel = gtk_label_new("Password:");
    passtext = gtk_entry_new();
    serverlabel = gtk_label_new("Server IP:");
    servertext = gtk_entry_new();
    loginimg = gtk_image_new_from_file(STK_LOGIN_PNG);
    image = gtk_image_new_from_file(STK_BEATU_PNG);
    widgets->loginw.resultlabel = gtk_label_new(NULL);

#ifdef LOGIN_STYLE_NEW
    loginbtn = gtk_tool_button_new(loginimg, "Login");
    gtk_tool_item_set_is_important(loginbtn, TRUE);
#else
    /* I'm afraid we cannot set the color of button */
    //loginbtn = gtk_button_new_with_label("   Login   ");
    loginbtn = gtk_button_new();
    gtk_button_set_image(GTK_BUTTON(loginbtn), loginimg);
#endif

#if 0
#if !GTK_CHECK_VERSION(2,12,0)
    GtkTooltips *tooltips;
    tooltips = gtk_tooltips_new();
#endif

    image = gtk_image_new_from_file(STK_CLOSE_PNG);
    toolitem = gtk_tool_button_new(image, "Close");
#if !GTK_CHECK_VERSION(2,12,0)
    gtk_tool_item_set_tooltip(toolitem, tooltips, "Close", NULL);
#else
    gtk_tool_item_set_tooltip_text(toolitem, "Close");
#endif
    gtk_tool_item_set_is_important(toolitem, TRUE);
    g_signal_connect(G_OBJECT(toolitem), "clicked", G_CALLBACK(stk_window_exit), (gpointer)widgets->tray);

    gtk_fixed_put(GTK_FIXED(layout), GTK_WIDGET(toolitem), 300-45, 0);
#else
    memset(&widgets->cbtn, 0, sizeof(ImageButton));
    strcpy(widgets->cbtn.normal, STK_CLOSE_NORMAL_PNG);
    strcpy(widgets->cbtn.on_enter, STK_CLOSE_HLIGHT_PNG);
    strcpy(widgets->cbtn.on_press, STK_CLOSE_DOWN_PNG);
    strcpy(widgets->cbtn.tips, "Close");

    stk_create_imgbtn(&widgets->cbtn, stk_window_exit2, (gpointer)widgets->tray);
    gtk_event_box_set_visible_window(GTK_EVENT_BOX(widgets->cbtn.event_box), FALSE);

    gtk_widget_size_request(widgets->cbtn.image, &imgsize);
    gtk_fixed_put(GTK_FIXED(layout), widgets->cbtn.event_box, STK_MAINWIN_WIDTH-imgsize.width, 0);
#endif

    widgets->loginw.layout = layout;
    widgets->loginw.usertext = usertext;
    widgets->loginw.passtext = passtext;
    widgets->loginw.servertext = servertext;

    g_signal_connect(G_OBJECT(loginbtn), "clicked", G_CALLBACK(stk_loginbtn_pressed), (gpointer)widgets);

    gtk_entry_set_visibility(GTK_ENTRY(usertext), TRUE);
    gtk_entry_set_visibility(GTK_ENTRY(passtext), FALSE);
    gtk_entry_set_visibility(GTK_ENTRY(servertext), TRUE);
    gtk_editable_set_editable(GTK_EDITABLE(usertext), TRUE);
    gtk_editable_set_editable(GTK_EDITABLE(passtext), TRUE);
    gtk_editable_set_editable(GTK_EDITABLE(servertext), TRUE);

    gtk_widget_size_request(image, &imgsize);
    width = (STK_MAINWIN_WIDTH-imgsize.width)/2;
    height = (STK_MAINWIN_HEIGHT-imgsize.height)/4;
    gtk_fixed_put(GTK_FIXED(layout), image, width, height);
    width += 20;
    height += imgsize.height;
    gtk_fixed_put(GTK_FIXED(layout), userlabel, width, height+20);
    gtk_fixed_put(GTK_FIXED(layout), usertext, width, height+40);
    gtk_fixed_put(GTK_FIXED(layout), passlabel, width, height+70);
    gtk_fixed_put(GTK_FIXED(layout), passtext, width, height+90);
    gtk_fixed_put(GTK_FIXED(layout), serverlabel, width, height+120);
    gtk_fixed_put(GTK_FIXED(layout), servertext, width, height+140);
    gtk_fixed_put(GTK_FIXED(layout), GTK_WIDGET(loginbtn), width, height+170);
    gtk_fixed_put(GTK_FIXED(layout), widgets->loginw.resultlabel, width, height+220);

    /* hide this, maybe not used at all. */
    gtk_widget_hide(widgets->loginw.resultlabel);

    gag = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(widgets->mainw), gag);
    gtk_widget_add_accelerator(GTK_WIDGET(loginbtn), "clicked", gag, GDK_Return, 0, GTK_ACCEL_VISIBLE); /* GDK_KEY_Return */

    gtk_container_add(GTK_CONTAINER(widgets->mainw), widgets->loginw.layout);
}

