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
    char t[STK_DEFAULT_SIZE] = "STK Client";

    if (title != NULL) {
        memset(t, 0, sizeof(t));
        strcpy(t, title);
    }

    dialog = gtk_dialog_new_with_buttons(t, NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);

    gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);
    label = gtk_label_new (msg);
    image = gtk_image_new_from_stock (GTK_STOCK_DIALOG_INFO, GTK_ICON_SIZE_DIALOG);
    hbox = gtk_hbox_new (FALSE, 5);
    gtk_container_set_border_width (GTK_CONTAINER (hbox), 10);
    gtk_box_pack_start_defaults (GTK_BOX (hbox), image);
    gtk_box_pack_start_defaults (GTK_BOX (hbox), label);
    /* Pack the dialog content into the dialog's GtkVBox. */
    gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG (dialog)->vbox), hbox);
    gtk_widget_show_all (dialog);
    /* Create the dialog as modal and destroy it when a button is clicked. */
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
}

void stk_print(char *fmt)
{
#ifdef STK_GUI_DEBUG
    stk_message(NULL, fmt);
#else
    g_print(fmt);
#endif
}



