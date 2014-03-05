/* 
 * File: stkclient.h
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */

#ifndef _STKCLIENT_H_
#define _STKCLIENT_H_

#define STK_SERVER_PORT        9007
#define STK_CLIENT_PORT        9008

#define STK_MAX_PACKET_SIZE    65535
#define STK_STDIN_SIZE         4096


#define STK_MAX_SIZE           4096
#define STK_DEFAULT_SIZE       64

#define STK_CLIENT_OFFLINE     0
#define STK_CLIENT_ONLINE      1

#define STK_GENDER_UNKNOWN     0
#define STK_GENDER_BOY         1
#define STK_GENDER_GIRL        2

#define STK_CLIENT_LOGIN_ERROR       -1
#define STK_CLIENT_LOGIN_SUCCESS      0
#define STK_CLIENT_LOGIN_AGAIN        1
#define STK_CLIENT_LOGIN_INVALID_UID  2
#define STK_CLIENT_LOGIN_INVALID_PASS 3

#define STK_NULL_POINTER       -1

#define STK_SOCKET_ERROR       -1
#define STK_SOCKET_CLOSED      -2

#define STK_CLIENT_TIMEOUT      5

/* used by stk_running */
#define STK_UNINITIALIZED  1
#define STK_INITIALIZED    2
#define STK_CONNECTE_REQ   3
#define STK_CONNECTING     4
#define STK_CONNECTE_ERR   5
#define STK_CONNECTED      6
#define STK_SOCKET_ERR     7
#define STK_USERID_ERR     8
#define STK_PASSWORD_ERR   9
#define STK_ALREADY_LOGGIN 10
#define STK_RUNNING        11
#define STK_EXITING        12
#define STK_SERVER_EXIT    13

#if defined(WIN32)
#define socket_t  SOCKET
#elif defined(_LINUX_)
#define socket_t  int
#endif

typedef struct _gchat_message{
    unsigned int uid;
    int  msg_len;
    char *timestamp;
    char *msg;
    struct _gchat_message *next;
}gchat_message;

typedef struct _group_member{
    unsigned int uid;
    struct _group_member *next;
}group_member;

typedef struct{
    int show;
    GtkWidget *window;
    GtkWidget *toolbox;
    GtkWidget *show_scrolled,*input_scrolled;
    GtkWidget *show_view,*input_view;
    GtkTextBuffer *show_buffer,*input_buffer;
    GtkWidget *send_button,*close_button;
    GtkWidget *hbox,*vbox;
    GtkTextTag *minfo, *binfo, *mtext, *btext;
}gchat_widgets;

typedef struct _stk_group{
    unsigned int  groupid;
    unsigned char groupname[STK_GROUP_NAME_SIZE];
    int           member_num;
    group_member  *members;
    struct _stk_group *next;
    GtkWidget *menu;
    gchat_widgets gchat;
    int gmsg_num;
    gchat_message *gchatmsg;
}stk_group;

typedef struct _chat_message{
    int  msg_len;
    char *timestamp;
    char *msg;
    struct _chat_message *next;
}chat_message;

typedef struct{
    int show;
    GtkWidget *window;
    GtkWidget *toolbox;
    GtkWidget *show_scrolled,*input_scrolled;
    GtkWidget *show_view,*input_view;
    GtkTextBuffer *show_buffer,*input_buffer;
    GtkWidget *send_button,*close_button;
    GtkWidget *hbox,*vbox;
    GtkTextTag *minfo, *binfo, *mtext, *btext;
    //struct chat_widgets *next;
}chat_widgets;

typedef struct{
    unsigned int  uid;
    unsigned char nickname[STK_NICKNAME_SIZE];
    unsigned char city[STK_CITY_SIZE];
    unsigned int  phone;
    unsigned char gender;
    struct list_head list;
    int state;
    GtkWidget *menu;
    chat_widgets chat;
    int msg_num;
    chat_message *chatmsg;
}stk_buddy;

typedef struct{
    unsigned int  uid;
    unsigned char nickname[STK_NICKNAME_SIZE];
    unsigned char city[STK_CITY_SIZE];
    unsigned int  phone;
    unsigned char gender;
    unsigned char pass[STK_PASS_SIZE];
    unsigned char serverip[STK_IPADDR_LENGTH];
    int           group_num;
    stk_group     *group;
    socket_t fd;
    int state;
}client_config;

#endif /* _STKCLIENT_H_ */

