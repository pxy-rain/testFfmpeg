//
// Created by pengxiangyu on 2020/7/30.
//
#ifndef TESTFFMPEG_FFMPEGPLAYER_H
#define TESTFFMPEG_FFMPEGPLAYER_H
#include <jni.h>
#ifdef __cplusplus
extern "C" {
#endif
int FfmpegVideoPlay(JNIEnv *env,const char *inputpath,jobject surface);
int videoPlay(JNIEnv* env,jobject surface);
#ifdef __cplusplus
}
#endif

#endif //TESTFFMPEG_FFMPEGPLAYER_H
