/* 
 * File: stkui.h
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */
#ifndef _STKUI_H_
#define _STKUI_H_

#define STK_MAINWIN_WIDTH     300
#define STK_MAINWIN_HEIGHT    600

#define STK_CHATWIN_WIDTH     500
#define STK_CHATWIN_HEIGHT    460

/* main window color */
/* 
 * "#67A3CD" equal RGB(103,163,205)
 */

#define STKCLIENT  "stkclient"
#define STKCHATWIN "stkchat"

#define DEFAULT_RGB (RGB(103,163,205))

#define WMAPP_TRAY   (WM_APP + 1)
#define WMAPP_SOCKET (WM_APP + 2)
#define WMAPP_MSG    (WM_APP + 3)

#define IDM_SHOW       1
#define IDM_HIDE       2
#define IDM_EXIT       3

#define IDM_CLOSE      4
#define IDM_MIN        5

#define IDM_BEAUTY     6

#define IDM_USERNAME   7
#define IDM_USERPASS   8
#define IDM_SERVERIP   9
#define IDM_LOGIN      10
#define IDM_CANCEL     11
#define IDM_RESULT     12

#define IDM_LOGINING   13
#define IDM_AVATAR     14
#define IDM_PROFILE    15

#define IDM_TABCTRL    16
#define IDM_BUDDYTREE  17
#define IDM_GROUPTREE  18

#define STK_TREE_BUDDY  0
#define STK_TREE_GROUP  1

#define STK_CLIENT_ICO     "pixmaps/stkclient.ico"

#define STK_CLO_BMP        "pixmaps/btn_close.bmp"
#define STK_MIN_BMP        "pixmaps/btn_min.bmp"

#define STK_BEATU_BMP      "pixmaps/beauty.bmp"

#define STK_CLO_ICO        "pixmaps/btn_close.ico"
#define STK_MIN_ICO        "pixmaps/btn_min.ico"

#define STK_LOGIN_BMP      "pixmaps/btn_login.bmp"
#define STK_CANCEL_BMP     "pixmaps/btn_cancel.bmp"

#define STK_LOGINING_BMP   "pixmaps/logining.bmp"

#define STK_AVATAR_1_BMP   "pixmaps/avatar.bmp"
#define STK_AVATAR_2_BMP   "pixmaps/avatar_girl.bmp"

#define STK_BUDDY_BMP      "pixmaps/buddy.bmp"
#define STK_GROUP_BMP      "pixmaps/group.bmp"

#define STK_BUDDY_ICO      "pixmaps/buddy.ico"
#define STK_GROUP_ICO      "pixmaps/group.ico"

#define STK_MSG_COMING_WAV "sounds/message.wav"

/* 
 * Tray Refer:  http://www.cnblogs.com/Clingingboy/archive/2011/08/31/2161459.html
 */

typedef struct{
    HINSTANCE hins;
    HWND hwnd;
    HMENU menu;
    NOTIFYICONDATA nid;
    HWND avatarlabel;
    HWND profilelabel;
    HWND tabcontrol;
    HWND buddytreeview;
    HWND grouptreeview;
    int bimgindex;
    int gimgindex;
}StkWidget;

typedef struct{
    HWND beautylabel;
    HWND userlabel;
    HWND usertext;
    HWND passlabel;
    HWND passtext;
    HWND serverlabel;
    HWND servertext;
    HWND loginbtn;
    HWND cancelbtn;
    HWND logininglabel;
    HWND resultlabel;
    int width;
    int height;
}LoginWidget;

#endif /* _STKUI_H_ */

