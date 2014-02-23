/* 
 * File: stkwidget.h
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */

#ifndef _STKWIDGET_H_
#define _STKWIDGET_H_

#include <gdk/gdkkeysyms.h>

#define USE_GTK_THREAD

#define STK_WINDOW_WIDTH  300
#define STK_WINDOW_HEIGHT 600

#if GTK_CHECK_VERSION(3,0,0)
#define GDK_Return GDK_KEY_Return
#define gtk_widget_hide_all gtk_widget_hide
#endif

#if !GTK_CHECK_VERSION(2,12,0)
//#define gtk_tool_item_set_tooltip_text(tool_item, text) (gtk_tool_item_set_tooltip((tool_item), (NULL), (text), (NULL)))
#endif

#define MAIN_COLRO_STRING "#67A3CD"

#define STK_ICON_PNG "pixmaps/icon.png"

#define STK_CLOSE_PNG "pixmaps/btn_close.png"
#define STK_MIN_PNG "pixmaps/btn_min.png"
#define STK_SESSION_PNG "pixmaps/buddy.png"

#define STK_AVATAR_PNG "pixmaps/avatar.png"
#define STK_BUDDY_PNG "pixmaps/buddy.png"

#define STK_CHAT_PNG "pixmaps/chat.png"
#define STK_VOICE_PNG "pixmaps/voice.png"
#define STK_VIDEO_PNG "pixmaps/video.png"

enum
{
  STK_PIXBUF_COL,
  STK_TEXT_COL,
  STK_COL_NUM
};

typedef struct{
    int width;
    int height;
}SCREENSIZE;

typedef struct{
    GtkWidget *layout;
    GtkWidget *usertext;
    GtkWidget *passtext;
    GtkWidget *servertext;
}LOGINWIDGET;

typedef struct{
    GdkScreen *screen;
    SCREENSIZE size;
    GtkWidget *mainw;
    GtkStatusIcon *tray;
    GtkWidget *treev;
    LOGINWIDGET loginw;
}STKWIDGETS;

#endif /* _STKWIDGET_H_ */

