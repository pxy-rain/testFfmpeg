#include <jni.h>
#include <string>
#include <android/log.h>

extern "C"{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "FfmpegPlayer.h"
}

#define TAG "FfmpegDecode"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__)

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapplication_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */,
        jstring inputurl,
        jobject surface) {

    const char *inputpath = env->GetStringUTFChars(inputurl,0);

    char info[10000] = { 0 };
    const char *filepath = "/mnt/sdcard/Movies/ScreenCaptures/20200729-234214.mp4";
    sprintf(info, "%s\n", avcodec_configuration());
    AVFormatContext *fmt_ctx = NULL;


    ///FfmpegVideoPlay(env,filepath,surface);
    videoPlay(env,surface);
    return env->NewStringUTF(info);
}
