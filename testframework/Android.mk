LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

ifdef TARGET_USES_TESTFRAMEWORK
#testframework lib
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)
LOCAL_SRC_FILES := \
        TestFrameworkApi.cpp \
        TestFrameworkCommon.cpp \
        TestFrameworkHash.cpp \
        TestFramework.cpp \
        TestFrameworkService.cpp

LOCAL_CFLAGS := -DCUSTOM_EVENTS_TESTFRAMEWORK

LOCAL_SHARED_LIBRARIES += \
        libutils \
        libcutils \
        libbinder

LOCAL_MODULE:= libtestframework
LOCAL_MODULE_TAGS := debug
include $(BUILD_SHARED_LIBRARY)

#testframework servcice
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
        TestFrameworkServiceMain.cpp

LOCAL_SHARED_LIBRARIES := libtestframework libcutils libutils libbinder

LOCAL_CFLAGS := -DCUSTOM_EVENTS_TESTFRAMEWORK
LOCAL_MODULE:= testframeworkservice
LOCAL_MODULE_TAGS := debug

include $(BUILD_EXECUTABLE)

endif
