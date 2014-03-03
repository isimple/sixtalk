/* 
 * File: stkwav.c
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stkwav.h"

#ifdef _LINUX_

char* stk_linux_analyze_wav(char *fname, wav_t *wav)
{
    FILE* fp;
    char buffer[256];
    char *wavbuf = NULL;
    int  read_len = 0;
    int  offset = 0;

    if(NULL == fname){
        printf("fname is NULL\n");
        return NULL;
    }

    fp = fopen(fname, "r");
    if(NULL == fp){
        printf("fopen %s failed\n", fname);
        return NULL;
    }

    /* handle RIFF WAVE chunk */
    read_len = fread(buffer, 1, 12, fp);
    if(read_len < 12){
        printf("error wav file\n");
        fclose(fp);
        return NULL;
    }
    if(strncmp("RIFF", buffer, 4)){
        printf("error wav file\n");
        fclose(fp);
        return NULL;
    }
    memcpy(wav->riff.id, buffer, 4); 
    wav->riff.size = *(int *)(buffer + 4);
    if(strncmp("WAVE", buffer + 8, 4)){
        printf("error wav file\n");
        fclose(fp);
        return NULL;
    }
    memcpy(wav->riff.type, buffer + 8, 4);
    wav->file_size = wav->riff.size + 8;

    offset += 12;
    while(1){
        char id_buffer[5] = {0};
        int  tmp_size = 0;

        read_len = fread(buffer, 1, 8, fp);  
        if(read_len < 8){
            printf("error wav file\n");
            fclose(fp);
            return NULL;
        }
        memcpy(id_buffer, buffer, 4);
        tmp_size = *(int *)(buffer + 4);


        if(0 == strncmp("fmt", id_buffer, 3)) {
            memcpy(wav->format.id, id_buffer, 3);
            wav->format.size = tmp_size;
            read_len = fread(buffer, 1, tmp_size, fp);
            if(read_len < tmp_size){
                printf("error wav file\n");
                fclose(fp);
                return NULL;
            }
            wav->format.compression_code  = *(short *)buffer;
            wav->format.channels          = *(short *)(buffer + 2);
            wav->format.samples_per_sec   = *(int *)(buffer + 4);
            wav->format.avg_bytes_per_sec = *(int *)(buffer + 8);
            wav->format.block_align       = *(short *)(buffer + 12);
            wav->format.bits_per_sample   = *(short *)(buffer + 14);
        } else if (0 == strncmp("data", id_buffer, 4)){ /* we believe data chunk is the last one */
            memcpy(wav->data.id, id_buffer, 4); 
            wav->data.size = tmp_size;
            offset += 8;
            wav->data_offset = offset;
            wav->data_size = wav->data.size;
            break;
        } else { /* someone says there may be user-defined chunk, just skip it. */
            printf("unhandled chunk: %s, size: %d\n", id_buffer, tmp_size);
            fseek(fp, tmp_size, SEEK_CUR);
        }
        offset += 8 + tmp_size;
    }

    /* now, read wav data into buf */
    wavbuf = (char *)malloc(wav->data_size);
    if(wavbuf == NULL) {
        perror("malloc for wav data error");
        fclose(fp);
        return NULL;
    }

    fseek(fp, wav->data_offset, SEEK_SET);
    read_len = fread(wavbuf, 1, wav->data_size, fp);  
    if (read_len != wav->data_size) {
        printf("fread wav data error\n");
        free(wavbuf);
        wavbuf = NULL;
    }

    fclose(fp);
    return wavbuf;
}

int stk_linux_play_sound(char *wavbuf, wav_t *wav)
{
    int ret;
    int fd;
    int arg;
  
    fd = open(STK_DSP_DEVICE, O_WRONLY);
    if(fd == -1){
        perror("open dsp device fail");
        return -1;
    }
 
    arg = wav->format.samples_per_sec;
    ret = ioctl(fd, SOUND_PCM_WRITE_RATE, &arg);
    if(ret == -1){
        perror("error from SOUND_PCM_WRITE_RATE ioctl");
        return -1;
    }

    arg = wav->format.bits_per_sample;
    ret = ioctl(fd, SOUND_PCM_WRITE_BITS, &arg);
    if(ret == -1){
        perror("error from SOUND_PCM_WRITE_BITS ioctl");
        return -1;
    }
 
    ret = write(fd, wavbuf, wav->data_size);
    if(ret == -1){
        perror("Fail to play the sound");
        return -1;
    }

    free(wavbuf);
    close(fd);
    return ret;
}

void stk_linux_play_wav(char *filename)
{
    int ret;
    char *wavbuf;
    wav_t wav;

    memset(&wav, 0, sizeof(wav_t));
    wavbuf = stk_linux_analyze_wav(filename, &wav);
    if (wavbuf == NULL) {
        return;
    } else {
        stk_linux_play_sound(wavbuf, &wav);
    }
    return;
}

#endif

void stk_play_wav(char *filename)
{

#if defined(WIN32)
    /* search 'Playing WAVE Resources' for details */
    //PlayResource(filename); 
    sndPlaySound(filename, SND_FILENAME);
#elif defined(_LINUX_)
    stk_linux_play_wav(filename);
#endif

    return;
}

