/* 
 * File: stk.h
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */

#ifndef _STK_H_
#define _STK_H_

#ifdef WIN32   
#include <winsock2.h>
#endif

#include <gtk/gtk.h>

#include "stklist.h"
#include "stkprotocol.h"
#include "stkclient.h"

//#define STK_GUI_DEBUG

/*
 *********************************************
 *               stkbuddy.c                  *
 *********************************************
 */
stk_buddy *stk_find_buddy(unsigned int uid);
int stk_add_buddy(stk_buddy *buddy);
int stk_update_buddy(stk_buddy *buddy);
unsigned short stk_get_buddynum(void);
stk_buddy *stk_get_next(stk_buddy *buddy);


/*
 *********************************************
 *              stkpacket.c                  *
 *********************************************
 */
int stk_recv_msg(client_config *client);


#endif /* _STK_H_ */

