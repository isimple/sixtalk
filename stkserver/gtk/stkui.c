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

#define MAIN_COLRO_STRING "#67A3CD"
#define STK_MAINWIN_WIDTH  500
#define STK_MAINWIN_HEIGHT 300

#define STK_ICON_PNG "pixmaps/manager.png"

static GtkWidget *treeview = NULL;

enum
{
  STK_UID_COL,
  STK_NICENAME_COL,
  STK_STATUS_COL,
  STK_CITY_COL,
  STK_PHONE_COL,
  STK_GENDER_COL,
  STK_COL_NUM
};

GtkWidget *stk_window_create()
{
    GdkColor color;

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    /*
     * set main window attribution
     */
    /* not to display the window in the task bar, only works under Linux, gtk bugs?  */
#if !GTK_CHECK_VERSION(2,2,0)
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(window),TRUE);
#else
    gtk_window_set_type_hint(GTK_WINDOW(window), GDK_WINDOW_TYPE_HINT_DROPDOWN_MENU);
#endif

    //gtk_window_unmaximize(GTK_WINDOW(window));
    gtk_window_set_title(GTK_WINDOW(window), "stkserver");
    gtk_widget_set_size_request(window, STK_MAINWIN_WIDTH, STK_MAINWIN_HEIGHT);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
    //gtk_window_set_resizable(GTK_WINDOW(window),FALSE);
    //gtk_window_set_keep_above(GTK_WINDOW(window),TRUE);

    /* set window background color */
    gdk_color_parse(MAIN_COLRO_STRING, &color);
    gtk_widget_modify_bg(window, GTK_STATE_NORMAL, &color);

    gtk_window_set_icon(GTK_WINDOW(window), gdk_pixbuf_new_from_file(STK_ICON_PNG, NULL));

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(gtk_main_quit), NULL);

    return window;
}

gboolean stk_tree_update(stk_client *client)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    unsigned int uid, old_uid;
    char buf[64] = {0};
    char *str = NULL;
    gboolean valid;
    GValue v;

    uid = client->stkc_uid;
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));

    for (valid = gtk_tree_model_get_iter_first(model, &iter); valid; valid = gtk_tree_model_iter_next(model, &iter)) {
        gtk_tree_model_get(model, &iter, STK_UID_COL, &str, -1);
        old_uid = atoi(str);
        g_free (str);
        if (uid == old_uid) {
            break;
        }
    }

    if (valid) {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%s", (client->stkc_state == STK_CLIENT_ONLINE)?"online":"offline");
        v.g_type = 0;
        g_value_init(&v, G_TYPE_STRING);
        g_value_set_string(&v, buf);
        gtk_list_store_set_value(GTK_LIST_STORE(model), &iter, STK_STATUS_COL, &v);
    }

    return FALSE;
}


static void stk_tree_setup(GtkWidget *tree)
{
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    /*
     * Create a new GtkCellRendererText, add it to the tree view column and
     * append the column to the tree view.
     */
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("UserID    ",renderer, "text", STK_UID_COL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW (tree), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Nickname    ", renderer, "text", STK_NICENAME_COL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW (tree), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Status    ", renderer, "text", STK_STATUS_COL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW (tree), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("City    ", renderer, "text", STK_CITY_COL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW (tree), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Phone    ", renderer, "text", STK_PHONE_COL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW (tree), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Gender    ", renderer, "text", STK_GENDER_COL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW (tree), column);

}

static void stk_tree_fill(GtkWidget *tree, stk_client *client)
{
    GtkListStore *store;
    GtkTreeIter iter;
    char buf_uid[64] = {0};
    char buf_nickname[64] = {0};
    char buf_status[64] = {0};
    char buf_city[64] = {0};
    char buf_phone[64] = {0};
    char buf_gender[64] = {0};

    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree)));

    sprintf(buf_uid,      "%u", client->stkc_uid);
    sprintf(buf_nickname, "%s", client->stkc_nickname);
    sprintf(buf_status,   "%s", (client->stkc_state == STK_CLIENT_ONLINE)?"online":"offline");
    sprintf(buf_city,     "%s", client->stkc_city);
    sprintf(buf_phone,    "%u", client->stkc_phone);
    sprintf(buf_gender,   "%s", (client->stkc_gender == STK_GENDER_BOY)?"boy":"girl");

    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 
                    STK_UID_COL, buf_uid,
                    STK_NICENAME_COL, buf_nickname,
                    STK_STATUS_COL, buf_status,
                    STK_CITY_COL, buf_city,
                    STK_PHONE_COL, buf_phone,
                    STK_GENDER_COL, buf_gender,
                    -1);
}

void stk_create_userlist(GtkWidget *window)
{
    GtkWidget *vbox;
    GtkWidget *label;
    GtkWidget *sw;
    GtkWidget *tree;
    GtkListStore *store;
    GtkTreeSelection *select_item;
    stk_client *client;
    int user_num;

    vbox = gtk_vbox_new(FALSE, 8);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    label = gtk_label_new("User List");
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

    sw = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(sw), GTK_SHADOW_ETCHED_IN);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(vbox), sw, TRUE, TRUE, 0);

    store = gtk_list_store_new(STK_COL_NUM, G_TYPE_STRING, G_TYPE_STRING,
                    G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree), TRUE);

    select_item = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
    gtk_tree_selection_set_mode(select_item, GTK_SELECTION_SINGLE);
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(tree), TRUE);
    gtk_tree_view_set_headers_clickable(GTK_TREE_VIEW(tree), TRUE);
    gtk_tree_view_set_reorderable(GTK_TREE_VIEW(tree), TRUE);
    //gtk_tree_view_set_hover_selection(GTK_TREE_VIEW(tree), TRUE);
    gtk_tree_view_set_hover_expand(GTK_TREE_VIEW(tree), TRUE);

    stk_tree_setup(tree);
    gtk_container_add(GTK_CONTAINER (sw), tree);

    user_num = stk_get_usernum();
    client = NULL;
    while (user_num--) {
        client = stk_get_next(client);
        stk_tree_fill(tree, client);
    }

    treeview = tree;
}


