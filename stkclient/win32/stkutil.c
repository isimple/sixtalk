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

#define STK_LOG_FILE       "stklog.txt"

HANDLE runningMutex = NULL;
int stk_running = STK_UNINITIALIZED;

void stk_init_running()
{
    runningMutex = CreateMutex(NULL, FALSE, NULL);
}

int stk_get_running()
{
    int running;
    WaitForSingleObject(runningMutex, INFINITE);
    running = stk_running;
    ReleaseMutex(runningMutex);
    return running;
}

void stk_set_running(int running)
{
    WaitForSingleObject(runningMutex, INFINITE);
    stk_running = running;
    ReleaseMutex(runningMutex);
}

void stk_get_timestamp(char *str)
{
    SYSTEMTIME st;

    if(str == NULL) {
         printf("Are U Joking with me?\n");
         return;
    }

    GetLocalTime(&st);
    sprintf(str, "%d-%d-%d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

    return; 
}

void stk_play_wav(char *filename)
{
    /* search 'Playing WAVE Resources' for details */
    //PlayResource(filename); 
    sndPlaySound(filename, SND_FILENAME | SND_ASYNC);

    return;
}

#ifdef DEBUG_TO_FILE
void stk_log(const char *fmt, ...)
{
    char str[STK_LONG_SIZE] = {0};
    FILE *fp;
    va_list args;

    va_start(args, fmt);
    fp = fopen(STK_LOG_FILE,"at");
    vsprintf(str, fmt, args);
	fwrite(str, strlen(str), 1, fp);
    va_end(args);

    fclose(fp);
}
#else
void stk_log(const char* fmt, ...)
{
    char str[STK_LONG_SIZE] = {0};
    va_list args;

    va_start(args, fmt);
    vsprintf(str, fmt, args);
	printf("%s", str);
    va_end(args);  
}
#endif
