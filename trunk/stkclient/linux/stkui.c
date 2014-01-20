/* 
 * File: stkui.c
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */

#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "stk.h"

void stk_print_hello()
{
    printf("====================================================\n");
    printf("=================== STK IM CLIENT  =================\n");
    printf("====================================================\n");
    fflush(stdout);
}

void stk_usage()
{
    stk_print_hello();
    printf("info - show my profile\n");
    printf("list - list my friend\n");
    printf("chat - chat to my friend\n");
    printf("exit - exit client\n");
    printf("====================================================\n");
    fflush(stdout);
}

void stk_hanle_input(int fd, client_config *client)
{
    int len;
    char line[STK_STDIN_SIZE] = {0};

    /* NOTICE: \n will be read to buffer then when read from stdin read/fgets/gets */
    //fgets(line, STK_STDIN_SIZE, stdin);
    len = read(fd, line, STK_STDIN_SIZE);
    line[len-1] = '\0';

    if (!strcmp(line, "help") || !strcmp(line, "?")){
        stk_usage();
        printf("Please input command:");
        fflush(stdout);
    } else if (!strcmp(line, "info")) {
        stk_print_buddy((stk_buddy *)client);
        fflush(stdout);
    } else if (!strcmp(line, "list")) {
        stk_print_buddylist();
    } else if (!strcmp(line, "chat")) {
        
    } else if (!strcmp(line, "exit")) {
        printf("\n### exiting STK Client......\n\n");
        fflush(stdout);
        exit(0);
    } else {
        stk_print_hello();
        printf("Please input command:");
        fflush(stdout);
    }
    return;
}




