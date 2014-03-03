/* 
 * File: stkutil.c
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stk.h"

void stk_message(char *title, const char *msg)
{
    GtkWidget *dialog, *label, *image, *hbox;;
    char t[STK_DEFAULT_SIZE] = "stkclient";
    GtkWidget *content_area;

    if (title != NULL) {
        memset(t, 0, sizeof(t));
        strcpy(t, title);
    }

    dialog = gtk_dialog_new_with_buttons(t, NULL, GTK_DIALOG_MODAL| GTK_DIALOG_DESTROY_WITH_PARENT, 
                                            GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);

#if !GTK_CHECK_VERSION(3,0,0)
    gtk_dialog_set_has_separator(GTK_DIALOG (dialog), FALSE);
    content_area = GTK_DIALOG(dialog)->vbox;
#else
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
#endif
    label = gtk_label_new (msg);
    image = gtk_image_new_from_stock(GTK_STOCK_DIALOG_INFO, GTK_ICON_SIZE_DIALOG);
    hbox = gtk_hbox_new (FALSE, 5);
    gtk_container_set_border_width(GTK_CONTAINER (hbox), 10);
    gtk_box_pack_start(GTK_BOX (hbox), image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX (hbox), label, FALSE, FALSE, 0);
    /* Pack the dialog content into the dialog's GtkVBox. */
    gtk_box_pack_start(GTK_BOX(content_area), hbox, FALSE, FALSE, 0);
    gtk_widget_show_all (dialog);
    /* Create the dialog as modal and destroy it when a button is clicked. */
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void stk_print(char *fmt)
{
#ifdef STK_GUI_DEBUG
    stk_message(NULL, fmt);
#else
    g_print(fmt);
#endif
}

void stk_get_timestamp(char *str)
{
    time_t timep;
    struct tm *p;

    if(str == NULL) {
         printf("Are U Joking with me?\n");
         return;
    }

    time(&timep);
    p = localtime(&timep); 
    sprintf(str, "%d-%d-%d %02d:%02d:%02d", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);

    return; 
}

gboolean on_button_press(GtkWidget* widget, GdkEventButton* event, ImageButton *ib)
{
    gtk_container_remove(GTK_CONTAINER(ib->event_box), ib->image);

    ib->image = gtk_image_new_from_file(ib->on_press);
    gtk_container_add(GTK_CONTAINER(ib->event_box), ib->image);
    gtk_widget_show(ib->image);

    return TRUE;
}

gboolean on_button_release(GtkWidget* widget, GdkEventButton* event, ImageButton *ib)
{
    gtk_container_remove(GTK_CONTAINER(ib->event_box), ib->image);

    ib->image = gtk_image_new_from_file(ib->on_enter);
    gtk_container_add(GTK_CONTAINER(ib->event_box), ib->image);
    gtk_widget_show(ib->image);

    return TRUE;
}

gboolean on_enter_notify(GtkWidget* widget, GdkEventCrossing* event, ImageButton *ib)
{
    gtk_container_remove(GTK_CONTAINER(ib->event_box), ib->image);

    ib->image = gtk_image_new_from_file(ib->on_enter);
    gtk_container_add(GTK_CONTAINER(ib->event_box), ib->image);
    gtk_widget_show(ib->image);

    return TRUE;
}

gboolean on_leave_notify(GtkWidget* widget, GdkEventCrossing* event, ImageButton *ib)
{
    gtk_container_remove(GTK_CONTAINER(ib->event_box), ib->image);

    ib->image = gtk_image_new_from_file(ib->normal);
    gtk_container_add(GTK_CONTAINER(ib->event_box), ib->image);
    gtk_widget_show(ib->image);

    return TRUE;
}

void stk_create_imgbtn(ImageButton *ib, gpointer func, gpointer data)
{
    if (ib == NULL) {
        printf("What happened to master? Does him get 2\n");
        return;
    }

    ib->image = gtk_image_new_from_file(ib->normal);
    ib->event_box = gtk_event_box_new();
    gtk_container_add(GTK_CONTAINER(ib->event_box), ib->image);

#if GTK_CHECK_VERSION(2,12,0)
    if (ib->tips != NULL) {
        gtk_widget_set_tooltip_text(ib->event_box, ib->tips);
    }
#endif

    gtk_widget_add_events(ib->event_box, GDK_BUTTON_PRESS_MASK);
    g_signal_connect(G_OBJECT(ib->event_box), "button_press_event", G_CALLBACK(on_button_press), (gpointer)ib);

    gtk_widget_add_events(ib->event_box, GDK_BUTTON_RELEASE_MASK);
    g_signal_connect(G_OBJECT(ib->event_box), "button_release_event", G_CALLBACK(func), data);

    gtk_widget_add_events(ib->event_box, GDK_ENTER_NOTIFY_MASK);
    g_signal_connect(G_OBJECT(ib->event_box), "enter_notify_event", G_CALLBACK(on_enter_notify), (gpointer)ib);

    gtk_widget_add_events(ib->event_box, GDK_LEAVE_NOTIFY_MASK);
    g_signal_connect(G_OBJECT(ib->event_box), "leave_notify_event", G_CALLBACK(on_leave_notify), (gpointer)ib);
    return;
}
#if 0
void stk_movement(StkWidget widgets)
{
    int x, y, toward;

    /* check positon of move widget put in fixed container */
    x = 0;
    y = 20;
    toward = 1;

    while(1) {
        gtk_status_icon_set_blinking(widgets.tray, TRUE);
        g_usleep(1000*250);
        gdk_threads_enter();
        //gtk_fixed_move(GTK_FIXED(widgets.hfix),widgets.image, x, y);
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

    gtk_status_icon_set_blinking(widgets.tray, FALSE);
    //gtk_fixed_move(GTK_FIXED(widgets.hfix),widgets.image, 0, 20);
}

static stk_buddy *get_buddy_from_tree(GtkWidget *widget)
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

    return NULL;
}
#endif

