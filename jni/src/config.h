/**
 * @file config.h
 gcc cmdutils.c ffplay.c -lSDL2 -I"/usr/include/ffmpeg/" -L"/usr/lib" -I"/usr/include/SDL2/" -l"avformat" -l"avcodec" -l"avdevice" -l"avutil" -l"avfilter" -l"swresample" -l"swscale" -lm && ./a.out  http://live.cgtn.com/500d/prog_index.m3u8 
 gcc cmdutils.c ffplay.c -lSDL2 -I"/home/libiao/ffmpeg-3.3/" -L"/home/libiao/ffmpeg-3.3/" -I"../SDL2/include/" -ldl -lpthread -lbz2 -lz -llzma -l"avformat" -l"avcodec" -l"avdevice" -l"avutil" -l"avfilter" -l"swresample" -l"swscale" -lm && ./a.out  http://live.cgtn.com/500d/prog_index.m3u8 
 *  
 * @author db0@qq.com
 * @version 1.0.1
 * @date 2017-05-08
 */

#ifndef ffplay_config
#define ffplay_config

#define CONFIG_AVDEVICE 0
#define CONFIG_AVFILTER 0
#define CONFIG_RTSP_DEMUXER 0
#define CONFIG_MMSH_PROTOCOL 0

#endif
