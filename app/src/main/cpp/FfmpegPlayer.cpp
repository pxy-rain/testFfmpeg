//
// Created by pengxiangyu on 2020/7/30.
//
#include "FfmpegPlayer.h"
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <jni.h>

#define TAG "FfmpegDecode"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__)

extern "C"{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"

int FfmpegVideoPlay(JNIEnv *env,const char *inputpath,jobject surface){
    AVFormatContext *fmt_ctx = NULL;//格式化IO上下文
    AVCodecContext *codeCtx = NULL;//视频编解码上下文
    AVStream *stream=NULL;  //视频流信息
    int ret,stream_index;   //视频流索引
    AVPacket avpkt; //视频数据容器
    AVCodec *codec=NULL;	//视频解码器
    AVFrame *pframe=NULL;   //解码后的Frame数据，不能直接播放还需转换
    AVFrame *rgba_frame = NULL;//转换后的Frame 用于播放
    int buffer_size;//输出buffer
    uint8_t *out_buffer;
    struct SwsContext *data_convert_context;
    ANativeWindow *native_window=NULL;
    // 定义绘图缓冲区
    ANativeWindow_Buffer window_buffer;
    //默认窗口大小
    int videoWidth = 640;
    int videoHeight = 480;

    //打开输入视频文件
    if(avformat_open_input(&fmt_ctx,inputpath,NULL,NULL) < 0){
        LOGD("FfmpegPlayer.cpp 28L:open file failed");
        return -1;
    }
    //获取视频文件流相关信息
    if(avformat_find_stream_info(fmt_ctx,NULL) < 0){
        LOGD("FfmpegPlayer.cpp 33L:Failed to get video file information flow");
        goto _Fail;
    }
    //找到视频流
    stream_index = av_find_best_stream(fmt_ctx,AVMEDIA_TYPE_VIDEO,-1,-1,NULL,0);
    if(stream_index < 0){   //失败
        LOGD("FfmpegPlayer.cpp 39L:Can not find video strem");
        goto _Fail;
    }
    stream = fmt_ctx->streams[stream_index];
    //初始化编解码上下文
    codeCtx = avcodec_alloc_context3(NULL);
    if(!codeCtx){
        return -1;
    }
    //拷贝流参数到编解码上下文
    if((ret = avcodec_parameters_to_context(codeCtx,stream->codecpar)) < 0){
        LOGD("FfmpegPlayer.cpp 50L:Failed to copy codec parameters to decoder context");
    }
    //初始化视频解码器
    codec = avcodec_find_decoder(codeCtx->codec_id);
    if(codec == NULL){
        LOGD("FfmpegPlayer.cpp 60L:Can not find video decoder");
        goto _Fail;
    }
    //打开解码器
    if(avcodec_open2(codeCtx,codec,NULL) < 0){
        LOGD("FfmpegPlayer.cpp 60L:Can not open codec");
        goto _Fail;
    }
    //获取视频宽和高
    videoWidth = codeCtx->width;
    videoHeight = codeCtx->height;
    //初始化native window用于播放视频
    native_window = ANativeWindow_fromSurface(env,surface);
    if (native_window == NULL) {
        LOGD("FfmpegPlayer.cpp 74L: Can not create native window");
        goto _Fail;
    }
    //
    ret = ANativeWindow_setBuffersGeometry(native_window,videoWidth,videoHeight,WINDOW_FORMAT_RGBA_8888);
    if(ret < 0){
        LOGD("FfmpegPlayer.cpp 79L:Can not set native window buffer");

        goto _Fail;
    }
    pframe = av_frame_alloc();
    rgba_frame = av_frame_alloc();
    buffer_size = av_image_get_buffer_size(AV_PIX_FMT_RGBA,videoWidth,
    videoHeight,1);
    //申请buffer内存
    out_buffer = (uint8_t *)av_malloc(buffer_size );//* sizeof(uint8_t)
    av_image_fill_arrays(rgba_frame->data,rgba_frame->linesize,out_buffer,
            AV_PIX_FMT_RGBA,videoWidth,videoHeight,1);
    //数据格式转换上下文
    data_convert_context = sws_getContext(videoWidth,videoHeight,
            codeCtx->pix_fmt,videoWidth,videoHeight,AV_PIX_FMT_RGBA,
            SWS_BICUBIC,NULL,NULL,NULL);
    while(av_read_frame(fmt_ctx,&avpkt) >= 0){
        if (avpkt.stream_index == stream_index) {
            // 解码
            ret = avcodec_send_packet(codeCtx, &avpkt);
            if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
                LOGD("FfmpegPlayer.cpp 107L: codec step 1 fail");
                goto _Fail;
            }
            ret = avcodec_receive_frame(codeCtx, pframe);
            if (ret < 0 && ret != AVERROR_EOF) {
                LOGD("Player Error : codec step 2 fail");
                goto _Fail;
            }
            // 数据格式转换
            ret = sws_scale(
                    data_convert_context,
                    (const uint8_t* const*) pframe->data, pframe->linesize,
                    0, videoHeight,
                    rgba_frame->data, rgba_frame->linesize);
            if (ret <= 0) {
                LOGD("Player Error : data convert fail");
                goto _Fail;
            }
            // 播放
            ret = ANativeWindow_lock(native_window, &window_buffer, NULL);
            if (ret < 0) {
                LOGD("Player Error : Can not lock native window");
            } else {
                // 将图像绘制到界面上
                // 注意 : 这里 rgba_frame 一行的像素和 window_buffer 一行的像素长度可能不一致
                // 需要转换好 否则可能花屏
                uint8_t *bits = (uint8_t *) window_buffer.bits;
                for (int h = 0; h < videoHeight; h++) {
                    memcpy(bits + h * window_buffer.stride * 4,
                           out_buffer + h * rgba_frame->linesize[0],
                           rgba_frame->linesize[0]);
                }
                ANativeWindow_unlockAndPost(native_window);
            }
        }
        // 释放 packet 引用
        av_packet_unref(&avpkt);
    }
    _Fail:
    if(data_convert_context) {
        sws_freeContext(data_convert_context);
    }
    if(out_buffer){
        av_free(out_buffer);
    }
    if(rgba_frame){
        av_frame_free(&rgba_frame);
    }
    if (pframe) {
        av_frame_free(&pframe);
    }
    ANativeWindow_release(native_window);
    // Close the codec
    if (codeCtx) {
        avcodec_close(codeCtx);
    }
    avformat_close_input(&fmt_ctx);

    return 0;
}
}