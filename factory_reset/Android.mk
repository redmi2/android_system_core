LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= factory_reset.c

LOCAL_SHARED_LIBRARIES := libcutils libc

LOCAL_MODULE:= factory_reset

include $(BUILD_EXECUTABLE)


