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

#define STK_MAINWIN_WIDTH     300
#define STK_MAINWIN_HEIGHT    600

#define STK_CHATWIN_WIDTH     480
#define STK_CHATWIN_HEIGHT    440

#define STK_DISTANCE_RSCREEN  40

#if GTK_CHECK_VERSION(3,0,0)
#define GDK_Return GDK_KEY_Return
#define gtk_widget_hide_all gtk_widget_hide
#endif

/* main window color */
#define MAIN_COLRO_STRING     "#67A3CD"
//#define MAIN_COLRO_STRING     "#C8C8FA"
//#define MAIN_COLRO_STRING     "#96FA96"

/* program icon */
#define STK_ICON_PNG          "pixmaps/icon.png"

/* close button images */
#define STK_CLOSE_NORMAL_PNG  "pixmaps/btn_close_normal.png"
#define STK_CLOSE_HLIGHT_PNG  "pixmaps/btn_close_highlight.png"
#define STK_CLOSE_DOWN_PNG    "pixmaps/btn_close_down.png"

/* minimize button images */
#define STK_MIN_NORMAL_PNG    "pixmaps/btn_min_normal.png"
#define STK_MIN_HLIGHT_PNG    "pixmaps/btn_min_highlight.png"
#define STK_MIN_DOWN_PNG      "pixmaps/btn_min_down.png"

#if 0
#define STK_CLOSE_PNG         "pixmaps/btn_close.png"
#define STK_MIN_PNG           "pixmaps/btn_min.png"
#endif

/* login button image */
#define STK_LOGIN_PNG         "pixmaps/btn_login.png"
#define STK_CANCEL_PNG        "pixmaps/btn_cancel.png"

/*  */
#define STK_SESSION_PNG       "pixmaps/buddy.png"
#define STK_GSESSION_PNG      "pixmaps/group.png"

#define STK_CHAT_PNG          "pixmaps/chat.png"
#define STK_VOICE_PNG         "pixmaps/voice.png"
#define STK_VIDEO_PNG         "pixmaps/video.png"

/* userinfo button image */
#define STK_AVATAR_PNG        "pixmaps/avatar.png"
#define STK_AVATAR_GIRL_PNG  "pixmaps/avatar_girl.png"
#define STK_BUDDY_PNG         "pixmaps/buddy.png"
#define STK_GROUP_PNG         "pixmaps/group.png"

/* connect waiting image */
#define STK_CONNECT_PNG       "pixmaps/logining.png"
//#define STK_CONNECT_PNG       "pixmaps/loading.gif"

#define STK_BEATU_PNG         "pixmaps/wakana.gif"

#define STK_MSG_COMING_WAV    "sounds/message.wav"

#define STK_IMAGE_PATH        64

enum
{
  STK_BUDDY_PIXBUF_COL,
  STK_BUDDY_ID_COL,
  STK_BUDDY_NAME_COL,
  STK_BUDDY_COL_NUM
};

enum
{
  STK_GROUP_PIXBUF_COL,
  STK_GROUP_ID_COL,
  STK_GROUP_NAME_COL,
  STK_GROUP_COL_NUM
};

typedef struct{
    int width;
    int height;
}ScreenSize;

typedef struct{
    GtkWidget *layout;
    GtkWidget *connlayout;
    GtkWidget *usertext;
    GtkWidget *passtext;
    GtkWidget *servertext;
    GtkWidget *resultlabel;
}LoginWidget;

typedef struct{
    char      normal[STK_IMAGE_PATH];
    char      on_enter[STK_IMAGE_PATH];
    char      on_press[STK_IMAGE_PATH];
    char      tips[STK_DEFAULT_SIZE];
    GtkWidget *image;
    GtkWidget *event_box;
}ImageButton;

typedef struct{
    GtkWidget     *mainw;
    GtkStatusIcon *tray;
    GtkWidget     *buddytree;
    GtkWidget     *grouptree;
    LoginWidget   loginw;
    ImageButton   cbtn;
    ImageButton   minbtn;
}StkWidget;


#endif /* _STKWIDGET_H_ */

