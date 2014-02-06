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

typedef struct{
    GtkWidget *layout;
    GtkWidget *usertext;
    GtkWidget *passtext;
    GtkWidget *servertext;
}LoginWidget;

typedef struct{
    GtkWidget *mainw;
    GtkWidget *treev;
    LoginWidget loginw;
}StkWidgets;

StkWidgets stkwidgets;
client_config client;
unsigned char sendbuf[STK_MAX_PACKET_SIZE] = {0};

void clean_login_window()
{
//    GList *list = gtk_container_get_children(GTK_CONTAINER(stkwidgets.loginw));
//    gtk_widget_destroy();
    gtk_widget_destroy(stkwidgets.loginw.layout);

}

static void setup_tree_view (GtkWidget *tree, GtkTreeStore *store)
{
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    /*
     * Create a new GtkCellRendererText, add it to the tree view column and
     * append the column to the tree view.
     */
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Buddys", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW (tree), column);

    gtk_tree_view_set_model(GTK_TREE_VIEW(tree), GTK_TREE_MODEL(store));
    g_object_unref(store);
}

static void add_to_tree(GtkWidget *tree, const gchar *str)
{
  GtkListStore *store;
  GtkTreeIter iter;

  store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree)));

  gtk_list_store_append(store, &iter);
  gtk_list_store_set(store, &iter, 0, str, -1);
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
    GtkTreeSelection *selection;

    sprintf(buf, "%s (%d)", client.nickname, client.uid);

    vfix = gtk_fixed_new();
    hfix = gtk_fixed_new();
    label = gtk_label_new(buf);
    image = gtk_image_new_from_file("img.png");
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);	
    gtk_fixed_put((GtkFixed *)hfix, image, 0, 20);
    gtk_fixed_put((GtkFixed *)hfix, label, 90, 30);
    gtk_fixed_put((GtkFixed *)vfix, hfix, 20, 10);

    tree = gtk_tree_view_new();
    store = gtk_list_store_new(1, G_TYPE_STRING);
    setup_tree_view(tree, store);

    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree), FALSE);

    gtk_fixed_put((GtkFixed *)vfix, tree, 20, 120);
    gtk_container_add(GTK_CONTAINER(window), vfix);

    while (buddy_num--) {
        buddy = stk_get_next(buddy);
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%s (%d)", buddy->nickname, buddy->uid);
        add_to_tree(tree, buf);
    }

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));

    g_signal_connect(G_OBJECT(selection), "changed", G_CALLBACK(selection), label);

    gtk_widget_show_all(window);
}


void login_pressed(GtkWidget *widget, LoginWidget *loginwidget)
{
    gchar buf[STK_DEFAULT_SIZE];
    gchar *username, *passwd, *serverip;
    int ret;

    memset(buf, 0, sizeof(buf));
    username = (gchar *)gtk_entry_get_text(GTK_ENTRY(loginwidget->usertext));
    passwd = (gchar *)gtk_entry_get_text(GTK_ENTRY(loginwidget->passtext));
    serverip = (gchar *)gtk_entry_get_text(GTK_ENTRY(loginwidget->servertext));

    if (username[0] == '\0') {
        gchar *err = "Username is NULL\n";
        strcpy(buf, err);
        goto error;
    } else {
        client.uid = atoi(username);
    }

    if (passwd[0] == '\0') {
        gchar *err = "Password is NULL\n";
        strcpy(buf, err);
        goto error; 
    } else {
        strcpy(client.pass, passwd);
    }

    if (serverip[0] == '\0') {
        gchar *err = "Server IP is NULL\n";
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

void init_main_window(int argc,char *argv[], GtkWidget *window)
{
    g_signal_connect (G_OBJECT(window), "destroy", G_CALLBACK (gtk_main_quit), NULL);
    g_signal_connect (G_OBJECT(window), "delete_event", G_CALLBACK (gtk_main_quit), NULL);

    /* set main window attribution */
    gtk_window_unmaximize(GTK_WINDOW(window));
    gtk_window_set_resizable (GTK_WINDOW(window), TRUE);
    gtk_window_set_title (GTK_WINDOW(window), "STKClient");
    gtk_widget_set_size_request(window, 300, 600);
    //gtk_window_set_default_size(GTK_WINDOW(window), 300, 600);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(window),FALSE);

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

    gtk_init (&argc, &argv);
    stkwidgets.mainw = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    init_main_window(argc, argv, stkwidgets.mainw);

    init_login_window(&stkwidgets.loginw);

    gtk_container_add(GTK_CONTAINER(stkwidgets.mainw), stkwidgets.loginw.layout);

    gtk_widget_show_all(stkwidgets.mainw);

    gtk_main ();

}

