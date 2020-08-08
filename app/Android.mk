LOCAL_PATH:= $(call my-dir)	
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \	#指定编译进模块的源文件
    screencap.cpp

LOCAL_SHARED_LIBRARIES := \	#指定链接的共享库
    libcutils \
    libutils \
    libbinder \
    libskia \
    libui \
    libgui

LOCAL_MODULE:= screencap	#LOCAL_MODULE变量必须定义，以标识你在Android.mk文件中描述的每个模块。名称必须是唯一的

LOCAL_MODULE_TAGS := optional	#指定在什么版本下编译	optional是所有版本下都编译

LOCAL_CFLAGS += -Wall -Werror -Wunused -Wunreachable-code	#编译器选项 相当于在源文件中增加一个宏定义

include $(BUILD_EXECUTABLE)
