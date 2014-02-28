/* 
 * File: stkwav.h
 * 
 *  Copyright (C) 2014 SixTeam
 *  All Rights Reserved
 *
 */

#ifndef _STKWAV_H_
#define _STKWAV_H_

#if defined(WIN32)
#include <windows.h>
#elif defined(_LINUX_)
#include <fcntl.h>
#include <linux/soundcard.h>

/*
 * if your linux does not have /dev/dsp 
 * U can do the following
 * # mknod /dev/dsp c 14 3
 * # modprobe snd-pcm-oss
 */
#define STK_DSP_DEVICE "/dev/dsp"
#define STK_CHUNKID_LEN (4+1)
/*
 *            Wave format analyze
 *
 * Chunk
 * ------------------------------------------------
 * |             RIFF WAVE Chunk                  |
 * |             ID  = 'RIFF'                     |
 * |             RiffType = 'WAVE'                |
 * ------------------------------------------------
 * |             Format Chunk                     |
 * |             ID = 'fmt '                      |
 * ------------------------------------------------
 * |             Fact Chunk(optional)             |
 * |             ID = 'fact'                      |
 * ------------------------------------------------
 * |             Data Chunk                       |
 * |             ID = 'data'                      |
 * ------------------------------------------------
 * 
 * RIFF WAVE Chunk
 * ==================================
 * |       |所占字节数|  具体内容   |
 * ==================================
 * | ID    |  4 Bytes |   'RIFF'    |
 * ----------------------------------
 * | Size  |  4 Bytes |             |
 * ----------------------------------
 * | Type  |  4 Bytes |   'WAVE'    |
 * ----------------------------------
 *
 * Format Chunk
 * ====================================================================
 * |               |   字节数  |              具体内容                |
 * ====================================================================
 * | ID            |  4 Bytes  |   'fmt '                             |
 * --------------------------------------------------------------------
 * | Size          |  4 Bytes  | 数值为16或18，18则最后又附加信息     |
 * --------------------------------------------------------------------  ----
 * | FormatTag     |  2 Bytes  | 编码方式，一般为0x0001               |     |
 * --------------------------------------------------------------------     |
 * | Channels      |  2 Bytes  | 声道数目，1--单声道；2--双声道       |     |
 * --------------------------------------------------------------------     |
 * | SamplesPerSec |  4 Bytes  | 采样频率                             |     |
 * --------------------------------------------------------------------     |
 * | AvgBytesPerSec|  4 Bytes  | 每秒所需字节数                       |     |===> WAVE_FORMAT
 * --------------------------------------------------------------------     |
 * | BlockAlign    |  2 Bytes  | 数据块对齐单位(每个采样需要的字节数) |     |
 * --------------------------------------------------------------------     |
 * | BitsPerSample |  2 Bytes  | 每个采样需要的bit数                  |     |
 * --------------------------------------------------------------------     |
 * |               |  2 Bytes  | 附加信息（可选，通过Size来判断有无） |     |
 * --------------------------------------------------------------------  ----
 *
 * Fact Chunk
 * ==================================
 * |       |所占字节数|  具体内容   |
 * ==================================
 * | ID    |  4 Bytes |   'fact'    |
 * ----------------------------------
 * | Size  |  4 Bytes |   数值为4   |
 * ----------------------------------
 * | data  |  4 Bytes |             |
 * ----------------------------------
 *
 * Data Chunk
 * ==================================
 * |       |所占字节数|  具体内容   |
 * ==================================
 * | ID    |  4 Bytes |   'data'    |
 * ----------------------------------
 * | Size  |  4 Bytes |             |
 * ----------------------------------
 * | data  |          |             |
 * ----------------------------------
 *
 * for details, See http://blog.sina.com.cn/s/blog_717f471b0100uw98.html
 * also See http://www.cnblogs.com/Xiao_bird/archive/2009/09/08/1562476.html
 *
 * about linux Audio programming, see
 * https://www.ibm.com/developerworks/cn/linux/l-audio/
 *
 */

typedef struct{
    char id[STK_CHUNKID_LEN];      /* ID:"RIFF" */
    int  size;                     /* filelen - 8 */
    char type[4];                  /* type:"WAVE" */
}wav_riff_t;

typedef struct{
    char  id[STK_CHUNKID_LEN];     /*ID:"fmt" */
    int   size;
    short compression_code;
    short channels;
    int   samples_per_sec;
    int   avg_bytes_per_sec;
    short block_align;
    short bits_per_sample;
}wav_format_t;

typedef struct{
    char id[STK_CHUNKID_LEN];      /*ID:"fact" */
    int  size;
}wav_fact_t;

typedef struct{
    char id[STK_CHUNKID_LEN];      /*ID:"data" */
    int  size;
}wav_data_t;

typedef struct{
    wav_riff_t   riff;
    wav_format_t format;
    wav_fact_t   fact;
    wav_data_t   data;
    int          file_size;
    int          data_offset;
    int          data_size;
}wav_t;


#endif 
#endif /* _STKWAV_H_ */

